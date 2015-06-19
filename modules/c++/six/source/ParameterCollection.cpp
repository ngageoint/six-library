#include "six/Parameter.h"
#include "six/ParameterCollection.h"

#include <algorithm>

six::Parameter& six::ParameterCollection::at(size_t i)
{
    return mParams.at(i);
}

const six::Parameter& six::ParameterCollection::at(size_t i) const 
{
    return mParams.at(i);
}

six::Parameter& six::ParameterCollection::operator[](size_t i) 
{
    return mParams[i];
}

const six::Parameter& six::ParameterCollection::operator[](size_t i) const 
{
    return mParams[i];
}

void six::ParameterCollection::push_back(const six::Parameter & p) 
{
    mParams.push_back(p);
}

size_t six::ParameterCollection::size() const
{
    return mParams.size();
}

bool six::ParameterCollection::empty() const
{
    return mParams.empty();
}

six::Parameter* six::ParameterCollection::begin()
{
    return &front();
}

six::Parameter* six::ParameterCollection::end()
{
    return &back() + 1;
}

const six::Parameter* six::ParameterCollection::begin() const
{
    return &front();
}

const six::Parameter* six::ParameterCollection::end() const
{
    return &back() + 1; 
}

const six::Parameter& six::ParameterCollection::front() const
{
    return mParams.front();
}

const six::Parameter& six::ParameterCollection::back() const
{
    return mParams.back();
}

six::Parameter& six::ParameterCollection::front()
{
    return mParams.front();
}

six::Parameter& six::ParameterCollection::back()
{
    return mParams.back();
}

size_t six::ParameterCollection::findParameter(const std::string & paramName) const
{
    for(size_t i = 0; i < mParams.size(); ++i)
    {
        if(paramName == mParams[i].getName()) return i;
    }

    return mParams.size();
}
