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
#ifndef __SCENE_COORDINATE_TRANSFORM_H__
#define __SCENE_COORDINATE_TRANSFORM_H__

#include "scene/Types.h"
#include "scene/EllipsoidModel.h"


namespace scene
{

struct CoordinateTransform
{
    /**
     * Default constructor initializes the CoordinateTransform object with
     * a WGS84EllipsoidModel.
     */
    CoordinateTransform() = default;

    /**
     * This constructor initializes the CoordinateTransform object with
     * the attributes from the supplied EllipsoidModel.  The constructor
     * just copies the attribute values from the given EllipsoidModel object
     * to its own EllipsoidModel object.
     *
     * @param initVals      The EllipsoidModel to copy attribute values from
     */
    CoordinateTransform(const EllipsoidModel *initVals);

    virtual ~CoordinateTransform();

    /**
     * This function sets the EllipsoidModel attributes to the same values as
     * the given EllipsoidModel.
     *
     * @param initVals      The EllipsoidModel to copy attribute values from
     */
    virtual void setEllipsoidModel(const EllipsoidModel & initVals);

    /**
     * This function returns a pointer to the EllipsoidModel object
     *
     * @return A reference to the EllipsoidModel
     */
    virtual EllipsoidModel* getEllipsoidModel();

    /**
     * This function returns a constant pointer to the EllipsoidModel object
     *
     * @return A constant reference to the EllipsoidModel
     */
    virtual const EllipsoidModel* getEllipsoidModel() const;

    /**
     * This function returns a pointer to a clone of the CoordinateTransform 
     * object.
     */
    virtual CoordinateTransform* clone() const;

protected:

    EllipsoidModel* model = new WGS84EllipsoidModel();

};

}

#endif


