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


#include "str/Tokenizer.h"


str::Tokenizer::Tokenizer(const std::string& str, const std::string& delim)
{
    int str_l = (int)str.length();
    int pos = 0;
    int start, end;
    while (true)
    {
        start = (int)str.find_first_not_of(delim, pos);
        if (start == std::string::npos)
        {
            break;
        }
        end = (int)str.find_first_of(delim, start);
        if (end == std::string::npos)
        {
            end = str_l;
        }
        vec.push_back(str.substr(start, end - start));
        pos = end;

    }

}
