/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <sys/Backtrace.h>

std::string sys::getBacktrace(bool* pSupported)
{
    bool supported;
    std::vector<std::string> frames;
    bool& supported_ = pSupported != nullptr ? *pSupported : supported;
    return getBacktrace(supported_, frames);
}
std::string sys::getBacktrace(bool& supported, std::vector<std::string>& frames)
{
    return except::getBacktrace(supported, frames);
}
