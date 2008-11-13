#!/usr/bin/python

import os;

tests = ['MCGeometry']

for file in tests:
	compileStr = 'g++ -I../ ../*.cpp t%s.cpp -DDBC=7 -o t%s' % (file, file)
	print compileStr
	os.system(compileStr)
