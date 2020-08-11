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
#include <NitfIsd.h>

/**
 * Write FileSecurity as string so CSM can use it
 */
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

/**
 * Write a DESubheader as a string so CSM can use it
 */
std::string toString(nitf::DESubheader subheader)
{
    const uint32_t subheaderFieldsLen(subheader.getSubheaderFieldsLength());

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

/**
 * Find DLL required to run CSM
 * \param installPathname Directory where SIX is installed
 * \return path to CSM DLL
 */
std::string findDllPathname(const std::string& installPathname)
{
    const std::string csmPluginPathname =
            sys::Path(installPathname).join("share").join("CSM").join("plugins");

    const std::vector<std::string> csmPluginContents =
            sys::Path::list(csmPluginPathname);

    // Get rid of contents like '.' and '..'
    std::vector<std::string> csmPlugins;
    for (size_t ii = 0; ii < csmPluginContents.size(); ++ii)
    {
        const std::string pathname = sys::Path::joinPaths(csmPluginPathname,
                csmPluginContents[ii]);
        if (sys::Path(pathname).isFile())
        {
            csmPlugins.push_back(pathname);
        }
    }

    if (csmPlugins.size() != 1)
    {
        throw except::Exception(Ctxt("Expected exactly one plugin in "
                + csmPluginPathname));
    }

    return csmPlugins[0];
}

/**
 * Construct Nitf21ISD with DES
 * \param pathname Path to NITF
 * \param loadedReadControl NITFReadControl that has already been loaded
 * \param data SICD/SIDD metadata
 * \param registry XMLControlRegistry set up for either Complex or Derived data
 *
 * \return A Nitf21ISD object
 */
std::auto_ptr<csm::Nitf21Isd> constructIsd(const std::string& pathname,
        const six::NITFReadControl& loadedReadControl, const six::Data* data,
        const six::XMLControlRegistry& registry)
{
    std::auto_ptr<csm::Nitf21Isd> nitfIsd(new csm::Nitf21Isd(pathname));
    csm::Des des;

    // NITRO parsed the subheader into a nice structure - need to grab all
    // the fields and jam them back into a string like CSM wants
    nitf::DESegment segment = static_cast<nitf::DESegment>(
           loadedReadControl.getRecord().getDataExtensions().
           getFirst().getData());

    des.setSubHeader(toString(segment.getSubheader()));

    // The DES's data is just the XML string
    des.setData(six::toXMLString(data, &registry));
    nitfIsd->addFileDes(des);

    return nitfIsd;
}

/**
 * Return the absolute value of the difference between each element of a
 * Vector3
 */
six::Vector3 absoluteDifference(const six::Vector3& lhs, const six::Vector3& rhs)
{
    six::Vector3 difference = lhs - rhs;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        difference[ii] = std::abs(difference[ii]);
    }

    return difference;
}

/**
 * Return the absolute value of the difference between each element of a
 * RowColDouble
 */
six::RowColDouble absoluteDifference(const six::RowColDouble& lhs,
        const six::RowColDouble& rhs)
{
    six::RowColDouble difference = lhs - rhs;
    difference.row = std::abs(difference.row);
    difference.col = std::abs(difference.col);
    return difference;
}

