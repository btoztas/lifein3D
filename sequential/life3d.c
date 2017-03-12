#include <stdio.h>

#define EXIT_SUCCESS 1
#define EXIT_ERROR -1

void usage(){
  printf("Life 3D\t\tPARALLEL AND DISTRIBUTED COMPUTING\t\tTecnico Lisboa\n\n");
  printf("usage:\t life3d <input.in> <number of iterations>\n\n\n");
}


int main(int argc, char* argv[]){

  if(argc!=3){
    usage();
    return EXIT_ERROR;
  }


  return EXIT_SUCCESS;
}
