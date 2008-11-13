#!/usr/bin/python

import os;

tests = ['Plane','Sphere']

for file in tests:
	compileStr = 'g++ -I../../ t%s.cpp -Wall -DDBC=7 -o t%s' % (file, file)
	print compileStr
	os.system(compileStr)
