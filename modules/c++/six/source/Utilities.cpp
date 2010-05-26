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
        return BooleanType::BOOL_TRUE;
    else if (type == "false" || type == "0" || type == "no")
        return BooleanType::BOOL_FALSE;
    else
        return BooleanType::BOOL_NOT_SET;
}

template<> std::string toString<BooleanType>(const BooleanType& value)
{
    return toString<bool>(value == BooleanType::BOOL_TRUE);
}

template<> DateTime six::toType<DateTime>(const std::string& dateTime)
{
    try
    {
        //Try an XML format type
        if (dateTime.length() >= 19)
            return DateTime(dateTime.substr(0, 19), "%Y-%m-%dT%H:%M:%S");
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

    // FIXME!!
#ifndef WIN32
    dateTime.format("%FT%TZ", date, 255);
#else
    dateTime.format("%Y-%m-%dT%H:%M:%SZ", date, 255);
#endif
    std::string strDate(date);
    return strDate;
}

template<> std::string six::toString(const RadarModeType& type)
{
    std::string mode = "SPOTLIGHT";
    switch (type)
    {
    case RadarModeType::MODE_INVALID:
        throw except::Exception(Ctxt("Radar mode is not valid!"));
    case RadarModeType::MODE_NOT_SET:
        throw except::Exception(Ctxt("Radar mode not set!"));
    case RadarModeType::MODE_STRIPMAP:
        mode = "STRIPMAP";
        break;
    case RadarModeType::MODE_DYNAMIC_STRIPMAP:
        mode = "DYNAMIC STRIPMAP";
        break;
    default:
        break;
    }
    return mode;
}

template<> RadarModeType six::toType<RadarModeType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "SPOTLIGHT")
        return RadarModeType::MODE_SPOTLIGHT;
    if (type == "STRIPMAP")
        return RadarModeType::MODE_STRIPMAP;
    if (type == "DYNAMIC STRIPMAP")
        return RadarModeType::MODE_DYNAMIC_STRIPMAP;
    if (type.empty())
        return RadarModeType::MODE_NOT_SET;
    return RadarModeType::MODE_INVALID;
}

template<> std::string six::toString(const DataClass& type)
{
    switch (type)
    {
    case DataClass::DATA_COMPLEX:
        return "SICD";
    case DataClass::DATA_DERIVED:
        return "SIDD";
    default:
        throw except::Exception(Ctxt("Unsupported data class"));
    }
}

template<> std::string six::toString(const DataType& type)
{
    switch (type)
    {
    case DataType::TYPE_COMPLEX:
        return "SICD";
    case DataType::TYPE_DERIVED:
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
    if (p == PixelType::PIXEL_TYPE_NOT_SET)
        throw except::Exception(Ctxt(FmtX("Type not understood [%s]",
                                          type.c_str())));
}

template<> std::string six::toString(const PixelType& type)
{
    if (type == PixelType::PIXEL_TYPE_NOT_SET)
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
        return MagnificationMethod::MAG_NEAREST_NEIGHBOR;
    if (type == "BILINEAR")
        return MagnificationMethod::MAG_BILINEAR;
    return MagnificationMethod::MAG_NOT_SET;
}

template<> std::string six::toString(const MagnificationMethod& method)
{
    switch (method)
    {
    case MagnificationMethod::MAG_NEAREST_NEIGHBOR:
        return "NEAREST_NEIGHBOR";
    case MagnificationMethod::MAG_BILINEAR:
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
        return DecimationMethod::DEC_NEAREST_NEIGHBOR;
    if (type == "BILINEAR")
        return DecimationMethod::DEC_BILINEAR;
    if (type == "BRIGHTEST_PIXEL")
        return DecimationMethod::DEC_BRIGHTEST_PIXEL;
    if (type == "LAGRANGE")
        return DecimationMethod::DEC_LAGRANGE;
    return DecimationMethod::DEC_NOT_SET;
}

template<> std::string six::toString(const DecimationMethod& method)
{
    switch (method)
    {
    case DecimationMethod::DEC_NEAREST_NEIGHBOR:
        return "NEAREST_NEIGHBOR";
    case DecimationMethod::DEC_BILINEAR:
        return "BILINEAR";
    case DecimationMethod::DEC_BRIGHTEST_PIXEL:
        return "BRIGHTEST_PIXEL";
    case DecimationMethod::DEC_LAGRANGE:
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
        return EarthModelType::EARTH_WGS84;
    return EarthModelType::EARTH_NOT_SET;
}

template<> std::string six::toString(const EarthModelType& t)
{
    switch (t)
    {
    case EarthModelType::EARTH_WGS84:
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
        return OrientationType::ORIENT_UP;
    else if (type == "DOWN")
        return OrientationType::ORIENT_DOWN;
    else if (type == "LEFT")
        return OrientationType::ORIENT_LEFT;
    else if (type == "RIGHT")
        return OrientationType::ORIENT_RIGHT;
    else if (type == "ARBITRARY")
        return OrientationType::ORIENT_ARBITRARY;
    else
        throw except::Exception(Ctxt("Unsupported orientation type"));
}

template<> std::string six::toString(const OrientationType& t)
{
    switch (t)
    {
    case OrientationType::ORIENT_UP:
        return "UP";
    case OrientationType::ORIENT_DOWN:
        return "DOWN";
    case OrientationType::ORIENT_LEFT:
        return "LEFT";
    case OrientationType::ORIENT_RIGHT:
        return "RIGHT";
    case OrientationType::ORIENT_ARBITRARY:
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
        return PolarizationType::POL_OTHER;
    else if (type == "V")
        return PolarizationType::POL_V;
    else if (type == "H")
        return PolarizationType::POL_H;
    else if (type == "RHC")
        return PolarizationType::POL_RHC;
    else if (type == "LHC")
        return PolarizationType::POL_LHC;
    else
        throw except::Exception(Ctxt("Unsupported polarization type"));
}

template<> std::string six::toString(const PolarizationType& t)
{
    switch (t)
    {
    case PolarizationType::POL_OTHER:
        return "OTHER";
    case PolarizationType::POL_V:
        return "V";
    case PolarizationType::POL_H:
        return "H";
    case PolarizationType::POL_RHC:
        return "RHC";
    case PolarizationType::POL_LHC:
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
        return DualPolarizationType::DUAL_POL_OTHER;
    else if (type == "V:V")
        return DualPolarizationType::DUAL_POL_V_V;
    else if (type == "V:H")
        return DualPolarizationType::DUAL_POL_V_H;
    else if (type == "H:V")
        return DualPolarizationType::DUAL_POL_H_V;
    else if (type == "H:H")
        return DualPolarizationType::DUAL_POL_H_H;
    else if (type == "RHC:RHC")
        return DualPolarizationType::DUAL_POL_RHC_RHC;
    else if (type == "RHC:LHC")
        return DualPolarizationType::DUAL_POL_RHC_LHC;
    else if (type == "LHC:LHC")
        return DualPolarizationType::DUAL_POL_LHC_LHC;
    else
        throw except::Exception(
                                Ctxt(
                                     "Unsupported conversion to dual polarization type"));
}

template<> std::string six::toString(const DualPolarizationType& t)
{
    switch (t)
    {
    case DualPolarizationType::DUAL_POL_OTHER:
        return "OTHER";
    case DualPolarizationType::DUAL_POL_V_V:
        return "V:V";
    case DualPolarizationType::DUAL_POL_V_H:
        return "V:H";
    case DualPolarizationType::DUAL_POL_H_V:
        return "H:V";
    case DualPolarizationType::DUAL_POL_H_H:
        return "H:H";
    case DualPolarizationType::DUAL_POL_RHC_RHC:
        return "RHC:RHC";
    case DualPolarizationType::DUAL_POL_RHC_LHC:
        return "RHC:LHC";
    case DualPolarizationType::DUAL_POL_LHC_LHC:
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
        return DemodType::DEMOD_STRETCH;
    else if (type == "CHIRP")
        return DemodType::DEMOD_CHIRP;
    else
        throw except::Exception(Ctxt("Unsupported demod type"));
}

template<> std::string six::toString(const DemodType& t)
{
    switch (t)
    {
    case DemodType::DEMOD_STRETCH:
        return "STRETCH";
    case DemodType::DEMOD_CHIRP:
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
        return ImageFormationType::IF_OTHER;
    else if (type == "PFA")
        return ImageFormationType::IF_PFA;
    else if (type == "RMA")
        return ImageFormationType::IF_RMA;
    else if (type == "RGAZCOMP")
        return ImageFormationType::IF_RGAZCOMP;
    else
        throw except::Exception(Ctxt("Unsupported image formation type"));
}

template<> std::string six::toString(const ImageFormationType& t)
{
    switch (t)
    {
    case ImageFormationType::IF_OTHER:
        return "OTHER";
    case ImageFormationType::IF_PFA:
        return "PFA";
    case ImageFormationType::IF_RMA:
        return "RMA";
    case ImageFormationType::IF_RGAZCOMP:
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
        return SlowTimeBeamCompensationType::SLOW_TIME_BEAM_NO;
    else if (type == "GLOBAL")
        return SlowTimeBeamCompensationType::SLOW_TIME_BEAM_GLOBAL;
    else if (type == "SV")
        return SlowTimeBeamCompensationType::SLOW_TIME_BEAM_SV;
    else
        throw except::Exception(
                                Ctxt(
                                     "Unsupported slow time beam compensation type"));
}

template<> std::string six::toString(const SlowTimeBeamCompensationType& t)
{
    switch (t)
    {
    case SlowTimeBeamCompensationType::SLOW_TIME_BEAM_NO:
        return "NO";
    case SlowTimeBeamCompensationType::SLOW_TIME_BEAM_GLOBAL:
        return "GLOBAL";
    case SlowTimeBeamCompensationType::SLOW_TIME_BEAM_SV:
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
        return ImageBeamCompensationType::IMAGE_BEAM_NO;
    else if (type == "SV")
        return ImageBeamCompensationType::IMAGE_BEAM_SV;
    else
        throw except::Exception(
                                Ctxt("Unsupported image beam compensation type"));
}

template<> std::string six::toString(const ImageBeamCompensationType& t)
{
    switch (t)
    {
    case ImageBeamCompensationType::IMAGE_BEAM_NO:
        return "NO";
    case ImageBeamCompensationType::IMAGE_BEAM_SV:
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
        return AutofocusType::AUTOFOCUS_NO;
    else if (type == "GLOBAL")
        return AutofocusType::AUTOFOCUS_GLOBAL;
    else if (type == "SV")
        return AutofocusType::AUTOFOCUS_SV;
    else
        throw except::Exception(Ctxt("Unsupported autofocus type"));
}

template<> std::string six::toString(const AutofocusType& t)
{
    switch (t)
    {
    case AutofocusType::AUTOFOCUS_NO:
        return "NO";
    case AutofocusType::AUTOFOCUS_GLOBAL:
        return "GLOBAL";
    case AutofocusType::AUTOFOCUS_SV:
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
        return RMAlgoType::RMA_OMEGA_K;
    else if (type == "CSA")
        return RMAlgoType::RMA_CSA;
    else if (type == "RG_DOP")
        return RMAlgoType::RMA_RG_DOP;
    else
        throw except::Exception(Ctxt("Unsupported RM algorithm type"));
}

template<> std::string six::toString(const RMAlgoType& t)
{
    switch (t)
    {
    case RMAlgoType::RMA_OMEGA_K:
        return "OMEGA_K";
    case RMAlgoType::RMA_CSA:
        return "CSA";
    case RMAlgoType::RMA_RG_DOP:
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
        return SideOfTrackType::SIDE_LEFT;
    else if (type == "R")
        return SideOfTrackType::SIDE_RIGHT;
    else
        throw except::Exception(Ctxt("Unsupported side of track"));
}

template<> std::string six::toString(const SideOfTrackType& t)
{
    switch (t)
    {
    case SideOfTrackType::SIDE_LEFT:
        return "L";
    case SideOfTrackType::SIDE_RIGHT:
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
        return ComplexImagePlaneType::PLANE_OTHER;
    else if (type == "SLANT")
        return ComplexImagePlaneType::PLANE_SLANT;
    else if (type == "GROUND")
        return ComplexImagePlaneType::PLANE_GROUND;
    else
        throw except::Exception(Ctxt("Unsupported complex image plane"));
}

template<> std::string six::toString(const ComplexImagePlaneType& t)
{
    switch (t)
    {
    case ComplexImagePlaneType::PLANE_OTHER:
        return "OTHER";
    case ComplexImagePlaneType::PLANE_SLANT:
        return "SLANT";
    case ComplexImagePlaneType::PLANE_GROUND:
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
        return ComplexImageGridType::GRID_RGAZIM;
    else if (type == "RGZERO")
        return ComplexImageGridType::GRID_RGZERO;
    else if (type == "XRGYCR")
        return ComplexImageGridType::GRID_XRGYCR;
    else if (type == "XCTYAT")
        return ComplexImageGridType::GRID_XCTYAT;
    else if (type == "PLANE")
        return ComplexImageGridType::GRID_PLANE;
    else
        throw except::Exception(Ctxt("Unsupported complex image grid"));
}

template<> std::string six::toString(const ComplexImageGridType& t)
{
    switch (t)
    {
    case ComplexImageGridType::GRID_RGAZIM:
        return "RGAZIM";
    case ComplexImageGridType::GRID_RGZERO:
        return "RGZERO";
    case ComplexImageGridType::GRID_XRGYCR:
        return "XRGYCR";
    case ComplexImageGridType::GRID_XCTYAT:
        return "XCTYAT";
    case ComplexImageGridType::GRID_PLANE:
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
        return FFTSign::FFT_SIGN_NEG;
    else if (type == "+1")
        return FFTSign::FFT_SIGN_POS;
    else if (type == "0")
        return FFTSign::FFT_SIGN_NOT_SET;
    else
        throw except::Exception(Ctxt("Unsupported fft sign"));
}

template<> std::string six::toString(const FFTSign& value)
{
    switch (value)
    {
    case FFTSign::FFT_SIGN_NEG:
        return "-1";
    case FFTSign::FFT_SIGN_POS:
        return "+1";
    case FFTSign::FFT_SIGN_NOT_SET:
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
        return AppliedType::APPLIED_TRUE;
    else if (type == "NOT_APPLIED")
        return AppliedType::APPLIED_FALSE;
    else
        throw except::Exception(Ctxt("Unsupported applied type"));
}

template<> std::string six::toString(const AppliedType& value)
{
    switch (value)
    {
    case AppliedType::APPLIED_TRUE:
        // TODO: Needs correction -- support alternate value to match
        // schema from 03/17/2009.
        return "APPILED";
    case AppliedType::APPLIED_FALSE:
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
        return CollectType::COLLECT_MONOSTATIC;
    else if (type == "BISTATIC")
        return CollectType::COLLECT_BISTATIC;
    else
        throw except::Exception(Ctxt("Unsupported collect type"));
}

template<> std::string six::toString(const CollectType& value)
{
    switch (value)
    {
    case CollectType::COLLECT_MONOSTATIC:
        return "MONOSTATIC";
    case CollectType::COLLECT_BISTATIC:
        return "BISTATIC";
    default:
        throw except::Exception(Ctxt("Unsupported collect type"));
    }
}

template<> std::string six::toString(const six::FrameType& value)
{
    switch (value)
    {
    case FrameType::FRAME_ECF:
        return "ECF";
    case FrameType::FRAME_RIC_ECF:
        return "RIC_ECF";
    case FrameType::FRAME_RIC_ECI:
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
        return FrameType::FRAME_ECF;
    else if (type == "RIC_ECF")
        return FrameType::FRAME_RIC_ECF;
    else if (type == "RIC_ECI")
        return FrameType::FRAME_RIC_ECI;
    else
        throw except::Exception(Ctxt("Unsupported frame type"));
}
