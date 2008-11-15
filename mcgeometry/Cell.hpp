
#ifndef CELL_HPP
#define CELL_HPP

#include <vector>
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

    bool isPointInside(const std::vector<double>& position) const;
private:
    typedef std::map< Quadric*, CellPVec > HoodMap;

    const QASVec _boundingSurfaces;
    HoodMap _hood;
};
/*----------------------------------------------------------------------------*/
inline bool Cell::isPointInside(const std::vector<double>& position) const
{
    return false;
}
/*----------------------------------------------------------------------------*/
#endif

