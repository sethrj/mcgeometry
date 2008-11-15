
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
    typedef std::vector<Cell*>                 CellPVec;

    typedef std::pair<Quadric*, double>        QuadricAndDist;

    //! constructor requires an immutable bounding surface
    Cell(const QASVec& boundingSurfaces) 
        : _boundingSurfaces(boundingSurfaces)
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

    //! see if our cell contains the point; possibly skip one surface if
    // testing "next region"
    //  do this by comparing the "sense" of each of our surface to what the
    //  surface says about the position's sense relative to it
    bool isPointInside(const std::vector<double>& position,
                       const Quadric* surfaceToSkip = NULL) const;

    //! find the nearest surface from a point in a given direction
    QuadricAndDist intersect( const std::vector<double>& position,
                              const std::vector<double>& direction) const;

private:
    typedef std::map< Quadric*, CellPVec > HoodMap;

    const QASVec _boundingSurfaces;
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
inline Cell::QuadricAndDist Cell::intersect(
                            const std::vector<double>& position,
                            const std::vector<double>& direction) const
{
    Require(position.size() == 3);
    Require(direction.size() == 3);

    Quadric* hitQuadric = NULL;
    double   distance   = std::numeric_limits<double>::infinity();

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
            distance = theResult.second;
            hitQuadric = it->first;
        }
    }

    // really do this? yes, if intersect is only called if isInsideCell is a
    // certainty
    Ensure(hitQuadric != NULL);
    Ensure(distance   != std::numeric_limits<double>::infinity());

    return std::make_pair(hitQuadric, distance);
}

/*----------------------------------------------------------------------------*/
#endif

