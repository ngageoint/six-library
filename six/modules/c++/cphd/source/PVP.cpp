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
    initialize();
}

void Pvp::initialize()
{
    // Default size and formats for each PVP
    // listed in Table 11-6 CPHD1.0 Spec
    setDefaultValues(1,"F8", txTime);
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", txPos);
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", txVel);
    setDefaultValues(1,"F8", rcvTime);
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", rcvPos);
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", rcvVel);
    setDefaultValues(3,"X=F8;Y=F8;Z=F8;", srpPos);
    setDefaultValues(1,"F8", ampSF);
    setDefaultValues(1,"F8", aFDOP);
    setDefaultValues(1,"F8", aFRR1);
    setDefaultValues(1,"F8", aFRR2);
    setDefaultValues(1,"F8", fx1);
    setDefaultValues(1,"F8", fx2);
    setDefaultValues(1,"F8", fxN1);
    setDefaultValues(1,"F8", fxN2);
    setDefaultValues(1,"F8", toa1);
    setDefaultValues(1,"F8", toa2);
    setDefaultValues(1,"F8", toaE1);
    setDefaultValues(1,"F8", toaE2);
    setDefaultValues(1,"F8", tdTropoSRP);
    setDefaultValues(1,"F8", tdIonoSRP);
    setDefaultValues(1,"F8", sc0);
    setDefaultValues(1,"F8", scss);
    setDefaultValues(1,"I8", signal);
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

void Pvp::setOffset(size_t offset, PVPType& param)
{
    validate(param.getSize(), offset);
    validateFormat(param.getFormat());
    param.setOffset(offset);
}

void Pvp::append(PVPType& param)
{
    size_t currentOffset = mParamLocations.size();
    setOffset(currentOffset, param);
}

// Assumes addedPVP is already correct size
void Pvp::setCustomParameter(size_t size, size_t offset, const std::string& format, const std::string& name)
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

void Pvp::appendCustomParameter(size_t size, const std::string& format, const std::string& name)
{
    size_t currentOffset = mParamLocations.size();
    setCustomParameter(size, currentOffset, format, name);
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
    if(!six::Init::isUndefined<size_t>(ampSF.getOffset()))
    {
        res += ampSF.getSize();
    }
    if(!six::Init::isUndefined<size_t>(fxN1.getOffset()))
    {
        res += fxN1.getSize();
    }
    if(!six::Init::isUndefined<size_t>(fxN2.getOffset()))
    {
        res += fxN2.getSize();
    }
    if(!six::Init::isUndefined<size_t>(toaE1.getOffset()))
    {
        res += toaE1.getSize();
    }
    if(!six::Init::isUndefined<size_t>(toaE2.getOffset()))
    {
        res += toaE2.getSize();
    }
    if(!six::Init::isUndefined<size_t>(tdIonoSRP.getOffset()))
    {
        res += tdIonoSRP.getSize();
    }
    if(!six::Init::isUndefined<size_t>(signal.getOffset()))
    {
        res += signal.getSize();
    }
    for (auto it = addedPVP.begin(); it != addedPVP.end(); ++it)
    {
        res += it->second.getSize();
    }
    return res;
}

size_t Pvp::sizeInBytes() const
{
    return getReqSetSize() * PVPType::WORD_BYTE_SIZE;
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

    if(!six::Init::isUndefined<size_t>(p.ampSF.getOffset()))
    {
        os << "  AmpSF         : \n" << p.ampSF << "\n";
    }
    if(!six::Init::isUndefined<size_t>(p.fxN1.getOffset()))
    {
        os << "  FxN1          : \n" << p.fxN1 << "\n";
    }
    if(!six::Init::isUndefined<size_t>(p.fxN2.getOffset()))
    {
        os << "  FxN2          : \n" << p.fxN2 << "\n";
    }
    if(!six::Init::isUndefined<size_t>(p.toaE1.getOffset()))
    {
        os << "  TOAE1         : \n" << p.toaE1 << "\n";
    }
    if(!six::Init::isUndefined<size_t>(p.toaE2.getOffset()))
    {
        os << "  TOAE2         : \n" << p.toaE2 << "\n";
    }
    if(!six::Init::isUndefined<size_t>(p.tdIonoSRP.getOffset()))
    {
        os << "  TdIonoSRP     : \n" << p.tdIonoSRP << "\n";
    }
    if(!six::Init::isUndefined<size_t>(p.signal.getOffset()))
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
