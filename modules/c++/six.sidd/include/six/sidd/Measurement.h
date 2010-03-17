/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __SIX_MEASUREMENT_H__
#define __SIX_MEASUREMENT_H__

#include "six/Types.h"
#include "six/Init.h"

namespace six
{

namespace sidd
{
/*!
 *  \struct ProductPlane
 *  \brief SIDD ProductPlane
 *
 *  The product plane definition for the product, defined by a row
 *  and column vector
 */
struct ProductPlane
{
    //!  Constructor, undefined basis
    ProductPlane()
    {
        rowUnitVector = Init::undefined<Vector3>();
        colUnitVector = Init::undefined<Vector3>();
    }

    //! Row vector for basis
    Vector3 rowUnitVector;

    //! Col vector for basis
    Vector3 colUnitVector;
};

/*!
 *  \struct Projection
 *  \brief Pure base class for projections
 *
 *  Contains information that is common to all projection types.
 */
struct Projection
{
    ProjectionType projectionType;
    ReferencePoint referencePoint;
    RowColDouble sampleSpacing;

    //!  Empty constructor
    Projection()
    {
    }
    //!  Empty
    virtual ~Projection() {}
    
    //!  Pure
    virtual Projection* clone() const = 0;
};

/*!
 *  \struct GeographicProjection
 *  \brief SIDD GeographicProjection (GGD)
 *
 *  Geographic mapping of the pixel grid referred 
 *  to as GGD in the Design and Exploitation document.
 *
 */
struct GeographicProjection: public Projection
{

    //!  Initialize base class projection type
    GeographicProjection()
    {
        this->projectionType = PROJECTION_GEOGRAPHIC;
    }
    //!  Define a clone operation
    virtual Projection* clone() const
    {
        GeographicProjection* g = new GeographicProjection(*this);
        return g;
    }
    virtual ~GeographicProjection() {}

};

/*!
 *  \struct CylindricalProection
 *  \brief SIDD CylindricalProjection
 *
 *  Cylindrical mapping of the pixel grid 
 *  referred to as CGD in the Design and Exploitation document.
 *
 */
struct CylindricalProjection: public Projection
{
    //!  Cylindrical projection creation (curvature is undefined)
    CylindricalProjection()
    {
        this->projectionType = PROJECTION_CYLINDRICAL;
        this->curvatureRadius = Init::undefined<double>();
    }

    /*!
     *  Radius of Curvature defined at scene center.  
     *  If not present, the radius of curvature will be derived 
     *  based upon the equations provided in the Design and 
     *  Exploitation Document
     */
    double curvatureRadius;

    /*!
     *  Define a copy operator
     *
     */
    virtual Projection* clone() const
    {
        CylindricalProjection* c = new CylindricalProjection(*this);
        return c;
    }
};

/*!
 *  \struct PlaneProjection
 *  \brief Planar representation of the pixel grid
 *
 *  Derived Projection for plane projection, PGD according to D&E
 *
 */
struct PlaneProjection: public Projection
{
    //!  Constructor
    PlaneProjection()
    {
        this->projectionType = PROJECTION_PLANE;
    }

    //!  Product plane definition (defined by a basis)
    ProductPlane productPlane;

    //!  Clone operation
    virtual Projection* clone() const
    {
        PlaneProjection* p = new PlaneProjection(*this);
        return p;
    }

};

/*!
 *  \struct Measurement
 *  \brief SIDD Measurement block
 *
 *  This contains information about the projection
 */
struct Measurement
{
    //!  PGD, GGD or CGD according to D&E
    Projection* projection;

    //!  Number of rows/cols in the SIDD product
    RowColInt pixelFootprint;

    //!  The ARP Polynomial
    PolyXYZ arpPoly;

    //!  SIDD TimeCOAPoly
    Poly2D timeCOAPoly;

    /*!
     *  To initialize a measurenet, we need a product projection
     *  definition.  The currently available are geographic, plane
     *  and cylindrical (see ProjectionType).
     */
    Measurement(ProjectionType projectionType);

    //!  Deep copy including projection
    Measurement* clone();

    //!  Deletes projection if non-NULL
    ~Measurement()
    {
        if (projection)
            delete projection;
    }
};

}
}
#endif
