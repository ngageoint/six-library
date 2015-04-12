/* =========================================================================
 * This file is part of except-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * except-c++ is free software; you can redistribute it and/or modify
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


#include <iostream>
#include "except/Context.h"

/*!
 * \file Context.cpp
 * \brief defines a class that contains the information surrounding an
 * exception or error.
 */

except::Context::Context(const except::Context& c)
{
    mMessage = c.getMessage();
    mTime = c.getTime();
    mFunc = c.getFunction();
    mFile = c.getFile();
    mLine = c.getLine();
}

except::Context& except::Context::operator=(const except::Context& c)
{
    if (&c != this)
    {
        mMessage = c.getMessage();
        mTime = c.getTime();
        mFunc = c.getFunction();
        mFile = c.getFile();
        mLine = c.getLine();
    }
    return *this;
}

std::ostream& operator<< (std::ostream& os, const except::Context& c)
{
    os << "(" << c.getFile() << ", ";
    os << c.getLine() << ", ";
    os << c.getFunction() << "): [";
    os << c.getTime() << "] '";
    os << c.getMessage() << "' ";
    return os;
}

