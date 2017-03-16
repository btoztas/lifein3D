#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct _cell{

  int x, y, z;

} cell;

typedef struct _node
{
  cell *this;
  struct _node *left;
  struct _node *right;

} node;

typedef struct _bintree{

  node *root;

} bintree;

typedef struct _world{

  int alive_cells;
  int size;
  bintree *data;

} world;

cell *create_cell(int x, int y, int z){

  cell *new = (cell*)malloc(sizeof(cell));
  new->x = x;
  new->y = y;
  new->z = z;

  return new;
}

node *create_bintree_node(cell *this){

  node *new   = (node*)malloc(sizeof(node));
  new->this  = this;
  new->left  = NULL;
  new->right = NULL;

  return new;

}

bintree *create_bintree(){

  bintree *tree = (bintree*)malloc(sizeof(bintree));
  tree->root = NULL;

  return tree;

}

world *create_world(int size){

  world *new = (world*)malloc(sizeof(world));
  new->alive_cells = 0;
  new->size = size;

  return new;

}

void destroy_cell(cell *this){

  free(this);

}

void destroy_bintree_nodes(node *root){

  if(root->left != NULL)
    destroy_bintree_nodes(root->left);

  else if(root->right != NULL)
    destroy_bintree_nodes(root->right);

  else{
    destroy_cell(root->this);
    free(root);
  }

}

// function to free the data structures
void destroy_bintree(bintree *tree){

  destroy_bintree_nodes(tree->root);
  free(tree);

}

// function to free the game world
void destroy_world(world *game){

  destroy_bintree(game->data);
  free(game);

}

// function to compare two cell positions in the tree
int most_priority_index(int x1, int y1, int z1, int x2, int y2, int z2)
{
  if(x1>x2)
    return(1);
  if(x1==x2)
  {
    if(y1>y2)
      return(1);
    if(y1==y2)
    {
      if(z1>z2)
        return(1);
      if(z1==z2)
        return(0);
    }
  }
  return(-1);
}


int compare_cells(cell *c1, cell *c2){

  return most_priority_index(c1->x, c1->y, c1->z, c2->x, c2->y, c2->z);

}


void insert_bintree(bintree *tree, cell *new_cell){

  node *new_node = create_bintree_node(new_cell);
  node *aux, *parent;

  if(tree->root == NULL){
    tree->root = new_node;
  }else{
    aux = tree->root;
    while(aux!=NULL){
      parent = aux;
      if(compare_cells(new_node->this, aux->this)==1)
        aux=aux->right;
      else
        aux=aux->left;
    }
    if(compare_cells(new_node->this, parent->this)==1)
      parent->right = new_node;
    else
      parent->left = new_node;

  }
}



int check_alive(int x, int y, int z, bintree *tree){

  node *aux = tree->root;
  int ret;
  while(aux!=NULL){
    ret = most_priority_index((aux->this)->x, (aux->this)->y, (aux->this)->z, x, y, z);
    if(ret==-1)
      aux=aux->right;
    if(ret==1)
      aux=aux->left;
    else
      return 1;
  }
  return 0;

}


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




// game rules

int rules(int x, int y, int z, world* game) {

  int live = 0;

  if(x+1 == game->size) {
    if(check_alive(0,y, z, game->data)) live++;
  } else {
    if(check_alive(x+1,y, z, game->data)) live++;
  }

  if(x-1 == -1) {
    if(check_alive(game->size-1,y, z, game->data)) live++;
  } else {
    if(check_alive(x-1,y, z, game->data)) live++;
  }

  if(y+1 == game->size) {
    if(check_alive(x,0, z, game->data)) live++;
  } else {
    if(check_alive(x,y+1, z, game->data)) live++;
  }

  if(y-1 == -1) {
    if(check_alive(x,game->size-1, z, game->data)) live++;
  } else {
    if(check_alive(x,y-1, z, game->data)) live++;
  }

  if(z+1 == game->size) {
    if(check_alive(x,y,0, game->data)) live++;
  } else {
    if(check_alive(x,y,z+1, game->data)) live++;
  }

  if(z-1 == -1) {
    if(check_alive(x,y,game->size-1, game->data)) live++;
  } else {
    if(check_alive(x,y,z-1, game->data)) live++;
  }

  if(check_alive(x,y,z, game->data)) {
    if(live < 2) /*a live cell with fewer than two live nieghbors dies*/
      return 0;
    if(live > 2 && live < 5) /*a live cell with two to four live nieghbors lives on to the next generation*/
      return 1;
    if(live > 4) /*a live cell with more than four live nieghbors dies*/
      return 0;
  } else {
    if(live == 2 || live == 3) /*a dead cell with two or three live neighbors becomes a live cell*/
      return 1;
    else
      return 0;
  }

  return -1;

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
  int num_iterations = atoi(argv[2]);
  printf("%d\n", num_iterations);

  // read file
  FILE *file;
  file = open_file(file_name);
  world *game = createWorld(file);

  fclose(file);
  free(file_name);
  exit(EXIT_SUCCESS);
}
