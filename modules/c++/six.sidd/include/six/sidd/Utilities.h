#ifndef __SIX_SIDD_UTILITIES_H__
#define __SIX_SIDD_UTILITIES_H__

#include <import/scene.h>
#include "six/sidd/DerivedData.h"

namespace six
{
namespace sidd
{
class Utilities
{
public:
    // TODO: This is only needed because getSceneGeometry() isn't implemented
    //       for all projection types
    static scene::SideOfTrack getSideOfTrack(const DerivedData* derived);

    static scene::SceneGeometry*
    getSceneGeometry(const DerivedData* derived);

    static scene::GridGeometry*
    getGridGeometry(const DerivedData* derived);

    static void setProductValues(Poly2D timeCOAPoly, PolyXYZ arpPoly,
            ReferencePoint ref, const Vector3* row, const Vector3* col,
            RangeAzimuth<double>res, Product* product);

    static void setProductValues(Vector3 arpVel, Vector3 arpPos,
            Vector3 refPos, const Vector3* row, const Vector3* col,
            RangeAzimuth<double>res, Product* product);

    static void setCollectionValues(Poly2D timeCOAPoly, PolyXYZ arpPoly,
            ReferencePoint ref, const Vector3* row, const Vector3* col,
            Collection* collection);

    static void setCollectionValues(Vector3 arpVel, Vector3 arpPos,
            Vector3 refPos, const Vector3* row, const Vector3* col,
            Collection* collection);

    static std::pair<six::PolarizationType, six::PolarizationType>
            convertDualPolarization(six::DualPolarizationType pol);

};
}
}
#endif
