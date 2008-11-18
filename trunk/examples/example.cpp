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

    int N(5);
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

    MeshTiming(N, Geo);

    std::vector<double> limits(6, 0.0);
    limits[1] = N; limits[3] = N; limits[3] = N;
//  SimulateMC(100000, Geo, limits);
}

//! MeshTiming will perform some timing tests to see how much faster the 
//! the connected geometry is.
void MeshTiming(int, mcGeometry::MCGeometry&){
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
