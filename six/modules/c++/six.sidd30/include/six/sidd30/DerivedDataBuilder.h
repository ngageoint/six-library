/* =========================================================================
 * This file is part of six.sidd30-c++
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

#ifndef __SIX_DERIVED_DATA_BUILDER_H__
#define __SIX_DERIVED_DATA_BUILDER_H__

#include "six/sidd30/DerivedData.h"

namespace six
{
namespace sidd
{
/*!
 *  \class DerivedDataBuilder
 *  \brief Incrementally builds a DerivedData object
 *
 *  This class implements the Builder design pattern by building
 *  and maintaining a DerivedData instance.  This class is the preferred
 *  method for creating a DerivedData object.  It should be used for one-offs
 *  or using get() in conjunction with the clone() method in the
 *  DerivedData object as a prototype.
 *
 */
class DerivedDataBuilder
{
public:
    //!  Construct. Creates and owns a new DerivedData object
    DerivedDataBuilder();

    /*!
     *  Construct, taking in a DerivedData object.  Warning: the builder
     *  does not take ownership of this object in this case.
     */
    DerivedDataBuilder(DerivedData *data);

    //!  Destructor
    virtual ~DerivedDataBuilder();

    /*!
     *  Add a display object of the given pixel type.  If one already
     *  exists in this object, it is deleted.  The value of the pixelType
     *  determines not only the pixelType in the Display object, but
     *  also the DisplayType.  When a Display is created, the proper
     *  Remap object is created.
     *
     *  \param pixelType what type of pixel is the product
     *  \return Reference to self
     */
    virtual DerivedDataBuilder& addDisplay(PixelType pixelType);

    /*!
     *  Add geographic data.  Use this for SIDD 2.0.
     */
    virtual DerivedDataBuilder& addGeoData();

    /*!
     *  Add geographic and target information.  This requires that the
     *  user provide us with a RegionType, in order to properly initialize
     *  the block.  Use this for SIDD 1.0
     *
     *  \param regionType What kind of region type scheme does the product use
     *  \return Reference to self
     */
    virtual DerivedDataBuilder& addGeographicAndTarget(RegionType regionType);

    /*!
     *  Add the block containing the meta-data for doing measurements.
     *  In order to initialize this properly, we need to know the
     *  type of projection we are supporting.  The currently supported
     *  SIDD projections are geographic, cylindrical, and projection
     *
     *  \param projectionType defaults to plane projection
     *  \return Reference to self
     *
     */
    virtual DerivedDataBuilder& addMeasurement(ProjectionType projectionType =
            ProjectionType::PLANE);

    /*!
     *  Add the section associated with exploitation features.  The parameter
     *  refers to the number of collections that were used to create the
     *  derived product.  The builder initializes the ExploitationFeatures
     *  constructor with this, which in turn, creates that number of
     *  collections in its vector
     *
     *  \param num The number of collections to create this product (defaults
     *  to one)
     *  \return Refrence to self
     */
    virtual DerivedDataBuilder& addExploitationFeatures(unsigned int num = 1);

    /*!
     *  Add the Optional ProductProcessing block to the SIDD Data object.
     *  This will in-turn lead ProductProcessing to create one ProcessingModule
     *  (at least one is required)
     */
    virtual DerivedDataBuilder& addProductProcessing();

    /*!
     *  Add the Optional DownstreamReprocessing block to the SIDD Data object.
     *  This will construct the block only.  The underlying values are
     *  not set, and the GeometricChip set is empty
     *
     *  \return Reference to self
     */
    virtual DerivedDataBuilder& addDownstreamReprocessing();


    /*!
     *  Add the optional error statistics component.
     *  \todo This should probably call take an argument for the SCPType,
     *  since we require that to do any work
     *
     *  \return Reference to self
     *
     */
    virtual DerivedDataBuilder& addErrorStatistics();

    /*!
     *  Create the radiometric calibration parameters.  All
     *  sub-parameters are optional and left undefined.
     *
     *  \return Reference to self
     */
    virtual DerivedDataBuilder& addRadiometric();

    /*!
     * Add optional MatchInformation element
     *
     * \return Reference to self
     */
    virtual DerivedDataBuilder& addMatchInformation();

    /*!
     *  Add the optional compression component.
     *
     * \return Reference to self
     */
    virtual DerivedDataBuilder& addCompression();

    /*!
    *  Add the optional digital elevation data component.
    *
    * \return Reference to self
    */
    virtual DerivedDataBuilder& addDigitalElevationData();

    /*!
     * Add the optional NITF LUT element.
     *
     * \return Reference to self
     */
    virtual DerivedDataBuilder& addNitfLUT();

    /*!
     *  Get the pointer to the object that is being built, but do not
     *  take ownership
     *
     */
    DerivedData* get();

    /*!
     *  Get the pointer to the object that is being build, and do take
     *  ownership.  Note that while the builder is in scope, it will
     *  continue to refer to this location (but it will not delete when
     *  it goes out of scope).
     */
    DerivedData* steal();

protected:
    DerivedData *mData;
    bool mAdopt;
};

}
}
#endif
