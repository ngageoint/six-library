/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include "six/Utilities.h"

using namespace six;

template<> BooleanType six::toType<BooleanType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    str::lower(type);
    if (type == "true" || type == "1" || type == "yes")
        return BooleanType::IS_TRUE;
    else if (type == "false" || type == "0" || type == "no")
        return BooleanType::IS_FALSE;
    else
        return BooleanType::NOT_SET;
}

template<> std::string six::toString<BooleanType>(const BooleanType& value)
{
    return six::toString<bool>(value == BooleanType::IS_TRUE);
}

template<> DateTime six::toType<DateTime>(const std::string& dateTime)
{
    try
    {
        //Try an XML format type
        if (dateTime.length() >= 19)
            return DateTime(dateTime.substr(0, dateTime.length() - 1),
                    "%Y-%m-%dT%H:%M:%S");
    }
    catch (except::Exception&)
    {
    }

    try
    {
        //Try a NITF 2.1 format
        if (dateTime.length() >= 14)
            return DateTime(dateTime.substr(0, 14), "%Y%m%d%H%M%S");
    }
    catch (except::Exception&)
    {
    }

    try
    {
        //Try a NITF 2.1 format - just date
        if (dateTime.length() >= 10)
            return DateTime(dateTime.substr(0, 10), "%Y-%m-%d");
    }
    catch (except::Exception&)
    {
    }

    try
    {
        //Try a simple 8-char format
        if (dateTime.length() == 8)
            return DateTime(dateTime.substr(0, 8), "%Y-%m-%d");
    }
    catch (except::Exception&)
    {
    }

    //should we really do this?
    return DateTime();
}

template<> std::string six::toString(const DateTime& dateTime)
{
    char date[256];
    date[255] = 0;
    dateTime.format("%Y-%m-%dT%H:%M:%.6SZ", date, 255);
    std::string strDate(date);
    return strDate;
}

template<> std::string six::toString(const RadarModeType& type)
{
    switch (type)
    {
    case RadarModeType::SPOTLIGHT:
        return "SPOTLIGHT";
    case RadarModeType::STRIPMAP:
        return "STRIPMAP";
    case RadarModeType::DYNAMIC_STRIPMAP:
        return "DYNAMIC STRIPMAP";
    case RadarModeType::SCANSAR:
        return "SCANSAR";
    default:
        throw except::Exception(Ctxt("Radar mode not set!"));
    }
}

template<> RadarModeType six::toType<RadarModeType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "SPOTLIGHT")
        return RadarModeType::SPOTLIGHT;
    if (type == "STRIPMAP")
        return RadarModeType::STRIPMAP;
    if (type == "DYNAMIC STRIPMAP")
        return RadarModeType::SCANSAR;
    if (type == "SCANSAR")
        return RadarModeType::DYNAMIC_STRIPMAP;
    return RadarModeType::NOT_SET;
}

template<> std::string six::toString(const DataType& type)
{
    switch (type)
    {
    case DataType::COMPLEX:
        return "SICD";
    case DataType::DERIVED:
        return "SIDD";
    default:
        throw except::Exception(Ctxt("Unsupported data type"));
    }
}

template<> PixelType six::toType<PixelType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    PixelType p(type);
    if (p == PixelType::NOT_SET)
        throw except::Exception(Ctxt(FmtX("Type not understood [%s]",
                                          type.c_str())));
    return p;
}

template<> std::string six::toString(const PixelType& type)
{
    if (type == PixelType::NOT_SET)
    {
        throw except::Exception(Ctxt("Unsupported pixel type"));
    }
    return type.toString();
}

template<> MagnificationMethod six::toType<MagnificationMethod>(
                                                                const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NEAREST_NEIGHBOR")
        return MagnificationMethod::NEAREST_NEIGHBOR;
    if (type == "BILINEAR")
        return MagnificationMethod::BILINEAR;
    return MagnificationMethod::NOT_SET;
}

template<> std::string six::toString(const MagnificationMethod& method)
{
    switch (method)
    {
    case MagnificationMethod::NEAREST_NEIGHBOR:
        return "NEAREST_NEIGHBOR";
    case MagnificationMethod::BILINEAR:
        return "BILINEAR";
    default:
        throw except::Exception(Ctxt("Unsupported method"));
    }
}

template<> DecimationMethod six::toType<DecimationMethod>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NEAREST_NEIGHBOR")
        return DecimationMethod::NEAREST_NEIGHBOR;
    if (type == "BILINEAR")
        return DecimationMethod::BILINEAR;
    if (type == "BRIGHTEST_PIXEL")
        return DecimationMethod::BRIGHTEST_PIXEL;
    if (type == "LAGRANGE")
        return DecimationMethod::LAGRANGE;
    return DecimationMethod::NOT_SET;
}

template<> std::string six::toString(const DecimationMethod& method)
{
    switch (method)
    {
    case DecimationMethod::NEAREST_NEIGHBOR:
        return "NEAREST_NEIGHBOR";
    case DecimationMethod::BILINEAR:
        return "BILINEAR";
    case DecimationMethod::BRIGHTEST_PIXEL:
        return "BRIGHTEST_PIXEL";
    case DecimationMethod::LAGRANGE:
        return "LAGRANGE";
    default:
        throw except::Exception(Ctxt("Unsupported method"));
    }
}

template<> EarthModelType six::toType<EarthModelType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "WGS_84")
        return EarthModelType::WGS84;
    return EarthModelType::NOT_SET;
}

template<> std::string six::toString(const EarthModelType& t)
{
    switch (t)
    {
    case EarthModelType::WGS84:
        return "WGS_84";
    default:
        throw except::Exception(Ctxt("Unsupported earth model"));
    }
}

template<> OrientationType six::toType<OrientationType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);

    if (type == "UP")
        return OrientationType::UP;
    else if (type == "DOWN")
        return OrientationType::DOWN;
    else if (type == "LEFT")
        return OrientationType::LEFT;
    else if (type == "RIGHT")
        return OrientationType::RIGHT;
    else if (type == "ARBITRARY")
        return OrientationType::ARBITRARY;
    else
        throw except::Exception(Ctxt("Unsupported orientation type"));
}

template<> std::string six::toString(const OrientationType& t)
{
    switch (t)
    {
    case OrientationType::UP:
        return "UP";
    case OrientationType::DOWN:
        return "DOWN";
    case OrientationType::LEFT:
        return "LEFT";
    case OrientationType::RIGHT:
        return "RIGHT";
    case OrientationType::ARBITRARY:
        return "ARBITRARY";
    default:
        throw except::Exception(Ctxt("Unsupported orientation"));
    }
}

template<> PolarizationType six::toType<PolarizationType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
        return PolarizationType::OTHER;
    else if (type == "V")
        return PolarizationType::V;
    else if (type == "H")
        return PolarizationType::H;
    else if (type == "RHC")
        return PolarizationType::RHC;
    else if (type == "LHC")
        return PolarizationType::LHC;
    else
        throw except::Exception(Ctxt("Unsupported polarization type"));
}

template<> std::string six::toString(const PolarizationType& t)
{
    switch (t)
    {
    case PolarizationType::OTHER:
        return "OTHER";
    case PolarizationType::V:
        return "V";
    case PolarizationType::H:
        return "H";
    case PolarizationType::RHC:
        return "RHC";
    case PolarizationType::LHC:
        return "LHC";
    default:
        throw except::Exception(
                                Ctxt(
                                     "Unsupported conversion from polarization type"));
    }
}

template<> DualPolarizationType six::toType<DualPolarizationType>(
                                                                  const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
        return DualPolarizationType::OTHER;
    else if (type == "V:V")
        return DualPolarizationType::V_V;
    else if (type == "V:H")
        return DualPolarizationType::V_H;
    else if (type == "H:V")
        return DualPolarizationType::H_V;
    else if (type == "H:H")
        return DualPolarizationType::H_H;
    else if (type == "RHC:RHC")
        return DualPolarizationType::RHC_RHC;
    else if (type == "RHC:LHC")
        return DualPolarizationType::RHC_LHC;
    else if (type == "LHC:LHC")
        return DualPolarizationType::LHC_LHC;
    else
        throw except::Exception(
                                Ctxt(
                                     "Unsupported conversion to dual polarization type"));
}

template<> std::string six::toString(const DualPolarizationType& t)
{
    switch (t)
    {
    case DualPolarizationType::OTHER:
        return "OTHER";
    case DualPolarizationType::V_V:
        return "V:V";
    case DualPolarizationType::V_H:
        return "V:H";
    case DualPolarizationType::H_V:
        return "H:V";
    case DualPolarizationType::H_H:
        return "H:H";
    case DualPolarizationType::RHC_RHC:
        return "RHC:RHC";
    case DualPolarizationType::RHC_LHC:
        return "RHC:LHC";
    case DualPolarizationType::LHC_LHC:
        return "LHC:LHC";
    default:
        throw except::Exception(
                                Ctxt(
                                     "Unsupported dual polarization type to string"));
    }
}

template<> DemodType six::toType<DemodType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "STRETCH")
        return DemodType::STRETCH;
    else if (type == "CHIRP")
        return DemodType::CHIRP;
    else
        throw except::Exception(Ctxt("Unsupported demod type"));
}

template<> std::string six::toString(const DemodType& t)
{
    switch (t)
    {
    case DemodType::STRETCH:
        return "STRETCH";
    case DemodType::CHIRP:
        return "CHIRP";
    default:
        throw except::Exception(Ctxt("Unsupported demod type"));
    }
}

template<> ImageFormationType six::toType<ImageFormationType>(
                                                              const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
        return ImageFormationType::OTHER;
    else if (type == "PFA")
        return ImageFormationType::PFA;
    else if (type == "RMA")
        return ImageFormationType::RMA;
    else if (type == "RGAZCOMP")
        return ImageFormationType::RGAZCOMP;
    else
        throw except::Exception(Ctxt("Unsupported image formation type"));
}

template<> std::string six::toString(const ImageFormationType& t)
{
    switch (t)
    {
    case ImageFormationType::OTHER:
        return "OTHER";
    case ImageFormationType::PFA:
        return "PFA";
    case ImageFormationType::RMA:
        return "RMA";
    case ImageFormationType::RGAZCOMP:
        return "RGAZCOMP";
    default:
        throw except::Exception(Ctxt("Unsupported image formation type"));
    }
}

template<> SlowTimeBeamCompensationType six::toType<
        SlowTimeBeamCompensationType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NO")
        return SlowTimeBeamCompensationType::NO;
    else if (type == "GLOBAL")
        return SlowTimeBeamCompensationType::GLOBAL;
    else if (type == "SV")
        return SlowTimeBeamCompensationType::SV;
    else
        throw except::Exception(
                                Ctxt(
                                     "Unsupported slow time beam compensation type"));
}

template<> std::string six::toString(const SlowTimeBeamCompensationType& t)
{
    switch (t)
    {
    case SlowTimeBeamCompensationType::NO:
        return "NO";
    case SlowTimeBeamCompensationType::GLOBAL:
        return "GLOBAL";
    case SlowTimeBeamCompensationType::SV:
        return "SV";
    default:
        throw except::Exception(
                                Ctxt(
                                     "Unsupported slow time beam compensation type"));
    }
}

template<> ImageBeamCompensationType six::toType<ImageBeamCompensationType>(
                                                                            const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NO")
        return ImageBeamCompensationType::NO;
    else if (type == "SV")
        return ImageBeamCompensationType::SV;
    else
        throw except::Exception(
                                Ctxt("Unsupported image beam compensation type"));
}

template<> std::string six::toString(const ImageBeamCompensationType& t)
{
    switch (t)
    {
    case ImageBeamCompensationType::NO:
        return "NO";
    case ImageBeamCompensationType::SV:
        return "SV";
    default:
        throw except::Exception(
                                Ctxt("Unsupported image beam compensation type"));
    }
}

template<> AutofocusType six::toType<AutofocusType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NO")
        return AutofocusType::NO;
    else if (type == "GLOBAL")
        return AutofocusType::GLOBAL;
    else if (type == "SV")
        return AutofocusType::SV;
    else
        throw except::Exception(Ctxt("Unsupported autofocus type"));
}

template<> std::string six::toString(const AutofocusType& t)
{
    switch (t)
    {
    case AutofocusType::NO:
        return "NO";
    case AutofocusType::GLOBAL:
        return "GLOBAL";
    case AutofocusType::SV:
        return "SV";
    default:
        throw except::Exception(Ctxt("Unsupported autofocus type"));
    }
}

template<> RMAlgoType six::toType<RMAlgoType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OMEGA_K")
        return RMAlgoType::OMEGA_K;
    else if (type == "CSA")
        return RMAlgoType::CSA;
    else if (type == "RG_DOP")
        return RMAlgoType::RG_DOP;
    else
        throw except::Exception(Ctxt("Unsupported RM algorithm type"));
}

template<> std::string six::toString(const RMAlgoType& t)
{
    switch (t)
    {
    case RMAlgoType::OMEGA_K:
        return "OMEGA_K";
    case RMAlgoType::CSA:
        return "CSA";
    case RMAlgoType::RG_DOP:
        return "RG_DOP";
    default:
        throw except::Exception(Ctxt("Unsupported RM algorithm type"));
    }
}

template<> SideOfTrackType six::toType<SideOfTrackType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "L")
        return SideOfTrackType::LEFT;
    else if (type == "R")
        return SideOfTrackType::RIGHT;
    else
        throw except::Exception(Ctxt("Unsupported side of track"));
}

template<> std::string six::toString(const SideOfTrackType& t)
{
    switch (t)
    {
    case SideOfTrackType::LEFT:
        return "L";
    case SideOfTrackType::RIGHT:
        return "R";
    default:
        throw except::Exception(Ctxt("Unsupported side of track"));
    }
}

template<> ComplexImagePlaneType six::toType<ComplexImagePlaneType>(
                                                                    const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
        return ComplexImagePlaneType::OTHER;
    else if (type == "SLANT")
        return ComplexImagePlaneType::SLANT;
    else if (type == "GROUND")
        return ComplexImagePlaneType::GROUND;
    else
        throw except::Exception(Ctxt("Unsupported complex image plane"));
}

template<> std::string six::toString(const ComplexImagePlaneType& t)
{
    switch (t)
    {
    case ComplexImagePlaneType::OTHER:
        return "OTHER";
    case ComplexImagePlaneType::SLANT:
        return "SLANT";
    case ComplexImagePlaneType::GROUND:
        return "GROUND";
    default:
        throw except::Exception(Ctxt("Unsupported complex image plane"));
    }
}

template<> ComplexImageGridType six::toType<ComplexImageGridType>(
                                                                  const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "RGAZIM")
        return ComplexImageGridType::RGAZIM;
    else if (type == "RGZERO")
        return ComplexImageGridType::RGZERO;
    else if (type == "XRGYCR")
        return ComplexImageGridType::XRGYCR;
    else if (type == "XCTYAT")
        return ComplexImageGridType::XCTYAT;
    else if (type == "PLANE")
        return ComplexImageGridType::PLANE;
    else
        throw except::Exception(Ctxt("Unsupported complex image grid"));
}

template<> std::string six::toString(const ComplexImageGridType& t)
{
    switch (t)
    {
    case ComplexImageGridType::RGAZIM:
        return "RGAZIM";
    case ComplexImageGridType::RGZERO:
        return "RGZERO";
    case ComplexImageGridType::XRGYCR:
        return "XRGYCR";
    case ComplexImageGridType::XCTYAT:
        return "XCTYAT";
    case ComplexImageGridType::PLANE:
        return "PLANE";
    default:
        throw except::Exception(Ctxt("Unsupported complex image grid"));
    }
}

template<> FFTSign six::toType<FFTSign>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "-1")
        return FFTSign::NEG;
    else if (type == "+1")
        return FFTSign::POS;
    else if (type == "0")
        return FFTSign::NOT_SET;
    else
        throw except::Exception(Ctxt("Unsupported fft sign"));
}

template<> std::string six::toString(const FFTSign& value)
{
    switch (value)
    {
    case FFTSign::NEG:
        return "-1";
    case FFTSign::POS:
        return "+1";
    case FFTSign::NOT_SET:
        return "0";
    default:
        throw except::Exception(Ctxt("Unsupported fft sign"));
    }
}

template<> AppliedType six::toType<AppliedType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    // TODO: Needs correction -- added alternate value to match schema
    // from 03/17/2009.
    if (type == "APPLIED" || type == "APPILED")
        return AppliedType::IS_TRUE;
    else if (type == "NOT_APPLIED")
        return AppliedType::IS_FALSE;
    else
        throw except::Exception(Ctxt("Unsupported applied type"));
}

template<> std::string six::toString(const AppliedType& value)
{
    switch (value)
    {
    case AppliedType::IS_TRUE:
        // TODO: Needs correction -- support alternate value to match
        // schema from 03/17/2009.
//        return "APPILED";
        return "APPLIED"; // fixed in 2010-07-08 version of schema
    case AppliedType::IS_FALSE:
        return "NOT_APPLIED";
    default:
        throw except::Exception(Ctxt("Unsupported applied type"));
    }
}

template<> CollectType six::toType<CollectType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "MONOSTATIC")
        return CollectType::MONOSTATIC;
    else if (type == "BISTATIC")
        return CollectType::BISTATIC;
    else
        throw except::Exception(Ctxt("Unsupported collect type"));
}

template<> std::string six::toString(const CollectType& value)
{
    switch (value)
    {
    case CollectType::MONOSTATIC:
        return "MONOSTATIC";
    case CollectType::BISTATIC:
        return "BISTATIC";
    default:
        throw except::Exception(Ctxt("Unsupported collect type"));
    }
}

template<> std::string six::toString(const six::FrameType& value)
{
    switch (value)
    {
    case FrameType::ECF:
        return "ECF";
    case FrameType::RIC_ECF:
        return "RIC_ECF";
    case FrameType::RIC_ECI:
        return "RIC_ECI";
    default:
        throw except::Exception(Ctxt("Unsupported frame type"));
    }
}

template<> six::FrameType six::toType<six::FrameType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "ECF")
        return FrameType::ECF;
    else if (type == "RIC_ECF")
        return FrameType::RIC_ECF;
    else if (type == "RIC_ECI")
        return FrameType::RIC_ECI;
    else
        throw except::Exception(Ctxt("Unsupported frame type"));
}
