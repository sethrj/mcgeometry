#!/usr/bin/python
## \file runTests.py
## \brief Execute all the unit tests in TranSupport.
## \author Seth R. Johnson
## 
## Run each unit test inside the /bin/test/ folder
## 

import sys, os, glob

##! Path name to the current directory
pathName = os.path.dirname(sys.argv[0])
if (len(sys.argv) > 1):
	pathName = sys.argv[1];

print "Looking for unit tests in <%s>" % pathName

os.chdir(pathName)

##! All of the unit tests we might like to run
testFiles = glob.glob('*/t*')

#print "All eligible files: " + ",".join(testFiles)
print "="*80

for file in testFiles:
	if (os.access(file, os.X_OK)):
		print "  Running test <%s>" % file
		print "="*80
		os.system(file);
		print "="*80
