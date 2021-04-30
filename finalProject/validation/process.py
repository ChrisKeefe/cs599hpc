import numpy as np

a0 = np.array([[0, 1, 2],
               [7, 8, 9],
               [14, 15, 16]])
b0 = np.array([[0, 7, 14],
               [6, 13, 20],
               [12, 19, 26]])
print(a0)
print(b0)
c0 = np.matmul(a0, b0)
# print(c0)

print()

a1 = np.array([[3, 4, 5],
               [10, 11, 6],
               [17, 12, 13]])
b1 = np.array([[21, 28, 35],
               [27, 34, 5],
               [33, 4, 11]])
print(a1)
print(b1)
c1 = np.matmul(a1, b1)
# print(c1)

print()

a2 = np.array([[21, 22, 23],
               [28, 29, 24],
               [35, 30, 31]])
b2 = np.array([[18, 25, 32],
               [24, 31, 2],
               [30, 1, 8]])
print(a2)
print(b2)
c2 = np.matmul(a2, b2)
# print(c2)

print()

a3 = np.array([[18, 19, 20],
               [25, 26, 27],
               [32, 33, 34]])
b3 = np.array([[3, 10, 17],
               [9, 16, 23],
               [15, 22, 29]])
print(a3)
print(b3)
c3 = np.matmul(a3, b3)
# print(c3)

print()

a = [a0, a1, a2, a3]
b = [b0, b1, b2, b3]

tmp = []

# BORKED
for idx, submatrix in enumerate(a):
    tmp.append(submatrix[:, 0:1])
    submatrix = np.roll(submatrix, -1, axis=1)

for idx, submatrix in enumerate(a):
    left_idx = (idx + len(a) - 1) % len(a)
    a[left_idx][:, 2:3] = tmp[idx]
    
for submatrix in a:
    print(submatrix)
    print()

s0 = np.sum(c0)
s1 = np.sum(c1)
s2 = np.sum(c2)
s3 = np.sum(c3)
global_sum = sum([s0, s1, s2, s3])
print(global_sum)