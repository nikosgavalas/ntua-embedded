import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from math import sqrt

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

x = []
y = []
val = []

with open('./out.csv', 'r') as f:
    f.readline()
    for line in f:
        tokens = list(map(int, line.strip().split(',')))
        x.append(tokens[0])
        y.append(tokens[1])
        val.append(tokens[2])

X = np.array(x)
Y = np.array(y)
Val = np.array(val)

ax.scatter(X, Y, Val)

ax.set_xlabel('Bx')
ax.set_ylabel('By')
ax.set_zlabel('Execution Time')

plt.show()
