/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/


#ifndef __CPHD_CHANNEL_H__
#define __CPHD_CHANNEL_H__

#include <ostream>
#include <vector>
#include <stddef.h>

namespace cphd
{
struct ChannelParameters
{
    ChannelParameters();

    bool operator==(const ChannelParameters& other) const
    {
        return srpIndex == other.srpIndex &&
               nomTOARateSF == other.nomTOARateSF &&
               fxCtrNom == other.fxCtrNom &&
               bwSavedNom == other.bwSavedNom &&
               toaSavedNom == other.toaSavedNom &&
               txAntIndex == other.txAntIndex &&
               rcvAntIndex == other.rcvAntIndex &&
               twAntIndex == other.twAntIndex;
    }

    bool operator!=(const ChannelParameters& other) const
    {
        return !((*this) == other);
    }

    size_t srpIndex;
    double nomTOARateSF;
    double fxCtrNom;
    double bwSavedNom;
    double toaSavedNom;

    // optional
    size_t txAntIndex;

    // optional
    size_t rcvAntIndex;

    // optional
    size_t twAntIndex;
};

std::ostream& operator<< (std::ostream& os, const ChannelParameters& d);

struct Channel
{
    Channel()
    {
    }
    virtual ~Channel() {}

    bool operator==(const Channel& other) const
    {
        return parameters == other.parameters;
    }

    bool operator!=(const Channel& other) const
    {
        return !((*this) == other);
    }

    std::vector<ChannelParameters> parameters;
};

std::ostream& operator<< (std::ostream& os, const Channel& d);
}

#endif
