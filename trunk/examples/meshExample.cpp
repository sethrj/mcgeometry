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
#include "transupport/Timer.hpp"

using std::cout;
using std::endl;

void CreateMesh(int, mcGeometry::MCGeometry&);
void SimulateMC(int, mcGeometry::MCGeometry&, std::vector<double>& );
void MeshTiming(int, mcGeometry::MCGeometry&);

int main(int argc, char* argv[]){

    Require( argc == 2 );

    int N( std::atoi(argv[1]) );

    cout << "\n===============================\n"
         << "Example of creating/using a mesh-like geometry."
         << "\n===============================" << endl;
      

    mcGeometry::MCGeometry Geo;

    
    TIMER_START("Create the mesh");
    CreateMesh(N, Geo);
    TIMER_STOP("Create the mesh");

//    Geo.debugPrint();

    TIMER_START("Run without neighborhood (first sweep)");
    MeshTiming(N, Geo);
    TIMER_STOP("Run without neighborhood (first sweep)");

//    Geo.debugPrint();

    TIMER_START("Run with neighborhood (second sweep)");
    MeshTiming(N, Geo);
    TIMER_STOP("Run with neighborhood (second sweep)");

    std::vector<double> limits(6, 0.0);
    limits[1] = N; limits[3] = N; limits[3] = N;
//  SimulateMC(100000, Geo, limits);
    
    TIMER_PRINT();
    return 0;
}

//! Mesh will create and use a retangular mesh using N planes spaced 1.0 units
// apart
void CreateMesh(int N, mcGeometry::MCGeometry& Geo){
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
    // Could have done this with PlaneNormal but doing it with general plane
    // as an example
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

    cout << "Global binding box defined by following surfaces:\n"
         << minX << "\n"<< minY << "\n"<< minZ << "\n"
         << maxX << "\n"<< maxY << "\n"<< maxZ << endl;

    // yn and zn are defined so they don't have to be recalculated many times
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
    Surfaces.resize(1);
    Surfaces[0] = -minXId;
    Geo.addCell(ID, Surfaces); ++ID;

    Surfaces[0] = maxXId;
    Geo.addCell(ID, Surfaces); ++ID;

    Surfaces.resize(3);
    Surfaces[0] = minXId;
    Surfaces[1] = -maxXId;
    Surfaces[2] = maxYId;
    Geo.addCell(ID, Surfaces); ++ID;

    Surfaces[2] = -minYId;
    Geo.addCell(ID, Surfaces); ++ID;

    
    Surfaces.resize(5);
    Surfaces[0] = minXId;
    Surfaces[1] = -maxXId;
    Surfaces[2] = minYId;
    Surfaces[3] = -maxYId;
    Surfaces[4] = maxZId;
    Geo.addCell(ID, Surfaces); ++ID;

    Surfaces[4] = -minZId;
    Geo.addCell(ID, Surfaces); ++ID;


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
//                cout << "ID = " << ID << ", position: " << position << endl;
                for( dirIter = directions.begin(); dirIter != directions.end();
                        ++dirIter ){
                    Geo.intersect(position, *dirIter, ID, newPos, newID, 
                                    distance, RS);
                }
                ++ID;
            }
//            cout << endl;
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
