#!/usr/bin/python
## \file runTests.py
## \brief Execute all the unit tests in TranSupport.
## \author Seth R. Johnson
## 
## Run each unit test inside the /bin/test/ folder
## 

import sys, os, glob

pathName = os.path.dirname(sys.argv[0])
print "Looking for unit tests in <%s>" % pathName

os.chdir(pathName)

testFiles = glob.glob('*/t*')
execFiles = [];

#print "All eligible files: " + ",".join(testFiles)
print "="*80

for file in testFiles:
	if (os.access(file, os.X_OK)):
		print "   Running test <%s>" % file
		print "="*80
		os.system(file);
		print "="*80
