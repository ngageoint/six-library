/* =========================================================================
 * This file is part of logging-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * logging-c++ is free software; you can redistribute it and/or modify
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

///////////////////////////////////////////////////////////
//  StandardFormatter.cpp
///////////////////////////////////////////////////////////

#include <sstream>
#include <iostream>
#include <import/sys.h>
#include <import/str.h>
#include "logging/Formatter.h"

const char logging::Formatter::THREAD_ID[] = "%t";
const char logging::Formatter::LOG_NAME[]  = "%c";
const char logging::Formatter::LOG_LEVEL[] = "%p";
const char logging::Formatter::TIMESTAMP[] = "%d";
const char logging::Formatter::FILE_NAME[] = "%F";
const char logging::Formatter::LINE_NUM[]  = "%L";
const char logging::Formatter::MESSAGE[]   = "%m";
const char logging::Formatter::FUNCTION[]  = "%M";


std::string logging::Formatter::getPrologue() const
{
    return (mPrologue.empty()) ? "" : mPrologue + "\n";
}
std::string logging::Formatter::getEpilogue() const 
{ 
    return (mEpilogue.empty()) ? "" : mEpilogue + "\n"; 
}



