#!/usr/bin/env python
## \file runTests.py
## \brief Execute all the unit tests in TranSupport.
## \author Seth R. Johnson
## 
## Run each unit test inside the /bin/test/ folder
## 

import sys, os, subprocess, glob, re

##! Path name to the current directory
pathName = os.path.dirname(sys.argv[0])
if (len(sys.argv) > 1):
    print "Current directory: ", os.getcwd()
    pathName = sys.argv[1]


outputFileName = 'testresults.txt'

print "Looking for unit tests in <%s>" % pathName

os.chdir(pathName)

totalExeCounts  = 0
totalPassCounts = 0

##! All of the unit tests we might like to run
testFiles  = glob.glob('t*')
testFiles += glob.glob('*/t*')

OUTPUT = open(outputFileName,'w')

print >>OUTPUT, "="*80

for file in testFiles:
    if (os.path.isfile(file) and os.access(file, os.X_OK)):
        totalExeCounts += 1
        print >>OUTPUT, "  Running test <%s>" % file
        print >>OUTPUT, "="*80

        passedCount = 0
        failedCount = 0

        try:
            results = subprocess.Popen(file,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT).communicate()[0]
        except BaseError, e:
            print "   problem executing test %s: %s" % (file, str(e))
            print "Unit test <%s> FAILED" % file
            continue

        OUTPUT.writelines(results)
        for line in results.split('\n'):
            if re.match('  PASSED:', line):
                passedCount += 1
            if re.match('Segmentation fault', line):
                failedCount += 1
                print '  /---- Unit test %s segfaulted!' % file
            if re.match('  FAILED:', line):
                failedCount += 1
                print '  /---- Unit test %s failed' % file
            if (re.match(r'  (\|  |\+--)', line)):
                print line.rstrip()

        print >>OUTPUT, "="*80

        if (failedCount > 0):
            print "Unit test <%s> FAILED" % file
        elif (passedCount > 0):
            print "Unit test <%s> PASSED" % file
            totalPassCounts += 1
        else:
            print "Unit test <%s> had no tests!" % file

OUTPUT.close()
print "%d/%d unit tests passed." % (totalPassCounts, totalExeCounts)
if totalPassCounts != totalExeCounts:
    print "SOMETHING FAILED. Way to screw up my code!"
    print "see details in %s/%s" % (pathName, outputFileName)
