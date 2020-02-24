#include <six/sidd/Compression.h>

namespace six
{
namespace sidd
{
J2KCompression::Layer::Layer() :
    bitRate(0.0)
{
}

J2KCompression::J2KCompression() :
    numWaveletLevels(0),
    numBands(0)
{
}
}
}
