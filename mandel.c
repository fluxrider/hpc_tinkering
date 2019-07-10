// mpicc mandel.c -lm
// mpirun -np 4 a.out 20
// view.py mandel.out
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <complex.h>
#include <mpi.h>

int mandelbrot(double complex b) {
  double complex z = 0.0 + 0.0 * I;
  int i = 1;
  while(i < 1000) {
    z = (z * z) + b;
    if(cabs(z) > 2.0) break;
    i++;
  }
  return i;
}

int main(int argc, char * argv[]) {
  if(argc < 2) {
    printf("Give me command line argument dude\n");
    return 1;
  }
  int REZ = atoi(argv[1]);
  int N = 20 * REZ;

  int rank;
  int size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  printf("r: %d/%d\n", rank, size);

  if(rank == 0) {
    MPI_File_delete("mandel.out", MPI_INFO_NULL);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_File file;
  MPI_File_open(MPI_COMM_WORLD, "mandel.out", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);

  uint16_t data[N];
  int data_nbytes = N * 2;
  for(int row = rank; row < N; row += size) {
    for(int col = 0; col < N; col++) {
      double complex c = -1.5+0.1/REZ*col+(-1.0+0.1/REZ*row)*I;
      data[col] = mandelbrot(c);
    }
    MPI_File_write_at(file, row * data_nbytes, data, data_nbytes, MPI_UNSIGNED_CHAR,  MPI_STATUS_IGNORE);
  }

  printf("r: %d done\n", rank);
  MPI_File_close(&file);
  MPI_Finalize();
  return 0;
}
