/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
 *
 * str-c++ is free software; you can redistribute it and/or modify
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


#ifndef __STR_FORMAT_H__
#define __STR_FORMAT_H__

#include <stdarg.h>
#include <string>

namespace str
{

/*!
 *  \param format  The format
 *  \param ... Any printf like thing
 */
std::string format(const char *format, ...);

class Format
{
public:
    Format(const char* format, ...);

    operator std::string() const
    {
        return mString;
    }

    operator std::string& ()
    {
        return mString;
    }

protected:
    std::string mString;
};

}
#endif
