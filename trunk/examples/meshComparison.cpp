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
#include <cstdlib>
#include <map>
#include <cmath>
#include <algorithm>
#include "transupport/VectorPrint.hpp"
#include "transupport/constants.hpp"
#include "transupport/Timer.hpp"

using std::cout;
using std::endl;

void CreateMesh(int, mcGeometry::MCGeometry&);
void SimulateMCComb(int, int, mcGeometry::MCGeometry&);
void SimulateMCDet(int, int);
double randDouble();
void randDirection(std::vector<double>&);
void randPosition(double, std::vector<double>&);
double distanceToPlane(double, double);

int main(int argc, char* argv[]){

    Insist( argc == 3, "Please input number of divisions along each axis." );

    int N( std::atoi(argv[1]) );
    int numParticles(   std::atoi(argv[2]) );

    Insist( N > 0, "Number of divisions should be positive." );

    cout << "\n=====================================================" 
         << "\nComparing combinatorial mesh with deterministic mesh." 
         << "\nSize of mesh: " << N << "x" << N << "x" << N 
         << "\nTracking " << numParticles << " particles"
         << "\n=====================================================" << endl;

    TIMER_START("Creating the combinatorial mesh.");
    mcGeometry::MCGeometry Geo;
    CreateMesh(N, Geo);
    TIMER_STOP("Creating the combinatorial mesh.");

    TIMER_START("Transport in combinatorial mesh.");
    SimulateMCComb(numParticles, N, Geo);
    TIMER_STOP("Transport in combinatorial mesh.");

    TIMER_START("Transport in deterministic mesh.");
    SimulateMCDet(numParticles, N);
    TIMER_STOP("Transport in deterministic mesh.");

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

//! SimulateMCDet will simulate a Monte Carlo transport through a deterministic
//! mesh.
void SimulateMCDet(int N, int size){

    cout << "Deterministic Geometry." << endl;

    std::vector<double> position(3,0.0);
    std::vector<double> new_position(3,0.0);
    std::vector<double> direction(3,0.0);

    double dX, dY, dZ;  // Distance to 'planes' perp to X, Y, and Z axes
    double dSurf;       // Smallesst distance to all planes
    double dColl;       // Distance to collision
    double d;           // Distance traveled
    std::vector<double> point;       // Point on plane

    double xT;    // Total cross section
    unsigned int cellNumber;


    // Track particles
    for( int n = 0; n < N; ++n ){
        // Pick random position inside box
        randPosition(size, position);
        // Pick random direction
        randDirection(direction);
        
        while( fabs(position[0]) < N && fabs(position[1]) < N && 
            fabs(position[2]) < N ){

            // Find distance to 'planes'
            dX = distanceToPlane(position[0], direction[0]); 
            dY = distanceToPlane(position[1], direction[1]); 
            dZ = distanceToPlane(position[2], direction[2]); 

            dSurf= dX;
            if( dY < dSurf) dSurf= dY;
            if( dZ < dSurf) dSurf= dZ;

            // Sample distance to collision
            cellNumber = std::floor(position[0]) + (N+1)*std::floor(position[1])
                         + 2*(N+1)*std::floor(position[2]);
            if( cellNumber%2 ) xT = 0.5;
            else xT = 0.01;

            dColl = (-1.0/xT)*std::log(randDouble());

            // Determine minimum distance
            d = std::min(dColl, dSurf);
            position[0] += direction[0]*d;
            position[1] += direction[1]*d;
            position[2] += direction[2]*d;

        }
    }
}

//! SimulateMCComb will simulate a Monte Carlo transport through the mesh.  It 
//! will track particles along a random direction until it leaves the mesh.  A 
//! particle never changes direction even at a collision.
//! size: The size of the mesh
//! N: Number of 'particles' to track
void SimulateMCComb(int N, int size, mcGeometry::MCGeometry& Geo){

    cout << "Cominatorial Geometry." << endl;

    double numCells = size*size*size;    // Number of cells in mesh

    std::vector<double> position(3,0.0);
    std::vector<double> new_position(3,0.0);
    std::vector<double> direction(3,0.0);
    unsigned int cell;
    unsigned int new_cell;
    mcGeometry::MCGeometry::ReturnStatus returnStatus;

    double dColl;   // Distance to a collision
    double dSurf;   // Distance to a surface
    double xT;

    // Track particles
    for( int n = 0; n < N; ++n ){
        // Pick random position inside box
        randPosition(size, position);
        // Pick random direction
        randDirection(direction);
        
        cell = Geo.findCell(position);

        // Track particle through geometry
        while( cell < numCells ){   // If false, particle has escaped
            if( cell%2 ) xT = 0.5;
            else xT = 0.01;
            dColl = (-1.0/xT)*std::log(randDouble());
            Geo.findNewCell( position, direction, cell, new_position, new_cell, 
                        dSurf, returnStatus);

            if( dColl < dSurf ){    // Collision
                position[0] += direction[0]*dColl;
                position[1] += direction[1]*dColl;
                position[2] += direction[2]*dColl;
            }
            else{       // Move to boundary
                position = new_position;
                cell = new_cell;
            }
        }
    }
}

//! distanceToPlane will calculate the distance of a point to a plane 
//! it assumes normal component is always 1 so no need to complicate this.
// Distance to plane is: t = n.(a-p)/(n.d) n is normal vector, a is point on 
// plane, p is position d is direction vector.  Since all of our n's have only 
// one non-zero term the dot products are just the product of two numbers
double distanceToPlane(double x, double v){
    double p;
    if( v > 0 ) p = std::floor(x + 1);  // Moving in positive direction
    else p = std::floor(x);

    return (x-p)/v;
}

//! Pick a random direction on the unit sphere
void randDirection(std::vector<double>& v){
    double phi = 2*tranSupport::constants::PI*randDouble();
    
    v[0] = 2*randDouble() - 1;
    double mu = std::sqrt(1 - v[0]*v[0]);
    v[1] = mu*std::cos(phi);
    v[2] = mu*std::sin(phi);
}

inline void randPosition( double size, std::vector<double>& position){
    std::vector<double>::iterator vecIter;
    for(vecIter = position.begin(); vecIter != position.end(); ++vecIter){
        *vecIter = size*randDouble();
    }
}

inline double randDouble(){
    return std::rand()/(double(RAND_MAX) + 1);
}


