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

#include <complex>
#include <sys/Conf.h>
#include <six/Init.h>
#include <cphd/PVP.h>
#include <cphd/Utilities.h>

namespace cphd
{
const size_t PVPType::WORD_BYTE_SIZE = 8;

PVPType::PVPType() :
    // Overwrite these default values if required
    mSize(1),
    mOffset(six::Init::undefined<size_t>()),
    mFormat("F8")
{
}

APVPType::APVPType() :
    mName(six::Init::undefined<std::string>())
{
}

Pvp::Pvp(bool hasAmpSF, bool hasFxN1, bool hasFxN2,
         bool hasToaE1, bool hasToaE2, bool hasTDIonoSRP,
         bool hasSignal)
{
    initialize();
    specifyOptionalParameters(hasAmpSF, hasFxN1, hasFxN2,
                              hasToaE1, hasToaE2, hasTDIonoSRP,
                              hasSignal);
}

void Pvp::initialize()
{
    // Rest of the parameters are already initialized
    // in PVPType constructor
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", txPos);
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", txVel);
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", rcvPos);
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", rcvVel);
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", srpPos);
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
        if(mParamLocations.at(offset + ii) == true)
        {
            throw except::Exception(Ctxt(
                                    "This byte block is occupied"));
        }
    }

    // Mark each block as written
    for (size_t ii = 0; ii < size; ++ii)
    {
        mParamLocations.at(offset + ii) = true;
    }
}

void Pvp::specifyOptionalParameters(bool hasAmpSF, bool hasFxN1, bool hasFxN2,
                       bool hasToaE1, bool hasToaE2, bool hasTDIonoSRP,
                       bool hasSignal)
{
    mAmpSF = hasAmpSF;
    mFxN1 = hasFxN1;
    mFxN2 = hasFxN2;
    mToaE1 = hasToaE1;
    mToaE2 = hasToaE2;
    mTDIonoSRP = hasTDIonoSRP;
    mSignal = hasSignal;
}


void Pvp::setOffset(size_t offset, PVPType& param)
{
    validate(param.getSize(), offset);
    validateFormat(param.getFormat());
    param.setOffset(offset);
}

// Assumes addedPVP is already correct size
void Pvp::setParameter(size_t size, size_t offset, const std::string& format, const std::string& name)
{
    validate(size, offset);
    validateFormat(format);
    if (addedPVP.count(name) == 0)
    {
        addedPVP[name] = APVPType();
        addedPVP.find(name)->second.setData(size, offset, format, name);
        return;
    }
    throw except::Exception(Ctxt(
            "Additional parameter name is not unique"));
}

size_t Pvp::getAdditionalParamsSize() const
{
    size_t res = 0;
    for (auto it = addedPVP.begin(); it != addedPVP.end(); ++it)
    {
        res += it->second.getSize();
    }
    return res;
}

void Pvp::setDefaultValues(size_t size, const std::string& format, PVPType& param)
{
    param.setSize(size);
    param.setFormat(format);
}

// Returns num blocks (not bytes)
size_t Pvp::getReqSetSize() const
{
    size_t res = txTime.getSize() + txPos.getSize() + txVel.getSize() +
            rcvTime.getSize() + rcvPos.getSize() + rcvVel.getSize() + srpPos.getSize() +
            aFDOP.getSize() + aFRR1.getSize() + aFRR2.getSize() + fx1.getSize() +
            fx2.getSize() + toa1.getSize() + toa2.getSize() + tdTropoSRP.getSize() +
            sc0.getSize() + scss.getSize();
    if(hasAmpSF())
    {
        res += ampSF.getSize();
    }
    if(hasFxN1())
    {
        res += fxN1.getSize();
    }
    if(hasFxN2())
    {
        res += fxN2.getSize();
    }
    if(hasToaE1())
    {
        res += toaE1.getSize();
    }
    if(hasToaE2())
    {
        res += toaE2.getSize();
    }
    if(hasTDIonoSRP())
    {
        res += tdIonoSRP.getSize();
    }
    if(hasSignal())
    {
        res += signal.getSize();
    }
    for (auto it = addedPVP.begin(); it != addedPVP.end(); ++it)
    {
        res += it->second.getSize();
    }
    return res; // Num Bytes
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
    os << "  TxTime         : \n" << p.txTime << "\n"
        << "  TxPos         : \n" << p.txPos << "\n"
        << "  TxVel         : \n" << p.txVel << "\n"
        << "  RcvTime       : \n" << p.rcvTime << "\n"
        << "  RcvPos        : \n" << p.rcvPos << "\n"
        << "  RcvVel        : \n" << p.rcvVel << "\n"
        << "  SRPPos        : \n" << p.srpPos << "\n"
        << "  aFDOP         : \n" << p.aFDOP << "\n"
        << "  aFRR1         : \n" << p.aFRR1 << "\n"
        << "  aFRR2         : \n" << p.aFRR2 << "\n"
        << "  Fx1           : \n" << p.fx1 << "\n"
        << "  Fx2           : \n" << p.fx2 << "\n"
        << "  TOA1          : \n" << p.toa1 << "\n"
        << "  TOA2          : \n" << p.toa2 << "\n"
        << "  TdTropoSRP    : \n" << p.tdTropoSRP << "\n"
        << "  SC0           : \n" << p.sc0 << "\n"
        << "  SCSS          : \n" << p.scss << "\n";

    if(p.hasAmpSF())
    {
        os << "  AmpSF         : \n" << p.ampSF << "\n";
    }
    if(p.hasFxN1())
    {
        os << "  FxN1          : \n" << p.fxN1 << "\n";
    }
    if(p.hasFxN2())
    {
        os << "  FxN2          : \n" << p.fxN2 << "\n";
    }
    if(p.hasToaE1())
    {
        os << "  TOAE1         : \n" << p.toaE1 << "\n";
    }
    if(p.hasToaE2())
    {
        os << "  TOAE2         : \n" << p.toaE2 << "\n";
    }
    if(p.hasTDIonoSRP())
    {
        os << "  TdIonoSRP     : \n" << p.tdIonoSRP << "\n";
    }
    if(p.hasSignal())
    {
        os << "  SIGNAL        : \n" << p.signal << "\n";
    }
    for (auto it = p.addedPVP.begin(); it != p.addedPVP.end(); ++it)
    {
        os << "  Additional Parameter : " << it->second << "\n";
    }
    return os;
}
}


