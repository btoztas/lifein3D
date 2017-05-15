/*
 *   Send / Recv
 *
 *   Jos� Monteiro, DEI / IST
 *
 *   Last modification: 2 November 2014
 */

#include <stdio.h>
#include <mpi.h>

int main (int argc, char *argv[]) {

  MPI_Status status;
  int id, p, i, rounds;
  double secs;
  int eu[5];
  int *tu;

  MPI_Init (&argc, &argv, &eu);
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
  MPI_Comm_size (MPI_COMM_WORLD, &p);

  rounds = atoi(argv[1]);

  MPI_Barrier (MPI_COMM_WORLD);
  secs = - MPI_Wtime();
  int x;
  if(id ==0){
    for(x=0; x<5; x++){
      eu[x]= x+1;
    }
    MPI_Send(&eu, 5, MPI_INT, 1, 1, MPI_COMM_WORLD);
    MPI_Recv(&tu, 5, MPI_INT, 1, 2, MPI_COMM_WORLD, &status);
  }
  if(id ==1){
    for(x=0; x<5; x++){
      eu[x]= x+6;
    }
    MPI_Send(&eu, 5, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Recv(&tu, 5, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
  }

  MPI_Barrier (MPI_COMM_WORLD);
  //secs += MPI_Wtime();
  if(id ==0){
    printf("Acabei esta merda!! Recebi [%d,%d,%d,%d,%d] do filho da mãe 1\n", tu[0], tu[1], tu[2], tu[3], tu[4] );
  }else{
    printf("Acabei esta merda!! Recebi [%d,%d,%d,%d,%d] do filho da mãe 0\n", tu[0], tu[1], tu[2], tu[3], tu[4] );
  }
  MPI_Finalize();
  return 0;
  }
