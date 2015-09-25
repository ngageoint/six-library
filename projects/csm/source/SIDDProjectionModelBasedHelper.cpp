#include "Error.h"

#include <six/sidd/Utilities.h>
#include <six/csm/Utilities.h>
#include <six/csm/SIDDProjectionModelBasedHelper.h>

namespace six
{
namespace CSM
{
SIDDProjectionModelBasedHelper::SIDDProjectionModelBasedHelper(
        mem::SharedPtr<six::sidd::DerivedData> data,
        const scene::SceneGeometry& geometry) :
    ProjectionModelBasedSensorModelHelper(
            geometry,
            mem::SharedPtr<scene::ProjectionModel>(
                    six::sidd::Utilities::getProjectionModel(data.get()))),
    mData(data)
{
    if (!mData->measurement->projection->isMeasurable())
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           "Image projection type is not measurable",
                           "SIDDProjectionModelBasedHelper constructor");
    }

    const six::sidd::MeasurableProjection* const measProjection =
            reinterpret_cast<six::sidd::MeasurableProjection*>(
                    mData->measurement->projection.get());

    mSampleSpacing = measProjection->sampleSpacing;
    mRefPt = measProjection->referencePoint.rowCol;
}

types::RowCol<double> SIDDProjectionModelBasedHelper::getSampleSpacing() const
{
    return mSampleSpacing;
}

types::RowCol<double>
SIDDProjectionModelBasedHelper::fromPixel(const csm::ImageCoord& pos) const
{
    const types::RowCol<double> posRC(fromImageCoord(pos));
    types::RowCol<double> fullScenePos;
    if (mData->downstreamReprocessing.get() &&
        mData->downstreamReprocessing->geometricChip.get())
    {
        // The point that was passed in was with respect to the chip
        // ctrPt below will be with respect to the full image, so need to
        // adjust
        fullScenePos = mData->downstreamReprocessing->geometricChip->
                getFullImageCoordinateFromChip(posRC);
    }
    else
    {
        fullScenePos = posRC;
    }

    return types::RowCol<double>(
            (fullScenePos.row - mRefPt.row) * mSampleSpacing.row,
            (fullScenePos.col - mRefPt.col) * mSampleSpacing.col);
}

types::RowCol<double>
SIDDProjectionModelBasedHelper::toPixel(const types::RowCol<double>& pos) const
{
    const types::RowCol<double> fullScenePos =
            pos / mSampleSpacing + mRefPt;

    if (mData->downstreamReprocessing.get() &&
        mData->downstreamReprocessing->geometricChip.get())
    {
        // 'fullScenePos' is with respect to the original full image, but we
        // need it with respect to the chip that this SIDD actually represents
        return mData->downstreamReprocessing->geometricChip->
                getChipCoordinateFromFullImage(fullScenePos);
    }
    else
    {
        return fullScenePos;
    }
}
}
}
