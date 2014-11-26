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
#include "six/Options.h"

using namespace six;

Parameter Options::getParameter(std::string option) const
{
    ParameterIter p = mParameters.find(option);
    if (p == mParameters.end())
    {
        throw except::Exception(Ctxt("No such option exists"));
    }
    return p->second;
}

Parameter Options::getParameter(std::string option, Parameter defaultValue) const
{
    ParameterIter p = mParameters.find(option);
    if (p == mParameters.end())
    {
        return defaultValue;
    }
    return p->second;
}

void Options::setParameter(std::string option, Parameter value)
{
    mParameters[option] = value;
}

bool Options::hasParameter(std::string option) const
{
    ParameterIter p = mParameters.find(option);
    return (p != mParameters.end());
}

