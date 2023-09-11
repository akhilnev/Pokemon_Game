#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* Do not modify write_pgm() or read_pgm() */
int write_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *o;

  if (!(o = fopen(file, "w"))) {
    perror(file);

    return -1;
  }

  fprintf(o, "P5\n%u %u\n255\n", x, y);

  /* Assume input data is correctly formatted. *
   * There's no way to handle it, otherwise.   */

  if (fwrite(image, 1, x * y, o) != (x * y)) {
    perror("fwrite");
    fclose(o);

    return -1;
  }

  fclose(o);

  return 0;
}

/* A better implementation of this function would read the image dimensions *
 * from the input and allocate the storage, setting x and y so that the     *
 * user can determine the size of the file at runtime.  In order to         *
 * minimize complication, I've written this version to require the user to  *
 * know the size of the image in advance.                                   */
int read_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *f;
  char s[80];
  unsigned i, j;

  if (!(f = fopen(file, "r"))) {
    perror(file);

    return -1;
  }

  if (!fgets(s, 80, f) || strncmp(s, "P5", 2)) {
    fprintf(stderr, "Expected P6\n");

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (sscanf(s, "%u %u", &i, &j) != 2 || i != x || j != y) {
    fprintf(stderr, "Expected x and y dimensions %u %u\n", x, y);
    fclose(f);

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (strncmp(s, "255", 3)) {
    fprintf(stderr, "Expected 255\n");
    fclose(f);

    return -1;
  }

  if (fread(image, 1, x * y, f) != x * y) {
    perror("fread");
    fclose(f);

    return -1;
  }

  fclose(f);

  return 0;
}

int main(int argc, char *argv[])
{
  int8_t image[1024][1024]; // M 
  int8_t out[1024][1024]; // M'
  // what is Kernel matrix

  
  /* Example usage of PGM functions */
  /* This assumes that motorcycle.pgm is a pgm image of size 1024x1024 */
  read_pgm("motorcycle.pgm", image, 1024, 1024);
// Useful class notes 
// REQUIRED TO HAVE A MAKE FILE ( COORDINATES BUILD BY HANDLING GCC AND EXECUTION , NO NEED TO WRITE GCC)
// inside make file add all switches to overcome constant need of writing gcc 
// Make file is useful for big projects mainly, only rebuilds file necessary and diregaards other files speeding up the process
//image array = M
// int8_t - similar to char , more readable 
// ((int) image[x][y] ) * 2 as data format is not right we need to cast it in loop
// out = abs(accum) > 255 ? 255 : accum // for values greater than 255 or negative 255 ( ternary will not work directly check for overflow for negative too)
// if(accum>255){
//     out = 255
// }else if(accum<-255){
//     out = -255
// }else{
//     out = accum
// }

// The two kernels for x direction and y direction convulution 
int Ox[3][3] = {
    {-1,0,1},{-2,0,2},{-1,0,1}
    };;
int Oy[3][3] = {
{-1,-2,-1}, {0,0,0},{+1,+2,+1}
};
int n = 3;
// Horizontal convulution


 int8_t Cx[1024][1024]; // M 
 int8_t Cy[1024][1024]; // M 

for (int r = 0; r < 1024; r++) {
    for (int c = 0; c < 1024; c++) {
        int accumulator = 0;
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 3; i++) {
                accumulator += Ox[j][i] * ((int) image[r + (j - (n/2))][c + (i - (n/2))]);
            }
        }
        Cx[r][c]= accumulator;
        
    }
}

for (int r = 0; r < 1024; r++) {
    for (int c = 0; c < 1024; c++) {
        int accumulator = 0;
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 3; i++) {
                accumulator += Oy[j][i] * ((int) image[r + (j - (n/2))][c + (i - (n/2))]);
            }
        }
        Cy[r][c]= accumulator;
        
    }
}

for(int row = 0 ; row < 1024 ; row++){
    for(int col = 0 ; col < 1024 ; col++){
        out[row][col] = sqrt((Cx[row][col]*Cx[row][col])+(Cy[row][col]*Cy[row][col]));
    }
}

  /* After processing the image and storing your output in "out", write *
   * to motorcycle.edge.pgm.                                            */
  write_pgm("motorcycle.edge.pgm", out, 1024, 1024);
  
  return 0;
}