/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#include <ostream>

#include <cphd/Utilities.h>

namespace cphd
{
size_t getNumBytesPerSample(cphd::SampleType sampleType)
{
    switch (sampleType)
    {
        case cphd::SampleType::RE08I_IM08I:
            return 2;
        case cphd::SampleType::RE16I_IM16I:
            return 4;
        case cphd::SampleType::RE32F_IM32F:
            return 8;
        default:
            return 0;
    }
}

template<> std::string toString(const CollectionInformation& ci)
{
    std::ostringstream os;

    os << "CollectionInformation::" << "\n";
    os << "  collectorName  : " << ci.collectorName << "\n";
    
    if (!six::Init::isUndefined(ci.illuminatorName))
    {
        os << "  illuminatorName: " << ci.illuminatorName << "\n";
    }
    
    os << "  coreName       : " << ci.coreName << "\n";

    if (ci.collectType != CollectType::NOT_SET )
    {
        os << "  collectType    : " << ci.collectType.toString() << "\n";
    }
    
    os << "  radarMode      : " << ci.radarMode.toString() << "\n";

    if (!six::Init::isUndefined(ci.radarModeID))
    {
        os << "  radarModeID    : " << ci.radarModeID << "\n";
    }
    
    os << "  classification : " << ci.classification.level << "\n";

    for (size_t ii = 0; ii < ci.countryCodes.size(); ++ii)
    {
        os << "  Country Code   : " << ci.countryCodes[ii] << "\n";
    }

    return os.str();
}
}
