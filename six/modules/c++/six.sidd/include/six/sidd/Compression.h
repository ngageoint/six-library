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

        double bitRate;
    };

    J2KCompression();

    size_t numWaveletLevels;
    size_t numBands;

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
