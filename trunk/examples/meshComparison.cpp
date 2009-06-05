/*!
 * \file meshComparison.cpp
 * \brief Compare our combinatorial geometry to a evenly spaced mesh
 * \author Jeremy Lloyd Conlin
 */

/*----------------------------------------------------------------------------*/

#include "createGeometry.hpp"

#include "mcgeometry/MCGeometry.hpp"
#include "mcgeometry/_MCGeometry.hpp"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <map>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <blitz/tinyvec-et.h>
#include "transupport/VectorPrint.hpp"
#include "transupport/SuperTimer.hpp"
#include "transupport/constants.hpp"
#include "extra/BasicTally.hpp"

#include "extra/mtrand.h"

using std::cout;
using std::endl;

typedef std::vector<monteCarlo::BasicTally<double> > TallyVec;

//! Blitz++ TinyVector of length D stores position/direction/etc.
typedef blitz::TinyVector<double, 3> TVecDbl; 

//===== function declarations
void    SimulateMCComb(const int, const int, mcGeometry::MCGeometry&,
        TallyVec&);
void    SimulateMCDet(int, int, TallyVec&);
double  randDouble();
bool isInside(const int size, const std::vector<int>& index);
void source(const int size, TVecDbl& position,
                    TVecDbl& direction);
void    randDirection(TVecDbl&);
void    randPosition(double, TVecDbl&);
double  getXsn(const int cell);
unsigned int getCell(const TVecDbl& position, const int size);
double  distanceToPlane(int, double, double);
void    printPLTallies(int, const TallyVec&, const TallyVec&);
void    diffTallies(int, const TallyVec&, const TallyVec& );

void runProgram(char* argv[]){
    int numCells( std::atoi(argv[1]) );
    int numParticles( std::atoi(argv[2]) );
    int printFlag( std::atoi(argv[3]) );

    Insist( numCells > 0, "Number of divisions should be positive." );
    Insist( numParticles > 0, "Number of particles should be positive." );
    Insist( printFlag > -1, "printFlag must be either 0, 1 or 2.");
    Insist( printFlag < 3, "printFlag must be either 0, 1 or 2.");

    cout << "\n=====================================================" 
         << "\nComparing combinatorial mesh with deterministic mesh." 
         << "\nSize of mesh: " << numCells << "x" << numCells << "x"<<numCells 
         << "\nTracking " << numParticles << " particles"
         << "\n=====================================================" << endl;

    TIMER_START("Creating the combinatorial mesh.");
    mcGeometry::MCGeometry Geo;
    CreateMesh(numCells, Geo);
    TIMER_STOP("Creating the combinatorial mesh.");

    TallyVec combPL1;
    TallyVec combPL2;

    cout << "Combinatorial Geometry.\nFirst time." << endl;

    TIMER_START("First transport in combinatorial mesh.");
    SimulateMCComb(numParticles, numCells, Geo, combPL1);
    TIMER_STOP("First transport in combinatorial mesh.");

    cout << "\nSecond time." << endl;

    TIMER_START("Second transport in combinatorial mesh.");
    SimulateMCComb(numParticles, numCells, Geo, combPL2);
    TIMER_STOP("Second transport in combinatorial mesh.");

    TallyVec detPL;

    cout << "Deterministic Geometry." << endl;
    TIMER_START("Transport in deterministic mesh.");
    SimulateMCDet(numParticles, numCells, detPL);
    TIMER_STOP("Transport in deterministic mesh.");

    if( printFlag > 0){
        printPLTallies(numCells, combPL1, detPL);
        printPLTallies(numCells, combPL2, detPL);
    }
    
    cout << "Difference between pathlength tallies." << endl;
    if( printFlag > 1 ){
        diffTallies(numCells, combPL1, detPL);
        diffTallies(numCells, combPL2, detPL);
    }

    TIMER_PRINT();
}

int main(int argc, char* argv[]){

    if (argc != 4) {
        cout << "Syntax: meshComparsion numDivisions numParticles printFlag."
             << endl;
        return 1;
    }

    try {
        runProgram(argv);
    }
    catch (tranSupport::tranError &theErr) {
        cout<< "***********************************************************\n"
            << "Failure in meshComparison: " << endl
            << theErr.what() << endl;

        TIMER_PRINT();
        return 1;
    }
    return 0;
}


//! SimulateMCDet will simulate a Monte Carlo transport through a deterministic
//! mesh.
void SimulateMCDet(const int numParticles, const int size, TallyVec& PLTally)
{

    unsigned int numCells = size*size*size;    // Number of cells in mesh
    PLTally.resize(numCells);

    TVecDbl position(0.0);
    TVecDbl new_position(0.0);
    TVecDbl direction(0.0);

    double d;           // Distance traveled
    TVecDbl point;       // Point on plane

    TVecDbl dPlane(0.0); //distance to each plane

    double xT;    // Total cross section
    unsigned int cellNumber;
    std::vector<int> cellIndex(3, 0); //index of particle in 3 D

    int chosenDistanceIndex;

    // Track particles
    for( int n = 0; n < numParticles; ++n ){
        // source particle
        source(size, position, direction);
        
        for (int i = 0; i < 3; i++)
            cellIndex[i] = static_cast<int>(position[i]); // floors it

        while( isInside(size, cellIndex) ){

            cellNumber =   cellIndex[0]
                         + cellIndex[1]*size
                         + cellIndex[2]*size*size;

            //cellNumber = getCell(position, size);
            Check(cellNumber < numCells);

            // Find distance to 'planes'
            for (int i = 0; i < 3; i++)
                dPlane[i] = distanceToPlane(cellIndex[i],
                          position[i], direction[i]);

            // Sample distance to collision
            xT = getXsn(cellNumber);
            d = (-1.0/xT)*std::log(randDouble());
            chosenDistanceIndex = -1;

            Check(d >= 0.0);

            // Determine minimum distance
            for (int i = 0; i < 3; i++) {
                if (dPlane[i] < d) {
                    d = dPlane[i];
                    chosenDistanceIndex = i;
                }
            }
            Check(d >= 0.0);

            if ( chosenDistanceIndex == -1) {
                // we've collided
                PLTally[cellNumber].accumulateValue(d);
            }
            else {
                // we've moved to a new surface
                PLTally[cellNumber].accumulateValue(d);
                PLTally[cellNumber].flush();

                // update our cell index
                cellIndex[chosenDistanceIndex] += 
                    (direction[chosenDistanceIndex] > 0 ? 1 : -1);
            }
                 
            position[0] += direction[0]*d;
            position[1] += direction[1]*d;
            position[2] += direction[2]*d;

            // Score pathlength tally
            if( cellNumber > numCells ){
                cout << "cellNumber: " << cellNumber << ", numCells = " 
                     << numCells << ", n = " << n 
                     << "\n\tposition: " << position << endl;
            }
        }
    }
    TallyVec::iterator talIter;
    for( talIter = PLTally.begin(); talIter != PLTally.end(); ++talIter ){
        Check( talIter->checkFlushed() );
        talIter->setNumTrials(numParticles);
    }
}

//! SimulateMCComb will simulate a Monte Carlo transport through the mesh.  It 
//! will track particles along a random direction until it leaves the mesh.  A 
//! particle never changes direction even at a collision.
//! size: The size of the mesh
//! numParticles: Number of 'particles' to track
void SimulateMCComb(int numParticles, int size, mcGeometry::MCGeometry& Geo, 
                    TallyVec& PLTally)
{

    unsigned int numCells = size*size*size;    // Number of cells in mesh
    PLTally.resize(numCells);

    TVecDbl position(0.0);
    TVecDbl new_position(0.0);
    TVecDbl direction(0.0);
    unsigned int cell;
    unsigned int new_cell;
    mcGeometry::MCGeometry::ReturnStatus returnStatus;

    double dColl;   // Distance to a collision
    double dSurf;   // Distance to a surface
    double xT;

    // Track particles
    for( int n = 0; n < numParticles; ++n ){
        //create new particle
        source(size, position, direction);
        
        cell = getCell(position, size);
        //Check(cell == Geo.findCell(position));

        // Track particle through geometry
        while( cell < numCells ){   // If false, particle has escaped
            xT = getXsn(cell);
            dColl = (-1.0/xT)*std::log(randDouble());

            Geo.findDistance(position, direction, cell, dSurf);

            if( dColl < dSurf ){    // Collision
                // Score pathlength tally
                PLTally[cell].accumulateValue(dColl);

                position[0] += direction[0]*dColl;
                position[1] += direction[1]*dColl;
                position[2] += direction[2]*dColl;
            }
            else{       // Move to boundary
                Geo.findNewCell( position, direction, new_position,
                                 new_cell, returnStatus);
                // Score pathlength tally
                PLTally[cell].accumulateValue(dSurf);
                PLTally[cell].flush();

                position = new_position;
                cell = new_cell;
            }

        }
    }
    TallyVec::iterator talIter;
    for( talIter = PLTally.begin(); talIter != PLTally.end(); ++talIter ){
        Check( talIter->checkFlushed() );
        talIter->setNumTrials(numParticles);
    }

}

//! distanceToPlane will calculate the distance of a point to a plane 
//! it assumes normal component is always 1 so no need to complicate this.
// Distance to plane is: t = n.(a-p)/(n.d) n is normal vector, a is point on 
// plane, p is position d is direction vector.  Since all of our n's have only 
// one non-zero term the dot products are just the product of two numbers
inline double distanceToPlane(int cellIndex, double x, double v)
{
    if ( v > 0 ) {    // Moving in positive direction
        return (cellIndex + 1.0 - x)/v;
    }
    else {
        //return (x - cellIndex)/fabs(v);
        return (cellIndex - x) / v;
    }
}

inline bool isInside(const int size, const std::vector<int>& index)
{
    for (unsigned int i = 0; i < 3; i++) {
        if (index[i] < 0 || index[i] >= size)
            return false;
    }
    return true;
}

inline void source(const int size, TVecDbl& position,
                    TVecDbl& direction)
{
    // source along one face in normal direction, should result in each 
    // cell having 1/(size^2) tally result

//    position[0] = randDouble() * size;
//    position[1] = randDouble() * size;
//    position[2] = 0.0;
//
//    direction[0]= 0;
//    direction[1]= 0;
//    direction[2]= 1;

    // Pick random position inside box
    randPosition(size, position);
    // Pick random direction
    randDirection(direction);
}

//! Pick a random direction on the unit sphere
inline void randDirection(TVecDbl& v){
    double phi = tranSupport::constants::TWOPI*randDouble();
    
    v[0] = 2*randDouble() - 1;
    double mu = std::sqrt(1 - v[0]*v[0]);
    v[1] = mu*std::cos(phi);
    v[2] = mu*std::sin(phi);
}

inline void randPosition( double size, TVecDbl& position){
    position = randDouble(), randDouble(), randDouble();
    position *= size;
}

inline double getXsn(const int cell) {
    if( cell%2 )
        return 1.0;
    return 1.0;
}

inline unsigned int getCell(const TVecDbl& position, const int size)
{
    Require(blitz::all(position >= 0.0));
    return static_cast<unsigned int>(position[0])
        + (size)*static_cast<unsigned int>(position[1])
        + (size*size)*static_cast<unsigned int>(position[2]);
}
mtRand::MTRand randGen;

inline double randDouble(){
    return randGen();
}

void printPLTallies(int numCells, const TallyVec& comb, const TallyVec& det)
{

    Insist(comb.size() == det.size(), "PathLength tallies mush be same size.");

    cout << "\nCombinatorial Tallies \t\t\t Deterministic Tallies" << endl;

    cout << "Means:" << endl;
    int n = 0;
    for( int k = 0; k < numCells; ++k ){
        for( int j = 0; j < numCells; ++j ){
            std::ostringstream combRow;
            std::ostringstream detRow;
            combRow << "[ ";
            detRow << "[ ";
            for( int i = 0; i < numCells; ++i, ++n ){
//              cout << "n = " << n << endl;
                combRow << comb[n].getMean() << ", ";
                detRow << det[n].getMean() << ", ";
            }
            combRow << "]";
            detRow << "]";
            cout << combRow.str() << "\t\t" << detRow.str() << endl;
        }
        cout << endl;
    }
    cout << endl;

    cout << "Standard Deviations:" << endl;
    n = 0;
    for( int k = 0; k < numCells; ++k ){
        for( int j = 0; j < numCells; ++j ){
            std::ostringstream combRow;
            std::ostringstream detRow;
            combRow << "[ ";
            detRow << "[ ";
            for( int i = 0; i < numCells; ++i, ++n ){
//              cout << "n = " << n << endl;
                combRow << comb[n].getMeanStdev() << ", ";
                detRow << det[n].getMeanStdev() << ", ";
            }
            combRow << "]";
            detRow << "]";
            cout << combRow.str() << "\t\t" << detRow.str() << endl;
        }
        cout << endl;
    }
    cout << endl;

}

//! diffTallies will show the difference between the two tallies so it is 
//! easier to compare them.
void diffTallies(int numCells, const TallyVec& A, const TallyVec& B)
{
    monteCarlo::BasicTally<double> deviationStats;
    double cutoff = 0.5 + 2.5*std::log10(numCells);

    cout << "abs(diff) / max(stdev of mean)" << endl;
    cout << "Tallies in cells that differ more than " << cutoff << " stdevs"
         << endl;

    double diff;
    int n = 0;
    unsigned int countedTallies = 0;
    for( int k = 0; k < numCells; ++k ){
        for( int j = 0; j < numCells; ++j ){
            for( int i = 0; i < numCells; ++i, ++n ){
                diff = std::fabs(A[n].getMean() - B[n].getMean()) / 
                        std::max(A[n].getMeanStdev(), B[n].getMeanStdev());
                if (A[n].getMean() != 0) {
                    deviationStats += diff;
                    countedTallies++;
                }

                if (diff > cutoff) {
                    cout << "Cell "<< i + 1 << "," << j + 1 << "," << k + 1
                         << " [index " << n << "]: " << diff << endl;
                }
            }
        }
    }
    deviationStats.setNumTrials(countedTallies);
    cout << "Average cell deviation between the two: "
         << deviationStats.getMean() << "+-" << deviationStats.getStdev()
         << endl;
}
