/*
 * \file generateMeshGeom.cpp
 * \brief Uses a bunch of planes to make a square grid of cells
 *
 * The purpose of this file is to show that by putting the geometry generation
 * in a separate cpp file, the actual monte carlo code never has to use the
 * surface/plane/cell headers at all.
 */

#include "createGeometry.hpp"

#include "mcgeometry/MCGeometry.hpp"
#include "mcgeometry/Plane.hpp"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using namespace mcGeometry;

//! Mesh will create and use a retangular mesh using N planes spaced 1.0 units
// apart
void CreateMesh(int N, MCGeometry& Geo){
    // Normal vectors to planes perpendicular to axes
    std::vector<double> xNorm(3, 0.0);  xNorm[0] = 1.0;
    std::vector<double> yNorm(3, 0.0);  yNorm[1] = 1.0;
    std::vector<double> zNorm(3, 0.0);  zNorm[2] = 1.0;


    const unsigned int minXId = 1;
    const unsigned int minYId = N+2;
    const unsigned int minZId = 2*N+3;

    const unsigned int maxXId = N+1;
    const unsigned int maxYId = 2*N+2;
    const unsigned int maxZId = 3*N+3;

    // Create global bounding planes
    mcGeometry::PlaneX minX(0);
    Geo.addSurface(minXId, minX);
    mcGeometry::PlaneY minY(0);
    Geo.addSurface(minYId, minY);
    mcGeometry::PlaneZ minZ(0);
    Geo.addSurface(minZId, minZ);

    // Point is now corner opposite from origin
    mcGeometry::PlaneX maxX(N);
    Geo.addSurface(N+1, maxX);
    
    mcGeometry::PlaneY maxY(N);
    Geo.addSurface(2*(N+1), maxY);

    mcGeometry::PlaneZ maxZ(N);
    Geo.addSurface(3*(N+1), maxZ);

    cout << "Global bounding box defined by following surfaces:\n"
         << minX << "\n"<< minY << "\n"<< minZ << "\n"
         << maxX << "\n"<< maxY << "\n"<< maxZ << endl;

    // yn and zn are the first index of each surface along one of the axes
    unsigned int yn = N+1;
    unsigned int zn = 2*(yn);

    // Create interior mesh
    for( int n = 2; n <= N; ++n){
        mcGeometry::PlaneX PX(n - 1.0);
        Geo.addSurface(n, PX);
        mcGeometry::PlaneY PY(n - 1.0);
        Geo.addSurface(n+yn, PY);
        mcGeometry::PlaneZ PZ(n - 1.0);
        Geo.addSurface(n+zn, PZ);
    }

    // Turn interior mesh into cells
    unsigned int ID = 0;
    std::vector<int> Surfaces(6, 0);

    cout << endl;
    for( int k = 0; k < N; ++k ){
        for( int j = 0; j < N; ++j ){
            for( int i = 0; i < N; ++i ){
                Surfaces[0] = i+1;
                Surfaces[1] = -1*(i+2);
                Surfaces[2] = (N+1) + (j+1);
                Surfaces[3] = -1*( (N+1) + (j+2) );
                Surfaces[4] = 2*(N+1) + (k+1);
                Surfaces[5] = -1*( 2*(N+1) + (k+2) );

                Geo.addCell(ID, Surfaces);
                ++ID;
            }
        }
    }

    if (true) {
        // use a negated outside cell
        typedef mcGeometry::MCGeometry::CellT CellT;

        Surfaces.resize(0);

        Surfaces.push_back( minXId);
        Surfaces.push_back(-maxXId);
        Surfaces.push_back( minYId);
        Surfaces.push_back(-maxYId);
        Surfaces.push_back( minZId);
        Surfaces.push_back(-maxZId);
        Geo.addCell(ID, Surfaces, CellT::generateFlags(true, true)); 
        cout << "Outside cell ID: " << ID << endl;
        ++ID;

//    Geo.debugPrint();
    } else {
        // Create individual cells defining every thing outside of mesh
        Surfaces.resize(1);
        Surfaces[0] = -minXId;
        Geo.addCell(ID, Surfaces); ++ID;

        Surfaces[0] = maxXId;
        Geo.addCell(ID, Surfaces); ++ID;

        Surfaces.resize(3);
        Surfaces[0] = minXId;
        Surfaces[1] = -maxXId;

        Surfaces[2] = -minYId;
        Geo.addCell(ID, Surfaces); ++ID;

        Surfaces[2] = maxYId;
        Geo.addCell(ID, Surfaces); ++ID;

        Surfaces.resize(5);
        Surfaces[0] = minXId;
        Surfaces[1] = -maxXId;
        Surfaces[2] = minYId;
        Surfaces[3] = -maxYId;

        Surfaces[4] = -minZId;
        Geo.addCell(ID, Surfaces); ++ID;

        Surfaces[4] = maxZId;
        Geo.addCell(ID, Surfaces); ++ID;
    }

}

/*============================================================================*/
//! \function createTrickyGeometry
//  \author   Seth R. Johnson
//  Looks sort of like AMR
void createTrickyGeometry(mcGeometry::MCGeometry& geo)
{
    geo.addSurface(1, PlaneY(0.0));
    geo.addSurface(2, PlaneY(2.0));
    geo.addSurface(3, PlaneY(3.0));
    geo.addSurface(4, PlaneY(4.0));

    geo.addSurface(11, PlaneX(-2.0));
    geo.addSurface(12, PlaneX(-1.0));
    geo.addSurface(13, PlaneX( 0.0));
    geo.addSurface(14, PlaneX( 1.0));
    geo.addSurface(15, PlaneX( 2.0));

    geo.addSurface(21, PlaneZ(-2.0));
    geo.addSurface(22, PlaneZ(2.0));

    std::vector<int> surfaces(6, 0);
    surfaces[0] =  21;
    surfaces[1] = -22;

    // bottom row
    surfaces[2] =  1;
    surfaces[3] = -2;

    surfaces[4] =  11;
    surfaces[5] = -15;
    geo.addCell(1, surfaces);

    // second row
    surfaces[2] =  2;
    surfaces[3] = -3;

    surfaces[4] =  11;
    surfaces[5] = -13;
    geo.addCell(2, surfaces);

    surfaces[4] =  13;
    surfaces[5] = -15;
    geo.addCell(3, surfaces);

    // third row
    surfaces[2] =  3;
    surfaces[3] = -4;

    surfaces[4] =  11;
    surfaces[5] = -12;
    geo.addCell(4, surfaces);

    surfaces[4] =  12;
    surfaces[5] = -13;
    geo.addCell(5, surfaces);

    surfaces[4] =  13;
    surfaces[5] = -14;
    geo.addCell(6, surfaces);

    surfaces[4] =  14;
    surfaces[5] = -15;
    geo.addCell(7, surfaces);

    //outside cell
    surfaces[2] =  1;
    surfaces[3] = -4;

    surfaces[4] =  11;
    surfaces[5] = -15;

    geo.addCell(100, surfaces,
            MCGeometry::CellT::generateFlags(true, true));
}
