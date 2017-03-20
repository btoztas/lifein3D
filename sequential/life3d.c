#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _cell{

  int x, y, z;

} cell;

typedef struct _node{
  cell *this;
  int height;
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
  if(new == NULL)
    printf("Error allocating memory for a new cell.\n");
  new->x    = x;
  new->y    = y;
  new->z    = z;

  return new;
}

node *create_bintree_node(cell *this){

  node *new   = (node*)malloc(sizeof(node));
  if(new == NULL)
    printf("Error allocating memory for a new node.\n");
  new->this   = this;
  new->left   = NULL;
  new->right  = NULL;
  new->height = 1;

  return new;

}

bintree *create_bintree(){

  bintree *tree = (bintree*)malloc(sizeof(bintree));
  if(tree == NULL)
    printf("Error allocating memory for a new tree.\n");
  tree->root    = NULL;

  return tree;

}

world *create_world(int size){

  world *new       = (world*)malloc(sizeof(world));
  if(new == NULL)
    printf("Error allocating memory for a new world.\n");
  new->alive_cells = 0;
  new->size        = size;
  new->cells       = create_bintree();

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



void print_cell(cell *this){

  #ifdef DEBUG
    printf("\t  %d %d %d\n", this->x, this->y, this->z);
  #else
    printf("%d %d %d\n", this->x, this->y, this->z);
  #endif

}


void print_bintree(node *root){
  if(root->left != NULL){
    print_bintree(root->left);
  }
  print_cell(root->this);

  if(root->right != NULL){
    print_bintree(root->right);
  }

}




void print_world(world *game){

  #ifdef DEBUG
    printf("################################\n\tWorld Size: %d\n\tAlive Cells: %d\n\tPrinting Cells\n", game->size, game->alive_cells);
  #endif

  print_bintree((game->cells)->root);

  #ifdef DEBUG
    printf("################################\n");
  #endif
}



// function to compare two cell positions in the tree
int most_priority_index(int x1, int y1, int z1, int x2, int y2, int z2){
  if(x1 > x2)
    return(1);
  else if(x1 == x2){
    if(y1 > y2)
      return(1);
    else if(y1 == y2){
      if(z1 > z2)
        return(1);
      else if(z1 == z2)
        return(0);
    }
  }
  return(-1);
}


int compare_cells(cell *c1, cell *c2){

  return most_priority_index(c1->x, c1->y, c1->z, c2->x, c2->y, c2->z);

}

int max(int l, int r){

    return l > r ? l: r;
}

int height(node *this){

  if(this == NULL)
    return 0;
  return this->height;

}



/*
  node *new_node;
  if(root == NULL){
    printf("        ROOT\n");


    new_node = create_bintree_node(new_cell);

    printf("          JUST ADDED: %d, %d, %d\n", (new_node->this)->x, (new_node->this)->y, (new_node->this)->z);

  }
  if(root->left == NULL && root->right != NULL){
    root->height = height(root->right) + 1;
  }else if(root->right == NULL && root->left != NULL){
    root->height = height(root->left) + 1;
  }else if(root->right != NULL && root->left != NULL){
    root->height = max(height(root->left), height(root->right))+1;
  }
  printf("        SUBTREE HEIGHT: %d\n", root->height);

  return new_node;
}

*/


node  *insert_bintree(node *root, cell *new_cell){

    if(root == NULL){

      root = create_bintree_node(new_cell);

    }else if(compare_cells(new_cell, root->this)==1){

      root->right = insert_bintree(root->right, new_cell);

    }else{

      root->left = insert_bintree(root->left, new_cell);

    }

    root->height = max(height(root->left), height(root->right))+1;

    #ifdef DEBUG
      printf("        HEIGHT: %d\n", root->height);
    #endif

    return root;
}


void insert_data(bintree *tree, cell *new_cell){

  tree->root = insert_bintree(tree->root, new_cell);

}


void insert_cell(world *game, cell *new_cell){

  insert_data(game->cells, new_cell);

  #ifdef DEBUG
    printf("      Inserted in tree\n");
  #endif

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

  #ifdef DEBUG
    printf("World Size: %d\n\nCreating World\n", size);
  #endif

  new_world = create_world(size);


  #ifdef DEBUG
    printf("Populating World\n");
  #endif

  while(fscanf(file, "%d %d %d", &x, &y, &z) != EOF){

    // handle coordinates read
    #ifdef DEBUG
      printf("  Adding: %d %d %d\n", x, y, z);
    #endif

    new_cell = create_cell(x, y, z);

    #ifdef DEBUG
      printf("    Cell created\n");
    #endif

    insert_cell(new_world, new_cell);

    #ifdef DEBUG
      printf("    Cell inserted\n");
    #endif

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

  #ifdef DEBUG
    printf("    Creating next world\n");
  #endif
  world *next_world = create_world(actual_world->size);

  #ifdef DEBUG
    printf("    Testing cells\n");
  #endif
  for(int x=0; x<next_world->size; x++)
    for(int y=0; y<next_world->size; y++)
      for(int z=0; z<next_world->size; z++){

        if(test_cell(x, y, z, actual_world)){
          #ifdef DEBUG
            printf("      %d %d %d will be alive\n",x, y, z);
          #endif
          cell *new_cell = create_cell(x, y, z);
          insert_cell(next_world, new_cell);
        }
        else{
          #ifdef DEBUG
            printf("      %d %d %d will be dead\n",x, y, z);
          #endif
        }
      }

  #ifdef DEBUG
    printf("    Finished testing cells\n");
  #endif
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
  if(file_name == NULL)
    printf("Error allocating memory for filename.\n");
  strcpy(file_name, argv[1]);
  int num_iterations = atoi(argv[2]);
  #ifdef DEBUG
    printf("Iterations to do: %d\n\n", num_iterations);
  #endif

  // read file
  FILE *file;

  #ifdef DEBUG
    printf("Opening file\n");
  #endif

  file = open_file(file_name);

  #ifdef DEBUG
    printf("Reading file\n");
  #endif

  world *actual_world = file_to_world(file);

  #ifdef DEBUG
    printf("\nPrinting World\n");
    print_world(actual_world);
    printf("\nStarting to iterate\n");
  #endif

  for(int i=0; i<num_iterations; i++){

    #ifdef DEBUG
      printf("  Iteration number %d\n", i+1);
    #endif

    next_world = get_next_world(actual_world);

    #ifdef DEBUG
      printf("    Printing new world\n");
      print_world(next_world);
      printf("    Destroying previous world\n");
    #endif

    destroy_world(actual_world);
    actual_world = next_world;


  }


  #ifdef DEBUG
    printf("\nDestroying World\n");
  #else
    print_world(actual_world);
  #endif

  destroy_world(actual_world);

  #ifdef DEBUG
    printf("Freeing other variables\n");
  #endif

  fclose(file);
  free(file_name);
  exit(EXIT_SUCCESS);

}
