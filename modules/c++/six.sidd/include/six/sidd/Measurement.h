/* =========================================================================
 * This file is part of six.sidd-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SIDD_MEASUREMENT_H__
#define __SIX_SIDD_MEASUREMENT_H__

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

    virtual ~Projection() {}

    //!  Pure
    virtual Projection* clone() const = 0;

    virtual bool isMeasurable() const { return false; }

};

/*!
 *  \struct PolynomialProjection
 *  \brief SIDD PolynomialProjection
 *
 *  Allows geo-referencing via polynomials
 *
 */
struct PolynomialProjection : public Projection
{
    //!  Initialize base class projection type
    PolynomialProjection()
    {
        this->projectionType = ProjectionType::POLYNOMIAL;
    }

    virtual ~PolynomialProjection() {}

    /*!
     *  Define a copy operator
     *
     */
    virtual Projection* clone() const
    {
        return new PolynomialProjection(*this);
    }


    //! Find a latitude associated with a row-col
    Poly2D rowColToLat;

    //! Find a longitude associated with a row-col
    Poly2D rowColToLon;

    //! Find an altitude associated with a row-col
    Poly2D rowColToAlt;

    //! Find a row in the image associated with a lat-lon
    Poly2D latLonToRow;

    //! Find a col in the image associated with a lat-lon
    Poly2D latLonToCol;

};

/*!
 *  Provides some more utility (TimeCOAPOly) than projection but 
 *  still pure.
 */
struct MeasurableProjection : public Projection
{
    virtual ~MeasurableProjection() {}

    RowColDouble sampleSpacing;

    //!  SIDD TimeCOAPoly
    Poly2D timeCOAPoly;

    bool isMeasurable() const { return true; }

};

/*!
 *  \struct GeographicProjection
 *  \brief SIDD GeographicProjection (GGD)
 *
 *  Geographic mapping of the pixel grid referred 
 *  to as GGD in the Design and Exploitation document.
 *
 */
struct GeographicProjection : public MeasurableProjection
{

    //!  Initialize base class projection type
    GeographicProjection()
    {
        this->projectionType = ProjectionType::GEOGRAPHIC;
    }

    //!  Define a clone operation
    virtual Projection* clone() const
    {
        return new GeographicProjection(*this);
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
struct CylindricalProjection : public MeasurableProjection
{

    //!  Cylindrical projection creation (curvature is undefined)
    CylindricalProjection()
    {
        this->projectionType = ProjectionType::CYLINDRICAL;
        this->curvatureRadius = Init::undefined<double>();
    }

    virtual ~CylindricalProjection() {}

    /*!
     *  Define a copy operator
     *
     */
    virtual Projection* clone() const
    {
        return new CylindricalProjection(*this);
    }

    Vector3 stripmapDirection;

    /*!
     *  Radius of Curvature defined at scene center.  
     *  If not present, the radius of curvature will be derived 
     *  based upon the equations provided in the Design and 
     *  Exploitation Document
     */
    double curvatureRadius;

};

/*!
 *  \struct PlaneProjection
 *  \brief Planar representation of the pixel grid
 *
 *  Derived Projection for plane projection, PGD according to D&E
 *
 */
struct PlaneProjection : public MeasurableProjection
{
    //!  Constructor
    PlaneProjection()
    {
        this->projectionType = ProjectionType::PLANE;
    }
    
    virtual ~PlaneProjection() {}

    //!  Clone operation
    virtual Projection* clone() const
    {
        return new PlaneProjection(*this);
    }

    //!  Product plane definition (defined by a basis)
    ProductPlane productPlane;


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
    mem::ScopedCloneablePtr<Projection> projection;

    //!  Number of rows/cols in the SIDD product
    RowColInt pixelFootprint;

    //!  The ARP Polynomial
    PolyXYZ arpPoly;

    /*!
     *  To initialize a measurenet, we need a product projection
     *  definition.  The currently available are geographic, plane
     *  and cylindrical (see ProjectionType).
     */
    Measurement(ProjectionType projectionType);

    //!  Deep copy including projection
    Measurement* clone();

};

}
}
#endif
