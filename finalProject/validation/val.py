import numpy as np

a = np.array([[0, 1, 2, 3],
              [4, 5, 6, 7],
              [8, 9, 10, 11],
              [12, 13, 14, 15]])

b = np.array([[2, 1, 2, 1],
              [1, 1, 2, 1],
              [2, 1, 2, 2],
              [1, 2, 2, 2]])

print(a)
print(b)
c = np.matmul(a, b)
global_sum = np.sum(c)
print(c)
print(global_sum)