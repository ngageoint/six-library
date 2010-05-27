/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_SIDD_ANNOTATIONS_H__
#define __SIX_SIDD_ANNOTATIONS_H__

#include <import/six.h>
#include "six/sidd/SFA.h"

namespace six
{
namespace sidd
{

struct Annotation
{
    typedef ::six::sidd::sfa::Geometry SFAGeometry;
    typedef ::six::sidd::sfa::ReferenceSystem SFAReferenceSystem;

    std::string identifier;
    SFAReferenceSystem* spatialReferenceSystem;
    std::vector<SFAGeometry*> objects;

    Annotation() :
        identifier("")
    {
    }
    ~Annotation()
    {
        if (spatialReferenceSystem)
            delete spatialReferenceSystem;

        for (size_t i = 0, n = objects.size(); i < n; ++i)
            if (objects[i])
                delete objects[i];
    }
};

typedef std::vector<Annotation*> Annotations;

}
}
#endif
