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
  int eu1[5]={1,2,3,4,5};
  int eu2[5]={6,7,8,9,10};
  int *tu;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
  MPI_Comm_size (MPI_COMM_WORLD, &p);

  if(argc != 2){
  	if (!id) printf ("Command line: %s <n-rounds>\n", argv[0]);
  	MPI_Finalize();
  	exit(1);
  }
  rounds = atoi(argv[1]);

  MPI_Barrier (MPI_COMM_WORLD);
  secs = - MPI_Wtime();

  if(id ==0){
    MPI_Send(eu1, 5, MPI_INT, 1, 1, MPI_COMM_WORLD);
    MPI_Recv(tu, 5, MPI_INT, 1, 2, MPI_COMM_WORLD, &status);
  }
  if(id ==1){
    MPI_Send(eu2, 5, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Recv(tu, 5, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
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
