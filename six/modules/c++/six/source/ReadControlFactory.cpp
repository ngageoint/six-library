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

std::unique_ptr<six::ReadControl> ReadControlRegistry::newReadControl(
        const std::string& filename) const
{
    for (std::list<ReadControlCreator*>::const_iterator it = mCreators.begin(); it
            != mCreators.end(); ++it)
    {
        if ((*it)->supports(filename))
            return (*it)->newReadControl();
    }
    throw except::NotImplementedException(
                                          Ctxt(
                                               "No supported ReadControl for input file"));
}

ReadControlRegistry::~ReadControlRegistry()
{
    while(!mCreators.empty())
    {
        ReadControlCreator *creator = mCreators.front();
        if (creator)
            delete creator;
        mCreators.pop_front();
    }
}

