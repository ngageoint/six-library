/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * scene-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SCENE_ELLIPSOID_MODEL_H__
#define __SCENE_ELLIPSOID_MODEL_H__

#include "scene/Types.h"

namespace scene
{

struct EllipsoidModel
{
    EllipsoidModel() = default;

    /**
     * This constructor initializes the EllipsoidModel object with
     * the attributes from the supplied EllipsoidModel.  The constructor
     * just copies the attribute values from the given EllipsoidModel object
     * to its own attributes.
     *
     * @param m      The EllipsoidModel to copy attribute values from
     */
    EllipsoidModel(const EllipsoidModel & m);

    /**
     * This constructor initializes the EllipsoidModel object with the
     * given attribute values.
     *
     * @param unitsVal      The units to use (meters vs feet)
     * @param angularUnitsVal      The angular units to use (radians vs degrees)
     * @param eqRadiusVal   The equatorial radius to use for the ellipsoid
     * @param polRadiusVal  The polar radius to use for the ellipsoid
     */
    EllipsoidModel(Units unitsVal,
                   AngularUnits angularUnitsVal,
                   double eqRadiusVal,
                   double polRadiusVal);

    virtual ~EllipsoidModel(){}

    /**
     * Returns the value of the units attribute
     *
     * @return      The units attribute
     */
    virtual Units getUnits() const;

    /**
     * Returns the value of the angularUnits attribute
     *
     * @return      The angularUnits attribute
     */
    virtual AngularUnits getAngularUnits() const;

    /**
     * Returns the value of the equatorialRadius attribute
     *
     * @return      The equatorialRadius attribute
     */
    virtual double getEquatorialRadius() const;

    /**
     * Returns the value of the polarRadius attribute
     *
     * @return      The polarRadius attribute
     */
    virtual double getPolarRadius() const;

    /**
     * This function calculates the flattening of the ellipsoid and returns
     * the value.  The flattening is calculated from the values of the
     * two radii of the ellipsoid.
     *
     * @return      The calculated flattening value
     */
    virtual double calculateFlattening() const;

    /**
     * This function calculates the eccentricity of the ellipsoid and returns
     * the value.  The eccentricity is calculated from the values of the
     * two radii of the ellipsoid.
     *
     * @return      The calculated eccentricity value
     */
    virtual double calculateEccentricity() const;

    /**
     * Sets the value of the units attribute
     *
     * @param val   The value to set the units attribute to
     */
    virtual void setUnits(Units val);

    /**
     * Sets the value of the angularUnits attribute
     *
     * @param val   The value to set the angularUnits attribute to
     */
    virtual void setAngularUnits(AngularUnits val);

    /**
     * Sets the value of the equatorialRadius attribute
     *
     * @param val   The value to set the equatorialRadius attribute to
     */
    virtual void setEquatorialRadius(double val);

    /**
     * Sets the value of the polarRadius attribute
     *
     * @param val   The value to set the polarRadius attribute to
     */
    virtual void setPolarRadius(double val);

    /**
     * This operator sets the attributes of the EllipsoidModel object with
     * the attributes from the supplied EllipsoidModel.  The operator
     * just copies the attribute values from the given EllipsoidModel object
     * to its own attributes.
     *
     * @param m      The EllipsoidModel to copy attribute values from
     */
    virtual EllipsoidModel & operator=(const EllipsoidModel & m);

protected:
    Units units = INVALID_UNITS;
    AngularUnits angularUnits = INVALID_ANGULAR_UNITS;
    double equatorialRadius;
    double polarRadius;
};

class WGS84EllipsoidModel : public EllipsoidModel
{
public:
    static const double EQUATORIAL_RADIUS_METERS;
    static const double POLAR_RADIUS_METERS;

    /**
     * This constructor initializes the object with default values.  By default
     * the units are meters and the angular units are radians.  The radius
     * values are those specified by the WGS84 model and cannot be set by the
     * user.
     */
    WGS84EllipsoidModel();

    /**
     * This constructor initializes the object with the specified values
     * for the units attribute and the angularUnits attribute.  The radius
     * values are those specified by the WGS84 model and cannot be set by the
     *  user.
     *
     * @param unitsVal      The units to use (meters vs feet)
     * @param angularUnitsVal      The angular units to use (radians vs degrees)
     */
    WGS84EllipsoidModel(Units unitsVal,
                        AngularUnits angularUnitsVal);

    /**
     * This constructor initializes the WGS84EllipsoidModel object with
     * the attributes from the supplied WGS84EllipsoidModel.  The constructor
     * just copies the attribute values from the given EllipsoidModel object
     * to its own attributes.
     *
     * @param m      The WGS84EllipsoidModel to copy attribute values from
     */
    WGS84EllipsoidModel(const WGS84EllipsoidModel & m);

    virtual ~WGS84EllipsoidModel(){}

    /**
     * This function sets the radius values to those specified by the WGS84
     * model.  The values set will be in the units given by the object's units
     * attribute.
     */
    virtual void initRadiusValues();

    /**
     * This function overrides the base class implementation so that the user
     * cannot change the value of the equatorialReduis attribute.  The function
     * only prints a message saying that the value cannot be changed.
     *
     * @param val   This parameter is not used.
     */
    virtual void setEquatorialRadius(double val);

    /**
     * This function overrides the base class implementation so that the user
     * cannot change the value of the polarReduis attribute.  The function
     * only prints a message saying that the value cannot be changed.
     *
     * @param val   This parameter is not used.
     */
    virtual void setPolarRadius(double val);

    /**
     * This operator sets the attributes of the EllipsoidModel object with
     * the attributes from the supplied EllipsoidModel.  The operator
     * just copies the attribute values from the given EllipsoidModel object
     * to its own attributes.  This operator overloads the base class
     * implementation so that the radius values cannot be changed by the user.
     *
     * @param m      The EllipsoidModel to copy attribute values from
     */
    virtual EllipsoidModel & operator=(const EllipsoidModel & m);

    /**
     * Compute normal vector at given point
     * \param point A point in ECEF space
     * \return Normal vector to ellipsoid at given point
     */
    Vector3 getNormalVector(const Vector3& point) const;

};


}
#endif


