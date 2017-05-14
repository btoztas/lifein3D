#include <stdio.h>
#include <stdlib.h>

void main(int argc, char ** argv){
  if(argc!=3){
    printf("INSERE DOIS ARGUMENTOS FDP!\n");
    exit(-1);
  }



  if(nums<threads){
    printf("FODA-SE! TAS A POR MAIS DIVISOES PARA O NUMERO DE MERDAS QUE TENS!\n");
    exit(-1);
  }

  int threads=atoi(argv[2]);
  int nums=atoi(argv[1]);
  int *vec;
  int i,j;
  int var,var2=0;
  int resto=0, divisao=0;

  vec = (int*)malloc(nums*sizeof(int));
  for(i=0;i<nums;i++){
    vec[i]=i;
  }

  for(i=0;i<threads;i++){
    divisao=nums/threads;
    //resto=nums%threads;
    printf("Thread %d - ", i);
    if(i!=threads-1){
      for(j=i*divisao;j<(i+1)*divisao;j++){
        if(j==i*divisao){
          if(j==0)
            printf("[ %d %d ",vec[nums-2],vec[nums-1]);
          else
            printf("[ %d %d ",vec[j-2],vec[j-1]);
        }
        printf("%d ",vec[j]);
        if(j==(i+1)*divisao-1){
          if(j==nums-1)
            printf("%d %d ]",vec[0],vec[1]);
          else
            printf("%d %d ]",vec[j+1],vec[j+2]);
        }
      }
    }else{
      for(j=i*divisao;j<nums;j++){
        if(j==i*divisao)
            printf("[ %d %d ",vec[j-2],vec[j-1]);
        printf("%d ",vec[j]);
        if(j==nums-1)
          printf("%d %d ]",vec[0],vec[1]);
      }
    }

    printf("\n");
    var2=var;
  }

  free(vec);

}
