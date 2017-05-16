#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MATRIX_INDEX(x, y, size_y)  (x*size_y+y)

#define NEEDED_FIRST_X(id,p,n) (((id*n)/p)-1)
#define NEEDED_LAST_X(id,p,n) (((id+1)*n)/p)
#define NEEDED_BLOCK_SIZE(id,p,n) (NEEDED_LAST_X(id,p,n)-NEEDED_FIRST_X(id,p,n)+1)

#define TREATED_FIRST_X(id,p,n) ((id*n)/p)
#define TREATED_LAST_X(id,p,n) ((((id+1)*n)/p)-1)
#define TREATED_BLOCK_SIZE(id,p,n) (NEEDED_LAST_X(id,p,n)-NEEDED_FIRST_X(id,p,n)+1)

typedef struct _cell{

  int x, y, z;

} cell;

typedef struct _node{
  cell *this;
  int height;
  struct _node *left;
  struct _node *right;

} node;

typedef struct _world{

  int alive_cells;
  int *n_alive_cells;
  int size_x;
  int size_y;
  node **cells;

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


node **create_bintree_hash(int size_x, int size_y){

  node **new = (node**)calloc(size_x*size_y,sizeof(node*));
  return new;

}

world *create_world(int size_x, int size_y){

  world *new       = (world*)malloc(sizeof(world));
  if(new == NULL)
    printf("Error allocating memory for a new world.\n");
  new->alive_cells   = 0;
  new->size_x        = size_x;
  new->size_y        = size_y;
  new->cells         = create_bintree_hash(size_x, size_y);
  new->n_alive_cells = (int*)calloc(size_x,sizeof(int));
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
void destroy_bintree(node **tree, int size_x, int size_y){

  for(int i=0; i<size_x*size_y; i++)
    if(tree[i]!=NULL)
      destroy_bintree_nodes(tree[i]);

  free(tree);
}

// function to free the game world
void destroy_world(world *game){

  destroy_bintree(game->cells, game->size_x, game->size_y);
  free(game->n_alive_cells);
  free(game);

}


void no_struct_cell(cell *this, int *array_world, int *aux){


  array_world[*aux]=this->x;
  (*aux)++;
  array_world[*aux]=this->y;
  (*aux)++;
  array_world[*aux]=this->z;
  (*aux)++;

}

void no_struct_bintree(node *root, int *array_world, int *aux){

  if(root->left != NULL){
    no_struct_bintree(root->left, array_world, aux);
  }


  no_struct_cell(root->this, array_world, aux);

  if(root->right != NULL){

    no_struct_bintree(root->right, array_world, aux);
  }
}

void no_struct_game(world *game, int *array_world, int *array_world_indexes){

  int aux = 0;
  array_world_indexes[0]=0;

  for(int i=0; i<game->size_x*game->size_y; i++){

    if(game->cells[i]!=NULL){
      no_struct_bintree(game->cells[i], array_world, &aux);
    }
    if((i+1)%game->size_x == 0)
      array_world_indexes[(i+1)/(game->size_x)] = aux;
  }

}

void print_no_struct_game(int *array_world, int *array_world_indexes, int alive_cells, int size){

  for(int i=0; i<alive_cells*3; i++){
    printf("INDEX: %d\tVALUE: %d\n", i, array_world[i]);
    if((i+1)%3==0)
      printf("\n");
  }
  for(int i=0; i<size; i++){
    printf("X:%d INDEX:%d\n", i, array_world_indexes[i]);
  }

}


void free_no_struct_bintree(int *array_world, int *array_world_indexes){

  free(array_world);
  free(array_world_indexes);

}




void array_miniworl(int * array_world, int size, int n_alive, int threads){

  int i,j;
  int divisao=0;
  int aux_step=0;
  divisao=n_alive/threads;
  printf("%d\n",n_alive );
  for(i=0;i<threads;i++){


    //resto=nums%threads;

    printf("Thread %d - ", i);
    if(i!=threads-1){
      if(i==0)
        aux_step=0;
      else
        aux_step=(i*divisao)-1;
      for(j=i*divisao;j<(i+1)*divisao;j++){
        if(j==i*divisao){
          if(j==0)
            printf("[(%d,%d,%d),(%d,%d,%d),",array_world[n_alive-6],array_world[n_alive-5],array_world[n_alive-4],array_world[n_alive-3],array_world[n_alive-2],array_world[n_alive-1]);
          else
            printf("[(%d,%d,%d),(%d,%d,%d),",array_world[aux_step-6],array_world[aux_step-5],array_world[aux_step-4],array_world[aux_step-3],array_world[aux_step-2],array_world[aux_step-1]);
        }
        printf("(%d,%d,%d),",array_world[aux_step],array_world[aux_step+1],array_world[aux_step+2]);
        if(j==(i+1)*divisao-1){
          if(j==n_alive-1)
            printf("(%d,%d,%d),(%d,%d,%d)]",array_world[0],array_world[1],array_world[2],array_world[3],array_world[4],array_world[5]);
          else
            printf("(%d,%d,%d),(%d,%d,%d)]",array_world[aux_step+3],array_world[aux_step+4],array_world[aux_step+5],array_world[aux_step+6],array_world[aux_step+7],array_world[aux_step+8]);
        }
        aux_step=aux_step+3;
      }
    }else{
      aux_step=i*divisao-1;

      for(j=i*divisao;j<n_alive;j++){
        printf("---%d---",aux_step );
        if(j==i*divisao)
            printf("[(%d,%d,%d),(%d,%d,%d),",array_world[aux_step-6],array_world[aux_step-5],array_world[aux_step-4],array_world[aux_step-3],array_world[aux_step-2],array_world[aux_step-1]);
        printf("(%d,%d,%d),",array_world[aux_step],array_world[aux_step+1],array_world[aux_step+2]);
        if(j==n_alive-1)
          printf("(%d,%d,%d),(%d,%d,%d)]",array_world[0],array_world[1],array_world[2],array_world[3],array_world[4],array_world[5]);
        aux_step=aux_step+3;
      }

    }

    printf("\n");
  }
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
  #ifdef DEBUG
    printf("\t  Height: %d", root->height);
  #endif
  print_cell(root->this);

  if(root->right != NULL){
    print_bintree(root->right);
  }

}

void print_bintree_hash(node **tree, int size_x, int size_y){

  for(int i=0; i<size_x*size_y; i++)
    if(tree[i]!=NULL)
      print_bintree(tree[i]);

}


void print_world(world *game){

  #ifdef DEBUG
    printf("################################\n\tWorld Size: x=%d y=%d\n\tAlive Cells: %d\n\tPrinting Cells\n", game->size_x, game->size_y, game->alive_cells);


  #endif

  for (int i = 0; i < game->size_x; i++)
    printf("x = %d - %d cells\n", i, game->n_alive_cells[i]);

  print_bintree_hash(game->cells, game->size_x, game->size_y);

  #ifdef DEBUG
    printf("################################\n");
  #endif
}



// function to compare two cell positions in the tree
int most_priority_index(int x1, int y1, int z1, int x2, int y2, int z2){

  if(z1 > z2)
    return 1;
  else if(z1 == z2)
    return 0;

  return -1;
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


// A utility function to right rotate subtree rooted with y
// See the diagram given above.
node *right_rotate(node *y)
{
    node *x = y->left;
    node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right))+1;
    x->height = max(height(x->left), height(x->right))+1;

    // Return new root
    return x;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
node *left_rotate(node *x)
{
    node *y = x->right;
    node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    //  Update heights
    x->height = max(height(x->left), height(x->right))+1;
    y->height = max(height(y->left), height(y->right))+1;

    // Return new root
    return y;
}

// Get Balance factor of node N
int get_balance(node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}





node  *insert_bintree(node *root, cell *new_cell, int* n_cells, int* n_alive_cells, int size_y){
    if(root == NULL){

      root = create_bintree_node(new_cell);


      (*n_cells)++;
      n_alive_cells[new_cell->x]++;

    }else if(compare_cells(new_cell, root->this)==0){
      #ifdef DEBUG
      printf("\t\t\t\t\t\t\t\t\tCELL %d %d %d ALREADY EXISTS\n", root->this->x, root->this->y, root->this->z);
      #endif
      destroy_cell(new_cell);
      return root;
    }else if(compare_cells(new_cell, root->this)==1){

      root->right = insert_bintree(root->right, new_cell, n_cells, n_alive_cells, size_y);

    }else{

      root->left = insert_bintree(root->left, new_cell, n_cells, n_alive_cells, size_y);

    }

    root->height = max(height(root->left), height(root->right))+1;


    int balance = get_balance(root);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && compare_cells(new_cell, root->left->this)==-1)
        return right_rotate(root);

    // Right Right Case
    if (balance < -1 && compare_cells(new_cell, root->right->this)==1)
        return left_rotate(root);

    // Left Right Case
    if (balance > 1 && compare_cells(new_cell, root->left->this)==1)
    {
        root->left =  left_rotate(root->left);
        return right_rotate(root);
    }

    // Right Left Case
    if (balance < -1 && compare_cells(new_cell, root->right->this)==-1)
    {
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    #ifdef DEBUG
      printf("        HEIGHT: %d\n", root->height);
    #endif

    return root;
}


void insert_cell(world *game, cell *new_cell){


  game->cells[new_cell->x*game->size_y+new_cell->y] = insert_bintree(game->cells[new_cell->x*game->size_y + new_cell->y], new_cell, &(game->alive_cells), game->n_alive_cells, game->size_y);


  #ifdef DEBUG
    printf("      Inserted in tree\n"); fflush(stdout);
  #endif


  return;

}


// create the world from the array of cells
world * array_to_world(int * array_world, int size_x, int size_y, int n_alive){

  world *new_world;
  cell *new_cell;
  int counter=0;

  new_world = create_world(size_x, size_y);

  for(int i = 0; i<n_alive;i++){
    new_cell = create_cell(array_world[counter], array_world[counter+1], array_world[counter+2]);
    //printf("(%d,%d,%d)\n",array_world[counter], array_world[counter+1], array_world[counter+2]);
    insert_cell(new_world, new_cell);
    counter=counter+3;
  }

  return(new_world);
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

  new_world = create_world(size, size);


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


int check_alive(int size_x, int x, int y, int z, node **tree ){

  node *aux = tree[x * size_x + y];
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
// status = -1 (not tested) | 0 (dead) | 1 (alive)
int test_cell(int x, int y, int z, world* game, int status) {

  int live = 0;

  if(x+1 == game->size_x) {
    if(check_alive(game->size_x, 0,y, z, game->cells)) live++;
  } else {
    if(check_alive(game->size_x, x+1,y, z, game->cells)) live++;
  }

  if(x-1 == -1) {
    if(check_alive(game->size_x, game->size_x-1,y, z, game->cells)) live++;
  } else {
    if(check_alive(game->size_x, x-1,y, z, game->cells)) live++;
  }

  if(y+1 == game->size_y) {
    if(check_alive(game->size_x, x,0, z, game->cells)) live++;
  } else {
    if(check_alive(game->size_x, x,y+1, z, game->cells)) live++;
  }

  if(y-1 == -1) {
    if(check_alive(game->size_x, x,game->size_y-1, z, game->cells)) live++;
  } else {
    if(check_alive(game->size_x, x,y-1, z, game->cells)) live++;
  }

  if(z+1 == game->size_y) {
    if(check_alive(game->size_x, x,y,0, game->cells)) live++;
  } else {
    if(check_alive(game->size_x, x,y,z+1, game->cells)) live++;
  }

  if(z-1 == -1) {
    if(check_alive(game->size_x, x,y,game->size_y-1, game->cells)) live++;
  } else {
    if(check_alive(game->size_x, x,y,z-1, game->cells)) live++;
  }

  if(status == -1)
    status = check_alive(game->size_x, x,y,z, game->cells);
  if(status){
    if(live < 2) // a live cell with fewer than two live nieghbors dies
      return 0;
    if(live >= 2 && live < 5) // a live cell with two to four live nieghbors lives on to the next generation
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

// this function checks for a given node if it will by alive on the next world
// it also checks, for the dead neighbors of the given node, if they will be alive on the next world, adding them if so.
void handle_node(int x, int y, int z, world *actual_world, world *next_world){

  cell *new_cell;

  int live = 0;

  if(x+1 == actual_world->size_x) {
    if(check_alive(actual_world->size_x,0,y, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(0,y, z, actual_world, 0)){
        new_cell = create_cell(0,y, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", 0,y, z);
        #endif
      }
    }
  } else {
    if(check_alive(actual_world->size_x,x+1,y, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x+1,y, z, actual_world, 0)){
        new_cell = create_cell(x+1,y, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x+1,y, z);
        #endif
      }
    }
  }

  if(x-1 == -1) {
    if(check_alive(actual_world->size_x,actual_world->size_x-1,y, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(actual_world->size_x-1,y, z, actual_world, 0)){
        new_cell = create_cell(actual_world->size_x-1,y, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", actual_world->size_x-1,y, z);
        #endif
      }
    }
  } else {
    if(check_alive(actual_world->size_x,x-1,y, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x-1,y, z, actual_world, 0)){
        new_cell = create_cell(x-1,y, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x-1,y, z);
        #endif
      }
    }
  }

  if(y+1 == actual_world->size_y) {
    if(check_alive(actual_world->size_x,x,0, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,0, z, actual_world, 0)){
        new_cell = create_cell(x,0, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,0, z);
        #endif
      }
    }
  } else {
    if(check_alive(actual_world->size_x,x,y+1, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,y+1, z, actual_world, 0)){
        new_cell = create_cell(x,y+1, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,y+1, z);
        #endif
      }
    }
  }

  if(y-1 == -1) {
    if(check_alive(actual_world->size_x,x,actual_world->size_y-1, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,actual_world->size_y-1, z, actual_world, 0)){
        new_cell = create_cell(x,actual_world->size_y-1, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,actual_world->size_y-1, z);
        #endif
      }
    }
  } else {
    if(check_alive(actual_world->size_x,x,y-1, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,y-1, z, actual_world, 0)){
        new_cell = create_cell(x,y-1, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,y-1, z);
        #endif
      }
    }
  }

  if(z+1 == actual_world->size_y) {
    if(check_alive(actual_world->size_x,x,y,0, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,y,0, actual_world, 0)){
        new_cell = create_cell(x,y,0);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,y,0);
        #endif
      }
    }
  } else {
    if(check_alive(actual_world->size_x,x,y,z+1, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,y,z+1, actual_world, 0)){
        new_cell = create_cell(x,y,z+1);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,y,z+1);
        #endif
      }
    }
  }

  if(z-1 == -1) {
    if(check_alive(actual_world->size_x,x,y,actual_world->size_y-1, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,y,actual_world->size_y-1, actual_world, 0)){
        new_cell = create_cell(x,y,actual_world->size_y-1);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,y,actual_world->size_y-1);
        #endif
      }
    }
  } else {
    if(check_alive(actual_world->size_x,x,y,z-1, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,y,z-1, actual_world, 0)){
        new_cell = create_cell(x,y,z-1);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,y,z-1);
        #endif
      }
    }
  }

  if(live >= 2 && live < 5){ // a live cell with two to four live nieghbors lives on to the next generation
    new_cell = create_cell(x, y, z);
    insert_cell(next_world, new_cell);

    #ifdef DEBUG
    printf("\t\t\t\t\tINSERTED LIVE CELL %d %d %d\n", x, y, z);
    #endif
  }

}

// it works by going to all nodes of the tree, and executing handle_node
void solve_subtree(node *root, world* actual_world, world* next_world){

  if(root->left != NULL){
    solve_subtree(root->left, actual_world, next_world);
  }
  #ifdef DEBUG
  printf("\t\t\t\t\tTESTING CELL %d %d %d\n", root->this->x, root->this->y, root->this->z);
  #endif
  handle_node(root->this->x, root->this->y, root->this->z, actual_world, next_world);

  if(root->right != NULL){
    solve_subtree(root->right, actual_world, next_world);
  }
}


world *get_next_world(world *actual_world){

  #ifdef DEBUG
    printf("    Creating next world\n");
  #endif
  world *next_world = create_world(actual_world->size_x, actual_world->size_y);

  #ifdef DEBUG
    printf("    Testing cells\n");
  #endif

 //AQUI!!
  if(actual_world->alive_cells * 6 * 6 < 6*(actual_world->size_x)*(actual_world->size_y)*(actual_world->size_y)){
    #ifdef ITERATION
    printf("   Choose live cells\n");
    #endif
    for(int x=0; x<actual_world->size_x; x++)
      for(int y=0; y<actual_world->size_y; y++)
        if(actual_world->cells[x*actual_world->size_y+y] != NULL){
          #ifdef DEBUG
          printf("\t\t\t\tTESTING SUBTREE %d %d\n", x, y);
          #endif
          //this function will analyzer every node of the subtree, and add to the new world the cells
          solve_subtree(actual_world->cells[x* actual_world->size_y+ y], actual_world, next_world);

        }
    #ifdef DEBUG
    printf("\t\t\tFINISHED SUBTREE TESTS\n");
    #endif

  }else{
    #ifdef ITERATION
    printf("    Choose N^3\n");
    #endif
    for(int x=0; x<next_world->size_x; x++)
      for(int y=0; y<next_world->size_y; y++)
        for(int z=0; z<next_world->size_y; z++){

          if(test_cell(x, y, z, actual_world, -1)){
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
    }

  #ifdef DEBUG
    printf("    Finished testing cells\n");
  #endif
  return next_world;

}


world *get_next_miniworld(world *actual_world){

  #ifdef DEBUG
    printf("    Creating next world\n");
  #endif
  world *next_world = create_world(actual_world->size_x, actual_world->size_y);

  #ifdef DEBUG
    printf("    Testing cells\n");
  #endif

 //AQUI!!
  if(actual_world->alive_cells * 6 * 6 < 6*(actual_world->size_x)*(actual_world->size_y)*(actual_world->size_y)){
    #ifdef ITERATION
    printf("   Choose live cells\n");
    #endif
    for(int x=0; x<actual_world->size_x; x++)
      for(int y=0; y<actual_world->size_y; y++)
        if(actual_world->cells[x*actual_world->size_y+ y] != NULL){
          #ifdef DEBUG
          printf("\t\t\t\tTESTING SUBTREE %d %d\n", x, y);
          #endif
          //this function will analyzer every node of the subtree, and add to the new world the cells
          solve_subtree(actual_world->cells[x*actual_world->size_y+ y], actual_world, next_world);

        }
    #ifdef DEBUG
    printf("\t\t\tFINISHED SUBTREE TESTS\n");
    #endif

  }else{
    #ifdef ITERATION
    printf("    Choose N^3\n");
    #endif
    for(int x=0; x<next_world->size_x; x++)
      for(int y=0; y<next_world->size_y; y++)
        for(int z=0; z<next_world->size_y; z++){

          if(test_cell(x, y, z, actual_world, -1)){
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
    }

  #ifdef DEBUG
    printf("    Finished testing cells\n");
  #endif
  return next_world;

}




void padding ( char ch, int n ){
  int i;

  for ( i = 0; i < n; i++ )
    putchar ( ch );
}

void print_tree_padding ( node *root, int level ){


  if ( root == NULL ) {
    padding ( '\t', level );
    puts ( "~" );
  } else {
    print_tree_padding ( root->right, level + 1 );
    padding ( '\t', level );
    printf ( "%d %d %d\n", root->this->x, root->this->y, root->this->z );
    print_tree_padding ( root->left, level + 1 );
  }
}

void distribute_payload(int p, int world_size, int alive_cells, int * array_world, int * array_world_indexes, int ** mini_array_worlds, int *size){

  int first_x;
  int last_x;
  int i, j, w;

  printf("\nDIVISION:\n");
  for(i=0; i<p; i++){

    first_x = NEEDED_FIRST_X(i,p,world_size);
    last_x  = NEEDED_LAST_X(i,p,world_size);


    if(first_x !=-1 && last_x != world_size){

      printf("PC %d X=[%d,%d] - [%d,%d]\n", i, first_x, last_x, array_world_indexes[first_x], array_world_indexes[last_x+1]-1);

      mini_array_worlds[i] = (int *)malloc(sizeof(int) * (array_world_indexes[last_x+1]-1 - array_world_indexes[first_x] +1));
      size[i] = array_world_indexes[last_x+1]-1 - array_world_indexes[first_x] +1;

      for(w=0, j = array_world_indexes[first_x]; j<=array_world_indexes[last_x+1]-1; j++, w++){
        if(w%3==0)
          mini_array_worlds[i][w] = array_world[j] - first_x;
        else
          mini_array_worlds[i][w] = array_world[j];
      }

    }
    else if(first_x == -1){

      printf("PC %d X=[%d,%d]U{%d} - [%d,%d]U[%d,%d]\n",i,0,last_x,world_size-1,array_world_indexes[0],array_world_indexes[last_x+1]-1,array_world_indexes[world_size-1],alive_cells*3-1);
      mini_array_worlds[i] = (int *)malloc(sizeof(int) * ((array_world_indexes[last_x+1]-1 - array_world_indexes[0]) + (alive_cells*3-1 - array_world_indexes[world_size-1])+2));
      size[i] = ((array_world_indexes[last_x+1]-1 - array_world_indexes[0]) + (alive_cells*3-1 - array_world_indexes[world_size-1])+2);

      w=0;

      for(j = array_world_indexes[world_size-1]; j<=alive_cells*3-1; j++, w++){
        if(w%3==0)
          mini_array_worlds[i][w] = 0;
        else
          mini_array_worlds[i][w] = array_world[j];
      }

      for(j = array_world_indexes[0]; j<=array_world_indexes[last_x+1]-1; j++, w++){
        if(w%3==0)
          mini_array_worlds[i][w] = array_world[j] + 1;
        else
          mini_array_worlds[i][w] = array_world[j];
      }


    }
    else if(last_x == world_size){

      printf("PC %d X={0}U[%d,%d] - [%d,%d]U[%d,%d]\n",i,first_x,world_size-1,array_world_indexes[0],array_world_indexes[1]-1,array_world_indexes[first_x],alive_cells*3-1);
      mini_array_worlds[i] = (int *)malloc(sizeof(int) * ((array_world_indexes[1]-1 - array_world_indexes[0]) + (alive_cells*3-1 - array_world_indexes[first_x])+2));
      size[i] = ((array_world_indexes[1]-1 - array_world_indexes[0]) + (alive_cells*3-1 - array_world_indexes[first_x])+2);

      w=0;

      for(j = array_world_indexes[first_x]; j<=alive_cells*3-1; j++, w++){
        if(w%3==0)
          mini_array_worlds[i][w] = array_world[j]-first_x;
        else
          mini_array_worlds[i][w] = array_world[j];
      }
      for(j = array_world_indexes[0]; j<=array_world_indexes[1]-1; j++, w++){
        if(w%3==0)
          mini_array_worlds[i][w] = last_x - first_x +1;
        else
          mini_array_worlds[i][w] = array_world[j];
      }

    }
    printf("\n");
  }


  for(i = 0; i<p; i++){
    printf("PC %d has %d CELLS:\n", i, size[i]/3);

    for(j=0; j<size[i]; j++){

      printf("INDEX: %d\tVALUE: %d\n", j, mini_array_worlds[i][j]);
      if((j+1)%3==0)
        printf("\n");

    }
    printf("\n");
  }
}


world * file_to_miniworld(FILE *file, int p, int id){

  int size_x, size_y;
  int x, y, z;
  int first_x, last_x;
  world *new_mini_world;
  cell *new_cell;
  int add;




  fscanf(file, "%d", &size_y);

  first_x = NEEDED_FIRST_X(id,p,size_y);
  last_x  = NEEDED_LAST_X(id,p,size_y);



  if(first_x !=-1 && last_x != size_y){

    printf("\nPC %d\n TREATING X=[%d,%d]\n NEEDING  X=[%d,%d]\n", id, first_x+1, last_x-1, first_x, last_x);
    size_x = last_x - first_x + 1;

  }
  else if(first_x == -1){

    printf("\nPC %d\n TREATING X=[%d,%d]\n NEEDING  X=[%d,%d]U{%d}\n",id,0, last_x-1,  0,last_x,size_y-1);
    size_x = last_x + 2;

  }
  else if(last_x == size_y){

    printf("\nPC %d\n TREATING X=[%d,%d]\n NEEDING  X={%d}U[%d,%d]\n",id,first_x+1, size_y-1, 0,first_x,size_y-1);
    size_x = size_y - 1 - first_x + 2;

  }

  #ifdef DEBUG
    printf("World Size: X=%d, Y=%d\n\nCreated Mini World\n", size_x, size_y);
  #endif

  #ifdef DEBUG
    printf("Populating World\n");
  #endif

  new_mini_world = create_world(size_x, size_y);


  while(fscanf(file, "%d %d %d", &x, &y, &z) != EOF){

    add = 0;

    if(first_x !=-1 && last_x != size_y){
      #ifdef DEBUG
        printf("  Read      %d %d %d\n", x, y, z);
      #endif
      if(x>=first_x && x<=last_x){
        add = 1;
        x = x - first_x;
        //printf("PC %d X=[%d,%d]\n", i, first_x, last_x);
      }
    }

    else if(first_x == -1){
      #ifdef DEBUG
        printf("  Read      %d %d %d\n", x, y, z);
      #endif
      if(x==size_y-1 || (x>=0 && x<=last_x)){
        add = 1;
        if(x == size_y-1)
          x = 0;
        else
          x++;

        //printf("PC %d X=[%d,%d]U{%d}",i,0,last_x,size_y-1);
      }
    }

    else if(last_x == size_y){
      #ifdef DEBUG
        printf("  Read      %d %d %d\n", x, y, z);
      #endif
      if(x==0 || (x>=first_x && x<=size_y-1)){
        add = 1;
        if(x==0)
          x = last_x - first_x;
        else
          x = x - first_x;
        //printf("PC %d X={%d}U[%d,%d]\n",i,0,first_x,size_y-1);

      }
    }

    if(add){
      #ifdef DEBUG
        printf("  Adding as %d %d %d\n", x, y, z);
      #endif

      new_cell = create_cell(x, y, z);

      #ifdef DEBUG
        printf("    Cell created\n");
      #endif

      insert_cell(new_mini_world, new_cell);

      #ifdef DEBUG
        printf("    Cell inserted\n");
      #endif
    }

  }
  return new_mini_world;
}


void free_bounds(world *miniworld){

  printf("starting free bounds \n");
  for(int i=0; i<miniworld->size_y; i++){

    printf("freeing tree size_x = %d size_y = %d free bounds (0,%d) who is on index %d: \n", miniworld->size_x, miniworld->size_y, i, i);
    if(miniworld->cells[i] != NULL)
      print_bintree(miniworld->cells[i]);
    if(miniworld->cells[i] != NULL){
      destroy_bintree_nodes( miniworld->cells[i] );
      miniworld->cells[i] = NULL;
    }

    printf("freeing tree size_x = %d size_y = %d free bounds (%d,%d) who is on index %d:\n", miniworld->size_x, miniworld->size_y, miniworld->size_x-1, i, (miniworld->size_x-1)*miniworld->size_y + i);
    if(miniworld->cells[(miniworld->size_x-1)*miniworld->size_y + i] != NULL)
      print_bintree(miniworld->cells[(miniworld->size_x-1)*miniworld->size_y + i]);

    if(miniworld->cells[(miniworld->size_x-1)*miniworld->size_y + i] != NULL){
      destroy_bintree_nodes( miniworld->cells[(miniworld->size_x-1)*miniworld->size_y + i]);
      miniworld->cells[(miniworld->size_x-1)*miniworld->size_y + i] = NULL;
    }

  }

  printf("finish free bounds\n");


}



void sendbounds(world * miniworld){
  int aux=0;

  printf("ALLOCED lower_bound WITH SIZE %d\n", miniworld->n_alive_cells[1]*3);
  int *lower_bound = (int*)malloc(sizeof(int)*miniworld->n_alive_cells[1]*3);

  printf("ALLOCED upper_bound WITH SIZE %d\n", miniworld->n_alive_cells[miniworld->size_x-2]*3);
  int *upper_bound = (int*)malloc(sizeof(int)*miniworld->n_alive_cells[miniworld->size_x-2]*3);

  for(int j=0; j<miniworld->size_y; j++){
    printf("going for tree size_x = %d size_y = %d who is (%d,%d) who is on index %d: \n", miniworld->size_x, miniworld->size_y,1, j, miniworld->size_y+j);
    if(miniworld->cells[(miniworld->size_y)+j]!=NULL)
      print_bintree(miniworld->cells[miniworld->size_y+j]);

    if(miniworld->cells[(miniworld->size_y)+j]!=NULL)
      no_struct_bintree(miniworld->cells[miniworld->size_y+j], lower_bound, &aux);
  }

  aux=0;

  for(int j=0; j<miniworld->size_y; j++){

    printf("going for tree size_x = %d size_y = %d who is (%d,%d) who is on index %d: \n", miniworld->size_x, miniworld->size_y,miniworld->size_x-2, j, (miniworld->size_x-2)*miniworld->size_y+j);
    if(miniworld->cells[(miniworld->size_x-2)*miniworld->size_y+j]!=NULL)
      print_bintree(miniworld->cells[(miniworld->size_x-2)*miniworld->size_y+j]);

    if(miniworld->cells[(miniworld->size_x-2)*miniworld->size_y+j]!=NULL)
      no_struct_bintree(miniworld->cells[(miniworld->size_x-2)*miniworld->size_y+j], upper_bound, &aux);
  }
}

int main(int argc, char* argv[]){

  world *next_world;
  int * array_world, *array_world_indexes;
  int id, p;

  // if argc not expected, print program usage
  if(argc!=4){
    usage();
    exit(EXIT_FAILURE);
  }

  /*MPI_Init (&argc, &argv);
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
  MPI_Comm_size (MPI_COMM_WORLD, &p);*/
/*
  int size_x = 5;
  int size_y = 3;
  printf("SIZE_X = %d, SIZE_Y = %d\n", size_x, size_y);
  for(int i=0; i<size_x; i++)
    for(int j=0; j< size_y; j++)
      printf("(%d, %d) = %d\n", i, j, MATRIX_INDEX(i, j, size_y));

  size_x = 3;
  size_y = 5;
  printf("SIZE_X = %d, SIZE_Y = %d\n", size_x, size_y);
  for(int i=0; i<size_x; i++)
    for(int j=0; j< size_y; j++)
      printf("(%d, %d) = %d\n", i, j, MATRIX_INDEX(i, j, size_y));

  size_x = 5;
  size_y = 5;
  printf("SIZE_X = %d, SIZE_Y = %d\n", size_x, size_y);
  for(int i=0; i<size_x; i++)
    for(int j=0; j< size_y; j++)
      printf("(%d, %d) = %d\n", i, j, MATRIX_INDEX(i, j, size_y));
*/
 p = atoi(argv[3]);

  // handle file_name
  char *file_name = (char*)malloc(sizeof(char)*strlen(argv[1])+1);
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
/*
  #ifdef DEBUG
    printf("Reading file\n");
  #endif

  world * old_but_gold = file_to_world(file);
  printf("WHOLE WORLD:\n");
  print_world(old_but_gold);
  printf("\n\n");
*/

  world ** miniworlds = (world**)malloc(sizeof(world*)*p);
  world ** next_miniworlds = (world**)malloc(sizeof(world*)*p);

  for(int i=0; i<p; i++){
    rewind(file);
    miniworlds[i] = file_to_miniworld(file, p, i);
    print_world(miniworlds[i]);
  }

  for(int w=0; w<p; w++){

    free_bounds(miniworlds[w]);

  }

  for(int w=0; w<p; w++){

    sendbounds(miniworlds[w]);


  }

  for(int i=0; i<p; i++){
    printf("\nPC %d:\n", i);
    print_world(miniworlds[i]);
  }





/*
  int **mini_array_worlds = (int **)malloc(sizeof(int *) * p);
  int *size = (int *)malloc(sizeof(int) * p);



  world *old_but_gold = file_to_world(file);


  print_world(old_but_gold);


  array_world = (int *)malloc((old_but_gold->alive_cells)*3*sizeof(int));
  array_world_indexes = (int*)malloc(sizeof(int)*old_but_gold->size_x);


  #ifdef DEBUG
    printf("STARTING TO TRANSFORM WORLD INTO ARRAY\n");
  #endif
  no_struct_game(old_but_gold, array_world, array_world_indexes);
  #ifdef DEBUG
    printf("PRITING ARRAY WORLD NOW\n");
  #endif

  print_no_struct_game(array_world, array_world_indexes, old_but_gold->alive_cells, old_but_gold->size_x);
  distribute_payload(p, old_but_gold->size_x, old_but_gold->alive_cells, array_world, array_world_indexes, mini_array_worlds, size);

  world * miniworld = array_to_world(mini_array_worlds[0], old_but_gold->size_x,  NEEDED_BLOCK_SIZE(0,p,old_but_gold->size_x), size[0]/p);
  print_world(miniworld);



  // SEQUENTIAL


  #ifdef DEBUG
    printf("\nPrinting World\n");
    print_world(old_but_gold);
    printf("\nStarting to iterate\n");
  #endif

  for(int i=0; i<terations; i++){

    #if defined(DEBUG) || defined(ITERATION)
      printf("  Iteration number %d\n", i+1);
    #endif

    next_world = get_next_world(old_but_gold);

    #ifdef DEBUG
      printf("    Printing new world\n");
      print_world(next_world);
      printf("    Destroying previous world\n");
    #endif

    destroy_world(old_but_gold);
    old_but_gold = next_world;


  }


  #ifdef DEBUG
    printf("\nDestroying World\n");
    //print_tree_padding(old_but_gold->cells->root, 0 );
  #else
    print_world(old_but_gold);
  #endif

  destroy_world(old_but_gold);

  #ifdef DEBUG
    printf("Freeing other variables\n");
  #endif

  #ifdef TIMER
  double end = omp_get_wtime();
  printf("Total time = %lf\n", end-start);
  #endif

  fclose(file);
  free(file_name);
  exit(EXIT_SUCCESS);


  */




}
