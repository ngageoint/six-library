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
 * GNU Lesser General Public License for more detailformat.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include <complex>
#include <sys/Conf.h>
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

/*
 * Validate user input format
 * Returns if valid, throws if not
 */
void Pvp::validateFormat(std::string& format)
{
    if (format == "F4" || format == "F8")
    {
        return;
    }
    else if (format == "U1" || format == "U2" || format == "U4" ||
                format == "U8")
    {
        return;
    }
    else if (format == "I1" || format == "I2" || format == "I4" ||
                format == "I8")
    {
        return;
    }
    else if (format == "CI2" || format == "CI4" || format == "CI8" ||
                format == "CI16")
    {
        return;
    }
    else if (format == "CF8" || format == "CF16")
    {
        return;
    }
    else if (isFormatStr(format)) // Else if Check for string type S[1-9][0-9]
    {
        return;
    }
    else if (isMultipleParam(format))
    {
        return;
    }
    throw except::Exception(Ctxt("Invalid format provided"));
}

bool Pvp::isMultipleParam(std::string& format)
{
    std::string eqDelimiter = "=";
    std::string colDelimiter = ";";
    size_t idx = 0;
    while (format.find(colDelimiter, idx) != std::string::npos)
    {
        size_t pos = format.find(colDelimiter, idx);
        if(format.find(eqDelimiter, idx) == std::string::npos || format.find(eqDelimiter, idx) > pos)
        {
            return false;
        }

        size_t eqPos = format.find(eqDelimiter, idx);
        std::string name = format.substr(idx, eqPos-idx);

        //! Update the current idx
        if (eqPos + 1 >= format.size())
        {
            return false;
        }
        idx = ++eqPos;
        std::string format_string = format.substr(idx, pos-idx);
        try
        {
            validateFormat(format_string);
        }
        catch(std::exception& e)
        {
            return false;
        }

        //! Update the current idx
        if (pos + 1 >= format.size())
        {
            return true;
        }
        idx = ++pos;
    }
    return false;
}

bool Pvp::isFormatStr(std::string format)
{
    const char* ptr = format.c_str();
    if(format.size() <= 3)
    {
        if (*ptr == 'S')
        {
            ++ptr;
            if(std::isdigit(*ptr) && std::isdigit(*(ptr+1)))
            {
                return true;
            }
        }
    }
    return false;
}

void Pvp::setData(PVPType& param, size_t size, size_t offset, std::string format)
{
    validate(size, offset);
    validateFormat(format);
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
    validateFormat(format);
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
    os << "  TxTime         : " << p.txTime << "\n"
        << "  TxPos         : " << p.txPos << "\n"
        << "  TxVel         : " << p.txVel << "\n"
        << "  RcvTime       : " << p.rcvTime << "\n"
        << "  RcvPos        : " << p.rcvPos << "\n"
        << "  RcvVel        : " << p.rcvVel << "\n"
        << "  SRPPos        : " << p.srpPos << "\n"
        << "  aFDOP         : " << p.aFDOP << "\n"
        << "  aFRR1         : " << p.aFRR1 << "\n"
        << "  aFRR2         : " << p.aFRR2 << "\n"
        << "  Fx1           : " << p.fx1 << "\n"
        << "  Fx2           : " << p.fx2 << "\n"
        << "  TOA1          : " << p.toa1 << "\n"
        << "  TOA2          : " << p.toa2 << "\n"
        << "  TdTropoSRP    : " << p.tdTropoSRP << "\n"
        << "  SC0           : " << p.sc0 << "\n"
        << "  SCSS          : " << p.scss << "\n";

    if (p.ampSF.get())
    {
        os << "  AmpSF         : " << *p.ampSF << "\n";
    }
    if (p.fxN1.get())
    {
        os << "  FxN1          : " << *p.fxN1 << "\n";
    }
    if (p.fxN2.get())
    {
        os << "  FxN2          : " << *p.fxN2 << "\n";
    }
    if (p.toaE1.get())
    {
        os << "  TOAE1         : " << *p.toaE1 << "\n";
    }
    if (p.toaE2.get())
    {
        os << "  TOAE2         : " << *p.toaE2 << "\n";
    }
    if (p.tdIonoSRP.get())
    {
        os << "  TdIonoSRP     : " << *p.tdIonoSRP << "\n";
    }
    if (p.signal.get())
    {
        os << "  SIGNAL     : " << *p.signal << "\n";
    }

    for (size_t ii = 0; ii < p.addedPVP.size(); ++ii)
    {
        os << "  Additional Parameter : " << p.addedPVP[ii] << "\n";
    }
    return os;
}
}


