#ifndef __SIX_CSM_UTILITIES_H__
#define __SIX_CSM_UTILITIES_H__

#include "csm.h"

#include <scene/Types.h>

namespace six
{
namespace CSM
{
inline
scene::Vector3 toVector3(const csm::EcefCoord& pt)
{
    scene::Vector3 vec;
    vec[0] = pt.x;
    vec[1] = pt.y;
    vec[2] = pt.z;
    return vec;
}

inline
csm::EcefVector toEcefVector(const scene::Vector3& vec)
{
    return csm::EcefVector(vec[0], vec[1], vec[2]);
}

inline
csm::EcefCoord toEcefCoord(const scene::Vector3& vec)
{
    return csm::EcefCoord(vec[0], vec[1], vec[2]);
}

inline
csm::ImageCoord toImageCoord(const types::RowCol<double>& pt)
{
    return csm::ImageCoord(pt.row, pt.col);
}

inline
types::RowCol<double> fromImageCoord(const csm::ImageCoord& pos)
{
    return types::RowCol<double>(pos.line, pos.samp);
}
}
}

#endif
