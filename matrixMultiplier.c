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
//#define PROCESSES 4
#define AROWS 4
#define ACOLUMNS_BROWS 4
#define BCOLUMNS 4

struct MatrixCollection {
  struct Matrix matrixA;
  struct Matrix matrixB;
  struct Matrix matrixResult;
};


// initialize array a
int matrix_a_array[AROWS][ACOLUMNS_BROWS]  = {
  {1, 2, 3, 4},
  {5, 6, 7, 8},
  {4, 3, 2, 1},
  {8, 7, 6, 5}
};

// initialize array b
int matrix_b_array[ACOLUMNS_BROWS][BCOLUMNS]  = {
  {1, 3, 5, 7},
  {2, 4, 6, 8},
  {7, 3, 5, 7},
  {8, 6, 4, 2}
};

// initialize results matrix
int matrix_result_array[AROWS][BCOLUMNS];

int getValue(int row, int column);
void calculateRow(struct Matrix *matrixA, struct Matrix *matrixB, struct Matrix *matrixResult, int row);
void calculateRows(struct Matrix *matrixA, struct Matrix *matrixB, struct Matrix *matrixResult, int row, int num_processes, int num_rows);

int main(int argc, char **argv){
  clock_t begin = clock();
  //TODO this is in the second version
  int PROCESSES = atoi(argv[1]);
  // TODO end
  int shmid;
  void *shared_memory = (void *)0;
  struct MatrixCollection *matrices;
  struct Matrix *matrixA, *matrixB, *matrixResult;

  pid_t pid;
  //First we create the shared result matrix

  // get shared memory id
  shmid = shmget((key_t) 1234, sizeof(struct MatrixCollection), 0666 | IPC_CREAT);
  // check for error
  if (shmid == -1){
    fprintf(stderr, "shmget failed\n");
    exit(EXIT_FAILURE);
  }

  // attach shared memory segment
  shared_memory = shmat(shmid, (void*)0, 0);
  // check for error
  if (shared_memory == -1){
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }

  // put the matrices struct in the shared memory segment
  // typecast shared memory segment into a matrix struct
  matrices = (struct MatrixCollection *)shared_memory;
  matrixA = &(matrices -> matrixA);
  matrixB = &(matrices -> matrixB);
  matrixResult = &(matrices -> matrixResult);

  //fill the matrix with the default values
  fillMatrix(matrixResult, matrix_result_array);
  fillMatrix(matrixA, matrix_a_array);
  fillMatrix(matrixB, matrix_b_array);

  // controlling the flow
  int i;
  int status = 0;
  //for(i = 0; i<PROCESSES; i++){
  for(i = 0; i<PROCESSES; i++){
    pid_t pid = fork();
    switch(pid){
      case 0:
        // TODO this one is for task 2
        calculateRows(matrixA, matrixB, matrixResult, i, PROCESSES, AROWS);
        // TODO this one is for task 1
        // calculateRow(matrixA, matrixB, matrixResult, i);
        // exit child
        exit(0);
      default:
        break;
    }
  }

  while ((pid = wait(&status))>0);
  clock_t end = clock();
  printMatrix(matrixResult);
  double time_spent = (double)(end - begin)/CLOCKS_PER_SEC;

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

