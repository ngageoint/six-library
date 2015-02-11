/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#include <six/Init.h>
#include <cphd/SRP.h>
#include <six/Utilities.h>

namespace cphd
{
SRP::SRP() :
    numSRPs(0)
{
}

std::ostream& operator<< (std::ostream& os, const SRP& d)
{
    os << "SRP::" << "\n"
       << "  SRPType : " << d.srpType.toString() << "\n"
       << "  NumSRPs : " << d.numSRPs  << "\n";

    for (size_t ii = 0; ii < d.srpPT.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] SRPPT      : "
           <<six::toString(d.srpPT[ii]) << "\n";
    }

    for (size_t ii = 0; ii < d.srpPVTPoly.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] SRPPVTPoly :\n"
           << six::toString(d.srpPVTPoly[ii]) << "\n";
    }

    for (size_t ii = 0; ii < d.srpPVVPoly.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] SRPPVVPoly :\n"
           << six::toString(d.srpPVVPoly[ii]) << "\n";
    }
    return os;
}
}
