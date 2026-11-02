/* =========================================================================
 * This file is part of re-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * re-c++ is free software; you can redistribute it and/or modify
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

#ifndef __RE_REGEX_EXCEPTION_H__
#define __RE_REGEX_EXCEPTION_H__

#include "except/Exception.h"

namespace re
{
/*!
 *  \class RegexException
 *  \brief Error class for pcre
 *  
 *  Specialized for regular expression errors
 */
class RegexException : public except::Exception
{
public:

    /*!
     *  The constructor
     *  \param message A message to take
            *  \param errorId  The error id
     */
    RegexException(const char *message, int /*errorId*/ = 0) : except::Exception(message)
    {}

    /*!
     *  The constructor
     *  \param message A message to take
            *  \param errorId  The error id
     */
    RegexException(const std::string& message, int /*errorId*/ = 0) : except::Exception(message)
    {}

    /*!
     *  The constructor
     *  \param c A Context to take
            *  \param errorId  The error id
     */
    RegexException(const except::Context& c, int /*errorId*/ = 0) : except::Exception(c)
    {}
};

}
#endif
