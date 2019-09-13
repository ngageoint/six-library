/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#include <cphd/PVP.h>
#include <six/Init.h>

namespace cphd
{

PVPType::PVPType() :
    mSize(0),
    mOffset(six::Init::undefined<size_t>()),
    mFormat(six::Init::undefined<std::string>())
{
}

APVPType::APVPType() :
    mName(six::Init::undefined<std::string>())
{
}

Pvp::Pvp()
{
}


Pvp::Pvp(size_t numAdditionalParams) :
    addedPVP(numAdditionalParams)
{
}

void Pvp::validate(size_t size, size_t offset)
{
    //Check if size of array is sufficient for write
    if (offset + size > mParamLocations.size())
    {
        mParamLocations.resize(offset+size);
    }

    //Check if any blocks will be overwritten
    for (size_t ii = 0; ii < size; ++ii)
    {
        if(mParamLocations[offset + ii] == true)
        {
            throw except::Exception(Ctxt(
                                    "This byte block is occupied"));
        }
    }

    // Mark each block as written
    for (size_t ii = 0; ii < size; ++ii)
    {
        mParamLocations[offset + ii] = true;
    }
}

void Pvp::setData(PVPType& param, size_t size, size_t offset, std::string format)
{
    validate(size, offset);
    param.setData(size, offset, format);
}

// Assumes addedPVP is already correct size
void Pvp::setData(size_t size, size_t offset, std::string format, std::string name, size_t idx)
{
    validate(size, offset);
    if (idx >= addedPVP.size())
    {
        throw except::Exception(Ctxt(
                                "Additional Parameter specified does not exist"));
    }
    addedPVP[idx].setData(size, offset, format);
    addedPVP[idx].setName(name);
}

void Pvp::setNumAddedParameters(size_t size)
{
    addedPVP.resize(size);
}

size_t Pvp::getReqSetSize() const
{
    size_t res = txTime.getSize() + txPos.getSize() + txVel.getSize() +
            rcvTime.getSize() + rcvPos.getSize() + rcvVel.getSize() + srpPos.getSize() +
            aFDOP.getSize() + aFRR1.getSize() + aFRR2.getSize() + fx1.getSize() +
            fx2.getSize() + toa1.getSize() + toa2.getSize() + tdTropoSRP.getSize() +
            sc0.getSize() + scss.getSize();
    if(ampSF.get())
    {
        res += ampSF->getSize();
    }
    if(fxN1.get())
    {
        res += fxN1->getSize();
    }
    if(fxN2.get())
    {
        res += fxN2->getSize();
    }
    if(toaE1.get())
    {
        res += toaE1->getSize();
    }
    if(toaE2.get())
    {
        res += toaE2->getSize();
    }
    if(tdIonoSRP.get())
    {
        res += tdIonoSRP->getSize();
    }
    if(signal.get())
    {
        res += signal->getSize();
    }
    for (size_t i = 0; i < addedPVP.size(); ++i)
    {
        res += addedPVP[i].getSize();
    }
    return res * 8; // Num Bytes
}

std::ostream& operator<< (std::ostream& os, const PVPType& p)
{
    os << "    Size           : " << p.getSize() << "\n"
        << "    Offset         : " << p.getOffset() << "\n"
        << "    Format         : " << p.getFormat() << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const APVPType& a)
{
    os << "    Name           : " << a.getName() << "\n"
        << (PVPType)a;

    return os;
}

std::ostream& operator<< (std::ostream& os, const Pvp& p)
{
    os << "PVP:: \n"
        << "  TxTime           : " << p.txTime << "\n"
        << "  TxPos            : " << p.txPos  << "\n"
        << "  TxVel            : " << p.txVel  << "\n"
        << "  RcvTime          : " << p.rcvTime << "\n"
        << "  RcvPos           : " << p.rcvPos << "\n"
        << "  RcvVel           : " << p.rcvVel << "\n"
        << "  SRPPos           : " << p.srpPos << "\n"
        << "  AmpSF            : " << *(p.ampSF) << "\n"
        << "  aFDOP            : " << p.aFDOP << "\n"
        << "  aFRR1            : " << p.aFRR1 << "\n"
        << "  aFRR2            : " << p.aFRR2 << "\n"
        << "  FX1              : " << p.fx1 << "\n"
        << "  FX2              : " << p.fx2 << "\n"
        << "  FXN1             : " << *(p.fxN1) << "\n"
        << "  FXN2             : " << *(p.fxN2) << "\n"
        << "  TOA1             : " << p.toa1 << "\n"
        << "  TOA2             : " << p.toa2 << "\n"
        << "  TOAE1            : " << *(p.toaE1) << "\n"
        << "  TOAE2            : " << *(p.toaE2) << "\n"
        << "  TDTropoSRP       : " << p.tdTropoSRP << "\n"
        << "  TDIonoSRP        : " << *(p.tdIonoSRP) << "\n"
        << "  SC0              : " << p.sc0 << "\n"
        << "  SCSS              : " << p.scss << "\n"
        << "  SIGNAL              : " << *(p.signal) << "\n";
    for (size_t i = 0; i < p.addedPVP.size(); ++i)
    {
        os << "  AddedPVP:: \n"
            << p.addedPVP[i] << "\n";
    }
    return os;
}
}


