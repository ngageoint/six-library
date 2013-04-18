/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_OPTIONS_H__
#define __SIX_OPTIONS_H__

#include "six/Types.h"
#include "six/Parameter.h"
#include <map>

namespace six
{
/*!
 *  \class Options
 *  \brief Configurable option map
 *
 *  This class represents a map of options.  It is used by the readers
 *  and writers to get out information.  This is similar to how DOM
 *  parsers work in XML.
 *
 *  To make life (arguably) easier, this class uses a Parameter object,
 *  which has overloaded C++ casting routines to get the objects
 *  from a string type into the native one.
 *
 */
class Options
{
    std::map<std::string, Parameter> mParameters;

public:
    typedef std::map<std::string, Parameter> ParameterMap;
    typedef std::map<std::string, Parameter>::const_iterator ParameterIter;

    Options()
    {
    }
    virtual ~Options()
    {
    }
    /*!
     *  Get back the parameter with option key specified in argument
     *
     */
    virtual Parameter getParameter(std::string option) const;

    /*!
     *  Get back the parameter with option key specified in argument.
     *  If we fail to find that option, supplement it with defaultValue
     *  that is passed in
     */
    virtual Parameter
            getParameter(std::string option, Parameter defaultValue) const;

    /*!
     *  Set a parameter with key given in option parameter
     */
    virtual void setParameter(std::string option, Parameter value);

    /*!
     *  Is there a parameter of name given in argument
     */
    virtual bool hasParameter(std::string option) const;

    //!  Allows us to iterate a parameter list
    ParameterIter begin() const { return mParameters.begin(); }

    //!  Allows us to compare an iterator against end
    ParameterIter end() const { return mParameters.end(); }

};
}

#endif

