#ifndef __SIX_SICD_UTILITIES_H__
#define __SIX_SICD_UTILITIES_H__

#include <import/scene.h>
#include "six/sicd/ComplexData.h"

namespace six
{
namespace sicd
{
class Utilities
{
public:

    static scene::SceneGeometry* getSceneGeometry(const ComplexData* data);
    static scene::ProjectionModel* getProjectionModel(const ComplexData* data, 
            const scene::SceneGeometry* geom);

};
}
}
#endif
