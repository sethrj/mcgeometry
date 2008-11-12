#!/usr/bin/python

import os;

tests = ['tVectorComp', 'tVectorMath']

for file in tests:
	compileStr = 'g++ -I../../ %s.cpp -o %s' % (file, file)
	print compileStr
	os.system(compileStr)
