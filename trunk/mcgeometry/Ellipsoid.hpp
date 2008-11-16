/*!
 * \file Ellipsoid.hpp
 * \brief Ellipsoid object
 * \author Jeremy L. Conlin
 * 
 */


#ifndef MCG_ELLIPSOID_HPP
#define MCG_ELLIPSOID_HPP

#include <vector>
#include "Quadric.hpp"

namespace mcGeometry {
/*----------------------------------------------------------------------------*/
/*!
 * \class Ellipsoid
 * \brief A general ellipsoid
 */
class Ellipsoid : public Quadric {
public:
    Ellipsoid() {   }

    ~Ellipsoid();

    HitAndDist intersect(const std::vector<double>& Position, 
            const std::vector<double>& Direction, bool PosSense) const;

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    
};

/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
#endif 

