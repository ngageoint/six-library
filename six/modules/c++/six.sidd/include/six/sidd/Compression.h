#ifndef __SIX_SIDD_COMPRESSION_H__
#define __SIX_SIDD_COMPRESSION_H__

#include <vector>

#include <mem/ScopedCopyablePtr.h>

namespace six
{
namespace sidd
{
struct J2KCompression
{
    struct Layer
    {
        Layer();

        double bitRate = 0.0;
    };

    J2KCompression();

    size_t numWaveletLevels = 0;
    size_t numBands = 0;

    std::vector<Layer> layerInfo;
};

struct Compression
{
    J2KCompression original;
    mem::ScopedCopyablePtr<J2KCompression> parsed;
};
}
}

#endif
