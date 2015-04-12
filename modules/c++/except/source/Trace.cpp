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
#include "except/Trace.h"

/*!
 * \file Trace.cpp
 * \brief Class for holding exception traces
 *
 */

except::Trace& except::Trace::operator= (const except::Trace& t)
{
    if (&t != this)
    {
        mStack = t.getStack();
    }
    return *this;
}

std::ostream& operator<< (std::ostream& os, const except::Trace& t)
{
    const std::list<except::Context>& stack = t.getStack();
    int size = stack.size();
    for (std::list<except::Context>::const_iterator it = stack.begin();
            it != stack.end(); ++it)
    {
        os << *it << std::endl;
    }
    return os;
}

