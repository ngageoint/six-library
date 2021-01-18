/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#include "six/ReadControlFactory.h"

using namespace six;

six::ReadControl* ReadControlRegistry::newReadControl(
        const std::string& filename) const
{
    for (const auto& creator : mCreators)
    {
        if (creator->supports(filename))
            return creator->newReadControl();
    }
    throw except::NotImplementedException(
                                          Ctxt(
                                               "No supported ReadControl for input file"));
}


// https://stackoverflow.com/a/11711991/8877
// "C++11 removes the need for manual locking.
// Concurrent execution shall wait if a static local variable is already being initialized."
six::ReadControlRegistry& six::ReadControlFactory::getInstance()
{
    static ReadControlRegistry instance;
    return instance;
}
