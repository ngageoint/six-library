#include <except/Exception.h>
#include <six/Parameter.h>
#include <six/ParameterCollection.h>

namespace six
{
size_t
ParameterCollection::findParameterIndex(const std::string& paramName) const
{
    for(size_t ii = 0; ii < mParams.size(); ++ii)
    {
        if (paramName == mParams[ii].getName())
        {
            return ii;
        }
    }

    throw except::NoSuchKeyException(Ctxt(
        "No parameter with name \"" + paramName
        + "\" found in this collection"));
}

bool
ParameterCollection::containsParameter(const std::string& paramName) const
{
    for(size_t ii = 0; ii < mParams.size(); ++ii)
    {
        if (paramName == mParams[ii].getName())
        {
            return true;
        }
    }

    return false;
}

bool
ParameterCollection::operator==(const ParameterCollection& rhs) const
{
    return mParams == rhs.mParams;
}
}
