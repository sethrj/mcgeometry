Current version: 1.1 (feature-complete with simple geometries; could use some work)

---


This project provides a combinatorial geometry (Constructive Solid Geometry) library for use in Monte Carlo particle transport applications. MCGeometry is built with both speed and flexibility in mind.

**Where to start**: download the [source code](http://mcgeometry.googlecode.com/files/source-v1.1.tgz) and doxygen-generated [documentation](http://mcgeometry.googlecode.com/files/documentation-v1.1.tgz). Use [CMake](http://www.cmake.org/cmake/resources/software.html) to build the software as described in the README file.

Current capabilities:
  * Generalized cells bounded by planes, cylinders, and spheres
  * Automatic cell-to-cell connectivity
  * Completely lossless particle transport with no "soft equivalence" checking
  * Constant-time operation after geometry is connected (no global searching)
  * Uses zero-based `unsigned int` internal index to refer to cells for constant-time connectivity with whatever cross section/tally code the developer wishes to use, but translates the internal index to "User ID"s.
  * Requires the [Blitz++](http://sourceforge.net/project/showfiles.php?group_id=63961) library to store direction/position/geometry vectors.
    * Note: if using GCC 4.3 to compile, be sure to use the updated version of Blitz hosted through CVS at [SourceForge](http://sourceforge.net/scm/?type=cvs&group_id=63961), not the last-published version 0.9.
    * To check out blitz, use the anonymous cvs with blank password:
```
cvs -d:pserver:anonymous@blitz.cvs.sourceforge.net:/cvsroot/blitz login 
cvs -z3 -d:pserver:anonymous@blitz.cvs.sourceforge.net:/cvsroot/blitz co -P blitz
```

![http://mcgeometry.googlecode.com/svn/trunk/doc/frontpage/spheres.png](http://mcgeometry.googlecode.com/svn/trunk/doc/frontpage/spheres.png)
![http://mcgeometry.googlecode.com/svn/trunk/doc/frontpage/tricky_geometry.png](http://mcgeometry.googlecode.com/svn/trunk/doc/frontpage/tricky_geometry.png)


---


The following statements apply to work done by Seth R. Johnson on this project:
> This material is based upon work supported under a National Science Foundation Graduate
> Research Fellowship. Any opinions, findings, conclusions or recommendations expressed in
> this publication are those of the author(s) and do not necessarily reflect the views of
> the National Science Foundation.