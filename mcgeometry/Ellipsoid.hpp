/*!
 * \file Ellipsoid.hpp
 * \brief Ellipsoid object
 * \author Jeremy L. Conlin
 * 
 */


#ifndef MCG_ELLIPSOID_HPP
#define MCG_ELLIPSOID_HPP

#include <vector>
#include "Surface.hpp"

namespace mcGeometry {
/*----------------------------------------------------------------------------*/
/*!
 * \class Ellipsoid
 * \brief A general ellipsoid
 */
class Ellipsoid : public Surface {
public:
    Ellipsoid() {   }

    ~Ellipsoid();

    void intersect( const std::vector<double>& position, 
                    const std::vector<double>& direction,
                    const bool PosSense,
                    bool& hit,
                    double& distance) const;

protected:
    //! output to a stream
    std::ostream& _output( std::ostream& os ) const;

private:
    
};

/*----------------------------------------------------------------------------*/
} // end namespace mcGeometry
#endif 

