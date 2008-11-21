/*!
 * \file meshTiming.cpp
 * \brief  Measure timing data for a simply connected geometry
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
void MeshTiming(int, mcGeometry::MCGeometry&, bool);

int main(int argc, char* argv[]){

    Insist( argc == 2, "Please input number of divisions along each axis." );

    int N( std::atoi(argv[1]) );

    Insist( N > 0, "Number of divisions should be positive." );

    cout << "===============================" << endl
         << "Example of creating/using a mesh-like geometry." << endl
         << "===============================" << endl;
      

    mcGeometry::MCGeometry Geo;

    
    cout << "Creating the mesh..." << endl;
    TIMER_START("Create the mesh");
    CreateMesh(N, Geo);
    TIMER_STOP("Create the mesh");

//    Geo.debugPrint();

    cout << "Running forward sweep..." << endl;
    TIMER_START("Run without neighborhood (first sweep)");
    MeshTiming(N, Geo, false);
    TIMER_STOP("Run without neighborhood (first sweep)");

//    Geo.debugPrint();

    cout << "Running full sweep..." << endl;
    TIMER_START("Run with most neighborhood (second sweep)");
    MeshTiming(N, Geo, true);
    TIMER_STOP("Run with most neighborhood (second sweep)");

//    Geo.debugPrint();

    cout << "Running full sweep again..." << endl;
    TIMER_START("Run with full neighborhood (third sweep)");
    MeshTiming(N, Geo, true);
    TIMER_STOP("Run with full neighborhood (third sweep)");
    
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

    cout << "Global bounding box defined by following surfaces:\n"
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

//! MeshTiming will perform some timing tests to see how much faster the 
//! the connected geometry is.  N is the number of mesh cells in each 
//! dimension.
void MeshTiming(int N, mcGeometry::MCGeometry& Geo, bool fullDirections){

    mcGeometry::MCGeometry::UserCellIDType ID(0);
    mcGeometry::MCGeometry::UserCellIDType newID(0);
    mcGeometry::MCGeometry::ReturnStatus RS;

    std::vector<double> position(3,0.0);
    std::vector<double> newPos(3,0.0);
    double distance;


    // Create possible directions
    std::vector<double> dir(3,0.0);
    std::vector<std::vector<double> > directions;
    dir[0] = 1.0;                   // Positive x-direction
    directions.push_back(dir);
    dir[0] = 0.0; dir[1] = 1.0;    // Positive y-direction
    directions.push_back(dir);
    dir[1] = 0.0; dir[2] = 1.0;    // Positive z-direction
    directions.push_back(dir);

    if (fullDirections == true) {
        dir.assign(3,0.0);

        // add the other half of the unit sphere
        dir[0] = -1.0;                   // Negative x-direction
        directions.push_back(dir);
        dir[0] = 0.0; dir[1] = -1.0;    // Negative y-direction
        directions.push_back(dir);
        dir[1] = 0.0; dir[2] = -1.0;    // Negative z-direction
        directions.push_back(dir);
    }


    std::vector<std::vector<double> >::iterator dirIter;
    for( int k = 0; k < N; ++k ){
        position[2] = k + 0.5;
        for( int j = 0; j < N; ++j ){
            position[1] = j + 0.5;
            for( int i = 0; i < N; ++i ){
                position[0] = i + 0.5;
//                cout << "ID = " << ID << ", position: " << position << endl;
                for( dirIter = directions.begin(); dirIter != directions.end();
                        ++dirIter )
                {
                    Geo.findNewCell(position, *dirIter, ID, newPos, newID, 
                                    distance, RS);
                }
                ++ID;
            }
//            cout << endl;
        }
    }
}


