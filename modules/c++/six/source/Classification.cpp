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
#include "six/Classification.h"

using namespace six;

Classification::~Classification()
{
    if (guidance)
        delete guidance;
}

Classification::Classification(const Classification& c)
{
    level = c.level;
    securityMarkings = c.securityMarkings;
    fileOptions = c.fileOptions;
    guidance = NULL;
    if (c.guidance)
    {
        guidance = c.guidance->clone();
    }
}

Classification& Classification::operator=(const Classification& c)
{

    if (this != &c)
    {
        level = c.level;
        securityMarkings = c.securityMarkings;
        fileOptions = c.fileOptions;
        guidance = NULL;
        if (c.guidance)
        {
            guidance = c.guidance->clone();
        }
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Classification& c)
{
    os << "Classification: " << std::endl;
    os << "------------------------------------" << std::endl;
    os << "Level: " << c.level << std::endl;

    os << "Security Markings: " << std::endl;
    for (unsigned int i = 0; i < c.securityMarkings.size(); ++i)
    {
        os << "    [" << c.securityMarkings[i] << "]" << std::endl;
    }

    six::ClassificationGuidance* cg = c.guidance;
    if (cg)
    {
        os << "Guidance: " << std::endl;
        //os << "   Date: " << six::toString<DateTime>(cg->date) << std::endl;
        os << "   Authority: " << cg->authority << std::endl;
    }

    os << "File specific options: " << std::endl;
    for (Options::ParameterIter p = c.fileOptions.begin(); p
            != c.fileOptions.end(); ++p)
    {
        os << "    {" << p->first << "}  " << p->second.str() << std::endl;
    }
    return os;
}

