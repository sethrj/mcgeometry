/*!
 * \file meshTiming.cpp
 * \brief  Measure timing data for a simply connected geometry
 * \author Jeremy Lloyd Conlin
 */

/*----------------------------------------------------------------------------*/

#include "createGeometry.hpp"

#include "mcgeometry/MCGeometry.hpp"
#include "mcgeometry/Plane.hpp"
#include "mcgeometry/Sphere.hpp"

#include "mcgeometry/MCGeometry.i.hpp"
#include "mcgeometry/Plane.i.hpp"
#include "mcgeometry/Sphere.i.hpp"

#include <iostream>
#include <vector>
#include "transupport/VectorPrint.hpp"
#include "transupport/SuperTimer.hpp"

using std::cout;
using std::endl;

//! Blitz++ TinyVector of length D stores position/direction/etc.
typedef blitz::TinyVector<double, 3> TVecDbl; 


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
    TIMER_START("0 Create the mesh");
    CreateMesh(N, Geo);
    TIMER_STOP("0 Create the mesh");

//    Geo.debugPrint();

    cout << "Running forward sweep..." << endl;
    TIMER_START("1 Run without neighborhood (first sweep)");
    MeshTiming(N, Geo, false);
    TIMER_STOP("1 Run without neighborhood (first sweep)");

//    Geo.debugPrint();

    cout << "Running forward sweep again..." << endl;
    TIMER_START("2 Run with full neighborhood (second sweep)");
    MeshTiming(N, Geo, false);
    TIMER_STOP("2 Run with full neighborhood (second sweep)");

//    Geo.debugPrint();

    cout << "Running full sweep..." << endl;
    TIMER_START("3 Run with most neighborhood (first full sweep)");
    MeshTiming(N, Geo, true);
    TIMER_STOP("3 Run with most neighborhood (first full sweep)");

//    Geo.debugPrint();

    cout << "Running full sweep again..." << endl;
    TIMER_START("4 Run with full neighborhood (second full sweep)");
    MeshTiming(N, Geo, true);
    TIMER_STOP("4 Run with full neighborhood (second full sweep)");

    cout << "Running full sweep once more..." << endl;
    TIMER_START("5 Run with full neighborhood (third full sweep)");
    MeshTiming(N, Geo, true);
    TIMER_STOP("5 Run with full neighborhood (third full sweep)");    

    TIMER_PRINT();
    return 0;
}

//! MeshTiming will perform some timing tests to see how much faster the 
//! the connected geometry is.  N is the number of mesh cells in each 
//! dimension.
void MeshTiming(int N, mcGeometry::MCGeometry& Geo, bool fullDirections){

    mcGeometry::MCGeometry::UserCellIDType ID(0);
    mcGeometry::MCGeometry::UserCellIDType newID(0);
    mcGeometry::MCGeometry::ReturnStatus RS;

    TVecDbl position(0.0);
    TVecDbl newPos(0.0);
    double distance;


    // Create possible directions
    TVecDbl dir(0.0);
    std::vector< TVecDbl > directions;
    dir[0] = 1.0;                   // Positive x-direction
    directions.push_back(dir);
    dir[0] = 0.0; dir[1] = 1.0;    // Positive y-direction
    directions.push_back(dir);
    dir[1] = 0.0; dir[2] = 1.0;    // Positive z-direction
    directions.push_back(dir);

    if (fullDirections == true) {
        dir = 0.0;

        // add the other half of the unit sphere
        dir[0] = -1.0;                   // Negative x-direction
        directions.push_back(dir);
        dir[0] = 0.0; dir[1] = -1.0;    // Negative y-direction
        directions.push_back(dir);
        dir[1] = 0.0; dir[2] = -1.0;    // Negative z-direction
        directions.push_back(dir);
    }


    std::vector< TVecDbl >::iterator dirIter;
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


