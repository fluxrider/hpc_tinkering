#!/usr/bin/python
import sys
import numpy as np
import matplotlib.pyplot as mp
import math

data = np.fromfile(file=sys.argv[1], dtype='uint16')
N = int(math.sqrt(data.shape[0]))
data = data.reshape(N,N)

#print(data)
image = mp.imshow(data)
mp.show(image)
