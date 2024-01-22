/* =========================================================================
 * This file is part of re-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#ifndef __RE_REGEX_FILE_PREDICATE_H__
#define __RE_REGEX_FILE_PREDICATE_H__

#include "sys/FileFinder.h"
#include "re/Regex.h"

namespace re
{

struct RegexPredicate : public sys::FilePredicate
{
public:
    RegexPredicate(const std::string& match)
    {
        mRegex.compile(match);
    }
    
    bool operator()(const std::string& filename) const override
    {
        return mRegex.matches(filename);
    }

private:
    re::Regex mRegex;

};

}
#endif

