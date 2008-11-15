
#ifndef CELL_HPP
#define CELL_HPP

#include <vector>
#include <map>
#include <utility>
#include <limits>
#include "Quadric.hpp"
#include "transupport/dbc.hpp"

//#include <iostream>
//using std::cout;
//using std::endl;

/*----------------------------------------------------------------------------*/
class Cell {
//    friend class MCGeometry;
public:
    typedef std::pair<Quadric*, bool>          QuadricAndSense;
    typedef std::vector<QuadricAndSense>       QASVec;
    typedef std::vector<Cell*>                 CellVec;

    //! constructor requires an immutable bounding surface
    Cell(const QASVec& boundingSurfaces, const unsigned int userId = 0) 
        : _boundingSurfaces(boundingSurfaces),
          _userId(userId)
    {
        Require(_boundingSurfaces.size() > 0);
    }

    ~Cell() {
//        cout << "Oh no, I (cell " << this << ") am dying!" << endl;
        /* * */
    }

    //! get a list of bounding surfaces; maybe should make MCGeometry a friend
    // instead?
    const QASVec& getBoundingSurfaces() const {
        return _boundingSurfaces;
    }

    //! get a list of known cell neighbors for a quadric
    //  non-const so that whatever we pass can add to it
    //  possibly poor design here?
    //const CellVec& getNeighbors(const Quadric* surface) const {
    CellVec& getNeighbors(Quadric* surface) {
        // I think surface can't be const because findResult returns a pair
        // that has a non-const Quadric* in it
        HoodMap::iterator findResult = _hood.find(surface);
        Require(findResult != _hood.end());

        return findResult->second;
    }

    const unsigned int getUserId() const {
        return _userId;
    }

    ////! get a list of known cell neighbors for a quadric
    //void addNeighbor(const Quadric*, ) {
    //    return _boundingSurfaces;
    //}

    //! see if our cell contains the point; possibly skip one surface if
    // testing "next region"
    //  do this by comparing the "sense" of each of our surface to what the
    //  surface says about the position's sense relative to it
    bool isPointInside(const std::vector<double>& position,
                       const Quadric* surfaceToSkip = NULL) const;

    //! find the nearest surface from a point in a given direction
    void intersect( const std::vector<double>& position,
                    const std::vector<double>& direction,
                    Quadric*& hitQuadric,
                    bool&     quadricSense,
                    double&   distance) const;

private:
    typedef std::map< Quadric*, CellVec > HoodMap;

    const QASVec _boundingSurfaces;
    const unsigned int _userId;

    HoodMap _hood;
};

/*----------------------------------------------------------------------------*/
inline bool Cell::isPointInside(const std::vector<double>& position,
                                const Quadric* surfaceToSkip) const
{
    // loop over all surfaces
    for (QASVec::const_iterator it = _boundingSurfaces.begin();
                                      it != _boundingSurfaces.end(); ++it)
    {
        // it->first is the pointer to the Quadric object
        // it->second is the surface sense
        
        // if we need to check it
        if (it->first != surfaceToSkip)
        {
           if ( it->first->hasPosSense(position) != it->second ) {
               // if the surface reports that the sense of the point is 
               // NOT the same sense as we know this cell is defined, then
               // it is not inside.
               return false;
           }
        }
    }
    // we only get to this point if the point has the correct sense wrt every
    // surface that we checked
    return true;
}
/*----------------------------------------------------------------------------*/
inline void Cell::intersect(
                    const std::vector<double>& position,
                    const std::vector<double>& direction,
                    Quadric*& hitQuadric,
                    bool&     quadricSense,
                    double&   distance) const
{
    //since we only *pass* these variables to the lower level and don't actually
    //base anything on their properties in this function; let the lower level
    //handle the input checking instead of being redundant by including it here
    //and at MCGeometry
//    Require(position.size() == 3);
//    Require(direction.size() == 3);

    hitQuadric   = NULL;
    quadricSense = false;
    distance     = std::numeric_limits<double>::infinity();

    // loop over all surfaces
    for (QASVec::const_iterator it =  _boundingSurfaces.begin();
                                it != _boundingSurfaces.end(); ++it)
    {
        // call "intersect" on the quadric, also passing our sense 
        // of the quadric (it->second)
        Quadric::HitAndDist theResult = 
            it->first->intersect(position, direction, it->second);

        // if this check fails, then two surfaces apparently have exactly the
        // same distance from the particle; i.e. the particle ran into a corner?
        // what happens then?
//        Check(theResult.second != distance);

//        cout << "Test for " << typeid(*(it->first)).name()
//             << ": hit=" << theResult.first << " dist=" << theResult.second
//             << endl;

        if (theResult.first // if it hits, and if it's a smaller distance
                && (theResult.second < distance))
        {
            distance     = theResult.second;
            hitQuadric   = it->first;
            quadricSense = it->second;
        }
    }

    // really do this? yes, if intersect is only called if isInsideCell is a
    // certainty
    Ensure(hitQuadric != NULL);
    Ensure(distance   != std::numeric_limits<double>::infinity());
}

/*----------------------------------------------------------------------------*/
#endif

