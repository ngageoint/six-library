/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#include <cphd/Utilities.h>
#include <cphd/Antenna.h>

namespace cphd
{
Antenna::Antenna() :
    numTxAnt(0),
    numRcvAnt(0),
    numTWAnt(0)
{
}

std::ostream& operator<< (std::ostream& os, const Antenna& d)
{
    os << "Antenna::\n"
       << "  numTxAnt : " << d.numTxAnt << "\n"
       << "  numRcvAnt: " << d.numRcvAnt << "\n"
       << "  numTWAnt : " << d.numTWAnt << "\n";

    for (size_t ii = 0; ii < d.tx.size(); ++ii)
    {
         os << "  [" << (ii + 1) << "] Tx:\n  " << d.tx[ii] << "\n";
    }

    for (size_t ii = 0; ii < d.rcv.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] Rcv:\n  " << d.rcv[ii] << "\n";
    }

    for (size_t ii = 0; ii < d.twoWay.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] TwoWay:\n  " << d.twoWay[ii] << "\n";
    }

    return os;
}
}
