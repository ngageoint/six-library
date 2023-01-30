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

bool LookupTable::Predefined::operator==(
        const LookupTable::Predefined& rhs) const
{
    return (databaseName == rhs.databaseName &&
        remapFamily == rhs.remapFamily &&
        remapMember == rhs.remapMember);
}

LookupTable::Custom::Custom(size_t numEntries, size_t numLuts)
{
    for (size_t ii = 0; ii < numLuts; ++ii)
    {
        lutValues.push_back(LUT(numEntries, 2));
    }
}

bool LookupTable::operator==(const LookupTable& rhs) const
{
    return (predefined == rhs.predefined &&
        custom == rhs.custom);
}
}
}
