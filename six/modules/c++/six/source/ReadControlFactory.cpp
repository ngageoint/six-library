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

#include <mutex>

#include "six/ReadControlFactory.h"

using namespace six;

std::unique_ptr<six::ReadControl> ReadControlRegistry::newReadControl(
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


static std::unique_ptr<six::ReadControlRegistry> mInstance;
static std::mutex g_ReadControlFactory_mutex;
six::ReadControlRegistry& six::ReadControlFactory::getInstance()
{
    //double-checked locking
    if (mInstance == nullptr)
    {
        std::lock_guard<std::mutex> guard(g_ReadControlFactory_mutex);
        if (mInstance == nullptr)
        {
            mInstance.reset(new ReadControlRegistry); //create the instance
        }
    }
    return *mInstance;
}
