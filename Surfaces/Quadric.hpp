

#ifndef QUADRIC_HPP
#define QUADRIC_HPP

#include <iostream>
#include <utility>

using std::cout;
using std::endl;

class Quadric{
    public:
        virtual ~Quadric()  {   }
        typedef std::pair<bool, double> HitAndDist;

        // Inersect will determine if the particle at Postion and Direction
        // will intersect with the surface.  It returns a std::pair HitAndDist
        // It assumes that the PosSense defines whether the particles thinks it
        // has a positive sense with respect to the surface
        virtual HitAndDist intersect(std::vector<double>& Position, 
                std::vector<double& Direction, bool PosSense) = 0;

        friend std::ostream& operator<<( std::ostream& os, QuadricSurface );

    protected:
        unsigned int _ID;

        QuadricSurface(unsigned int id) 
            : _ID(id)         // Make a unique ID for each Quadric surface
            {  }

        HitAndDist intersect( double A, double B, double C, bool sense );

    private:

};



inline HitAndDist Quadric::intersect(double A, double B, double C, bool sense){
	bool particleHitsSurface = false;
	double distanceToIntercept = -1.0;

	double Q = B*B - A*C;

	if (Q < 0) {
		particleHitsSurface = false;
		distanceToIntercept = 0.0;
	} 
    else {
        if (isLocPos == false) { //inside the surface (negative orientation)
			if (B <= 0) {   // headed away from the surface
				if (A > 0) {    // surface is curving upward
					particleHitsSurface = true;
					distanceToIntercept = (std::sqrt(Q) - B)/A;
				} 
                else {  // surface curving away and headed in, never hits it
					particleHitsSurface = false;
					distanceToIntercept = 0.0;
				}
			} 
            else {  // particle is heading toward the surface
				particleHitsSurface = true;
				distanceToIntercept = std::max(0.0, -C/(std::sqrt(Q) + B));
			}
		} 
        else {  // particle is outside
			if (B >= 0) {   // particle headed away
				if (A >= 0) {
					particleHitsSurface = false;
					distanceToIntercept = 0.0;
				} 
                else {
					particleHitsSurface = true;
					distanceToIntercept = -(std::sqrt(Q) + B)/A;
				}
			} 
            else {
				particleHitsSurface = true;
				distanceToIntercept = std::max(0.0, C/(std::sqrt(Q) - B));
			}
		}
	}
	return std::make_pair(particleHitsSurface, distanceToIntercept);
}

#endif

