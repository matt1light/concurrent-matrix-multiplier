#include <matrixOperations.h>
#include <stdio.h>

#define ROWS 4
#define COLUMNS 4
// matrix constructorish
// TODO is this reference right?
void fillMatrix(struct Matrix *matrix, int matrix_array[ROWS][COLUMNS]){
  matrix -> rows = ROWS ;
  matrix -> columns = COLUMNS;
  int i;
  int j;
  for (i = 0; i<ROWS; i++){
    for (j = 0; j<COLUMNS; j++){
      (matrix -> values)[i][j] = matrix_array[i][j];
    }
  }
}

int getIndex(struct Matrix *matrix, int row, int column){
  // get semaphore?
  // then get value
  // release semaphore?
  // release value
  //
  // For now all we do is return the value
  return (matrix -> values)[row][column];
}

void setIndex(struct Matrix *matrix, int row, int column, int value){
  // get semaphore
  // set the value of the matrix
  // release semaphore
  //
  // For now all we do is set the value
  (matrix -> values)[row][column] = value;
}

void printMatrix(struct Matrix *matrix){
  int i, j;
  for(i=0; i<matrix->rows; i++){
    for(j=0; j<matrix->columns; j++){
      printf("%d\t", getIndex(matrix, i, j));
    }
    printf("\n");
  }
  printf("\n\n");
}
