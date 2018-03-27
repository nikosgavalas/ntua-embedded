#!/usr/bin/python3

import sys
from subprocess import check_output
from measure import get_avg

# N = 144
SIZES_X = [1, 2, 3, 4, 6, 8, 9, 12, 16, 18, 24, 36, 48, 72, 144]

# M = 176
SIZES_Y = [1, 2, 4, 8, 11, 16, 22, 44, 88, 176]

minTime = 999999999
size = ""

print("|Block Size|Time (us)|Block Size|Time (us)|Block Size|Time (us)|")
print("|:--------:|---------|:--------:|---------|:--------:|---------|")

lines = []
column_height = (len(SIZES_X) * len(SIZES_Y)) / 3

i = 0
for s1 in SIZES_X:
	for s2 in SIZES_Y:

		time = get_avg(["./main", str(s1), str(s2)], 100)

		line = "|" + str(s1) + "x" + str(s2) + "|" + str(time)
		l_index = int(i % column_height)

		if len(lines) < column_height:
			lines.append(line)
		else:
			lines[l_index] = lines[l_index] + line

		if time < minTime:
			minTime = time
			size = str(s1) + "x" + str(s2)
		
		i += 1

for line in lines:
	print(line + "|")
	sys.stdout.flush()

print("Best:")
print("\tBlock Size:\t" + size)
print("\tExecution Time:\t" + str(minTime))