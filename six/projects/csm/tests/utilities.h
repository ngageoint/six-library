/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <six/Utilities.h>

std::string toString(nitf::FileSecurity security)
{
    const std::string str = security.getClassificationSystem().toString() +
            security.getCodewords().toString() +
            security.getControlAndHandling().toString() +
            security.getReleasingInstructions().toString() +
            security.getDeclassificationType().toString() +
            security.getDeclassificationDate().toString() +
            security.getDeclassificationExemption().toString() +
            security.getDowngrade().toString() +
            security.getDowngradeDateTime().toString() +
            security.getClassificationText().toString() +
            security.getClassificationAuthorityType().toString() +
            security.getClassificationAuthority().toString() +
            security.getClassificationReason().toString() +
            security.getSecuritySourceDate().toString() +
            security.getSecurityControlNumber().toString();

    return str;
}

std::string toString(nitf::DESubheader subheader)
{
    const nitf::Uint32 subheaderFieldsLen(subheader.getSubheaderFieldsLength());

    std::ostringstream ostr;
    ostr << subheader.getFilePartType().toString()
         << subheader.getTypeID().toString()
         << subheader.getVersion().toString()
         << subheader.getSecurityClass().toString()
         << toString(subheader.getSecurityGroup())
         << std::setw(4) << std::setfill('0') << subheaderFieldsLen;

    // This tacks on XML_DATA_CONTENT when applicable
    if (subheaderFieldsLen > 0)
    {
        nitf::TRE tre = subheader.getSubheaderFields();
        for (nitf::TRE::Iterator iter = tre.begin(); iter != tre.end(); ++iter)
        {
            nitf::Pair pair = *iter;
            ostr << tre.getField(pair.getKey()).toString();
        }
    }

    return ostr.str();
}




