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
#pragma once

#include <memory>

#include <import/mt.h>

#include "six/ReadControl.h"

namespace six
{

struct ReadControlCreator
{
    virtual ~ReadControlCreator() = default;

    virtual std::unique_ptr<six::ReadControl> newReadControl() const = 0;

    virtual bool supports(const std::string& filename) const = 0;

protected:
    ReadControlCreator() = default;
};

class ReadControlRegistry
{
    std::list<ReadControlCreator*> mCreators;
public:
    ReadControlRegistry()
    {
    }

    virtual ~ReadControlRegistry();

    /**
     * Add a known creator to the registry. The registry takes ownership.
     */
    inline void addCreator(ReadControlCreator* creator)
    {
        mCreators.push_back(creator);
    }

    virtual std::unique_ptr<six::ReadControl> newReadControl(const std::string& filename) const;

};

//!  Singleton declaration of our ReadControlRegistry
typedef mt::Singleton<ReadControlRegistry, true> ReadControlFactory;

}

