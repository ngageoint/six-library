/* =========================================================================
 * This file is part of unique-c++ 
 * =========================================================================
 * 
 * © 2009 MDA Information Systems LLC
 *
 * unique-c++ is free software; you can redistribute it and/or modify
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

#ifndef __UNIQUE_UUID_HPP__
#define __UNIQUE_UUID_HPP__

#include <import/except.h>

namespace unique
{

typedef except::InvalidFormatException UUIDException;

/*!
 * Create a 36-character UUID and return as a std::string
 */ 
std::string generateUUID();

}
#endif

