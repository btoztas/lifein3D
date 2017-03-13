#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _world{
  // define data structers
} world;

// function usage
void usage(){
  printf("Life 3D\t\tPARALLEL AND DISTRIBUTED COMPUTING\t\tTecnico Lisboa\n\n");
  printf("usage:\t life3d <input.in> <number of iterations>\n\n\n");
}

// open file
FILE *open_file(char *file_name){
   FILE *fp = fopen(file_name,"r"); // read mode
   if(fp == NULL){
      printf("Error while opening file %s.\n", file_name);
      exit(EXIT_FAILURE);
   }
   return fp;
}

// create initial world from input file
world *createWorld(FILE *file){
  int size;
  int x, y, z;

  fscanf(file, "%d", &size);
  printf("World Size: %d\n", size);

  while(fscanf(file, "%d %d %d", &x, &y, &z) != EOF){

    // handle coordinates read
    printf("%d %d %d\n", x, y, z);

  }
  return NULL;
}

int main(int argc, char* argv[]){

  // if argc not expected, print program usage
  if(argc!=3){
    usage();
    exit(EXIT_FAILURE);
  }

  // handle file_name
  char *file_name = (char*)malloc(sizeof(char)*strlen(argv[1]));
  strcpy(file_name, argv[1]);

  // read file
  FILE *file;
  file = open_file(file_name);
  world *game = createWorld(file);

  fclose(file);
  free(file_name);
  exit(EXIT_SUCCESS);
}
