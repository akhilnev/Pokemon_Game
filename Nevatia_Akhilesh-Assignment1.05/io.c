#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "io.h"
#include "character.h"
#include "poke327.h"
#define COLOR_BROWN 9 

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();

init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
init_pair(COLOR_BROWN, COLOR_YELLOW, COLOR_MAGENTA); // Brown color pair


}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}


static int compare_trainer_distance(const void *v1, const void *v2)
{
  const character_t *const *c1 = v1;
  const character_t *const *c2 = v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static character_t *io_nearest_visible_trainer()
{
  character_t **c, *n;
  uint32_t x, y, count;

  c = malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{

uint32_t y, x;
    character_t *c;

    clear();
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            if (world.cur_map->cmap[y][x]) {
                mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
            } else {
                switch (world.cur_map->map[y][x]) {
                case ter_boulder:
                    attron(COLOR_PAIR(COLOR_MAGENTA));
                    mvaddch(y + 1, x, BOULDER_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_MAGENTA));
                    break;

               case ter_mountain:
                    attron(COLOR_PAIR(COLOR_WHITE)); // Use predefined white color pair
                    mvaddch(y + 1, x, MOUNTAIN_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_WHITE));
                    break;
             case ter_tree:
                    attron(COLOR_PAIR(COLOR_BROWN)); // Brown for trees
                    mvaddch(y + 1, x, TREE_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_BROWN));
                    break;
                // case ter_forest:
                //     attron(COLOR_PAIR(COLOR_GREEN));
                //     mvaddch(y + 1, x, FOREST_SYMBOL);
                //     attroff(COLOR_PAIR(COLOR_GREEN));
                //     break;
                case ter_forest:
                  attron(COLOR_PAIR(COLOR_BROWN));  // Set the color to brown
                  mvaddch(y + 1, x, FOREST_SYMBOL);
                  attroff(COLOR_PAIR(COLOR_BROWN));  // Reset the color
                  break;
                case ter_path:
                    attron(COLOR_PAIR(COLOR_YELLOW));
                    mvaddch(y + 1, x, PATH_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_YELLOW));
                    break;
                case ter_gate:
                    attron(COLOR_PAIR(COLOR_YELLOW));
                    mvaddch(y + 1, x, GATE_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_YELLOW));
                    break;
                case ter_mart:
                    attron(COLOR_PAIR(COLOR_BLUE));
                    mvaddch(y + 1, x, POKEMART_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_BLUE));
                    break;
                case ter_center:
                    attron(COLOR_PAIR(COLOR_RED));
                    mvaddch(y + 1, x, POKEMON_CENTER_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_RED));
                    break;
                case ter_grass:
                    attron(COLOR_PAIR(COLOR_GREEN));
                    mvaddch(y + 1, x, TALL_GRASS_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_GREEN));
                    break;
                case ter_clearing:
                    attron(COLOR_PAIR(COLOR_GREEN));
                    mvaddch(y + 1, x, SHORT_GRASS_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_GREEN));
                    break;
                case ter_water:
                    attron(COLOR_PAIR(COLOR_CYAN));
                    mvaddch(y + 1, x, WATER_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_CYAN));
                    break;
                default:
                    /* Use zero as an error symbol, since it stands out somewhat, and it's *
                     * not otherwise used.                                                 */
                    attron(COLOR_PAIR(COLOR_CYAN));
                    mvaddch(y + 1, x, ERROR_SYMBOL);
                    attroff(COLOR_PAIR(COLOR_CYAN));
                }
            }
        }
    }

  mvprintw(23, 1, "PC position: (%2d,%2d) on the world map: %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Closest Visible Trianer: ");
  if ((c = io_nearest_visible_trainer())) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at vector %d%cx%d%c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}


// Removed TELECPORT HERE 

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]                  ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] == INT_MAX ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[40], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

/**
 * Displays a list of trainers with their character type, symbol, and position relative to the player character.
 * If the count of trainers is less than or equal to 13, the list is displayed without scrolling.
 * If the count of trainers is greater than 13, the list is displayed with scrolling.
 * 
 * @param c An array of pointers to character_t structs representing the trainers.
 * @param count The number of trainers in the array.
 */
static void io_list_trainers_display(character_t **c, uint32_t count)
{
  uint32_t i;
  char (*s)[40]; /* pointer to array of 40 char */

  s = malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->npc->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "West" : "East"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "press esc to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  character_t **c;
  uint32_t x, y, count;

  c = malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display(c, count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  mvprintw(0, 0, "Hello you have entered the Pokemart! Need to stock up on a few PokeBalls? ");
  refresh();
  getch();
}

void io_pokemon_center()
{
  mvprintw(0, 0, "Hello You have entered the PokeCenter! Ready to train? ");
  refresh();
  getch();
}

void io_battle(character_t *aggressor, character_t *defender)
{
  character_t *npc;

  io_display();
  mvprintw(0, 0, "Pokemon Battle is triggered! Loading soon............");
  refresh();
  getch();
  if (aggressor->pc) {
    npc = defender;
  } else {
    npc = aggressor;
  }

  npc->npc->defeated = 1;
  if (npc->npc->ctype == char_hiker || npc->npc->ctype == char_rival) {
    npc->npc->mtype = move_wander;
  }
}

uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input) {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input) {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart) {
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      io_pokemon_center();
    }
    break;
  }

  if (world.cur_map->map[dest[dim_y]][dest[dim_x]] == ter_gate) {
    /* Can't leave the map */  
    return 1;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]->npc &&
        world.cur_map->cmap[dest[dim_y]][dest[dim_x]]->npc->defeated) {
      // Some kind of greeting here would be nice
      return 1;
    } else if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]->npc) {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }
  
  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      INT_MAX) {
    return 1;
  }

  return 0;
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do {
    switch (key = getch()) {
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'p':
      /* Teleport the PC to a random place in the map.*/
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;

    default:
      mvprintw(0, 0, "Undefined Key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}
