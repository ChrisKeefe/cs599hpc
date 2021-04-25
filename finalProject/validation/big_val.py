import numpy as np

# Build 2d array [[0, 1, 2, 3],
#                 [0, 1, 2, 3],
#                 [0, 1, 2, 3],
#                 [0, 1, 2, 3]]

dim = 4
if(False):
    a = np.arange(0, dim, 1)
    a = np.tile(a, (dim, 1))

    b = np.arange(0, dim, 1)
    b = np.tile(b, (dim, 1))
else:
# Build array [[0, 1, 2, 3],
#              [4, 5, 6, 7],
#              [8, ...
#              [...,    15]]
    a = np.arange(0, dim * dim, 1).reshape(dim, dim)
    b = np.arange(0, dim * dim, 1).reshape(dim, dim)

print(a)
print(b)

c = np.matmul(a, b)
global_sum = np.sum(c)
print(c)
print(global_sum)