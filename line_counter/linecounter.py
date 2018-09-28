#!/usr/bin/env python3
import subprocess
import argparse
import os
import sys
'''
Accumulates and prints number of lines of supplied filetypes in directory
author: ramonnr@kth.se
Free to use, redistribute and modify
'''

parser = argparse.ArgumentParser(description = 'Returns total number of lines for all given filetypes in directory')
parser.add_argument('--dir', type=str, default = '.', dest = 'directory',
	help = 'Destination to read from, use, default is current directory')
parser.add_argument('filetypes', metavar = 'ftypes', type=str, nargs = '+',
	help = 'Type of file(s) to check linecount in')
args = parser.parse_args()

types = args.filetypes
directory = args.directory
#sanitize input 
for t in types:
	if not t.startswith('.'):
		t = '.' + t
if not directory.endswith('/'):
	directory += '/'
if not os.path.isdir(directory):
	print("Supplied directory ({}) a directory :(".format(directory))
	sys.exit(-1)

n_lines = 0
for f in os.listdir(directory):
	for t in types: 
		if f.endswith(t):
			with open(directory + f,'r') as currfile:
				n_lines += len(currfile.readlines())
		continue

print(n_lines)