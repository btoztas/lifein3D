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

typedef struct _world{

  int alive_cells;
  int size;
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


node **create_bintree_hash(int size){

  node **new = (node**)calloc(size*size,sizeof(node*));

  return new;

}

world *create_world(int size){

  world *new       = (world*)malloc(sizeof(world));
  if(new == NULL)
    printf("Error allocating memory for a new world.\n");
  new->alive_cells = 0;
  new->size        = size;
  new->cells       = create_bintree_hash(size);

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
void destroy_bintree(node **tree, int size){

  for(int i=0; i<size*size; i++)
    if(tree[i]!=NULL)
      destroy_bintree_nodes(tree[i]);

  free(tree);
}

// function to free the game world
void destroy_world(world *game){

  destroy_bintree(game->cells, game->size);
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
  #ifdef DEBUG
    printf("\t  Height: %d", root->height);
  #endif
  print_cell(root->this);



  if(root->right != NULL){
    print_bintree(root->right);
  }

}

void print_bintree_hash(node **tree, int size){

  for(int i=0; i<size*size; i++)
    if(tree[i]!=NULL)
      print_bintree(tree[i]);

}


void print_world(world *game){

  #ifdef DEBUG
    printf("################################\n\tWorld Size: %d\n\tAlive Cells: %d\n\tPrinting Cells\n", game->size, game->alive_cells);
  #endif

  print_bintree_hash(game->cells, game->size);

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




node  *insert_bintree(node *root, cell *new_cell, int* n_cells){

    if(root == NULL){

      root = create_bintree_node(new_cell);
      (*n_cells)++;

    }else if(compare_cells(new_cell, root->this)==0){
      #ifdef DEBUG
      printf("\t\t\t\t\t\t\t\t\tCELL ALREADY EXISTS\n");
      #endif
      destroy_cell(new_cell);
      return root;
    }else if(compare_cells(new_cell, root->this)==1){

      root->right = insert_bintree(root->right, new_cell, n_cells);

    }else{

      root->left = insert_bintree(root->left, new_cell, n_cells);

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

  
  game->cells[new_cell->x * game->size + new_cell->y] = insert_bintree(game->cells[new_cell->x * game->size + new_cell->y], new_cell, &(game->alive_cells));

  #ifdef DEBUG
    printf("      Inserted in tree\n"); fflush(stdout);
  #endif


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


int check_alive(int size, int x, int y, int z, node **tree ){

  node *aux = tree[x * size + y];
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

  if(x+1 == game->size) {
    if(check_alive(game->size, 0,y, z, game->cells)) live++;
  } else {
    if(check_alive(game->size, x+1,y, z, game->cells)) live++;
  }

  if(x-1 == -1) {
    if(check_alive(game->size, game->size-1,y, z, game->cells)) live++;
  } else {
    if(check_alive(game->size, x-1,y, z, game->cells)) live++;
  }

  if(y+1 == game->size) {
    if(check_alive(game->size, x,0, z, game->cells)) live++;
  } else {
    if(check_alive(game->size, x,y+1, z, game->cells)) live++;
  }

  if(y-1 == -1) {
    if(check_alive(game->size, x,game->size-1, z, game->cells)) live++;
  } else {
    if(check_alive(game->size, x,y-1, z, game->cells)) live++;
  }

  if(z+1 == game->size) {
    if(check_alive(game->size, x,y,0, game->cells)) live++;
  } else {
    if(check_alive(game->size, x,y,z+1, game->cells)) live++;
  }

  if(z-1 == -1) {
    if(check_alive(game->size, x,y,game->size-1, game->cells)) live++;
  } else {
    if(check_alive(game->size, x,y,z-1, game->cells)) live++;
  }

  if(status == -1)
    status = check_alive(game->size, x,y,z, game->cells);
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

  if(x+1 == actual_world->size) {
    if(check_alive(actual_world->size,0,y, z, actual_world->cells)) {

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
    if(check_alive(actual_world->size,x+1,y, z, actual_world->cells)) {

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
    if(check_alive(actual_world->size,actual_world->size-1,y, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(actual_world->size-1,y, z, actual_world, 0)){
        new_cell = create_cell(actual_world->size-1,y, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", actual_world->size-1,y, z);
        #endif
      }
    }
  } else {
    if(check_alive(actual_world->size,x-1,y, z, actual_world->cells)) {

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

  if(y+1 == actual_world->size) {
    if(check_alive(actual_world->size,x,0, z, actual_world->cells)) {

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
    if(check_alive(actual_world->size,x,y+1, z, actual_world->cells)) {

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
    if(check_alive(actual_world->size,x,actual_world->size-1, z, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,actual_world->size-1, z, actual_world, 0)){
        new_cell = create_cell(x,actual_world->size-1, z);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,actual_world->size-1, z);
        #endif
      }
    }
  } else {
    if(check_alive(actual_world->size,x,y-1, z, actual_world->cells)) {

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

  if(z+1 == actual_world->size) {
    if(check_alive(actual_world->size,x,y,0, actual_world->cells)) {

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
    if(check_alive(actual_world->size,x,y,z+1, actual_world->cells)) {

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
    if(check_alive(actual_world->size,x,y,actual_world->size-1, actual_world->cells)) {

      live++;

    }else{
      if(test_cell(x,y,actual_world->size-1, actual_world, 0)){
        new_cell = create_cell(x,y,actual_world->size-1);
        insert_cell(next_world, new_cell);

        #ifdef DEBUG
        printf("\t\t\t\t\t\t INSERTED DEAD CELL %d %d %d\n", x,y,actual_world->size-1);
        #endif
      }
    }
  } else {
    if(check_alive(actual_world->size,x,y,z-1, actual_world->cells)) {

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
  world *next_world = create_world(actual_world->size);

  #ifdef DEBUG
    printf("    Testing cells\n");
  #endif

 //AQUI!!
  if(actual_world->alive_cells * 6 * 6 < 6*(actual_world->size)*(actual_world->size)*(actual_world->size)){
    #ifdef ITERATION
    printf("   Choose live cells\n");
    #endif
    for(int x=0; x<actual_world->size; x++)
      for(int y=0; y<actual_world->size; y++)
        if(actual_world->cells[x * actual_world->size + y] != NULL){
          #ifdef DEBUG
          printf("\t\t\t\tTESTING SUBTREE %d %d\n", x, y);
          #endif
          //this function will analyzer every node of the subtree, and add to the new world the cells
          solve_subtree(actual_world->cells[x * actual_world->size + y], actual_world, next_world);

        }
    #ifdef DEBUG
    printf("\t\t\tFINISHED SUBTREE TESTS\n");
    #endif

  }else{
    #ifdef ITERATION
    printf("    Choose N^3\n");
    #endif
    for(int x=0; x<next_world->size; x++)
      for(int y=0; y<next_world->size; y++)
        for(int z=0; z<next_world->size; z++){

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




int main(int argc, char* argv[]){

  world *next_world;

  // if argc not expected, print program usage
  if(argc!=3){
    usage();
    exit(EXIT_FAILURE);
  }

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

    #if defined(DEBUG) || defined(ITERATION)
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
    //print_tree_padding(actual_world->cells->root, 0 );
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
