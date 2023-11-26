#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "heap.h"
#include "poke327.h"
#include "character.h"
#include "io.h"
#include "parse.h"
#include "poke.h"

  std::vector<pokemon_t*> pokemon;
  std::vector<moves_t*> moves;
  std::vector<pokemon_moves_t*> pokemon_moves;
  std::vector<pokemon_species_t*> pokemon_species;
  std::vector<experience_t*> experience;
  std::vector<type_names_t*> type_names;
  std::vector<pokemon_stats_t*> pokemon_stats;
  std::vector<stats_t*> stats;
  std::vector<pokemon_types_t*> pokemon_types;


typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

world_t world;

pair_t all_dirs[8] = {
  { -1, -1 },
  { -1,  0 },
  { -1,  1 },
  {  0, -1 },
  {  0,  1 },
  {  1, -1 },
  {  1,  0 },
  {  1,  1 },
};

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t edge_penalty(int8_t x, int8_t y)
{
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static void dijkstra_path(map_t *m, pair_t from, pair_t to)
{
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint16_t x, y;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = (path_t *) heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        mapxy(x, y) = ter_path;
        heightxy(x, y) = 0;
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

static int build_paths(map_t *m)
{
  pair_t from, to;

  /*  printf("%d %d %d %d\n", m->n, m->s, m->e, m->w);*/

  if (m->e != -1 && m->w != -1) {
    from[dim_x] = 1;
    to[dim_x] = MAP_X - 2;
    from[dim_y] = m->w;
    to[dim_y] = m->e;

    dijkstra_path(m, from, to);
  }

  if (m->n != -1 && m->s != -1) {
    from[dim_y] = 1;
    to[dim_y] = MAP_Y - 2;
    from[dim_x] = m->n;
    to[dim_x] = m->s;

    dijkstra_path(m, from, to);
  }

  if (m->e == -1) {
    if (m->s == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->w == -1) {
    if (m->s == -1) {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->n == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->s == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }

    dijkstra_path(m, from, to);
  }

  return 0;
}

static int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};

static int smooth_height(map_t *m)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  /*  FILE *out;*/
  uint8_t height[MAP_Y][MAP_X];

  memset(&height, 0, sizeof (height));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1) {
      head = tail = (queue_node_t *) malloc(sizeof (*tail));
    } else {
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);
  */
  
  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
      height[y - 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1]) {
      height[y][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
      height[y + 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x]) {
      height[y - 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x]) {
      height[y + 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
      height[y - 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1]) {
      height[y][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
      height[y + 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->height, sizeof (m->height), 1, out);
  fclose(out);
  */

  return 0;
}

static void find_building_location(map_t *m, pair_t p)
{
  do {
    p[dim_x] = rand() % (MAP_X - 3) + 1;
    p[dim_y] = rand() % (MAP_Y - 3) + 1;

    if ((((mapxy(p[dim_x] - 1, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x] + 2, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] - 1) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] + 2) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path)))   &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path)))) {
          break;
    }
  } while (1);
}

static int place_pokemart(map_t *m)
{
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(map_t *m)
{  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_center;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

/* Chooses tree or boulder for border cell.  Choice is biased by dominance *
 * of neighboring cells.                                                   */
static terrain_type_t border_type(map_t *m, int32_t x, int32_t y)
{
  int32_t p, q;
  int32_t r, t;
  int32_t miny, minx, maxy, maxx;
  
  r = t = 0;
  
  miny = y - 1 >= 0 ? y - 1 : 0;
  maxy = y + 1 <= MAP_Y ? y + 1: MAP_Y;
  minx = x - 1 >= 0 ? x - 1 : 0;
  maxx = x + 1 <= MAP_X ? x + 1: MAP_X;

  for (q = miny; q < maxy; q++) {
    for (p = minx; p < maxx; p++) {
      if (q != y || p != x) {
        if (m->map[q][p] == ter_mountain ||
            m->map[q][p] == ter_boulder) {
          r++;
        } else if (m->map[q][p] == ter_forest ||
                   m->map[q][p] == ter_tree) {
          t++;
        }
      }
    }
  }
  
  if (t == r) {
    return rand() & 1 ? ter_boulder : ter_tree;
  } else if (t > r) {
    if (rand() % 10) {
      return ter_tree;
    } else {
      return ter_boulder;
    }
  } else {
    if (rand() % 10) {
      return ter_boulder;
    } else {
      return ter_tree;
    }
  }
}

static int map_terrain(map_t *m, int8_t n, int8_t s, int8_t e, int8_t w)
{
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  //  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest, num_water, num_total;
  terrain_type_t type;
  int added_current = 0;
  
  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
  num_water = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest + num_water;

  memset(&m->map, 0, sizeof (m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0) {
      type = ter_grass;
    } else if (i == num_grass) {
      type = ter_clearing;
    } else if (i == num_grass + num_clearing) {
      type = ter_mountain;
    } else if (i == num_grass + num_clearing + num_mountain) {
      type = ter_forest;
    } else if (i == num_grass + num_clearing + num_mountain + num_forest) {
      type = ter_water;
    }
    m->map[y][x] = type;
    if (i == 0) {
      head = tail = (queue_node_t *) malloc(sizeof (*tail));
    } else {
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    type = m->map[y][x];
    
    if (x - 1 >= 0 && !m->map[y][x - 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x - 1] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y - 1][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y + 1][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x + 1] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (y == 0 || y == MAP_Y - 1 ||
          x == 0 || x == MAP_X - 1) {
        mapxy(x, y) = border_type(m, x, y);
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  if (n != -1) {
    mapxy(n,         0        ) = ter_gate;
    mapxy(n,         1        ) = ter_path;
  }
  if (s != -1) {
    mapxy(s,         MAP_Y - 1) = ter_gate;
    mapxy(s,         MAP_Y - 2) = ter_path;
  }
  if (w != -1) {
    mapxy(0,         w        ) = ter_gate;
    mapxy(1,         w        ) = ter_path;
  }
  if (e != -1) {
    mapxy(MAP_X - 1, e        ) = ter_gate;
    mapxy(MAP_X - 2, e        ) = ter_path;
  }

  return 0;
}

static int place_boulders(map_t *m)
{
  int i;
  int x, y;

  for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_forest && m->map[y][x] != ter_path) {
      m->map[y][x] = ter_boulder;
    }
  }

  return 0;
}

static int place_trees(map_t *m)
{
  int i;
  int x, y;
  
  for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_mountain &&
        m->map[y][x] != ter_path     &&
        m->map[y][x] != ter_water) {
      m->map[y][x] = ter_tree;
    }
  }

  return 0;
}

void rand_pos(pair_t pos)
{
  pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
  pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;
}

void new_hiker()
{
  pair_t pos;
  npc *c;
  int distance;
  int min;
  int max;
  int x;
  int y;
  int level; 

  do {
    rand_pos(pos);
  } while (world.hiker_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]         ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4            ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_hiker;
  c->mtype = move_hiker;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = 'h';
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;

  x = abs(world.cur_idx[dim_x] - WORLD_SIZE/2);
  y = abs(world.cur_idx[dim_y] - WORLD_SIZE/2);
  distance = x + y;
//  poke pp;
  if(distance <= 200 ) {
    min = 1;
    max = distance/2;
    level = rand() % (max - min) + min; 
  }
  else {
    min = (distance-200)/2;
    max = 100;
    level = rand() % (max - min) + min;
  }

  c->poke_list[0] = pokemon_generator(level);
  c->pokemon_count = 1;
  while(rand() % 100 > 60 && c->pokemon_count < 6) {
    if(distance <= 200 ) {
    min = 1;
    max = distance/2;
    level = rand() % (max - min) + min; 
  }
  else {
    min = (distance-200)/2;
    max = 100;
    level = rand() % (max - min) + min;
  }
    c->poke_list[c->pokemon_count] = pokemon_generator(level);
    c->pokemon_count++;
  }


  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;

  //  printf("Hiker at %d,%d\n", pos[dim_x], pos[dim_y]);
}

void new_rival()
{
  pair_t pos;
  npc *c;
    int distance;
  int min;
  int max;
  int x;
  int y;
  int level; 

  do {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0        ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]         ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4            ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_rival;
  c->mtype = move_rival;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = 'r';
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;

 x = abs(world.cur_idx[dim_x] - WORLD_SIZE/2);
  y = abs(world.cur_idx[dim_y] - WORLD_SIZE/2);
  distance = x + y;
//  poke pp;
  if(distance <= 200 ) {
    min = 1;
    max = distance/2;
    level = rand() % (max - min) + min; 
  }
  else {
    min = (distance-200)/2;
    max = 100;
    level = rand() % (max - min) + min;
  }

  c->poke_list[0] = pokemon_generator(level);
  c->pokemon_count = 1;
  while(rand() % 100 > 60 && c->pokemon_count < 6) {
    if(distance <= 200 ) {
    min = 1;
    max = distance/2;
    level = rand() % (max - min) + min; 
  }
  else {
    min = (distance-200)/2;
    max = 100;
    level = rand() % (max - min) + min;
  }
    c->poke_list[c->pokemon_count] = pokemon_generator(level);
    c->pokemon_count++;
  }


  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void new_swimmer()
{
  pair_t pos;
  npc *c;
  int distance;
  int min;
  int max;
  int x;
  int y;
  int level; 

  do {
    rand_pos(pos);
  } while (world.cur_map->map[pos[dim_y]][pos[dim_x]] != ter_water ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_swimmer;
  c->mtype = move_swim;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = SWIMMER_SYMBOL;
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;

 x = abs(world.cur_idx[dim_x] - WORLD_SIZE/2);
  y = abs(world.cur_idx[dim_y] - WORLD_SIZE/2);
  distance = x + y;
//  poke pp;
  if(distance <= 200 ) {
    min = 1;
    max = distance/2;
    level = rand() % (max - min) + min; 
  }
  else {
    min = (distance-200)/2;
    max = 100;
    level = rand() % (max - min) + min;
  }

  c->poke_list[0] = pokemon_generator(level);
  c->pokemon_count = 1;
  while(rand() % 100 > 60 && c->pokemon_count < 6) {
    if(distance <= 200 ) {
    min = 1;
    max = distance/2;
    level = rand() % (max - min) + min; 
  }
  else {
    min = (distance-200)/2;
    max = 100;
    level = rand() % (max - min) + min;
  }
    c->poke_list[c->pokemon_count] = pokemon_generator(level);
    c->pokemon_count++;
  }


  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void new_char_other()
{
  pair_t pos;
  npc *c;
  int distance;
  int min;
  int max;
  int x;
  int y;
  int level; 

  do {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0        ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]         ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4            ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_other;
  switch (rand() % 4) {
  case 0:
    c->mtype = move_pace;
    c->symbol = PACER_SYMBOL;
    break;
  case 1:
    c->mtype = move_wander;
    c->symbol = WANDERER_SYMBOL;
    break;
  case 2:
    c->mtype = move_sentry;
    c->symbol = SENTRY_SYMBOL;
    break;
  case 3:
    c->mtype = move_explore;
    c->symbol = EXPLORER_SYMBOL;
    break;
  }
  rand_dir(c->dir);
  c->defeated = 0;
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;

 x = abs(world.cur_idx[dim_x] - WORLD_SIZE/2);
  y = abs(world.cur_idx[dim_y] - WORLD_SIZE/2);
  distance = x + y;
//  poke pp;
  if(distance <= 200 ) {
    min = 1;
    max = distance/2;
    level = rand() % (max - min) + min; 
  }
  else {
    min = (distance-200)/2;
    max = 100;
    level = rand() % (max - min) + min;
  }

  c->poke_list[0] = pokemon_generator(level);
  c->pokemon_count = 1;
  while(rand() % 100 > 60 && c->pokemon_count < 6) {
    if(distance <= 200 ) {
    min = 1;
    max = distance/2;
    level = rand() % (max - min) + min; 
  }
  else {
    min = (distance-200)/2;
    max = 100;
    level = rand() % (max - min) + min;
  }
    c->poke_list[c->pokemon_count] = pokemon_generator(level);
    c->pokemon_count++;
  }


  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void place_characters()
{
  world.cur_map->num_trainers = 3;

  //Always place a hiker and a rival, then place a random number of others
  new_hiker();
  new_rival();
  new_swimmer();
  do {
    //higher probability of non- hikers and rivals
    switch(rand() % 10) {
    case 0:
      new_hiker();
      break;
    case 1:
      new_rival();
      break;
    case 2:
      new_swimmer();
      break;
    default:
      new_char_other();
      break;
    }
    /* Game attempts to continue to place trainers until the probability *
     * roll fails, but if the map is full (or almost full), it's         *
     * impossible (or very difficult) to continue to add, so we abort if *
     * we've tried MAX_TRAINER_TRIES times.                              */
  } while (++world.cur_map->num_trainers < MIN_TRAINERS ||
           ((rand() % 100) < ADD_TRAINER_PROB));
}

void init_pc()
{
  int x, y;

  do {
    x = rand() % (MAP_X - 2) + 1;
    y = rand() % (MAP_Y - 2) + 1;
  } while (world.cur_map->map[y][x] != ter_path);

  world.pc.pos[dim_x] = x;
  world.pc.pos[dim_y] = y;
  world.pc.symbol = '@';

  world.cur_map->cmap[y][x] = &world.pc;
  world.pc.next_turn = 0;

  heap_insert(&world.cur_map->turn, &world.pc);
}

void place_pc()
{
  character *c;

  if (world.pc.pos[dim_x] == 1) {
    world.pc.pos[dim_x] = MAP_X - 2;
  } else if (world.pc.pos[dim_x] == MAP_X - 2) {
    world.pc.pos[dim_x] = 1;
  } else if (world.pc.pos[dim_y] == 1) {
    world.pc.pos[dim_y] = MAP_Y - 2;
  } else if (world.pc.pos[dim_y] == MAP_Y - 2) {
    world.pc.pos[dim_y] = 1;
  }

  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;

  if ((c = (character *) heap_peek_min(&world.cur_map->turn))) {
    world.pc.next_turn = c->next_turn;
  } else {
    world.pc.next_turn = 0;
  }
}

// New map expects cur_idx to refer to the index to be generated.  If that
// map has already been generated then the only thing this does is set
// cur_map.
int new_map(int teleport)
{
  int d, p;
  int e, w, n, s;
  int x, y;
  
  if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]]) {
    world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]];
    place_pc();

    return 0;
  }

  world.cur_map                                             =
    world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]] =
    (map_t *) malloc(sizeof (*world.cur_map));

  smooth_height(world.cur_map);
  
  if (!world.cur_idx[dim_y]) {
    n = -1;
  } else if (world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]) {
    n = world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]->s;
  } else {
    n = 3 + rand() % (MAP_X - 6);
  }
  if (world.cur_idx[dim_y] == WORLD_SIZE - 1) {
    s = -1;
  } else if (world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]) {
    s = world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]->n;
  } else  {
    s = 3 + rand() % (MAP_X - 6);
  }
  if (!world.cur_idx[dim_x]) {
    w = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]) {
    w = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]->e;
  } else {
    w = 3 + rand() % (MAP_Y - 6);
  }
  if (world.cur_idx[dim_x] == WORLD_SIZE - 1) {
    e = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]) {
    e = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]->w;
  } else {
    e = 3 + rand() % (MAP_Y - 6);
  }
  
  map_terrain(world.cur_map, n, s, e, w);
     
  place_boulders(world.cur_map);
  place_trees(world.cur_map);
  build_paths(world.cur_map);
  d = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
       abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  p = d > 200 ? 5 : (50 - ((45 * d) / 200));
  //  printf("d=%d, p=%d\n", d, p);
  if ((rand() % 100) < p || !d) {
    place_pokemart(world.cur_map);
  }
  if ((rand() % 100) < p || !d) {
    place_center(world.cur_map);
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      world.cur_map->cmap[y][x] = NULL;
    }
  }

  heap_init(&world.cur_map->turn, cmp_char_turns, delete_character);

  if ((world.cur_idx[dim_x] == WORLD_SIZE / 2) &&
      (world.cur_idx[dim_y] == WORLD_SIZE / 2)) {
    init_pc();
  } else {
    place_pc();
  }

  pathfind(world.cur_map);
  if (teleport) {
    do {
      world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;
      world.pc.pos[dim_x] = rand_range(1, MAP_X - 2);
      world.pc.pos[dim_y] = rand_range(1, MAP_Y - 2);
    } while (world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ||
             (move_cost[char_pc][world.cur_map->map[world.pc.pos[dim_y]]
                                                   [world.pc.pos[dim_x]]] ==
              INT_MAX)                                                      ||
             world.rival_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] < 0);
    world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;
    pathfind(world.cur_map);
  }
  
  place_characters();

  return 0;
}

// The world is global because of its size, so init_world is parameterless
void init_world()
{
  world.quit = 0;
  world.cur_idx[dim_x] = world.cur_idx[dim_y] = WORLD_SIZE / 2;
  world.char_seq_num = 0;
  new_map(0);
}

void delete_world()
{
  int x, y;

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      if (world.world[y][x]) {
        heap_delete(&world.world[y][x]->turn);
        free(world.world[y][x]);
        world.world[y][x] = NULL;
      }
    }
  }
}

void print_hiker_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.hiker_dist[y][x] == INT_MAX) {
        printf("   ");
      } else {
        printf(" %5d", world.hiker_dist[y][x]);
      }
    }
    printf("\n");
  }
}

void print_rival_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.rival_dist[y][x] == INT_MAX || world.rival_dist[y][x] < 0) {
        printf("   ");
      } else {
        printf(" %02d", world.rival_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void leave_map(pair_t d)
{
  if (d[dim_x] == 0) {
    world.cur_idx[dim_x]--;
  } else if (d[dim_y] == 0) {
    world.cur_idx[dim_y]--;
  } else if (d[dim_x] == MAP_X - 1) {
    world.cur_idx[dim_x]++;
  } else {
    world.cur_idx[dim_y]++;
  }
  new_map(0);
}

void pokemon_level_up(poke pp) {
  pp.level = pp.level + 1;

  pp.health = ((((pp.base_hp + pp.iv_hp) * 2) * pp.level)/100) + pp.level + 10;

    pp.attack = ((((pp.base_attack + pp.iv_attack) * 2) * pp.level)/100) + 5;
    pp.defense = ((((pp.base_defense + pp.iv_defense) * 2) * pp.level)/100) + 5;
    pp.special_attack = ((((pp.base_special_attack + pp.iv_special_attack) * 2) * pp.level)/100) + 5;
    pp.special_defense = ((((pp.base_special_defense + pp.iv_special_defense) * 2) * pp.level)/100) + 5;
    pp.speed = ((((pp.base_speed + pp.iv_speed) * 2) * pp.level)/100) + 5;
}

poke pokemon_generator(int level) {
  poke pp;
  pp.level = level;
  pp.gender = rand() % 2;
  pp.shiny = rand() % 8192;

  pp.pokemon_index = rand() % 1090;
  pp.name = pokemon[pp.pokemon_index]->identifier;
  pp.pokemon_id = pokemon[pp.pokemon_index]->id;

  pp.iv_hp = rand() % 16;
  pp.iv_attack = rand() % 16;
  pp.iv_defense = rand() & 16;
  pp.iv_speed = rand() & 16;
  pp.iv_special_attack = rand() & 16;
  pp.iv_special_defense = rand() & 16;

  int i = 0;
  while(pokemon_stats[i]->pokemon_id != pp.pokemon_id) {
    i++;
    }

    pp.base_hp = pokemon_stats[i]->base_stat;
    pp.base_attack = pokemon_stats[i + 1]->base_stat;
    pp.base_defense = pokemon_stats[i + 2]->base_stat;
    pp.base_special_attack = pokemon_stats[i + 3]->base_stat;
    pp.base_special_defense = pokemon_stats[i + 4]->base_stat;
    pp.base_speed = pokemon_stats[i + 5]->base_stat;

    pp.health = ((((pp.base_hp + pp.iv_hp) * 2) * pp.level)/100) + pp.level + 10;
    pp.currentHP = pp.health;

    pp.attack = ((((pp.base_attack + pp.iv_attack) * 2) * pp.level)/100) + 5;
    pp.defense = ((((pp.base_defense + pp.iv_defense) * 2) * pp.level)/100) + 5;
    pp.special_attack = ((((pp.base_special_attack + pp.iv_special_attack) * 2) * pp.level)/100) + 5;
    pp.special_defense = ((((pp.base_special_defense + pp.iv_special_defense) * 2) * pp.level)/100) + 5;
    pp.speed = ((((pp.base_speed + pp.iv_speed) * 2) * pp.level)/100) + 5;


    i = 0;
    int flag = 0;
    int count = 0;
    pp.move_count = 0;
    while(1) {
      while(pokemon_moves[i]->pokemon_id == pp.pokemon_id) {
        if(pokemon_moves[i]->pokemon_move_method_id == 1 && pokemon_moves[i]->level <= pp.level) {
          if(count == 0) {
            pp.move1 = moves[pokemon_moves[i]->move_id - 1]->identifier;
            pp.moveId1 = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[0].move = moves[pokemon_moves[i]->move_id - 1]->identifier;
            pp.pm[0].moveId = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[0].priority = moves[pokemon_moves[i]->move_id - 1]->priority;
            pp.pm[0].accuracy = moves[pokemon_moves[i]->move_id - 1]->accuracy;
            pp.pm[0].power = moves[pokemon_moves[i]->move_id - 1]->power;
            }
          if(count == 1) {
            pp.move2 = moves[pokemon_moves[i]->move_id - 1]->identifier;
            pp.moveId2 = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[1].move = moves[pokemon_moves[i]->move_id - 1]->identifier;
            pp.pm[1].moveId = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[1].priority = moves[pokemon_moves[i]->move_id - 1]->priority;
            pp.pm[1].accuracy = moves[pokemon_moves[i]->move_id - 1]->accuracy;
            pp.pm[1].power = moves[pokemon_moves[i]->move_id - 1]->power;
            } 
          if(count == 2) {
            pp.move3 = moves[pokemon_moves[i]->move_id - 1]->identifier;
            pp.moveId3 = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[2].move = moves[pokemon_moves[i]->move_id - 1]->identifier;
            pp.pm[2].moveId = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[2].priority = moves[pokemon_moves[i]->move_id - 1]->priority;
            pp.pm[2].accuracy = moves[pokemon_moves[i]->move_id - 1]->accuracy;
            pp.pm[2].power = moves[pokemon_moves[i]->move_id - 1]->power;
            } 
          if(count == 3) {
            pp.move4 = moves[pokemon_moves[i]->move_id - 1]->identifier;
            pp.moveId4 = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[3].move = moves[pokemon_moves[i]->move_id - 1]->identifier;
            pp.pm[3].moveId = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[3].priority = moves[pokemon_moves[i]->move_id - 1]->priority;
            pp.pm[3].accuracy = moves[pokemon_moves[i]->move_id - 1]->accuracy;
            pp.pm[3].power = moves[pokemon_moves[i]->move_id - 1]->power;
            } 
               
          flag = 1;
          count = count + 1 % 4;
          pp.move_count++;
          }
          i++;
          }
        i++;
    if(flag == 1) {
    break;
    }
    }

    return pp;
}


void game_loop()
{
  character *c;
  npc *n;
  pc *p;
  pair_t d;
  int x;
  int y;
  int distance;
  int min;
  int max;
  
  while (!world.quit) {
    c = (character *) heap_remove_min(&world.cur_map->turn);
    n = dynamic_cast<npc *> (c);
    p = dynamic_cast<pc *> (c);

    move_func[n ? n->mtype : move_pc](c, d);

    world.cur_map->cmap[c->pos[dim_y]][c->pos[dim_x]] = NULL;
    if (p && (d[dim_x] == 0 || d[dim_x] == MAP_X - 1 ||
              d[dim_y] == 0 || d[dim_y] == MAP_Y - 1)) {
      leave_map(d);
      d[dim_x] = c->pos[dim_x];
      d[dim_y] = c->pos[dim_y];
    }
    world.cur_map->cmap[d[dim_y]][d[dim_x]] = c;

    if(p && world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]] == ter_grass) {
      int k = rand() % 100;
      if(k >= 90) {
      x = abs(world.cur_idx[dim_x] - WORLD_SIZE/2);
      y = abs(world.cur_idx[dim_y] - WORLD_SIZE/2);
      distance = x + y;
      poke pp;
        if(distance <= 200 ) {
          min = 1;
          max = distance/2;
          pp.level = rand() % (max - min) + min; 
        }
        else {
          min = (distance-200)/2;
          max = 100;
          pp.level = rand() % (max - min) + min;
        }

          pp.pokemon_index = rand() % 1090;
          pp.name = pokemon[pp.pokemon_index]->identifier;
          pp.pokemon_id = pokemon[pp.pokemon_index]->id;

          pp.iv_hp = rand() % 16;
          pp.iv_attack = rand() % 16;
          pp.iv_defense = rand() & 16;
          pp.iv_speed = rand() & 16;
          pp.iv_special_attack = rand() & 16;
          pp.iv_special_defense = rand() & 16;

          int i = 0;
          while(pokemon_stats[i]->pokemon_id != pp.pokemon_id) {
            i++;
          }

          pp.base_hp = pokemon_stats[i]->base_stat;
          pp.base_attack = pokemon_stats[i + 1]->base_stat;
          pp.base_defense = pokemon_stats[i + 2]->base_stat;
          pp.base_special_attack = pokemon_stats[i + 3]->base_stat;
          pp.base_special_defense = pokemon_stats[i + 4]->base_stat;
          pp.base_speed = pokemon_stats[i + 5]->base_stat;

          pp.health = ((((pp.base_hp + pp.iv_hp) * 2) * pp.level)/100) + pp.level + 10;
          pp.currentHP = pp.health;

          pp.attack = ((((pp.base_attack + pp.iv_attack) * 2) * pp.level)/100) + 5;
          pp.defense = ((((pp.base_defense + pp.iv_defense) * 2) * pp.level)/100) + 5;
          pp.special_attack = ((((pp.base_special_attack + pp.iv_special_attack) * 2) * pp.level)/100) + 5;
          pp.special_defense = ((((pp.base_special_defense + pp.iv_special_defense) * 2) * pp.level)/100) + 5;
          pp.speed = ((((pp.base_speed + pp.iv_speed) * 2) * pp.level)/100) + 5;


          i = 0;
          int flag = 0;
          int count = 0;
          pp.move_count = 0;
          while(1) {
            while(pokemon_moves[i]->pokemon_id == pp.pokemon_id) {
              if(pokemon_moves[i]->pokemon_move_method_id == 1 && pokemon_moves[i]->level <= pp.level) {
                if(count == 0) {
                pp.move1 = moves[pokemon_moves[i]->move_id - 1]->identifier;
                pp.moveId1 = moves[pokemon_moves[i]->move_id - 1]->id;
                pp.pm[0].move = moves[pokemon_moves[i]->move_id - 1]->identifier;
                pp.pm[0].moveId = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[0].priority = moves[pokemon_moves[i]->move_id - 1]->priority;
            pp.pm[0].accuracy = moves[pokemon_moves[i]->move_id - 1]->accuracy;
            pp.pm[0].power = moves[pokemon_moves[i]->move_id - 1]->power;  
                
                }
                if(count == 1) {
                  pp.move2 = moves[pokemon_moves[i]->move_id - 1]->identifier;
                  pp.moveId2 = moves[pokemon_moves[i]->move_id - 1]->id;
                  pp.pm[1].move = moves[pokemon_moves[i]->move_id - 1]->identifier;
                  pp.pm[1].moveId = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[1].priority = moves[pokemon_moves[i]->move_id - 1]->priority;
            pp.pm[1].accuracy = moves[pokemon_moves[i]->move_id - 1]->accuracy;
            pp.pm[1].power = moves[pokemon_moves[i]->move_id - 1]->power;
                } 
                if(count == 2) {
                  pp.move3 = moves[pokemon_moves[i]->move_id - 1]->identifier;
                  pp.moveId3 = moves[pokemon_moves[i]->move_id - 1]->id;
                  pp.pm[2].move = moves[pokemon_moves[i]->move_id - 1]->identifier;
                  pp.pm[2].moveId = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[2].priority = moves[pokemon_moves[i]->move_id - 1]->priority;
            pp.pm[2].accuracy = moves[pokemon_moves[i]->move_id - 1]->accuracy;
            pp.pm[2].power = moves[pokemon_moves[i]->move_id - 1]->power;
                } 
                if(count == 3) {
                  pp.move4 = moves[pokemon_moves[i]->move_id - 1]->identifier;
                  pp.moveId4 = moves[pokemon_moves[i]->move_id - 1]->id;
                  pp.pm[3].move = moves[pokemon_moves[i]->move_id - 1]->identifier;
                  pp.pm[3].moveId = moves[pokemon_moves[i]->move_id - 1]->id;
            pp.pm[3].priority = moves[pokemon_moves[i]->move_id - 1]->priority;
            pp.pm[3].accuracy = moves[pokemon_moves[i]->move_id - 1]->accuracy;
            pp.pm[3].power = moves[pokemon_moves[i]->move_id - 1]->power;
                } 
                
                flag = 1;
                count = count + 1 % 4;
                pp.move_count++;
              }
              i++;
            }
              i++;
            if(flag == 1) {
              pokemon_battle(p, &pp);
              break;
            }
          }
      }



      

    }

    if (p) {
      pathfind(world.cur_map);
    }

    c->next_turn += move_cost[n ? n->ctype : char_pc]
                             [world.cur_map->map[d[dim_y]][d[dim_x]]];

    c->pos[dim_y] = d[dim_y];
    c->pos[dim_x] = d[dim_x];

    heap_insert(&world.cur_map->turn, c);
  }
}

void usage(char *s)
{
  fprintf(stderr, "Usage: %s [-s|--seed <seed>]\n", s);

  exit(1);
}

void print_pokemon() {

  for(int i = 0; i < (int) pokemon.size() - 1; i++) {
    if(pokemon[i]->id != INT_MAX) {
      printf("id: %d \n", pokemon[i]->id);
    }

      printf("identifier: %s \n", pokemon[i]->identifier);

    if(pokemon[i]->species_id != INT_MAX) {
      printf("species id: %d \n", pokemon[i]->species_id);
    }

    if(pokemon[i]->height != INT_MAX) {
      printf("height: %d \n", pokemon[i]->height);
    }

    if(pokemon[i]->weight != INT_MAX) {
      printf("weight: %d \n", pokemon[i]->weight);
    }

    if(pokemon[i]->base_experience != INT_MAX) {
      printf("base_experience: %d \n", pokemon[i]->base_experience);
    }

    if(pokemon[i]->order != INT_MAX) {
      printf("order: %d \n", pokemon[i]->order);
    }

    if(pokemon[i]->is_default != INT_MAX) {
      printf("is_default: %d \n", pokemon[i]->is_default);
    }

    printf("\n");
  }

  
}

void print_moves() {
  for(int i = 0; i < (int) moves.size() - 1; i++) {
    if(moves[i]->id != INT_MAX) {
      printf("id: %d \n", moves[i]->id);
    }
      printf("identifier: %s \n", moves[i]->identifier);

    if(moves[i]->generation_id != INT_MAX) {
      printf("generation_id: %d \n", moves[i]->generation_id);
    }

    if(moves[i]->type_id != INT_MAX) {
      printf("type_id: %d \n", moves[i]->type_id);
    }

    if(moves[i]->power != INT_MAX) {
      printf("power: %d \n", moves[i]->power);
    }

    if(moves[i]->pp != INT_MAX) {
      printf("pp: %d \n", moves[i]->pp);
    }

    if(moves[i]->accuracy != INT_MAX) {
      printf("accuracy: %d \n", moves[i]->accuracy);
    }

    if(moves[i]->priority != INT_MAX) {
      printf("priority: %d \n", moves[i]->priority);
    }

    if(moves[i]->target_id != INT_MAX) {
      printf("target_id: %d \n", moves[i]->target_id);
    }

    if(moves[i]->damage_c != INT_MAX) {
      printf("damage_c: %d \n", moves[i]->damage_c);
    }

    if(moves[i]->effect_id != INT_MAX) {
      printf("effect_id: %d \n", moves[i]->effect_id);
    }

    if(moves[i]->effect_chance != INT_MAX) {
      printf("effect_chance: %d \n", moves[i]->effect_chance);
    }

    if(moves[i]->contest_type_id != INT_MAX) {
      printf("contest_type_id: %d \n", moves[i]->contest_type_id);
    }

    if(moves[i]->contest_effect_id != INT_MAX) {
      printf("contest_effect_id: %d \n", moves[i]->contest_effect_id);
    }

    if(moves[i]->super_contest_effect_id != INT_MAX) {
      printf("super_contest_effect_id: %d \n", moves[i]->super_contest_effect_id);
    }

    printf("\n");
  }
}

void print_pokemon_moves() {
  for(int i = 0; i < (int) pokemon_moves.size() - 1; i++) { 
      if(pokemon_moves[i]->pokemon_id != INT_MAX) {
      printf("pokemon_id: %d \n", pokemon_moves[i]->pokemon_id);
    }

    if(pokemon_moves[i]->version_group_id != INT_MAX) {
      printf("version_group_id: %d \n", pokemon_moves[i]->version_group_id);
    }

    if(pokemon_moves[i]->move_id != INT_MAX) {
      printf("move_id: %d \n", pokemon_moves[i]->move_id);
    }

    if(pokemon_moves[i]->pokemon_move_method_id != INT_MAX) {
      printf("pokemon_move_method_id: %d \n", pokemon_moves[i]->pokemon_move_method_id);
    }

    if(pokemon_moves[i]->level != INT_MAX) {
      printf("level: %d \n", pokemon_moves[i]->level);
    }

    if(pokemon_moves[i]->order != INT_MAX) {
      printf("order: %d \n", pokemon_moves[i]->order);
    }

    printf("\n");
  }
}

void print_pokemon_species() {
  for(int i = 0; i < (int) pokemon_species.size() - 1; i++) { 
    if(pokemon_species[i]->id != INT_MAX) {
      printf("id: %d \n", pokemon_species[i]->id);
    }
      printf("identifier: %s \n", pokemon_species[i]->identifier);

    if(pokemon_species[i]->generation_id != INT_MAX) {
      printf("generation_id: %d \n", pokemon_species[i]->generation_id);
    }

    if(pokemon_species[i]->evolves_from_species_id != INT_MAX) {
      printf("evolves_from_species_id: %d \n", pokemon_species[i]->evolves_from_species_id);
    }

    if(pokemon_species[i]->evolution_chain_id != INT_MAX) {
      printf("evolution_chain_id: %d \n", pokemon_species[i]->evolution_chain_id);
    }

    if(pokemon_species[i]->color_id != INT_MAX) {
      printf("color_id: %d \n", pokemon_species[i]->color_id);
    }

    if(pokemon_species[i]->shape_id != INT_MAX) {
      printf("shape_id: %d \n", pokemon_species[i]->shape_id);
    }

    if(pokemon_species[i]->habitat_id != INT_MAX) {
      printf("habitat_id: %d \n", pokemon_species[i]->habitat_id);
    }

    if(pokemon_species[i]->gender_rate != INT_MAX) {
      printf("gender_rate: %d \n", pokemon_species[i]->gender_rate);
    }

    if(pokemon_species[i]->capture_rate != INT_MAX) {
      printf("capture_rate: %d \n", pokemon_species[i]->capture_rate);
    }

    if(pokemon_species[i]->base_happiness != INT_MAX) {
      printf("base_happiness: %d \n", pokemon_species[i]->base_happiness);
    }

    if(pokemon_species[i]->is_baby != INT_MAX) {
      printf("is_baby: %d \n", pokemon_species[i]->is_baby);
    }

    if(pokemon_species[i]->hatch_counter != INT_MAX) {
      printf("hatch_counter: %d \n", pokemon_species[i]->hatch_counter);
    }

    if(pokemon_species[i]->has_gender_differences != INT_MAX) {
      printf("has_gender_differences: %d \n", pokemon_species[i]->has_gender_differences);
    }

    if(pokemon_species[i]->growth_rate_id != INT_MAX) {
      printf("growth_rate_id: %d \n", pokemon_species[i]->growth_rate_id);
    }

    if(pokemon_species[i]->forms_switchable != INT_MAX) {
      printf("forms_switchable: %d \n", pokemon_species[i]->forms_switchable);
    }

    if(pokemon_species[i]->is_legendary != INT_MAX) {
      printf("is_legendary: %d \n", pokemon_species[i]->is_legendary);
    }

    if(pokemon_species[i]->is_mythical != INT_MAX) {
      printf("is_mythical: %d \n", pokemon_species[i]->is_mythical);
    }

    if(pokemon_species[i]->order != INT_MAX) {
      printf("order: %d \n", pokemon_species[i]->order);
    }

    if(pokemon_species[i]->conquest_order != INT_MAX) {
      printf("conquest_order: %d \n", pokemon_species[i]->conquest_order);
    }

    printf("\n");
  }
}

void print_experience() {
  for(int i = 0; i < (int) experience.size() - 1; i++) {
    if(experience[i]->growth_rate_id != INT_MAX) {
      printf("growth_rate_id: %d \n", experience[i]->growth_rate_id);
    }

    if(experience[i]->level != INT_MAX) {
      printf("level: %d \n", experience[i]->level);
    }

    if(experience[i]->growth_rate_id != INT_MAX) {
      printf("experience: %d \n", experience[i]->experience);
    }

    printf("\n");
  }

}

void print_type_names() {
  for(int i = 0; i < (int) type_names.size() - 1; i++) {
    if(type_names[i]->type_id != INT_MAX) {
      printf("type_id: %d \n", type_names[i]->type_id);
    }

    if(type_names[i]->local_lang_id != INT_MAX) {
      printf("local_lang_id: %d \n", type_names[i]->local_lang_id);
    }

      printf("name: %s \n", type_names[i]->name);

    printf("\n");
  }
}

void print_pokemon_stats() {
  for(int i = 0; i < (int) pokemon_stats.size() - 1; i++) {
    if(pokemon_stats[i]->pokemon_id != INT_MAX) {
      printf("pokemon_id: %d \n", pokemon_stats[i]->pokemon_id);
    }

    if(pokemon_stats[i]->stat_id != INT_MAX) {
      printf("stat_id: %d \n", pokemon_stats[i]->stat_id);
    }

    if(pokemon_stats[i]->base_stat != INT_MAX) {
      printf("base_stat: %d \n", pokemon_stats[i]->base_stat);
    }

    if(pokemon_stats[i]->effort != INT_MAX) {
      printf("effort: %d \n", pokemon_stats[i]->effort);
    }

    printf("\n");
  }
}

void print_stats() {
  for(int i = 0; i < (int) stats.size() - 1; i++) {
    if(stats[i]->id != INT_MAX) {
      printf("id: %d \n", stats[i]->id);
    }

    if(stats[i]->damage_c != INT_MAX) {
      printf("damage_c: %d \n", stats[i]->damage_c);
    }

    printf("identifier: %s \n", stats[i]->identifier);

    if(stats[i]->is_battle_only != INT_MAX) {
      printf("is_battle_only: %d \n", stats[i]->is_battle_only);
    }

    if(stats[i]->game_index != INT_MAX) {
      printf("game_index: %d \n", stats[i]->game_index);
    }

    printf("\n");
  }
}

void print_pokemon_types() {
  for(int i = 0; i < (int) pokemon_types.size() - 1; i++) {
    if(pokemon_types[i]->pokemon_id != INT_MAX) {
      printf("pokemon_id: %d \n", pokemon_types[i]->pokemon_id);
    }

    if(pokemon_types[i]->type_id != INT_MAX) {
      printf("type_id: %d \n", pokemon_types[i]->type_id);
    }

    if(pokemon_types[i]->slot != INT_MAX) {
      printf("slot: %d \n", pokemon_types[i]->slot);
    }

    printf("\n");
  }
}



int main(int argc, char *argv[])
{  

  struct timeval tv;
  uint32_t seed;
 int long_arg;
  int do_seed;
  //  char c;
  //  int x, y;
  int i;

  do_seed = 1;
  
  if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-seed")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%u", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

// if(argc == 2) {
//   if(!strcmp(argv[1], "pokemon")) {
//        pokemon_parser(&pokemon);
//    print_pokemon();
//   }
//   else if(!strcmp(argv[1], "moves")) {
//        moves_parser(&moves);
//         print_moves();
//   }
//   else if(!strcmp(argv[1], "pokemon_moves")) {
//   pokemon_moves_parser(&pokemon_moves);
//   print_pokemon_moves();
//   }
//   else if(!strcmp(argv[1], "pokemon_species")) {
//  pokemon_species_parser(&pokemon_species);
//  print_pokemon_species();
//   }
//   else if(!strcmp(argv[1], "experience")) {
//  experience_parser(&experience);
//  print_experience();
//   }
//   else if(!strcmp(argv[1], "type_names")) {
//  type_names_parser(&type_names);
//  print_type_names();
//   }
//   else if(!strcmp(argv[1], "pokemon_stats")) {
//  pokemon_stats_parser(&pokemon_stats);
//  print_pokemon_stats();
//   }
//   else if(!strcmp(argv[1], "stats")) {
//  stats_parser(&stats);
//  print_stats();
//   }
//   else if(!strcmp(argv[1], "pokemon_types")) {
//  pokemon_types_parser(&pokemon_types);
//  print_pokemon_types();
//   }

// }

pokemon_parser(&pokemon);
moves_parser(&moves);
pokemon_moves_parser(&pokemon_moves);
pokemon_species_parser(&pokemon_species);
experience_parser(&experience);
type_names_parser(&type_names);
pokemon_stats_parser(&pokemon_stats);
stats_parser(&stats);
pokemon_types_parser(&pokemon_types);



  if (do_seed) {
    /* Allows me to start the game more than once *
     * per second, as opposed to time().          */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Using seed: %u\n", seed);
  srand(seed);


  
 init_world();

poke starter1 = pokemon_generator(1);
poke starter2 = pokemon_generator(2);
poke starter3 = pokemon_generator(3);
printf("Do you want %s(1), %s(2), or %s(3) \n", starter1.name, starter2.name, starter3.name);
int choice;
scanf("%d", &choice);
if(choice == 1) {
  world.pc.poke_list[0] = starter1;
}
else if(choice == 2) {
  world.pc.poke_list[0] = starter2;
} 
else {
  world.pc.poke_list[0] = starter3;
}
world.pc.pokemon_count = 1;
world.pc.revive = 2;
world.pc.potion = 2;
world.pc.pokeball = 10;

io_init_terminal();
  /* print_hiker_dist(); */
  
  /*
  do {
    print_map();  
    printf("Current position is %d%cx%d%c (%d,%d).  "
           "Enter command: ",
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S',
           world.cur_idx[dim_x] - (WORLD_SIZE / 2),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2));
    scanf(" %c", &c);
    switch (c) {
    case 'n':
      if (world.cur_idx[dim_y]) {
        world.cur_idx[dim_y]--;
        new_map();
      }
      break;
    case 's':
      if (world.cur_idx[dim_y] < WORLD_SIZE - 1) {
        world.cur_idx[dim_y]++;
        new_map();
      }
      break;
    case 'e':
      if (world.cur_idx[dim_x] < WORLD_SIZE - 1) {
        world.cur_idx[dim_x]++;
        new_map();
      }
      break;
    case 'w':
      if (world.cur_idx[dim_x]) {
        world.cur_idx[dim_x]--;
        new_map();
      }
      break;
     case 'q':
      break;
    case 'f':
      scanf(" %d %d", &x, &y);
      if (x >= -(WORLD_SIZE / 2) && x <= WORLD_SIZE / 2 &&
          y >= -(WORLD_SIZE / 2) && y <= WORLD_SIZE / 2) {
        world.cur_idx[dim_x] = x + (WORLD_SIZE / 2);
        world.cur_idx[dim_y] = y + (WORLD_SIZE / 2);
        new_map();
      }
      break;
    case '?':
    case 'h':
      printf("Move with 'e'ast, 'w'est, 'n'orth, 's'outh or 'f'ly x y.\n"
             "Quit with 'q'.  '?' and 'h' print this help message.\n");
      break;
    default:
      fprintf(stderr, "%c: Invalid input.  Enter '?' for help.\n", c);
      break;
    }
  } while (c != 'q');

  */

  game_loop();
  
  delete_world();

  io_reset_terminal();


  return 0;
}
