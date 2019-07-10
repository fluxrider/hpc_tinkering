#!/usr/bin/env python
# mpirun -np 3 mandel.py 5
# view.py mandel.out
from mpi4py import MPI
import sys
import os
import numpy as np
import cmath # -1.0+1.0j

comm = MPI.COMM_WORLD
comm.Set_errhandler(MPI.ERRORS_ARE_FATAL)
rank = comm.Get_rank()
size = comm.Get_size()

REZ = int(sys.argv[1])
N = 20 * REZ

def mandelbrot(b):
  z = 0.0
  for i in range(1, 1001):
    z = (z * z) + b
    if abs(z) > 2: break
  return i

assert(mandelbrot(-1+1j) == 3)
assert(mandelbrot(-0.5+0.5j) == 1000)

# delete file (TODO figure out overwrite flag in Open)
if rank == 0:
  try: MPI.File.Delete("mandel.out")
  except: pass
comm.Barrier()

f = MPI.File.Open(comm, "mandel.out", MPI.MODE_WRONLY|MPI.MODE_CREATE)

data = np.empty(N, dtype=np.uint16)
for row in range(rank,N,size):
  for col in range(N):
    c = -1.5+0.1/REZ*col+(-1.0+0.1/REZ*row)*1j
    data[col] = mandelbrot(c)
  f.Write_at(row * data.nbytes, data)

print(f"rank {rank} done")
comm.Barrier()

f.Close()
MPI.Finalize()
