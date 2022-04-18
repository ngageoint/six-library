/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include <six/CollectionInformation.h>

#include <str/EncodedStringView.h>

namespace six
{
CollectionInformation::CollectionInformation()
{
    radarMode = Init::undefined<RadarModeType>();
}

  bool CollectionInformation::operator==(const CollectionInformation& rhs) const
{
    const auto& lhs = *this;
    const auto result = lhs.collectorName == rhs.collectorName &&
        lhs.collectType == rhs.collectType &&
        lhs.illuminatorName == rhs.illuminatorName &&
        lhs.coreName == rhs.coreName &&
        lhs.radarMode == rhs.radarMode &&
        lhs.radarModeID == rhs.radarModeID &&
        lhs.releaseInfo == rhs.releaseInfo &&
        lhs.getClassificationLevel() == rhs.getClassificationLevel();
    if (!result)
    {
        // no need to look at mClassification_u8
        return false;
    }

    // Everything is equal up to this point
    std::u8string lhs_classification_u8, rhs_classification_u8;
    const auto lhs_result = lhs.getClassificationLevel(lhs_classification_u8);
    const auto rhs_result = rhs.getClassificationLevel(rhs_classification_u8);
    if (lhs_result && rhs_result) // mClassification_u8 might not be set
    {
        // we've got two u8strings
        return lhs_classification_u8 == rhs_classification_u8;
    }
    return lhs_result == rhs_result; // either none, or only one
}

CollectionInformation* CollectionInformation::clone() const
{
    return new CollectionInformation(*this);
}

std::string CollectionInformation::getClassificationLevel() const
{
    std::u8string utf8;
    if (!getClassificationLevel(utf8))
    {
        return mClassification; // no UTF-8 value
    }
    return str::EncodedStringView(utf8).native(); // use the UTF-8 value, converted to native
}
bool CollectionInformation::getClassificationLevel(std::u8string& result) const
{
    if (mClassification_u8.has_value())
    {
        result = mClassification_u8.value();
        return true;
    }
    return false;
}
void CollectionInformation::setClassificationLevel(const std::u8string& classification)
{
    mClassification_u8 = classification;
}

std::ostream& operator<< (std::ostream& os, const six::CollectionInformation& c)
{
    os << "CollectionID:: \n"
        << "  CollectorName    : " << c.collectorName << "\n";
    if (!six::Init::isUndefined(c.illuminatorName))
    {
        os << "  IlluminatorName  : " << c.illuminatorName << "\n";
    }
    os << "  CoreName         : " << c.coreName << "\n";
    if (!six::Init::isUndefined(c.collectType))
    {
        os << "  CollectType      : " << c.collectType << "\n";
    }
    os << "  RadarMode        : " << c.radarMode << "\n";
    if (!six::Init::isUndefined(c.radarModeID))
    {
        os << "  RadarModeID      : " << c.radarModeID << "\n";
    }
    if (!six::Init::isUndefined(c.releaseInfo))
    {
        os << "  ReleaseInfo      : " << c.releaseInfo << "\n";
    }
    for (size_t ii = 0; ii < c.countryCodes.size(); ++ii)
    {
        os << "  CountryCodes     : " << c.countryCodes[ii] << "\n";
    }
    for (size_t ii = 0; ii < c.parameters.size(); ++ii)
    {
        os << "  Parameter name   : " << c.parameters[ii].getName() << "\n"
            << "  Parameter value  : " << c.parameters[ii].str() << "\n";
    }
    return os;
}
}
