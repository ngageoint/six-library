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

//!  File security classification system
const char Classification::OPT_ISCLAS[] = "ISCLAS";
//!  File security classification system
const char Classification::OPT_ISCLSY[] = "ISCLSY";
//!  File classification text
const char Classification::OPT_ISCLTX[] = "ISCLTX";
//!  File security codewords
const char Classification::OPT_ISCODE[] = "ISCODE";
//!  File security reason 
const char Classification::OPT_ISCRSN[] = "ISCRSN";
//!  File control and handling
const char Classification::OPT_ISCTLH[] = "ISCTLH";
//!  File security control number
const char Classification::OPT_ISCTLN[] = "ISCTLN";
//!  File security declassification date
const char Classification::OPT_ISDCDT[] = "ISDCDT";
//!  File security declassification type
const char Classification::OPT_ISDCTP[] = "ISDCTP";
//!  File security declassification exemption
const char Classification::OPT_ISDCXM[] = "ISDCXM";
//!  File security downgrade
const char Classification::OPT_ISDG[] = "ISDG";
//!  File security downgrade date
const char Classification::OPT_ISDGDT[] = "ISDGDT";
//!  File releasing instructions
const char Classification::OPT_ISREL[] = "ISREL";
//!  File security source date
const char Classification::OPT_ISSRDT[] = "ISSRDT";

const char Classification::OPT_DESCLAS[] = "DESCLAS";
//!  File security classification system
const char Classification::OPT_DESCLSY[] = "DESCLSY";
//!  File classification text
const char Classification::OPT_DESCLTX[] = "DESCLTX";
//!  File security codewords
const char Classification::OPT_DESCODE[] = "DESCODE";
//!  File security reason 
const char Classification::OPT_DESCRSN[] = "DESCRSN";
//!  File control and handling
const char Classification::OPT_DESCTLH[] = "DESCTLH";
//!  File security control number
const char Classification::OPT_DESCTLN[] = "DESCTLN";
//!  File security declassification date
const char Classification::OPT_DESDCDT[] = "DESDCDT";
//!  File security declassification type
const char Classification::OPT_DESDCTP[] = "DESDCTP";
//!  File security declassification exemption
const char Classification::OPT_DESDCXM[] = "DESDCXM";
//!  File security downgrade
const char Classification::OPT_DESDG[] = "DESDG";
//!  File security downgrade date
const char Classification::OPT_DESDGDT[] = "DESDGDT";
//!  File releasing instructions
const char Classification::OPT_DESREL[] = "DESREL";
//!  File security source date
const char Classification::OPT_DESSRDT[] = "DESSRDT";

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
        //os << "   Date: " << str::toString<DateTime>(cg->date) << std::endl;
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

