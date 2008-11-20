# meshTimingTest.py

"""
This module will run meshExample several times to see how the number of cells
affect the run time.
"""

import sys
import subprocess
import numpy
import re
import math

import matplotlib.pyplot as pyplot

class meshTime(object):

    def __init__(self):
        object.__init__(self)



    def RunAndParse(self, N):
        """
        """
        reg = '\s+(\d*\.?\d+)ms'

        self.NumCells = []
        self.MeshTimes = []
        self.ThirdSweep = []
        self.SecondSweep = []
        self.FirstSweep = []

        for n in xrange(N):
            cmd = "../bin/meshExample %i" %(n+1)
            print cmd

            # Run meshExample and get output
            P = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
            S = P.communicate()

            # Parse output to find numbers
            data = re.findall(reg, S[0])

            # Collect data
            self.NumCells.append(math.pow((n+1),3) )
            self.MeshTimes.append(data[0])
            self.ThirdSweep.append(data[1])
            self.SecondSweep.append(data[2])
            self.FirstSweep.append(data[3])

if __name__ == '__main__':

    N = int(sys.argv[1])
    mT = meshTime()
    mT.RunAndParse(N)

    pyplot.figure()
    pyplot.plot(mT.NumCells, mT.MeshTimes, label="Mesh Creation")
    pyplot.plot(mT.NumCells, mT.FirstSweep, label="First Sweep")
    pyplot.plot(mT.NumCells, mT.SecondSweep, label="Second Sweep")
    pyplot.plot(mT.NumCells, mT.ThirdSweep, label="Third Sweep")
    pyplot.legend(loc=2)
    pyplot.xlabel("Number of cells")
    pyplot.ylabel("Run time (ms)")

