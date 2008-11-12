#!/usr/bin/python

import os;

tests = ['tQuadric']

for file in tests:
	compileStr = 'g++ -I../../ %s.cpp -DDBC=7 -o %s' % (file, file)
	print compileStr
	os.system(compileStr)
