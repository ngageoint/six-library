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



//! THIS SHOULD ONLY BE USED WHEN THE TYPE IS booleantype in the spec!
template<> six::BooleanType str::toType<six::BooleanType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "true" || type == "1")
        return six::BOOL_TRUE;
    else if (type == "false" || type == "0")
        return six::BOOL_FALSE;
    else
        return six::BOOL_NOT_SET;
}
template<> six::DateTime str::toType<six::DateTime>(const std::string& dateTime)
{
    try
    {
        //Try an XML format type
        if (dateTime.length() >= 19)
            return six::DateTime(dateTime.substr(0, 19), "%Y-%m-%dT%H:%M:%S");
    }
    catch (except::Exception &ex)
    {
    }

    try
    {
        //Try a NITF 2.1 format
        if (dateTime.length() >= 14)
            return six::DateTime(dateTime.substr(0, 14), "%Y%m%d%H%M%S");
    }
    catch (except::Exception &ex)
    {
    }

    try
    {
        //Try a NITF 2.1 format - just date
        if (dateTime.length() >= 10)
            return six::DateTime(dateTime.substr(0, 10), "%Y-%m-%d");
    }
    catch (except::Exception &ex)
    {
    }

    try
    {
        //Try a simple 8-char format
        if (dateTime.length() == 8)
            return six::DateTime(dateTime.substr(0, 8), "%Y-%m-%d");
    }
    catch (except::Exception &ex)
    {
    }

    //should we really do this?
    return six::DateTime();
}

template<> std::string str::toString(const six::DateTime& dateTime)
{
    char date[256];
    date[255] = 0;
    dateTime.format("%FT%TZ", date, 255);
    std::string strDate(date);
    return strDate;
}

template<> std::string str::toString(const six::RadarModeType& type)
{
    std::string mode = "SPOTLIGHT";
    switch (type)
    {
    case six::MODE_INVALID:
        throw except::Exception(Ctxt("Radar mode is not valid!"));
    case six::MODE_NOT_SET:
        throw except::Exception(Ctxt("Radar mode not set!"));
    case six::MODE_STRIPMAP:
        mode = "STRIPMAP";
        break;
    case six::MODE_DYNAMIC_STRIPMAP:
        mode = "DYNAMIC_STRIPMAP";
        break;
    default:
        break;
    }
    return mode;
}

template<> six::RadarModeType str::toType<six::RadarModeType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "SPOTLIGHT")
        return six::MODE_SPOTLIGHT;
    if (type == "STRIPMAP")
        return six::MODE_STRIPMAP;
    if (type == "DYNAMIC_STRIPMAP")
        return six::MODE_DYNAMIC_STRIPMAP;
    if (type.empty())
        return six::MODE_NOT_SET;
    return six::MODE_INVALID;
}

template<> std::string str::toString(const six::DataClass& type)
{
    switch (type)
    {
    case six::DATA_COMPLEX:
        return "SICD";
    case six::DATA_DERIVED:
        return "SIDD";
    default:
        throw except::Exception(Ctxt("Unsupported data class"));
    }
}

template<> std::string str::toString(const six::DataType& type)
{
    switch (type)
    {
    case six::TYPE_COMPLEX:
        return "SICD";
    case six::TYPE_DERIVED:
        return "SIDD";
    default:
        throw except::Exception(Ctxt("Unsupported data type"));
    }
}

template<> six::PixelType str::toType<six::PixelType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "RE32F_IM32F")
        return six::RE32F_IM32F;
    if (type == "RE16I_IM16I")
        return six::RE16I_IM16I;
    if (type == "MONO8LU")
        return six::MONO8LU;
    if (type == "MONO8I")
        return six::MONO8I;
    if (type == "MONO16I")
        return six::MONO16I;
    if (type == "RGB8LU")
        return six::RGB8LU;
    if (type == "RGB24I")
        return six::RGB24I;
    throw except::Exception(
            Ctxt(FmtX("Type not understood [%s]", type.c_str())));
}

template<> std::string str::toString(const six::PixelType& type)
{
    std::string pixel;
    switch (type)
    {
    case six::RE32F_IM32F:
        pixel = "RE32F_IM32F";
        break;
    case six::RE16I_IM16I:
        pixel = "RE16I_IM16I";
        break;
    case six::MONO8I:
        pixel = "MONO8I";
        break;
    case six::MONO16I:
        pixel = "MONO16I";
        break;
    case six::MONO8LU:
        pixel = "MONO8LU";
        break;
    case six::RGB8LU:
        pixel = "RGB8LU";
        break;
    case six::RGB24I:
        pixel = "RGB24I";
        break;
    default:
        throw except::Exception(Ctxt("Unsupported pixel type"));
    };
    return pixel;
}

template<> six::MagnificationMethod str::toType<six::MagnificationMethod>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NEAREST_NEIGHBOR")
        return six::MAG_NEAREST_NEIGHBOR;
    if (type == "BILINEAR")
        return six::MAG_BILINEAR;
    return six::MAG_NOT_SET;
}

template<> std::string str::toString(const six::MagnificationMethod& method)
{
    switch (method)
    {
    case six::MAG_NEAREST_NEIGHBOR:
        return "NEAREST_NEIGHBOR";
    case six::MAG_BILINEAR:
        return "BILINEAR";
    default:
        throw except::Exception(Ctxt("Unsupported method"));
    }
}

template<> six::DecimationMethod str::toType<six::DecimationMethod>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NEAREST_NEIGHBOR")
        return six::DEC_NEAREST_NEIGHBOR;
    if (type == "BILINEAR")
        return six::DEC_BILINEAR;
    if (type == "BRIGHTEST_PIXEL")
        return six::DEC_BRIGHTEST_PIXEL;
    if (type == "LAGRANGE")
        return six::DEC_LAGRANGE;
    return six::DEC_NOT_SET;
}

template<> std::string str::toString(const six::DecimationMethod& method)
{
    switch (method)
    {
    case six::DEC_NEAREST_NEIGHBOR:
        return "NEAREST_NEIGHBOR";
    case six::DEC_BILINEAR:
        return "BILINEAR";
    case six::DEC_BRIGHTEST_PIXEL:
        return "BRIGHTEST_PIXEL";
    case six::DEC_LAGRANGE:
        return "LAGRANGE";
    default:
        throw except::Exception(Ctxt("Unsupported method"));
    }
}

template<> six::EarthModelType str::toType<six::EarthModelType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "WGS_84")
        return six::EARTH_WGS84;
    return six::EARTH_NOT_SET;
}

template<> std::string str::toString(const six::EarthModelType& t)
{
    switch (t)
    {
    case six::EARTH_WGS84:
        return "WGS_84";
    default:
        throw except::Exception(Ctxt("Unsupported earth model"));
    }
}

template<> six::PolarizationType str::toType<six::PolarizationType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
        return six::POL_OTHER;
    else if (type == "V")
        return six::POL_V;
    else if (type == "H")
        return six::POL_H;
    else if (type == "RHC")
        return six::POL_RHC;
    else if (type == "LHC")
        return six::POL_LHC;
    else
        throw except::Exception(Ctxt("Unsupported polarization type"));
}

template<> std::string str::toString(const six::PolarizationType& t)
{
    switch (t)
    {
    case six::POL_OTHER:
        return "OTHER";
    case six::POL_V:
        return "V";
    case six::POL_H:
        return "H";
    case six::POL_RHC:
        return "RHC";
    case six::POL_LHC:
        return "LHC";
    default:
        throw except::Exception(Ctxt(
                "Unsupported conversion from polarization type"));
    }
}

template<> six::DualPolarizationType str::toType<six::DualPolarizationType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
        return six::DUAL_POL_OTHER;
    else if (type == "V:V")
        return six::DUAL_POL_V_V;
    else if (type == "V:H")
        return six::DUAL_POL_V_H;
    else if (type == "H:V")
        return six::DUAL_POL_H_V;
    else if (type == "H:H")
        return six::DUAL_POL_H_H;
    else if (type == "RHC:RHC")
        return six::DUAL_POL_RHC_RHC;
    else if (type == "RHC:LHC")
        return six::DUAL_POL_RHC_LHC;
    else if (type == "LHC:LHC")
        return six::DUAL_POL_LHC_LHC;
    else
        throw except::Exception(Ctxt(
                "Unsupported conversion to dual polarization type"));
}

template<> std::string str::toString(const six::DualPolarizationType& t)
{
    switch (t)
    {
    case six::DUAL_POL_OTHER:
        return "OTHER";
    case six::DUAL_POL_V_V:
        return "V:V";
    case six::DUAL_POL_V_H:
        return "V:H";
    case six::DUAL_POL_H_V:
        return "H:V";
    case six::DUAL_POL_H_H:
        return "H:H";
    case six::DUAL_POL_RHC_RHC:
        return "RHC:RHC";
    case six::DUAL_POL_RHC_LHC:
        return "RHC:LHC";
    case six::DUAL_POL_LHC_LHC:
        return "LHC:LHC";
    default:
        throw except::Exception(Ctxt(
                "Unsupported dual polarization type to string"));
    }
}

template<> six::DemodType str::toType<six::DemodType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "STRETCH")
        return six::DEMOD_STRETCH;
    else if (type == "CHIRP")
        return six::DEMOD_CHIRP;
    else
        throw except::Exception(Ctxt("Unsupported demod type"));
}

template<> std::string str::toString(const six::DemodType& t)
{
    switch (t)
    {
    case six::DEMOD_STRETCH:
        return "STRETCH";
    case six::DEMOD_CHIRP:
        return "CHIRP";
    default:
        throw except::Exception(Ctxt("Unsupported demod type"));
    }
}

template<> six::ImageFormationType str::toType<six::ImageFormationType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
        return six::IF_OTHER;
    else if (type == "PFA")
        return six::IF_PFA;
    else if (type == "RMA")
        return six::IF_RMA;
    else if (type == "CSA")
        return six::IF_CSA;
    else if (type == "RDA")
        return six::IF_RDA;
    else
        throw except::Exception(Ctxt("Unsupported image formation type"));
}

template<> std::string str::toString(const six::ImageFormationType& t)
{
    switch (t)
    {
    case six::IF_OTHER:
        return "OTHER";
    case six::IF_PFA:
        return "PFA";
    case six::IF_RMA:
        return "RMA";
    case six::IF_CSA:
        return "CSA";
    case six::IF_RDA:
        return "RDA";
    default:
        throw except::Exception(Ctxt("Unsupported image formation type"));
    }
}

template<> six::SlowTimeBeamCompensationType str::toType<
        six::SlowTimeBeamCompensationType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NO")
        return six::SLOW_TIME_BEAM_NO;
    else if (type == "GLOBAL")
        return six::SLOW_TIME_BEAM_GLOBAL;
    else if (type == "SV")
        return six::SLOW_TIME_BEAM_SV;
    else
        throw except::Exception(Ctxt(
                "Unsupported slow time beam compensation type"));
}

template<> std::string str::toString(const six::SlowTimeBeamCompensationType& t)
{
    switch (t)
    {
    case six::SLOW_TIME_BEAM_NO:
        return "NO";
    case six::SLOW_TIME_BEAM_GLOBAL:
        return "GLOBAL";
    case six::SLOW_TIME_BEAM_SV:
        return "SV";
    default:
        throw except::Exception(Ctxt(
                "Unsupported slow time beam compensation type"));
    }
}

template<> six::ImageBeamCompensationType str::toType<
        six::ImageBeamCompensationType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NO")
        return six::IMAGE_BEAM_NO;
    else if (type == "SV")
        return six::IMAGE_BEAM_SV;
    else
        throw except::Exception(
                Ctxt("Unsupported image beam compensation type"));
}

template<> std::string str::toString(const six::ImageBeamCompensationType& t)
{
    switch (t)
    {
    case six::IMAGE_BEAM_NO:
        return "NO";
    case six::IMAGE_BEAM_SV:
        return "SV";
    default:
        throw except::Exception(
                Ctxt("Unsupported image beam compensation type"));
    }
}

template<> six::AutofocusType str::toType<six::AutofocusType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "NO")
        return six::AUTOFOCUS_NO;
    else if (type == "GLOBAL")
        return six::AUTOFOCUS_GLOBAL;
    else if (type == "SV")
        return six::AUTOFOCUS_SV;
    else
        throw except::Exception(Ctxt("Unsupported autofocus type"));
}

template<> std::string str::toString(const six::AutofocusType& t)
{
    switch (t)
    {
    case six::AUTOFOCUS_NO:
        return "NO";
    case six::AUTOFOCUS_GLOBAL:
        return "GLOBAL";
    case six::AUTOFOCUS_SV:
        return "SV";
    default:
        throw except::Exception(Ctxt("Unsupported autofocus type"));
    }
}

template<> six::PosVelError::FrameType str::toType<six::PosVelError::FrameType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "ECF")
        return six::PosVelError::FRAME_ECF;
    else if (type == "RIC_ECF")
        return six::PosVelError::FRAME_RIC_ECF;
    else if (type == "RIC_ECI")
        return six::PosVelError::FRAME_RIC_ECI;
    else
        throw except::Exception(Ctxt("Unsupported frame type"));
}

template<> six::SideOfTrackType str::toType<six::SideOfTrackType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "L")
        return six::SIDE_LEFT;
    else if (type == "R")
        return six::SIDE_RIGHT;
    else
        throw except::Exception(Ctxt("Unsupported side of track"));
}

template<> std::string str::toString(const six::SideOfTrackType& t)
{
    switch (t)
    {
    case six::SIDE_LEFT:
        return "L";
    case six::SIDE_RIGHT:
        return "R";
    default:
        throw except::Exception(Ctxt("Unsupported side of track"));
    }
}

template<> six::ComplexImagePlaneType str::toType<six::ComplexImagePlaneType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
        return six::PLANE_OTHER;
    else if (type == "SLANT")
        return six::PLANE_SLANT;
    else if (type == "GROUND")
        return six::PLANE_GROUND;
    else
        throw except::Exception(Ctxt("Unsupported complex image plane"));
}

template<> std::string str::toString(const six::ComplexImagePlaneType& t)
{
    switch (t)
    {
    case six::PLANE_OTHER:
        return "OTHER";
    case six::PLANE_SLANT:
        return "SLANT";
    case six::PLANE_GROUND:
        return "GROUND";
    default:
        throw except::Exception(Ctxt("Unsupported complex image plane"));
    }
}

template<> six::ComplexImageGridType str::toType<six::ComplexImageGridType>(
        const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "RGAZIM")
        return six::GRID_RGAZIM;
    else if (type == "RGZERO")
        return six::GRID_RGZERO;
    else if (type == "CARTESIAN")
        return six::GRID_CARTESIAN;
    else
        throw except::Exception(Ctxt("Unsupported complex image grid"));
}

template<> std::string str::toString(const six::ComplexImageGridType& t)
{
    switch (t)
    {
    case six::GRID_RGAZIM:
        return "RGAZIM";
    case six::GRID_RGZERO:
        return "RGZERO";
    case six::GRID_CARTESIAN:
        return "CARTESIAN";
    default:
        throw except::Exception(Ctxt("Unsupported complex image grid"));
    }
}

template <> six::FFTSign str::toType<six::FFTSign>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "-1")
        return six::FFT_SIGN_NEG;
    else if (type == "1")
        return six::FFT_SIGN_POS;
    else if (type == "0")
        return six::FFT_SIGN_NOT_SET;
    else
        throw except::Exception(Ctxt("Unsupported fft sign"));
}

template <> std::string str::toString(const six::FFTSign& value)
{
    switch (value)
    {
    case six::FFT_SIGN_NEG:
        return "-1";
    case six::FFT_SIGN_POS:
        return "1";
    case six::FFT_SIGN_NOT_SET:
        return "0";
    default:
        throw except::Exception(Ctxt("Unsupported fft sign"));
    }
}

template <> six::AppliedType str::toType<six::AppliedType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "APPLIED")
        return six::APPLIED_TRUE;
    else if (type == "NOT_APPLIED")
        return six::APPLIED_FALSE;
    else
        throw except::Exception(Ctxt("Unsupported applied type"));
}

template <> std::string str::toString(const six::AppliedType& value)
{
    switch (value)
    {
    case six::APPLIED_TRUE:
        return "APPLIED";
    case six::APPLIED_FALSE:
        return "NOT_APPLIED";
    default:
        throw except::Exception(Ctxt("Unsupported applied type"));
    }
}

template <> six::CollectType str::toType<six::CollectType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "MONOSTATIC")
        return six::COLLECT_MONOSTATIC;
    else if (type == "BISTATIC")
        return six::COLLECT_BISTATIC;
    else
        throw except::Exception(Ctxt("Unsupported collect type"));
}

template <> std::string str::toString(const six::CollectType& value)
{
    switch (value)
    {
    case six::COLLECT_MONOSTATIC:
        return "MONOSTATIC";
    case six::COLLECT_BISTATIC:
        return "BISTATIC";
    default:
        throw except::Exception(Ctxt("Unsupported collect type"));
    }
}

template<> bool str::toType<bool>(const std::string& s)
{
    std::string sc = s;
    str::lower(sc);
    return sc == "true" || sc == "1" || sc == "yes";
}

