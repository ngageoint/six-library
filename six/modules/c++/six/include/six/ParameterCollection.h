/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once
#ifndef __SIX_PARAMETER_COLLECTION_H__
#define __SIX_PARAMETER_COLLECTION_H__

#include <string>
#include <vector>
#include "six/Parameter.h"
#include "six/Exports.h"

namespace six
{

/*!
 *
 * \class ParameterCollection
 * \brief A collection of related parameters
 *
 * This class keeps track of a set of parameters.
 *
 */
class SIX_SIX_API ParameterCollection
{
public:

    /*!
     *  ParameterCollection's iterator
     */
    typedef std::vector<Parameter>::iterator ParameterCollectionIteratorT;

    /*!
     *  ParameterCollection's const iterator
     */
    typedef std::vector<Parameter>::const_iterator ConstParameterCollectionIteratorT;
    
    /*!
     * Given the name of a parameter, this function will search
     * for the first parameter of a matching name and return its
     * index. 
     * 
     *
     * \throw Exception if no matching parameter is found
     * \param paramName the name of the parameter to search for
     * \return the index of the parameter
     */
    size_t findParameterIndex(const std::string& paramName) const;

    /*!
     * Given the name of a parameter, this function will search for the
     * first parameter of a matching name and return a reference to it.
     *
     * \throw Exception if no matching parameter is found
     * \param paramName the name of the parameter to search for
     * \return the parameter
     */
    const Parameter& findParameter(const std::string& paramName) const
    {
        return mParams.at( findParameterIndex(paramName) );
    }

    /*!
     * Given the name of a parameter, this function will tell you if this
     * collection contains a matching parameter.
     *
     * \param paramName the name of the parameter to search for
     * \return true if there is a suitable parameter, false otherwise
     */
    bool containsParameter(const std::string& paramName) const;

    /*
     * Various container functions
     */

    Parameter& at(size_t i)
    {
        return mParams.at(i);
    }
    const Parameter& at(size_t i) const
    {
        return mParams.at(i);
    }

    Parameter& operator[](size_t i)
    {
        return mParams[i];
    }
    const Parameter& operator[](size_t i) const
    {
        return mParams[i];
    }

    void push_back(const Parameter& p)
    {
        mParams.push_back(p);
    }
    
    size_t size() const
    {
        return mParams.size();
    }
    bool empty() const
    {
        return mParams.empty();
    }
       
    ParameterCollectionIteratorT begin()
    {
        return mParams.begin();
    }
    ParameterCollectionIteratorT end()
    {
        return mParams.end();
    }

    ConstParameterCollectionIteratorT begin() const
    {
        return mParams.begin();
    }
    ConstParameterCollectionIteratorT end() const
    {
        return mParams.end();
    }

    const Parameter& front() const
    {
        return mParams.front();
    }
    const Parameter& back() const
    {
        return mParams.back();
    }
    
    Parameter& front()
    {
        return mParams.front();
    }
    Parameter& back()
    {
        return mParams.back();
    }

    bool operator==(const ParameterCollection& rhs) const;
    bool operator!=(const ParameterCollection& rhs) const
    {
        return !(*this == rhs);
    }

private:

    std::vector<Parameter> mParams;
};

}

#endif

