#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define DEBUG 0

typedef struct _cell{

  int x, y, z;

} cell;

typedef struct _node{
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
  bintree *cells;

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
  new->cells = create_bintree();

  return new;

}

void destroy_cell(cell *this){

  free(this);

}

void destroy_bintree_nodes(node *root){

  if(root->left != NULL)
    destroy_bintree_nodes(root->left);

  if(root->right != NULL)
    destroy_bintree_nodes(root->right);

  destroy_cell(root->this);
  free(root);

}

// function to free the cell structures
void destroy_bintree(bintree *tree){

  if(tree->root!=NULL)
    destroy_bintree_nodes(tree->root);
  free(tree);

}

// function to free the game world
void destroy_world(world *game){

  destroy_bintree(game->cells);
  free(game);

}

// function to compare two cell positions in the tree
int most_priority_index(int x1, int y1, int z1, int x2, int y2, int z2){
  if(x1>x2)
    return(1);
  else if(x1==x2){
    if(y1>y2)
      return(1);
    else if(y1==y2){
      if(z1>z2)
        return(1);
      else if(z1==z2)
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
  if(DEBUG)
    printf("      Created tree node\n");
  node *aux, *parent;

  if(tree->root == NULL){
    tree->root = new_node;
    if(DEBUG)
      printf("        First tree node added\n");
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

void print_cell(cell *this){

  if(DEBUG)
    printf("\t  %d %d %d\n", this->x, this->y, this->z);
  else
    printf("%d %d %d\n", this->x, this->y, this->z);

}


void print_bintree(node *root){

  if(root->right != NULL){
    print_bintree(root->right);
    if(DEBUG)
      printf("\t  Not leaf\n");
  }
  if(root->left != NULL){
    print_bintree(root->left);
    if(DEBUG)
      printf("\t  Not leaf\n");
  }
  print_cell(root->this);
  if(DEBUG)
    printf("\t  Leaf\n");

}




void print_world(world *game){

  if(DEBUG)
    printf("################################\n\tWorld Size: %d\n\tAlive Cells: %d\n", game->size, game->alive_cells);
  if(DEBUG)
    printf("\tPrinting Cells\n");
  print_bintree((game->cells)->root);
  if(DEBUG)
    printf("################################\n");

}




void insert_cell(world *game, cell *new_cell){

  insert_bintree(game->cells, new_cell);
  if(DEBUG)
    printf("      Inserted in tree\n");
  game->alive_cells++;

  return;

}




// create initial world from input file
world *file_to_world(FILE *file){
  int size;
  int x, y, z;

  world *new_world;
  cell *new_cell;


  fscanf(file, "%d", &size);
  if(DEBUG)
    printf("World Size: %d\n\n", size);

  if(DEBUG)
    printf("Creating World\n");

  new_world = create_world(size);
  if(DEBUG)
    printf("Populating World\n");

  while(fscanf(file, "%d %d %d", &x, &y, &z) != EOF){

    // handle coordinates read
    if(DEBUG)
      printf("  Adding: %d %d %d\n", x, y, z);
    new_cell = create_cell(x, y, z);
    if(DEBUG)
      printf("    Cell created\n");
    insert_cell(new_world, new_cell);
    if(DEBUG)
      printf("    Cell inserted\n");

  }
  return new_world;
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


// function usage
void usage(){
  printf("Life 3D\t\tPARALLEL AND DISTRIBUTED COMPUTING\t\tTecnico Lisboa\n\n");
  printf("usage:\t life3d <input.in> <number of iterations>\n\n\n");
}

int check_alive(int x, int y, int z, bintree *tree){

  node *aux = tree->root;
  int ret;

  while(aux!=NULL){
    ret = most_priority_index((aux->this)->x, (aux->this)->y, (aux->this)->z, x, y, z);
    if(ret==-1)
      aux=aux->right;
    else if(ret==1)
      aux=aux->left;
    else
      return 1;
  }
  return 0;

}


// game rules

int test_cell(int x, int y, int z, world* game) {

  int live = 0;

  if(x+1 == game->size) {
    if(check_alive(0,y, z, game->cells)) live++;
  } else {
    if(check_alive(x+1,y, z, game->cells)) live++;
  }

  if(x-1 == -1) {
    if(check_alive(game->size-1,y, z, game->cells)) live++;
  } else {
    if(check_alive(x-1,y, z, game->cells)) live++;
  }

  if(y+1 == game->size) {
    if(check_alive(x,0, z, game->cells)) live++;
  } else {
    if(check_alive(x,y+1, z, game->cells)) live++;
  }

  if(y-1 == -1) {
    if(check_alive(x,game->size-1, z, game->cells)) live++;
  } else {
    if(check_alive(x,y-1, z, game->cells)) live++;
  }

  if(z+1 == game->size) {
    if(check_alive(x,y,0, game->cells)) live++;
  } else {
    if(check_alive(x,y,z+1, game->cells)) live++;
  }

  if(z-1 == -1) {
    if(check_alive(x,y,game->size-1, game->cells)) live++;
  } else {
    if(check_alive(x,y,z-1, game->cells)) live++;
  }

  if(check_alive(x,y,z, game->cells)) {
    if(live < 2) // a live cell with fewer than two live nieghbors dies
      return 0;
    if(live > 2 && live < 5) // a live cell with two to four live nieghbors lives on to the next generation
      return 1;
    if(live > 4) // a live cell with more than four live nieghbors dies
      return 0;
  } else {
    if(live == 2 || live == 3) // a dead cell with two or three live neighbors becomes a live cell
      return 1;
    else
      return 0;
  }

  return -1;

}


world *get_next_world(world *actual_world){

  if(DEBUG)
    printf("    Creating next world\n");
  world *next_world = create_world(actual_world->size);

  if(DEBUG)
    printf("    Testing cells\n");
  for(int x=0; x<next_world->size; x++)
    for(int y=0; y<next_world->size; y++)
      for(int z=0; z<next_world->size; z++){

        if(test_cell(x, y, z, actual_world)){
          if(DEBUG)
            printf("      %d %d %d will be alive\n",x, y, z);
          cell *new_cell = create_cell(x, y, z);
          insert_cell(next_world, new_cell);
        }
        else
          if(DEBUG)
            printf("      %d %d %d will be dead\n",x, y, z);

      }

  if(DEBUG)
    printf("    Finished testing cells\n");
  return next_world;

}





int main(int argc, char* argv[]){

  world *next_world;

  // if argc not expected, print program usage
  if(argc!=3){
    usage();
    exit(EXIT_FAILURE);
  }

  // handle file_name
  char *file_name = (char*)malloc(sizeof(char)*strlen(argv[1]));
  strcpy(file_name, argv[1]);
  int num_iterations = atoi(argv[2]);
  if(DEBUG)
    printf("Iterations to do: %d\n\n", num_iterations);

  // read file
  FILE *file;
  if(DEBUG)
    printf("Opening file\n");
  file = open_file(file_name);
  if(DEBUG)
    printf("Reading file\n");
  world *actual_world = file_to_world(file);

  if(DEBUG)
    printf("\nPrinting World\n");
  if(DEBUG)
    print_world(actual_world);

  if(DEBUG)
    printf("\nStarting to iterate\n");
  for(int i=0; i<num_iterations; i++){

    if(DEBUG)
      printf("  Iteration number %d\n", i+1);
    next_world = get_next_world(actual_world);
    if(DEBUG)
      printf("    Printing new world\n");
    if(DEBUG)
      print_world(next_world);
    if(DEBUG)
      printf("    Destroying previous world\n");
    destroy_world(actual_world);

    actual_world = next_world;
  }


  if(DEBUG)
    printf("\nDestroying World\n");
  if(!DEBUG)
    print_world(actual_world);
  destroy_world(actual_world);

  if(DEBUG)
    printf("Freeing other variables\n");

  fclose(file);
  free(file_name);
  exit(EXIT_SUCCESS);

}
