#ifndef __SIX_CSM_SIDD_PROJECTION_MODEL_BASED_HELPER_H__
#define __SIX_CSM_SIDD_PROJECTION_MODEL_BASED_HELPER_H__

#include <six/sidd/DerivedData.h>
#include <six/csm/ProjectionModelBasedSensorModelHelper.h>

namespace six
{
namespace CSM
{
class SIDDProjectionModelBasedHelper :
        public ProjectionModelBasedSensorModelHelper
{
public:
    SIDDProjectionModelBasedHelper(
        mem::SharedPtr<six::sidd::DerivedData> data,
        const scene::SceneGeometry& geometry);

protected:
    virtual types::RowCol<double> getSampleSpacing() const;

    virtual types::RowCol<double> fromPixel(const csm::ImageCoord& pos) const;

    virtual
    types::RowCol<double> toPixel(const types::RowCol<double>& pos) const;

private:
    const mem::SharedPtr<six::sidd::DerivedData> mData;
    types::RowCol<double> mSampleSpacing;
    types::RowCol<double> mRefPt;
};
}
}

#endif
