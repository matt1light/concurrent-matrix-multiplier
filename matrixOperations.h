#define ROWS 4
#define COLUMNS 4
// declare matrix struct
struct Matrix {
  int rows;
  int columns;
  int values[ROWS][COLUMNS];
};


void fillMatrix(struct Matrix *matrix, int matrix_array[ROWS][COLUMNS]);

int getIndex(struct Matrix *matrix, int row, int column);

void setIndex(struct Matrix *matrix, int row, int column, int value);

void printMatrix(struct Matrix *matrix);
