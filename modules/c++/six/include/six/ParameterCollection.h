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

#ifndef __SIX_PARAMETER_COLLECTION_H__
#define __SIX_PARAMETER_COLLECTION_H__

#include <string>
#include <vector>
#include "six/Parameter.h"

namespace six
{

/*!
 *
 * \class ParameterCollection
 * \brief A collection of related parameters
 *
 * This class keeps track of a set of parameters.
 *
 * @todo convienence functions
 *
 */
class ParameterCollection
{
public:
    Parameter& at(size_t i);
    const Parameter& at(size_t i) const;

    Parameter& operator[](size_t i);
    const Parameter& operator[](size_t i) const;

    void push_back(const Parameter & p);
    
    size_t size() const;
    bool empty() const;
       
    Parameter* begin();
    Parameter* end();

    const Parameter* begin() const;
    const Parameter* end() const;

    const Parameter& front() const;
    const Parameter& back() const;
    
    Parameter& front();
    Parameter& back();

    /*!
     * Given the name of a parameter, this function will search
     * for the first parameter of a matching name and return its
     * index. If a parameter with a matching name is not found, the
     * number of parameters contained is returned (ie, an out of bounds
     * index)
     *
     * \param paramName the name of the parameter to search for
     * \return the index of the parameter, or if it is not present
     *         size() is returned.
     */
    size_t findParameter(const std::string & paramName) const;

private:

    std::vector<Parameter> mParams;
    Parameter* mRecentlyQueried;
};

}

#endif

