# meshComparison.py

"""
"""

import sys
import subprocess
import numpy
import re
import math

import matplotlib.pyplot as pyplot

class meshComp(object):

    def __init__(self):
        object.__init__(self)

    def RunAndParse(self, N):
        """
        """
        reg = '\s+(\d*\.?\d+)ms'

        self.NumCells = []
        self.MeshCreate = []
        self.CombMesh1 = []
        self.CombMesh2 = []
        self.DetMesh = []

        for n in xrange(N):
            cmd = "../bin/meshComparison %i 1000000 0" %(n+1)
            print cmd

            # Run meshExample and get output
            P = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
            S = P.communicate()

            # Parse output to find numbers
            data = re.findall(reg, S[0])

            # Collect data
            self.NumCells.append(math.pow((n+1),3) )
            self.MeshCreate.append(data[0])
            self.CombMesh1.append(data[1])
            self.CombMesh2.append(data[2])
            self.DetMesh.append(data[3])

if __name__ == '__main__':

    N = int(sys.argv[1])
    mC = meshComp()
    mC.RunAndParse(N)

    pyplot.figure()
    pyplot.plot(mC.NumCells, mC.MeshCreate, label="Mesh Creation")
    pyplot.plot(mC.NumCells, mC.CombMesh1, label="Combinatorial Mesh 1")
    pyplot.plot(mC.NumCells, mC.CombMesh2, label="Combinatorial Mesh 2")
    pyplot.plot(mC.NumCells, mC.DetMesh, label="Deterministic Mesh")
    pyplot.legend(loc=2)
    pyplot.xlabel("Number of cells")
    pyplot.ylabel("Run time (ms)")
