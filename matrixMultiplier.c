#include <stdio.h>
#include <unistd.h>
#include "matrixOperations.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <time.h>

//TODO this is in the first version
// the number of processes that should be used to execute do the matrix multiplication
//#define PROCESSES 4
//TODO end
// the number of rows in matrix a, will be the number of rows in the result
#define A_ROWS 4
// the number of columns in matrix a and number of rows in matrix b: must be the same for multiplication
#define A_COLUMNS_B_ROWS 4
// the number of columns in matrix b, will be the number of columns in the result
#define B_COLUMNS 4

// declaring prototypes
int getValue(int row, int column);
void calculateRow(struct Matrix *matrixA, struct Matrix *matrixB, struct Matrix *matrixResult, int row);
void calculateRows(struct Matrix *matrixA, struct Matrix *matrixB, struct Matrix *matrixResult, int row, int num_processes, int num_rows);


// struct to hold the matrices that we will be operating on
struct MatrixCollection {
  struct Matrix matrixA;
  struct Matrix matrixB;
  struct Matrix matrixResult;
};

// initialize array a
int matrix_a_array[A_ROWS][A_COLUMNS_B_ROWS]  = {
  {1, 2, 3, 4},
  {5, 6, 7, 8},
  {4, 3, 2, 1},
  {8, 7, 6, 5}
};

// initialize array b
int matrix_b_array[A_COLUMNS_B_ROWS][B_COLUMNS]  = {
  {1, 3, 5, 7},
  {2, 4, 6, 8},
  {7, 3, 5, 7},
  {8, 6, 4, 2}
};

// initialize results matrix
int matrix_result_array[A_ROWS][B_COLUMNS];

int main(int argc, char **argv){
  //TODO this is in the second version
  if(!argv[1]){
    printf("An argument specifying the number of processes assigned to solving the matrix is required");
    exit(0);
  }
  int PROCESSES = atoi(argv[1]);
  // TODO end
  int shmid;
  void *shared_memory = (void *)0;
  struct MatrixCollection *matrices;
  struct Matrix *matrixA, *matrixB, *matrixResult;
  pid_t pid;

  // start clock to calculate how long the actual multiplication takes
  clock_t begin = clock();

  // First we must assign shared memory for all of the matrices
  // allocate shared memory and return id
  shmid = shmget((key_t) 1234, sizeof(struct MatrixCollection), 0666 | IPC_CREAT);
  // check for errors
  if (shmid == -1){
    fprintf(stderr, "shmget failed\n");
    exit(EXIT_FAILURE);
  }

  // attach shared memory segment
  shared_memory = shmat(shmid, (void*)0, 0);
  // check for errors
  if (shared_memory == -1){
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }

  // put the matrix collection struct in the shared memory segment
  // typecast shared memory segment into a matrix struct
  matrices = (struct MatrixCollection *)shared_memory;
  matrixA = &(matrices -> matrixA);
  matrixB = &(matrices -> matrixB);
  matrixResult = &(matrices -> matrixResult);

  //fill each matrix with the default values
  fillMatrix(matrixResult, matrix_result_array);
  fillMatrix(matrixA, matrix_a_array);
  fillMatrix(matrixB, matrix_b_array);

  printf("Calculating the product of\nMatrix A:\n");
  printMatrix(matrixA);
  printf("and Matrix B:\n");
  printMatrix(matrixB);


  // i will be a counter for each of the child processes
  int i;
  // status is used to monitor whether or not all child processes have exited
  int status = 0;
  // Make the number of child processes specified
  for(i = 0; i<PROCESSES; i++){
    // create a child process
    pid_t pid = fork();
    // switch to separate child and parent behavior
    switch(pid){
      case 0:
        // TODO this one is for task 2
        // child process will calculate the values for certain rows in the results matrix
        calculateRows(matrixA, matrixB, matrixResult, i, PROCESSES, A_ROWS);
        //TODO end
        // TODO this one is for task 1
        // child process will calculate the values for one row in the results matrix
        // calculateRow(matrixA, matrixB, matrixResult, i);
        // TODO end
        // exit child
        exit(0);
      default:
        break;
    }
  }

  // wait for all the children to exit
  while ((pid = wait(&status))>0);
  // stop clock
  clock_t end = clock();

  // print the resultant matrix
  printf("The resulting matrix is:\n");
  printMatrix(matrixResult);

  // calculate the amount of time for execution
  double time_spent = (double)(end - begin)/CLOCKS_PER_SEC;
  // print execution time
  printf("The program took %f to execute", time_spent);

  // release the shared memory
  if (shmdt(shared_memory) == -1){
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  if (shmctl(shmid, IPC_RMID, 0) == -1){
    fprintf(stderr, "shmctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * Function that calculates one row of the output matrix
 * @param *matrixA
 * @param *matrixB
 * @param *resultMatrix
 * @param *row
 */
void calculateRows(struct Matrix *matrixA, struct Matrix *matrixB, struct Matrix *matrixResult, int process, int num_processes, int num_rows){
  int i;
  for (i=0; i<num_rows; i+=num_processes){
    calculateRow(matrixA, matrixB, matrixResult, i);
  }
}

void calculateRow(struct Matrix *matrixA, struct Matrix *matrixB, struct Matrix *matrixResult, int row){
  int i, j, k;
  int val1, val2, sum;
  for(k=0; k<matrixB->columns; k++){
    for(i=0; i<matrixA->rows; i++){
      sum = 0;
      for(j=0; j<matrixB->rows; j++){
        val1 = getIndex(matrixA, i, j);
        val2 = getIndex(matrixB, j, k);
        sum += (val1*val2);
      }
      setIndex(matrixResult, i, k, sum);
    }
  }
}
