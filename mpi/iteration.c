#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

void print_sendings(int*bound1,int*bound2,int*bound3,int*bound4,int s1, int s2, int s3,int s4, int after, int before,int rank){
	int i;
	printf("\nPC %d:\n\n",rank);
	fflush(stdout);
	printf("Mandei para o PC %d:[ ",before);
	fflush(stdout);
	for(i=0;i<s1;i++){
		printf("%d ",bound1[i]);
		fflush(stdout);
	}
	printf("]\n");
	fflush(stdout);
	printf("Mandei para o PC %d:[ ",after);
	fflush(stdout);
	for(i=0;i<s2;i++){
		printf("%d ",bound2[i]);
		fflush(stdout);
	}
	printf("]\n");
	fflush(stdout);
	printf("Recebi do PC %d:[ ",before);
	fflush(stdout);
	for(i=0;i<s3;i++){
		printf("%d ",bound3[i]);
		fflush(stdout);
	}
	printf("]\n");
	fflush(stdout);
	printf("Recebi do PC %d:[ ",after);
	fflush(stdout);
	for(i=0;i<s4;i++){
		printf("%d ",bound4[i]);
		fflush(stdout);
	}
	printf("]\n");
	fflush(stdout);
}

void print_mini_world(int ** mini_world, int world_size, int size_yz){
	int i,j;
	for(i=0;i<world_size;i++){
		printf("miniworld[%d] --> [",i);
		for(j=0;j<size_yz;j++){
			printf("%d,",mini_world[i][j]);
		}
		printf("]\n");
	}
	printf("\n\n");
}

int main(int argc, char ** argv){

	int rank,pcs;
	int s1,s2,s3,s4;
	int *bound1,*bound2,*bound3,*bound4;
	int before,after;
	int world_size;
	int ** mini_world;
	int i,j;
	int size_yz;
	int flag=1;

	world_size=atoi(argv[1]);
	size_yz=atoi(argv[2]);

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&pcs);
	MPI_Status statuses[2];
	MPI_Request requests[2];
	MPI_Status status;

	before=rank-1;
	after=rank+1;
	if(before==-1){
		before=pcs-1;
	}
	if(after==pcs){
		after=0;
	}

	mini_world=malloc(world_size*sizeof(int*));
	for(i=0;i<world_size;i++){
		mini_world[i]=malloc(size_yz*sizeof(int));
		for(j=0;j<size_yz;j++){
			mini_world[i][j]=rank*world_size*size_yz+world_size*i+j;
		}
	}

	bound1 = mini_world[1];
	bound2 = mini_world[world_size-2];

	MPI_Barrier(MPI_COMM_WORLD);

	s1=size_yz;
	s2=size_yz;

	MPI_Irecv(&s3,1,MPI_INT,before,1,MPI_COMM_WORLD,&requests[0]);
	MPI_Irecv(&s4,1,MPI_INT,after,1,MPI_COMM_WORLD,&requests[1]);
	MPI_Send(&s1,1,MPI_INT,before,1,MPI_COMM_WORLD);
	MPI_Send(&s2,1,MPI_INT,after,1,MPI_COMM_WORLD);

	MPI_Wait(requests,statuses);

	bound3=malloc(s3*sizeof(int));
	bound4=malloc(s4*sizeof(int));

	MPI_Irecv(bound3,s3,MPI_INT,before,2,MPI_COMM_WORLD,&requests[0]);
	MPI_Irecv(bound4,s4,MPI_INT,after,2,MPI_COMM_WORLD,&requests[1]);
	MPI_Send(bound1,s1,MPI_INT,before,2,MPI_COMM_WORLD);
	MPI_Send(bound2,s2,MPI_INT,after,2,MPI_COMM_WORLD);

	MPI_Wait(requests,statuses);

	MPI_Barrier(MPI_COMM_WORLD);

	for(i = 0; i < pcs; i++) {
    		MPI_Barrier(MPI_COMM_WORLD);
    		if (i == rank) {
			print_mini_world(mini_world,world_size,size_yz);
       			print_sendings(bound1,bound2,bound3,bound4,s1,s2,s3,s4,after,before,rank);
		}
	}

	MPI_Finalize();
	return 0;
}
