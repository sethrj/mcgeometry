/*!
 * \file example.cpp
 * \brief Example of using MCGeometry
 * \author Jeremy Lloyd Conlin
 */

/*----------------------------------------------------------------------------*/

#include "mcgeometry/MCGeometry.hpp"
#include "mcgeometry/Plane.hpp"
#include "mcgeometry/Sphere.hpp"

#include <iostream>
#include <vector>
#include "transupport/VectorPrint.hpp"

using std::cout;
using std::endl;

void Mesh(int);
void SimulateMC(int, mcGeometry::MCGeometry&, std::vector<double>& );
void MeshTiming(int, mcGeometry::MCGeometry&);

int main(int argc, char* argv[]){

    Require( argc == 2 );

    int N( std::atoi(argv[1]) );
    Mesh(N);

    return 0;
}

//! Mesh will create and use a retangular mesh using N planes spaced 1.0 units
// apart
void Mesh(int N){
    cout << "\n===============================\n"
         << "Example of creating/using a mesh-like geometry."
         << "\n===============================" << endl;

    mcGeometry::MCGeometry Geo;
      
    // Normal vectors to planes perpendicular to axes
    std::vector<double> xNorm(3, 0.0);  xNorm[0] = 1.0;
    std::vector<double> yNorm(3, 0.0);  yNorm[1] = 1.0;
    std::vector<double> zNorm(3, 0.0);  zNorm[2] = 1.0;

    // Create global bounding planes
    // Could have done this with PlaneNormal but doing it with general plane
    // as an example
    std::vector<double> Point(3,0.0);
    mcGeometry::Plane minX(xNorm, Point);
    Geo.addSurface(1, minX);
    mcGeometry::Plane minY(yNorm, Point);
    Geo.addSurface(N+2, minY);
    mcGeometry::Plane minZ(zNorm, Point);
    Geo.addSurface(2*N+3, minZ);

    // Point is now corner opposite from origin
    Point[0] = N; Point[1] = N; Point[2] = N;   
    mcGeometry::Plane maxX(xNorm, Point);
    Geo.addSurface(N+1, maxX);
    mcGeometry::Plane maxY(yNorm, Point);
    Geo.addSurface(2*(N+1), maxY);
    mcGeometry::Plane maxZ(zNorm, Point);
    Geo.addSurface(3*(N+1), maxZ);

    cout << "Global binding box defined by following surfaces:\n"
         << minX << "\n"<< minY << "\n"<< minZ << "\n"
         << maxX << "\n"<< maxY << "\n"<< maxZ << endl;

    // yn and zn are defined so they don't have to be recalculated many times
    unsigned int yn = N+1;
    unsigned int zn = 2*(yn);

    // Create interior mesh
    for( int n = 2; n <= N; ++n){
        mcGeometry::PlaneNormal<0> PX(n);
        Geo.addSurface(n, PX);
        mcGeometry::PlaneNormal<1> PY(n);
        Geo.addSurface(n+yn, PX);
        mcGeometry::PlaneNormal<2> PZ(n);
        Geo.addSurface(n+zn, PX);
    }

    // Turn interior mesh into cells
    unsigned int ID = 0;
    std::vector<int> Surfaces(6, 0);

    cout << endl;
    for( int k = 0; k < N; ++k ){
        for( int j = 0; j < N; ++j ){
            for( int i = 0; i < N; ++i ){
//              cout << "ID = " << ID << ", (i,j,k) = (" << i << ", " 
//                   << j << ", " << k << ")\t";
                Surfaces[0] = i+1;
                Surfaces[1] = -1*(i+2);
                Surfaces[2] = (N+1) + (j+1);
                Surfaces[3] = -1*( (N+1) + (j+2) );
                Surfaces[4] = 2*(N+1) + (k+1);
                Surfaces[5] = -1*( 2*(N+1) + (k+2) );
//              for( int l = 0; l < 6; ++l ){
//                  cout << Surfaces[l] << ", ";
//              }
//              cout << endl;
                Geo.addCell(ID, Surfaces);
                ++ID;
            }
//          cout << endl;
        }
//      cout << endl;
    }

    // Create cells defining every thing outside of mesh
    Surfaces.resize(1); Surfaces[0] = -1;   // minX
    Geo.addCell(ID, Surfaces); ++ID;
    Surfaces[0] = N+1;                      // maxX
    Geo.addCell(ID, Surfaces); ++ID;
    Surfaces[0] *= -1; Surfaces.push_back(1); 
    Surfaces.push_back(-1*(N+2));              // minY
    Geo.addCell(ID, Surfaces); ++ID;
    Surfaces[2] = 2*(N+1);                  // maxY
    Geo.addCell(ID, Surfaces); ++ID;
    Surfaces[2] *= -1; Surfaces.push_back(N+2);
    Surfaces.push_back(-2*(N+3));              // minZ
    Geo.addCell(ID, Surfaces); ++ID;
    Surfaces[4] = 3*(N+1);                  // maxZ
    Geo.addCell(ID, Surfaces);

    MeshTiming(N, Geo);

    std::vector<double> limits(6, 0.0);
    limits[1] = N; limits[3] = N; limits[3] = N;
//  SimulateMC(100000, Geo, limits);
}

//! MeshTiming will perform some timing tests to see how much faster the 
//! the connected geometry is.  N is the number of mesh cells in each 
//! dimension.
void MeshTiming(int N, mcGeometry::MCGeometry& Geo){

    mcGeometry::MCGeometry::UserCellIDType ID(0);
    mcGeometry::MCGeometry::UserCellIDType newID(0);
    mcGeometry::MCGeometry::ReturnStatus RS;

    std::vector<double> position(3,0.0);
    std::vector<double> newPos(3,0.0);
    double distance;

    // Create possible directions
    std::vector<double> dir(3,0.0);
    std::vector<std::vector<double> > directions;
    std::vector<std::vector<double> >::iterator dirIter;
    dir[0] = 1.0;                   // Positive x-direction
    directions.push_back(dir);
    dir[0] = 0.0; dir[1] = -1.0;    // Negative y-direction
    directions.push_back(dir);
    dir[1] = 0.0; dir[2] = -1.0;    // Negative z-direction
    directions.push_back(dir);

    for( int k = 0; k < N; ++k ){
        position[2] = k + 0.5;
        for( int j = 0; j < N; ++j ){
            position[1] = j + 0.5;
            for( int i = 0; i < N; ++i ){
                position[0] = i + 0.5;
                cout << "ID = " << ID << ", position: " << position << endl;
                for( dirIter = directions.begin(); dirIter != directions.end();
                        ++dirIter ){
                    Geo.intersect(position, *dirIter, ID, newPos, newID, 
                                    distance, RS);
                }
                ++ID;
            }
            cout << endl;
        }
    }
}


//! SimulateMC will simulate a Monte Carlo particle transport (without actually
//! any particles).  limits contains the min and max coordinates for each 
//! dimension.
void SimulateMC(int N, mcGeometry::MCGeometry& Geo, 
        std::vector<double>& limits ){

    Require(limits.size() == 6);

    for( int n = 0; n <= N; ++n ){
    }
}
