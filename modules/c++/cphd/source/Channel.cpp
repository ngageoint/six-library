/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#include <six/Init.h>
#include <cphd/Channel.h>

namespace cphd
{
ChannelParameters::ChannelParameters() :
    srpIndex(0),
    nomTOARateSF(0.0),
    fxCtrNom(0.0),
    bwSavedNom(0.0),
    toaSavedNom(0.0),
    txAntIndex(six::Init::undefined<size_t>()),
    rcvAntIndex(six::Init::undefined<size_t>()),
    twAntIndex(six::Init::undefined<size_t>())
{
}

std::ostream& operator<< (std::ostream& os, const ChannelParameters& d)
{
    os << "ChannelParameters::\n"
       << "  srpIndex    : " << d.srpIndex << "\n"
       << "  nomTOARateSF: " << d.nomTOARateSF << "\n"
       << "  fxCtrNom    : " << d.fxCtrNom<< "\n"
       << "  bwSavedNom  : " << d.bwSavedNom<< "\n"
       << "  toaSavedNom : " << d.toaSavedNom << "\n";

    if (!six::Init::isUndefined(d.txAntIndex))
    {
        os << "  txAntIndex : " << d.txAntIndex << "\n";
    }

    if (!six::Init::isUndefined(d.rcvAntIndex))
    {
        os << "  rcvAntIndex: " << d.rcvAntIndex << "\n";
    }
    if (!six::Init::isUndefined(d.twAntIndex))
    {
        os << "  twAntIndex : " << d.twAntIndex << "\n";
    }

    return os;
}

std::ostream& operator<< (std::ostream& os, const Channel& d)
{
    os << "Channel::\n";
    for (size_t ii = 0; ii < d.parameters.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] Parameters: " << d.parameters[ii] << "\n";
    }

    return os;
}
}
