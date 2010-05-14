srj_make_test(
  TESTS      
    tCell
    tCylinder
    tCylinderNormal
    tMCGeometry
    tPlane
    tPlaneNormal
    tSphere 
  DEPENDS    transupport mcgeometry
  SUBPROJECT mcgeometry)
