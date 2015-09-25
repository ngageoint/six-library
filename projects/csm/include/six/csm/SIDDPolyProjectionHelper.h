/* =========================================================================
 * This file is part of the CSM SICD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * The CSM SICD Plugin is free software; you can redistribute it and/or modify
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
 * License along with this program; if not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __SIX_CSM_SIDD_POLY_PROJECTION_HELPER_H__
#define __SIX_CSM_SIDD_POLY_PROJECTION_HELPER_H__

#include <six/sidd/DerivedData.h>
#include <six/csm/SensorModelHelper.h>

namespace six
{
namespace CSM
{
class SIDDPolyProjectionHelper : public SensorModelHelper
{
public:
    SIDDPolyProjectionHelper(mem::SharedPtr<six::sidd::DerivedData> data);

    virtual csm::ImageCoord groundToImage(
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual csm::ImageCoordCovar groundToImage(
            const csm::EcefCoordCovar& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* ) const;

    virtual csm::EcefCoord imageToGround(
            const csm::ImageCoord& imagePt,
            double height,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual csm::EcefCoordCovar imageToGround(
            const csm::ImageCoordCovar& imagePt,
            double height,
            double heightVariance,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual double getImageTime(const csm::ImageCoord& imagePt) const;

    virtual
    csm::EcefCoord getSensorPosition(const csm::ImageCoord& imagePt) const;

    virtual csm::EcefCoord getSensorPosition(double time) const;

    virtual csm::EcefVector
    getSensorVelocity(const csm::ImageCoord& imagePt) const;

    virtual csm::EcefVector getSensorVelocity(double time) const;

private:
    double getCenterTime() const
    {
        // TODO: If there's more than one collection, what should we use?
        const double duration =
                mData->exploitationFeatures->collections[0]->information->
                        collectionDuration;
        return (duration / 2);
    }

private:
    const mem::SharedPtr<six::sidd::DerivedData> mData;
    const six::sidd::PolynomialProjection* mProjection;
};
}
}

#endif
