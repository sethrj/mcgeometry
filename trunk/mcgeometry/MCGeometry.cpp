/*!
 * \file   MCGeometry.cpp
 * \brief  Contains implementation for the \c MCGeometry class
 * \author Seth R. Johnson
 */

#include "MCGeometry.hpp"

#include <utility>

#include "transupport/dbc.hpp"
#include "transupport/blitzStuff.hpp"

#include "Surface.hpp"
#include "Cell.hpp"

#include "Cell.i.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;

namespace mcGeometry {
/*============================================================================*\
 * subroutines that the user calls to create the geometry
 * and/or otherwise interface with the user ID
\*============================================================================*/

unsigned int MCGeometry::addSurface(
        const MCGeometry::UserSurfaceIDType userSurfaceId,
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
//! basically, parse a list of unsigned ints with +/- into surfaces and senses,
//  and add the cell
unsigned int MCGeometry::addCell(
        const UserCellIDType& userCellId,
        const IntVec& surfaceIds,
        const CellT::CellFlags flags)
{
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
//        cout << "Added surface " << userSurfaceId << " to cell " << userCellId << endl;
    }

    Check(surfaceIds.size() == boundingSurfaces.size());

    // call our internal function to do stuff to the parsed list of pointers
    return _addCell(userCellId, boundingSurfaces, flags);
}
/*----------------------------------------------------------------------------*/
//! add a cell based on a surface/sense vector
unsigned int MCGeometry::_addCell(
                                    const UserCellIDType&  userCellId,
                                    const CellT::SASVec&   boundingSurfaces,
                                    const CellT::CellFlags flags)
{
    // add a new "unmatched surface" for every surface in the cell
    _unMatchedSurfaces += boundingSurfaces.size();

    //====== add cell to the internal cell vector
    unsigned int newCellIndex = _cells.size();

    CellT* newCell = new CellT(boundingSurfaces, userCellId, newCellIndex,
                               flags);
    _cells.push_back(newCell);

//    cout << "Added cell with ID " << userCellId
//         << "that has index " << newCellIndex << endl;

    // add the reverse mapping to go from User ID to index and 
    // verify that this surface has not been added already (i.e. the
    // UserSurfaceIDType is unique)
    std::pair<CellRevIDMap::iterator, bool> result
        = _cellRevUserIds.insert(
                std::make_pair(userCellId, newCellIndex));

    Insist(result.second == true,
            "Tried to add a cell with an ID that was already there.");

    //====== loop back through the surfaces and add the connectivity
    for (CellT::SASVec::const_iterator bsIt = boundingSurfaces.begin();
                                       bsIt != boundingSurfaces.end(); ++bsIt)
    {
        SurfaceAndSense newQandS = *bsIt;

        if (flags & CellT::NEGATED) {
            // cell is inside out, so reverse the sense of the surface
            // with respect to how it connects to other cells
            newQandS.second = !(newQandS.second);
        }

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
//! get a user ID internal index  from a cell index
MCGeometry::UserCellIDType MCGeometry::getUserIdFromCellIndex(
                const unsigned int index) const
{
    Require( index < getNumCells() );

    return _cells[index]->getUserId();
}

/*----------------------------------------------------------------------------*/
//! get a user ID internal index  from a surface index
MCGeometry::UserSurfaceIDType MCGeometry::getUserIdFromSurfaceIndex(
                const unsigned int index) const
{
    Require( index < getNumSurfaces() );

    return _surfaces[index]->getUserId();
}
/*----------------------------------------------------------------------------*/
//! Get an internal index for a cell from a user ID.
unsigned int MCGeometry::getCellIndexFromUserId(
                const UserCellIDType userCellId) const
{
    // translate the user's given surface ID to a Surface pointer
    CellRevIDMap::const_iterator findCMResult = 
        _cellRevUserIds.find(userCellId);

    if (findCMResult == _cellRevUserIds.end()) {
        Insist(0, "FATAL ERROR: cell user ID does not exist.");
    }
    return findCMResult->second;
}
/*----------------------------------------------------------------------------*/
//! Get an internal index for a surface from a user ID.
unsigned int MCGeometry::getSurfaceIndexFromUserId(
             const UserSurfaceIDType userSurfaceId) const
{
    // translate the user's given surface ID to a Surface pointer
    SurfaceRevIDMap::const_iterator findSMResult = 
        _surfaceRevUserIds.find(userSurfaceId);

    if (findSMResult == _surfaceRevUserIds.end()) {
        Insist(0, "FATAL ERROR: surface user ID does not exist.");
    }
    return findSMResult->second;
}

/*============================================================================*\
 * other internal-use code
\*============================================================================*/
//! \cond
void MCGeometry::_warnGeometry( const std::string& shortMessage,
                                const TVecDbl& position,
                                const TVecDbl& direction,
                                const CellT* oldCell,
                                const std::string& longMessage) const
{
    std::ios_base::fmtflags ff;
    ff = cout.flags();
    cout << "      ****************************************************\n"
         << "        GEOMETRY WARNING: " << shortMessage << "\n"
         << "      ****************************************************\n"
         << std::setprecision(16)
         << std::scientific
         << "       POSITION:   " << position   << "\n"
         << std::fixed
         << "       DIRECTION:  " << direction << "\n"
         << std::setprecision(6)
         << "       CELL INDEX: " << oldCell->getIndex()
         << "       (user ID "    << oldCell->getUserId() << ") \n"
         << "       " << longMessage << "\n"
         << "      ****************************************************"
         << endl;

    cout.flags(ff);
}
                               
/*----------------------------------------------------------------------------*/
void MCGeometry::_failGeometry(
        const std::string& failureMessage,
        const unsigned int currentCellIndex,
        const TVecDbl& position,
        const TVecDbl& direction) const
{
    cout << "ERROR IN GEOMETRY: " << failureMessage << endl;
    
    cout << "Current cell index [" << currentCellIndex << "] ";

    Require(currentCellIndex < getNumCells());

    cout << "user ID [" << _cells[currentCellIndex]->getUserId() << "]"
         << endl;
    cout << "Known cell connectivity: ";

    const SASVec& boundingSurfaces
            = _cells[currentCellIndex]->getBoundingSurfaces();

    for (SASVec::const_iterator bsIt = boundingSurfaces.begin();
                                bsIt != boundingSurfaces.end(); bsIt++)
    {
        cout << *bsIt << ":[";

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

//! \endcond
/*----------------------------------------------------------------------------*/
void MCGeometry::debugPrint() const
{
    //-------------- PRINT USER SURFACE IDs ----------------//
    cout << "SURFACES: " << endl;
    for (SurfaceVec::const_iterator surfIt  = _surfaces.begin();
                                    surfIt != _surfaces.end(); ++surfIt)
    {
        cout << " SURFACE " << (*surfIt)->getUserId() << ": "
             << *(*surfIt);

        if ( (*surfIt)->isReflecting() )
            cout << " <REFLECTING>";

        cout << endl;
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
            cout << *bsIt << " ";
        }

        if ( (*cellIt)->isNegated() )
            cout << " <NEGATED>";

        if ( (*cellIt)->isDeadCell() )
            cout << " <DEADCELL>";

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
        cout << " " << surfToCellIt->first << ": ";
        
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
            cout << *bsIt << ":{";

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
// creation
MCGeometry::MCGeometry() :
    _unMatchedSurfaces(0)
{
    /* * */
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

/*============================================================================*/
} // end namespace mcGeometry
