#ifndef VISUALIZESURFACES_HPP
#define VISUALIZESURFACES_HPP

#include "mcgeometry/MCGeometry.hpp"
#include <string>
#include <blitz/tinyvec.h>

//! Blitz++ TinyVector of length D stores position/direction/etc.
typedef blitz::TinyVector<double, 3> TVecDbl; 

/*!
 * \brief Print out where surface crossings happen and what surface they cross
 *
 * \param geo      The geometry to check
 * \param fileName The file to write it
 * \param bounds   The depth/width/height of the bounding box
 * \param subtract The offset of the bounding box
 */
void visualizeSurfaces( mcGeometry::MCGeometry& geo,
                        const std::string& fileName,
                        const TVecDbl& bounds,
                        const TVecDbl& subtract);
#endif
