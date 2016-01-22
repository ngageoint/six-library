#include <six/Init.h>
#include <six/sidd/LookupTable.h>

namespace six
{
namespace sidd
{
LookupTable::Predefined::Predefined() :
    remapFamily(six::Init::undefined<size_t>()),
    remapMember(six::Init::undefined<size_t>())
{
}

LookupTable::Custom::Custom(size_t numEntries, size_t numBands) :
    lut(numEntries, numBands)
{
}
}
}
