/*!
 * \file MCGeometry.cpp
 * \brief MCGeometry class
 * \author Seth R. Johnson
 * 
 * The cpp file should hold methods that are *not* speed-critical. (It may be
 * a trivial difference but we can find out later.) For example, the Intersect
 * method should remain in the header because that is run on every particle
 * every event, but the geometry creation and output is only called outside of
 * the main problem.
 */

#include <utility>
#include <map>
#include "transupport/dbc.hpp"
#include "transupport/VectorPrint.hpp"
#include "MCGeometry.hpp"
#include "Surface.hpp"
#include "Cell.hpp"

#include <iostream>
using std::cout;
using std::endl;

namespace mcGeometry {
/*----------------------------------------------------------------------------*/

unsigned int MCGeometry::addSurface( const MCGeometry::UserSurfaceIDType
                                                             userSurfaceId,
                                     const Surface& inSurface)
{
    // this is ONLY if we are using MCNP-type input definitions
    Insist(userSurfaceId > 0, "Things will break if surfaceId = 0 is allowed.");

    // "clone" calls a routine in the quadric which allocates new memory
    // WE ARE NOW RESPONSIBLE FOR THIS MEMORY (and must delete it when
    // appropriate)
    Surface* newSurface = inSurface.clone(userSurfaceId);

    _surfaces.push_back(newSurface);
    unsigned int newSurfaceIndex = _surfaces.size() - 1;

    // add the reverse mapping and 
    // verify that this surface has not been added already (i.e. the
    // UserSurfaceIDType is unique)
    std::pair<SurfaceRevIDMap::iterator, bool> result
        = _surfaceRevUserIds.insert(
                std::make_pair(userSurfaceId, newSurfaceIndex));

//    cout << "Added surface with ID " << userSurfaceId
//         << "that has index " << newSurfaceIndex << endl;

    Insist(result.second == true,
             "Tried to add a surface with an ID that was already there.");

    Check(_surfaceRevUserIds.size() == _surfaces.size());

    return newSurfaceIndex;
}

/*----------------------------------------------------------------------------*/
unsigned int MCGeometry::addCell(const MCGeometry::UserCellIDType
                                                             userCellId,
                                 const IntVec surfaceIds)
{
    //---- convert surface ID to pairs of unsigned ints (surfaceIds) and bools
    //                                                  (surface sense) -----//
    CellT::SASVec boundingSurfaces;

    // bounding surfaces should have same length as input list of surface IDs
    // so from the start just reserve that many spaces in memory
    boundingSurfaces.reserve(surfaceIds.size());


    // -------- ITERATE OVER INPUT SURFACE LIST -------- //
    
    for (IntVec::const_iterator it = surfaceIds.begin();
                                it != surfaceIds.end(); ++it) {
        unsigned int userSurfaceId; //an integer that only the user deals with
        SurfaceAndSense newSurface;

        if (*it > 0) // user inputs a positive value (positive sense surface)
        {
            userSurfaceId     = *it;
            newSurface.second = true;
        }
        else // user input a negative value (negative surface sense)
        {
            userSurfaceId     = -(*it);
            newSurface.second = false;
        }

        // the value from the find result is the internal index
        newSurface.first = _surfaces[getSurfaceIndexFromUserId(userSurfaceId)];

        // add the surface to the vector of bounding surfaces
        boundingSurfaces.push_back(newSurface);

        // this counts as another unmatched surface
        _unMatchedSurfaces++;
//        cout << "Added surface " << userSurfaceId << " to cell " << userCellId
//             << " (unmatched surface count: " << _unMatchedSurfaces << ")"
//             << endl;
    }

    Check(surfaceIds.size() == boundingSurfaces.size());

    //====== add cell to the internal cell vector
    unsigned int newCellIndex = _cells.size();

    CellT* newCell = new CellT(boundingSurfaces, userCellId, newCellIndex);
    _cells.push_back(newCell);

//    cout << "Added cell with ID " << userCellId
//         << "that has index " << newCellIndex << endl;

    // add the reverse mapping and 
    // verify that this surface has not been added already (i.e. the
    // UserSurfaceIDType is unique)
    std::pair<CellRevIDMap::iterator, bool> result
        = _cellRevUserIds.insert(
                std::make_pair(userCellId, newCellIndex));

    Insist(result.second == true,
            "Tried to add a cell with an ID that was already there.");

    //====== loop back through the surfaces and add the connectivity
    for (CellT::SASVec::iterator bsIt = boundingSurfaces.begin();
                          bsIt != boundingSurfaces.end(); ++bsIt)
    {
        SurfaceAndSense &newQandS = *bsIt;

        // using the "associative array" capability of maps lets us access a
        // key, and it will either automatically initialize an empty vector or
        // return the vector that is already  
        // see C++ Standard Library pp. 182-183
        _surfToCellConnectivity[newQandS].push_back(newCell);
    }

    return newCellIndex;
}
/*----------------------------------------------------------------------------*/
void MCGeometry::completedGeometryInput()
{
    // resize all vectors so that no space is wasted

    // prevent new cells/surfaces from being added?

    // make sure SCConnectMap is not empty for anything?

    cout << "This doesn't do anything yet." << endl;
}
/*----------------------------------------------------------------------------*/
void MCGeometry::_completedConnectivity()
{
    cout << "Connectivity is complete." << endl;
}
/*----------------------------------------------------------------------------*/
void MCGeometry::_failGeometry(const std::string failureMessage,
                               const unsigned int currentCellIndex,
                               const std::vector<double>& position,
                               const std::vector<double>& direction)
{
    Require(currentCellIndex < getNumCells());

    cout << "ERROR IN GEOMETRY: " << failureMessage << endl;
    
    cout << "Current cell index [" << currentCellIndex
         << "] user ID [" << _cells[currentCellIndex]->getUserId() << "]"
         << endl;
    cout << "Known cell connectivity: ";

    const SASVec& boundingSurfaces
            = _cells[currentCellIndex]->getBoundingSurfaces();

    for (SASVec::const_iterator bsIt = boundingSurfaces.begin();
                                bsIt != boundingSurfaces.end(); bsIt++)
    {
        _printSAS(*bsIt);
        cout << ":[";

        const CellT::CellContainer& otherCells =
            _cells[currentCellIndex]->getNeighbors(bsIt->first);

        for (CellT::CellContainer::const_iterator
                                     outCelIt = otherCells.begin(); 
                                     outCelIt != otherCells.end();
                                     ++outCelIt)
        {
            cout << (*outCelIt)->getUserId() << " ";
        }
        cout << "] ";
    }


    cout << "Was checking position " << position << " and direction "
         << direction << endl;


    Insist(0, "Geometry failure.");
}
/*----------------------------------------------------------------------------*/
void MCGeometry::debugPrint() const
{
    //-------------- PRINT USER SURFACE IDs ----------------//
    cout << "SURFACES: " << endl;
    for (SurfaceVec::const_iterator surfIt  = _surfaces.begin();
                                    surfIt != _surfaces.end(); ++surfIt)
    {
        cout << " SURFACE " << (*surfIt)->getUserId() << ": "
             << *(*surfIt) << endl;
    }

    //-------------- PRINT CELLS TO SURFACES ----------------//
    cout << "CELLS: " << endl;
    
    for (CellVec::const_iterator cellIt = _cells.begin();
                                 cellIt != _cells.end(); ++cellIt)
    {
        cout << " CELL " << (*cellIt)->getUserId() << ": ";

        // query cell for surface pointers
        const CellT::SASVec& boundingSurfaces =
                        (*cellIt)->getBoundingSurfaces();

        for (CellT::SASVec::const_iterator bsIt  = boundingSurfaces.begin(); 
                                        bsIt != boundingSurfaces.end(); ++bsIt)
        {
            _printSAS(*bsIt);
            cout << " ";
        }

        cout << endl;
    }

    //-------------- PRINT SURFACES TO CELLS ----------------//
    cout << "SURFACES TO CELLS: " << endl;

    for (SCConnectMap::const_iterator
            surfToCellIt = _surfToCellConnectivity.begin();
            surfToCellIt != _surfToCellConnectivity.end();
            ++surfToCellIt)
    {
        // print the surface and orientation
        cout << " ";

        _printSAS(surfToCellIt->first);

        cout << ": ";
        
        // print the cells
        const CellVec& theCells = surfToCellIt->second;

        for (CellVec::const_iterator cIt = theCells.begin();
                                     cIt != theCells.end(); ++cIt)
        {
            cout << (*cIt)->getUserId() << " ";
        }

        cout << endl;
    }

    cout << "NEIGHBORHOOD CONNECTIVITY: " << endl;
    //-------------- PRINT CELLS TO CELLS ----------------//
    for (CellVec::const_iterator cellIt = _cells.begin();
                                 cellIt != _cells.end(); ++cellIt)
    {
        cout << " CELL " << (*cellIt)->getUserId() << ": ";

        // query cell for surface pointers
        const CellT::SASVec& boundingSurfaces
                    = (*cellIt)->getBoundingSurfaces();

        for (CellT::SASVec::const_iterator bsIt = boundingSurfaces.begin();
                                    bsIt != boundingSurfaces.end(); bsIt++)
        {
            _printSAS(*bsIt);
            cout << ":{";

            const CellT::CellContainer& otherCells =
                (*cellIt)->getNeighbors(bsIt->first);

            for (CellT::CellContainer::const_iterator
                                        outCelIt = otherCells.begin(); 
                                        outCelIt != otherCells.end();
                                        ++outCelIt)
            {
                cout << (*outCelIt)->getUserId() << " ";
            }
            cout << "} ";
        }

        cout << endl;
    }  
}

/*----------------------------------------------------------------------------*/
// TODO: make ostream<<(std::pair<Surface*, bool>)
void MCGeometry::_printSAS(const SurfaceAndSense& qas) const
{
    if (qas.second == true)
        cout << "+";
    else
        cout << "-";

    cout << qas.first->getUserId();
}
/*----------------------------------------------------------------------------*/
// clean up after ourselves
MCGeometry::~MCGeometry()
{
    // map iterators act as a pointer to (key, value) pair
    for (SurfaceVec::iterator surfIt = _surfaces.begin();
                              surfIt != _surfaces.end(); ++surfIt)
    {
        // dereferenced iterator is a pointer to the surface we need to delete
        delete (*surfIt);
    }
    
    for (CellVec::iterator cellIt = _cells.begin();
                           cellIt != _cells.end(); ++cellIt)
    {
        // dereferenced iterator is a pointer to the cell we need to delete
        delete (*cellIt);
    }
}

/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
