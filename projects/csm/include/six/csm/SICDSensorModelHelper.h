#ifndef __SIX_CSM_SICD_SENSOR_MODEL_HELPER_H__
#define __SIX_CSM_SICD_SENSOR_MODEL_HELPER_H__

#include <six/sicd/ComplexData.h>
#include <six/csm/ProjectionModelBasedSensorModelHelper.h>

namespace six
{
namespace CSM
{
class SICDSensorModelHelper : public ProjectionModelBasedSensorModelHelper
{
public:
    SICDSensorModelHelper(
        mem::SharedPtr<six::sicd::ComplexData> data,
        const scene::SceneGeometry& geometry);

protected:
    virtual types::RowCol<double> getSampleSpacing() const;

    virtual types::RowCol<double> fromPixel(const csm::ImageCoord& pos) const;

    virtual
    types::RowCol<double> toPixel(const types::RowCol<double>& pos) const;

private:
    const mem::SharedPtr<six::sicd::ComplexData> mData;
};
}
}

#endif
