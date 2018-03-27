#!/usr/bin/python3

from subprocess import check_output
from measure import get_avg

SIZES = ['1', '2', '4', '8', '16']

minTime = 999999999
size = ""

print("|Block Size|Time (us)|")
print("|:--------:|---------|")

for s in SIZES:
	time = get_avg(["./main", str(s), str(s)], 100)

	print("|" + s + "|" + str(time) + "|")

	if time < minTime:
		minTime = time
		size = str(s) + "x" + str(s)

print("Best:")
print("\tBlock Size:\t" + size)
print("\tExecution Time:\t" + str(minTime))