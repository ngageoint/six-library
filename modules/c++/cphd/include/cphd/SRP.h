/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/


#ifndef __CPHD_SRP_H__
#define __CPHD_SRP_H__

#include <iostream>
#include <cphd/Types.h>
#include <cphd/Utilities.h>
#include <six/Init.h>


namespace cphd
{
struct SRP
{
    SRP();

    bool operator==(const SRP& other) const
    {
        return srpType == other.srpType &&
               numSRPs == other.numSRPs &&
               srpPT == other.srpPT &&
               srpPVTPoly == other.srpPVTPoly &&
               srpPVVPoly == other.srpPVVPoly;
    }

    bool operator!=(const SRP& other) const
    {
        return !((*this) == other);
    }

    //! Type can be FIXEDPT, PVTPOLY, PVVPOLY, STEPPED
    SRPType srpType;

    //! Can be 1 to the number of channels
    //! Will be zero if SRPType is STEPPED ??
    size_t numSRPs;

    //! Conditional, when srpType is FIXEDPT
    std::vector<Vector3> srpPT;

     //! Conditional, when srpType is PVTPOLY
    std::vector<PolyXYZ> srpPVTPoly;

    //! Conditional, when srpType is PVVPOLY
    std::vector<PolyXYZ> srpPVVPoly;
};

std::ostream& operator<< (std::ostream& os, const SRP& d);
}

#endif
