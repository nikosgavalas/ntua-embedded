#!/usr/bin/python3

import sys
from subprocess import check_output

def get_avg(cmd, times):	
	s = 0
	for i in range(times):
		time = int(check_output(cmd).decode())
		s += time
	return int(s / times)

if __name__ == '__main__':
	cmd = [
		'./' + sys.argv[1],
		sys.argv[2],
		sys.argv[3]
	]
	times = int(sys.argv[4])

	print(get_avg(cmd, times))