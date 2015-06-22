#include "six/Parameter.h"
#include "six/ParameterCollection.h"

#include <import/except.h>

size_t six::ParameterCollection::findParameterIndex(const std::string& paramName) const
{
    for(size_t ii = 0; ii < mParams.size(); ++ii)
    {
        if (paramName == mParams[ii].getName())
        {
            return ii;
        }
    }

    throw except::Exception("No parameter with name \"" + paramName 
        + "\" found in this collection");
}

bool six::ParameterCollection::containsParameter(const std::string& paramName) const
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
