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
  int *eu;
  int *tu;
  int n_array1, n_array2;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
  MPI_Comm_size (MPI_COMM_WORLD, &p);

  n_array1=atoi(argv[2]);

  n_array2=atoi(argv[3]);

  MPI_Barrier (MPI_COMM_WORLD);
  secs = - MPI_Wtime();
  int x;
  if(id ==0){
    eu =(int *)malloc(sizeof(int)*n_array1);
    for(x=0; x<n_array1; x++){
      eu[x]= x+1;
    }
    MPI_Send(&eu, 5, MPI_INT, 1, 1, MPI_COMM_WORLD);
    MPI_Recv(&tu, 5, MPI_INT, 1, 2, MPI_COMM_WORLD, &status);

  }
  if(id ==1){
    eu =(int *)malloc(sizeof(int)*n_array2);
    for(x=0; x<n_array2; x++){
      eu[x]= x+n_array1;
    }
    MPI_Send(&eu, 5, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Recv(&tu, 5, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
  }

  MPI_Barrier (MPI_COMM_WORLD);
  //secs += MPI_Wtime();
  if(id ==0){

    printf("id=0 -> [ ");
    for(x=0; x<n_array2; x++){
      printf("%d ");
    }
    printf("]\n");
  }else{
    pprintf("id=1 -> [ ");
    for(x=0; x<n_array1; x++){
      printf("%d ");
    }
    printf("]\n");
  }
  MPI_Finalize();
  return 0;
  }
