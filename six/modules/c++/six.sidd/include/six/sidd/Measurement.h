/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <gsl/gsl.h>

#include <six/Types.h>
#include <six/Init.h>
#include <six/sidd/Enums.h>

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

    //! Equality operator
    bool operator==(const ProductPlane& rhs) const
    {
        return (rowUnitVector == rhs.rowUnitVector &&
            colUnitVector == rhs.colUnitVector);
    }

    bool operator!=(const ProductPlane& rhs) const
    {
        return !(*this == rhs);
    }
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

    virtual ~Projection() = default;

    //!  Pure
    virtual Projection* clone() const = 0;
    virtual bool isMeasurable() const { return false; }

    virtual bool equalTo(const Projection& rhs) const = 0;
};
inline bool operator==(const Projection& lhs, const Projection& rhs)
{
    return lhs.equalTo(rhs) && rhs.equalTo(lhs);
}
inline bool operator!=(const Projection& lhs, const Projection& rhs)
{
    return !(lhs == rhs);
}

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

    virtual ~PolynomialProjection() = default;

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

private:
    bool operator_eq(const PolynomialProjection& rhs) const;
    bool equalTo(const Projection& rhs) const override;
};

/*!
 *  Provides some more utility (TimeCOAPOly) than projection but
 *  still pure.
 */
struct MeasurableProjection : public Projection
{
    virtual ~MeasurableProjection() = default;

    RowColDouble sampleSpacing;

    //!  SIDD TimeCOAPoly
    Poly2D timeCOAPoly;

    bool isMeasurable() const { return true; }

    bool operator_eq(const MeasurableProjection& rhs) const;
private:
    bool equalTo(const Projection& rhs) const override;
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
    virtual ~GeographicProjection() = default;

private:
    bool operator_eq(const GeographicProjection& rhs) const;
    bool equalTo(const Projection& rhs) const override;
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

    virtual ~CylindricalProjection() = default;

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

private:
    bool operator_eq(const CylindricalProjection& rhs) const;
    bool equalTo(const Projection& rhs) const override;
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

    virtual ~PlaneProjection() = default;

    //!  Clone operation
    virtual Projection* clone() const
    {
        return new PlaneProjection(*this);
    }

    //!  Product plane definition (defined by a basis)
    ProductPlane productPlane;

private:
    bool operator_eq(const PlaneProjection& rhs) const;
    bool equalTo(const Projection& rhs) const override;
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
    const RowColInt& getPixelFootprint() const
    {
        return value(pixelFootprint); // be sure pixelFootprint is initialized
    }
    void setPixelFootprint(const types::RowCol<size_t>& aoiDims)
    {
        pixelFootprint.row = gsl::narrow<ptrdiff_t>(aoiDims.row);
        pixelFootprint.col = gsl::narrow<ptrdiff_t>(aoiDims.col);
    }

    /*!
     * Flag indicating whether ARP polynomial is based on the best available
     * ("collect time") or "predicted" ephemeris
     * New (and optional) in SIDD 2.0
     */
    ARPFlag arpFlag;

    //!  The ARP Polynomial
    PolyXYZ arpPoly;

    /*! Indicates the full image includes both valid data and some zero-filled
     *  pixels.
     *  New (and required) in SIDD 2.0
     */
    std::vector<RowColInt> validData;

    /*!
     *  To initialize a measurement, we need a product projection
     *  definition.  The currently available are geographic, plane
     *  and cylindrical (see ProjectionType).
     */
    Measurement(ProjectionType projectionType);

    //! Equality operator
    bool operator==(const Measurement& rhs) const
    {
        return (projection == rhs.projection && arpPoly == rhs.arpPoly &&
            pixelFootprint == rhs.pixelFootprint);
    }

    bool operator!=(const Measurement& rhs) const
    {
        return !(*this == rhs);
    }
};

}
}
#endif
