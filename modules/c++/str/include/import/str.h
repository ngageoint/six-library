/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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


#ifndef __IMPORT_STR_H__
#define __IMPORT_STR_H__

/*!
 *  \file str.h
 *  \brief The str module includes all objects for string manipulation
 *
 *  When performing string operations, it is sometimes the case that the
 *  Standard Library std::string class is simply not enough.  The str
 *  module is a conglomeration of useful string manipulations including
 *  a Tokenizer class and a c-style formatting functions.
 *
 *  This library is intended to compliment the STL while providing additional
 *  functionality that the standard lacks
 *
 *
 */

#include "str/Convert.h"
#include "str/Tokenizer.h"
#include "str/Format.h"
#include "str/Manip.h"
#define STR_MAJOR_VERSION 0
#define STR_MINOR_VERSION 1
#define STR_MICRO_VERSION 0

#endif
