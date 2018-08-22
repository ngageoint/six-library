/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <sstream>
#include <iomanip>

#include <nitf/PluginRegistry.hpp>
#include <logging/NullLogger.h>
#include <math/Utilities.h>
#include "six/Utilities.h"
#include "six/XMLControl.h"

namespace
{
NITF_TRE_STATIC_HANDLER_REF(XML_DATA_CONTENT);

void assign(math::linear::MatrixMxN<7, 7>& sensorCovar,
            size_t row,
            size_t col,
            double val)
{
    sensorCovar(row, col) = sensorCovar(col, row) = val;
}

void getSensorCovariance(const six::PosVelError& error,
                         double rangeBias,
                         math::linear::MatrixMxN<7, 7>& sensorCovar)
{
    sensorCovar(0, 0) = math::square(error.p1);
    sensorCovar(1, 1) = math::square(error.p2);
    sensorCovar(2, 2) = math::square(error.p3);
    sensorCovar(3, 3) = math::square(error.v1);
    sensorCovar(4, 4) = math::square(error.v2);
    sensorCovar(5, 5) = math::square(error.v3);
    sensorCovar(6, 6) = math::square(rangeBias);

    if (error.corrCoefs.get())
    {
        const six::CorrCoefs& corrCoefs(*error.corrCoefs);

        // Position Error
        assign(sensorCovar, 0, 1, error.p1 * error.p2 * corrCoefs.p1p2);
        assign(sensorCovar, 0, 2, error.p1 * error.p3 * corrCoefs.p1p3);
        assign(sensorCovar, 1, 2, error.p2 * error.p3 * corrCoefs.p2p3);

        // Velocity Error
        assign(sensorCovar, 3, 4, error.v1 * error.v2 * corrCoefs.v1v2);
        assign(sensorCovar, 3, 5, error.v1 * error.v3 * corrCoefs.v1v3);
        assign(sensorCovar, 4, 5, error.v2 * error.v3 * corrCoefs.v2v3);

        // Position-Velocity Covariance
        assign(sensorCovar, 0, 3, error.p1 * error.v1 * corrCoefs.p1v1);
        assign(sensorCovar, 0, 4, error.p1 * error.v2 * corrCoefs.p1v2);
        assign(sensorCovar, 0, 5, error.p1 * error.v3 * corrCoefs.p1v3);
        assign(sensorCovar, 1, 3, error.p2 * error.v1 * corrCoefs.p2v1);
        assign(sensorCovar, 1, 4, error.p2 * error.v2 *corrCoefs.p2v2);
        assign(sensorCovar, 1, 5, error.p2 * error.v3 * corrCoefs.p2v3);
        assign(sensorCovar, 2, 3, error.p3 * error.v1 * corrCoefs.p3v1);
        assign(sensorCovar, 2, 4, error.p3 * error.v2 * corrCoefs.p3v2);
        assign(sensorCovar, 2, 5, error.p3 * error.v3 * corrCoefs.p3v3);
    }
}
}

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

template<> std::string six::toString<float>(const float& value)
{
    if (six::Init::isUndefined(value))
    {
        throw six::UninitializedValueException(
            Ctxt("Attempted use of uninitialized float value"));
    }

   std::ostringstream os;
   os << std::uppercase << std::scientific << std::setprecision(15) << value;
   std::string strValue = os.str();

   // remove any + in scientific notation to meet SICD XML standard
   size_t plusPos = strValue.find("+");
   if(plusPos != std::string::npos)
   {
      strValue.erase(plusPos, 1);
   }
   return strValue;
}

template<> std::string six::toString<double>(const double& value)
{
    if (six::Init::isUndefined(value))
    {
        throw six::UninitializedValueException(
            Ctxt("Attempted use of uninitialized double value"));
    }

   std::ostringstream os;
   os << std::uppercase << std::scientific << std::setprecision(15) << value;
   std::string strValue = os.str();

   // remove any + in scientific notation to meet SICD XML standard
   size_t plusPos = strValue.find("+");
   if(plusPos != std::string::npos)
   {
      strValue.erase(plusPos, 1);
   }
   return strValue;
}

template<> std::string six::toString<BooleanType>(const BooleanType& value)
{
    return str::toString<bool>(value == BooleanType::IS_TRUE);
}

template<> std::string six::toString(const six::Vector3 & v)
{
    std::ostringstream os;
    os << "(X:" << v[0] << " Y:" << v[1] << " Z:" << v[2] << ")";
    return os.str();
}

template<> std::string six::toString(const six::PolyXYZ & p)
{
    std::ostringstream os;
    for (size_t i = 0 ; i < p.size() ; i++)
    {
        os << toString(p[i]) << "*y^" << i << ((i != p.size()) ? " + " : "")
           << "\n";
    }
    return os.str();
}

template<> DateTime six::toType<DateTime>(const std::string& dateTime)
{
    try
    {
        //Try an XML format type
        // NOTE: There may or may not be a Z on the end of this
        //       The spec specifies that there "should" be to indicate UTC,
        //       but Timeline/CollectStart seems to frequently be produced
        //       without it
        if (dateTime.length() >= 19)
        {
            const bool trimStr = (dateTime[dateTime.length() - 1] == 'Z');

            return DateTime(trimStr ?
                                dateTime.substr(0, dateTime.length() - 1) :
                                dateTime,
                            "%Y-%m-%dT%H:%M:%S");
        }
    }
    catch (const except::Exception&)
    {
    }

    try
    {
        //Try a NITF 2.1 format
        if (dateTime.length() >= 14)
            return DateTime(dateTime.substr(0, 14), "%Y%m%d%H%M%S");
    }
    catch (const except::Exception&)
    {
    }

    try
    {
        //Try a NITF 2.1 format - just date
        if (dateTime.length() >= 10)
            return DateTime(dateTime.substr(0, 10), "%Y-%m-%d");
    }
    catch (const except::Exception&)
    {
    }

    try
    {
        //Try a simple 8-char format
        if (dateTime.length() == 8)
            return DateTime(dateTime.substr(0, 8), "%Y-%m-%d");
    }
    catch (const except::Exception&)
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
        return RadarModeType::DYNAMIC_STRIPMAP;
    if (type == "SCANSAR")
        return RadarModeType::SCANSAR;
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
    if (type == "LAGRANGE")
        return MagnificationMethod::LAGRANGE;
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
    case MagnificationMethod::LAGRANGE:
        return "LAGRANGE";
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
        throw except::Exception(Ctxt("Unsupported orientation type '" + s + "'"));
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

template<> PolarizationSequenceType six::toType<PolarizationSequenceType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
    {
        return PolarizationSequenceType::OTHER;
    }
    else if (type == "V")
    {
        return PolarizationSequenceType::V;
    }
    else if (type == "H")
    {
        return PolarizationSequenceType::H;
    }
    else if (type == "RHC")
    {
        return PolarizationSequenceType::RHC;
    }
    else if (type == "LHC")
    {
        return PolarizationSequenceType::LHC;
    }
    else if (type == "UNKNOWN")
    {
        return PolarizationSequenceType::UNKNOWN;
    }
    else if (type == "SEQUENCE")
    {
        return PolarizationSequenceType::SEQUENCE;
    }
    else
    {
        throw except::Exception(Ctxt(
                "Unsupported polarization type '" + s + "'"));
    }
}

template<> std::string six::toString(const PolarizationSequenceType& t)
{
    switch (t)
    {
    case PolarizationSequenceType::OTHER:
        return "OTHER";
    case PolarizationSequenceType::V:
        return "V";
    case PolarizationSequenceType::H:
        return "H";
    case PolarizationSequenceType::RHC:
        return "RHC";
    case PolarizationSequenceType::LHC:
        return "LHC";
    case PolarizationSequenceType::UNKNOWN:
        return "UNKNOWN";
    case PolarizationSequenceType::SEQUENCE:
        return "SEQUENCE";
    default:
        throw except::Exception(Ctxt(
                "Unsupported conversion from polarization type"));
    }
}

template<> PolarizationType six::toType<PolarizationType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "OTHER")
    {
        return PolarizationType::OTHER;
    }
    else if (type == "V")
    {
        return PolarizationType::V;
    }
    else if (type == "H")
    {
        return PolarizationType::H;
    }
    else if (type == "RHC")
    {
        return PolarizationType::RHC;
    }
    else if (type == "LHC")
    {
        return PolarizationType::LHC;
    }
    else
    {
        throw except::Exception(Ctxt(
                "Unsupported polarization type '" + s + "'"));
    }
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
        throw except::Exception(Ctxt(
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
    else if (type == "LHC:RHC")
        return DualPolarizationType::LHC_RHC;
    else if (type == "UNKNOWN")
        return DualPolarizationType::UNKNOWN;
    else
    {
        throw except::Exception(Ctxt(
                "Unsupported conversion to dual polarization type '" + s + "'"));
    }
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
    case DualPolarizationType::LHC_RHC:
        return "LHC:RHC";
    case DualPolarizationType::UNKNOWN:
        return "UNKNOWN";
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
        throw except::Exception(Ctxt("Unsupported demod type '" + s + "'"));
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
    {
        throw except::Exception(Ctxt(
                "Unsupported image formation type + '" + s + "'"));
    }
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
    {
        throw except::Exception(Ctxt(
                "Unsupported slow time beam compensation type + '" + s + "'"));
    }
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
    {
        throw except::Exception(Ctxt(
                "Unsupported image beam compensation type + '" + s + "'"));
    }
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
    {
        throw except::Exception(Ctxt(
                "Unsupported autofocus type + '" + s + "'"));
    }
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
    {
        throw except::Exception(Ctxt(
                "Unsupported RM algorithm type '" + s + "'"));
    }
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
        throw except::Exception(Ctxt("Unsupported side of track '" + s + "'"));
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
    {
        throw except::Exception(Ctxt(
                "Unsupported complex image plane '" + s + "'"));
    }
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
    {
        throw except::Exception(Ctxt(
                "Unsupported complex image grid '" + s + "'"));
    }
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
    {
        return FFTSign::NEG;
    }
    else if (type == "+1" || type == "1")
    {
        // NOTE: The SICD Volume 1 spec says only "+1" and "-1" are allowed,
        //       and while the schema uses those same strings, it sets the
        //       type to xs:int so that "1" will pass schema validation.  Some
        //       producers do use "1" so for simplicity just support it here.
        return FFTSign::POS;
    }
    else if (type == "0")
    {
        return FFTSign::NOT_SET;
    }
    else
    {
        throw except::Exception(Ctxt("Unsupported fft sign '" + s + "'"));
    }
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
        throw except::Exception(Ctxt("Unsupported applied type '" + s + "'"));
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
        throw except::Exception(Ctxt("Unsupported collect type '" + s + "'"));
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
    switch (value.mValue)
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
        throw except::Exception(Ctxt("Unsupported frame type '" + s + "'"));
}

template<> std::string six::toString(const six::LatLonCorners& corners)
{
    // Print the 4 corners as a 5-point polygon (last point is the first point
    // repeated).  These are printed as lat/lon pairs with no separator other
    // than that each value always contain a leading +/- sign.  Lat values get
    // 2 leading digits and lon values get 3.  Both get 8 decimal digits.

    // BASE_WIDTH = sign + at least 2 leading digits + decimal point +
    // decimal digits
    static const size_t NUM_TRAILING_DIGITS = 8;
    static const size_t BASE_WIDTH = 1 + 2 + 1 + NUM_TRAILING_DIGITS;

    std::ostringstream ostr;
    ostr.fill('0');

    // This forces the leading 0's to the right of the +/- sign
    ostr.setf(std::ios::internal, std::ios::adjustfield);

    ostr << std::showpos << std::fixed
         << std::setprecision(NUM_TRAILING_DIGITS);

    for (size_t ii = 0; ii <= six::LatLonCorners::NUM_CORNERS; ++ii)
    {
        const six::LatLon& corner =
                corners.getCorner(ii % six::LatLonCorners::NUM_CORNERS);

        ostr << std::setw(BASE_WIDTH) << corner.getLat()
             << std::setw(BASE_WIDTH + 1) << corner.getLon();
    }

    return ostr.str();
}

void six::loadPluginDir(const std::string& pluginDir)
{
    nitf::PluginRegistry::loadDir(pluginDir);
}

void six::loadXmlDataContentHandler()
{
    if (!nitf::PluginRegistry::treHandlerExists("XML_DATA_CONTENT"))
    {
        nitf::PluginRegistry::registerTREHandler(XML_DATA_CONTENT_init,
                                                 XML_DATA_CONTENT_handler);
    }
}

std::auto_ptr<Data> six::parseData(const XMLControlRegistry& xmlReg,
                                   ::io::InputStream& xmlStream,
                                   DataType dataType,
                                   const std::vector<std::string>& schemaPaths,
                                   logging::Logger& log)
{
    xml::lite::MinidomParser xmlParser;
    xmlParser.preserveCharacterData(true);
    try
    {
        xmlParser.parse(xmlStream);
    }
    catch(const except::Throwable& ex)
    {
        throw except::Exception(ex, Ctxt("Invalid XML data"));
    }
    xml::lite::Document* doc = xmlParser.getDocument();

    //! Check the root localName for the XML type
    std::string xmlType = doc->getRootElement()->getLocalName();
    DataType xmlDataType;
    if (str::startsWith(xmlType, "SICD"))
        xmlDataType = DataType::COMPLEX;
    else if (str::startsWith(xmlType, "SIDD"))
        xmlDataType = DataType::DERIVED;
    else
        throw except::Exception(Ctxt("Unexpected XML type"));

    //! Only SIDDs can have mismatched types
    if (dataType == DataType::COMPLEX && dataType != xmlDataType)
    {
        throw except::Exception(Ctxt("Unexpected SIDD DES in SICD"));
    }

    //! Create the correct type of XMLControl
    const std::auto_ptr<XMLControl>
        xmlControl(xmlReg.newXMLControl(xmlDataType, &log));

    return std::auto_ptr<Data>(xmlControl->fromXML(doc, schemaPaths));
}

std::auto_ptr<Data> six::parseDataFromFile(const XMLControlRegistry& xmlReg,
    const std::string& pathname,
    DataType dataType,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log)
{
    io::FileInputStream inStream(pathname);
    return parseData(xmlReg, inStream, dataType, schemaPaths, log);
}

std::auto_ptr<Data> six::parseDataFromString(const XMLControlRegistry& xmlReg,
    const std::string& xmlStr,
    DataType dataType,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log)
{
    io::StringStream inStream;
    inStream.write(xmlStr);
    return parseData(xmlReg, inStream, dataType, schemaPaths, log);
}

std::string six::findSchemaPath(const std::string& progname)
{
    sys::OS os;
    std::string currentDir = os.getCurrentExecutable(progname);

    // Arbitrary depth to prevent infinite loop in case
    // of weird project structure
    const static size_t MAX_DEPTH = 5;
    size_t levelsTraversed = 0;

    std::string schemaPath;
    while (levelsTraversed < MAX_DEPTH)
    {
        currentDir = sys::Path::absolutePath(
                sys::Path::joinPaths(currentDir, ".."));
        const std::string confDir = sys::Path::joinPaths(currentDir, "conf");
        if (os.exists(confDir))
        {
            schemaPath = sys::Path(confDir).join("schema").join("six");
            break;
        }
        ++levelsTraversed;
    }

    // If we got lost, this will be empty
    return schemaPath;
}

void six::getErrors(const ErrorStatistics* errorStats,
                    const types::RgAz<double>& /*sampleSpacing*/,
                    scene::Errors& errors)
{
    errors.clear();

    if (errorStats)
    {
        const six::Components* const components(errorStats->components.get());

        if (components)
        {
            double rangeBias;
            if (components->radarSensor.get())
            {
                const RadarSensor& radarSensor(*components->radarSensor);

                if (!six::Init::isUndefined(radarSensor.rangeBiasDecorr))
                {
                    errors.mRangeCorrCoefZero =
                            radarSensor.rangeBiasDecorr.corrCoefZero;
                    errors.mRangeDecorrRate =
                            radarSensor.rangeBiasDecorr.decorrRate;
                }

                rangeBias = radarSensor.rangeBias;
            }
            else
            {
                rangeBias = 0.0;
            }

            if (components->posVelError.get())
            {
                const PosVelError& posVelError(*components->posVelError);
                errors.mFrameType = posVelError.frame;

                getSensorCovariance(posVelError,
                                    rangeBias,
                                    errors.mSensorErrorCovar);

                if (!six::Init::isUndefined(posVelError.positionDecorr))
                {
                    errors.mPositionCorrCoefZero =
                            posVelError.positionDecorr.corrCoefZero;
                    errors.mPositionDecorrRate =
                            posVelError.positionDecorr.decorrRate;
                }
            }

            if (components->ionoError.get())
            {
                const six::IonoError& ionoError(*components->ionoError);
                errors.mIonoErrorCovar(0, 0) =
                        math::square(ionoError.ionoRangeVertical);
                errors.mIonoErrorCovar(1, 1) =
                        math::square(ionoError.ionoRangeRateVertical);
                errors.mIonoErrorCovar(0, 1) =
                        errors.mIonoErrorCovar(1, 0) =
                                ionoError.ionoRangeVertical *
                                ionoError.ionoRangeRateVertical *
                                ionoError.ionoRgRgRateCC;
            }

            if (components->tropoError.get())
            {
                errors.mTropoErrorCovar(0, 0) =
                        math::square(components->tropoError->tropoRangeVertical);
            }
        }

        if (errorStats->compositeSCP.get() &&
            errorStats->compositeSCP->scpType == CompositeSCP::RG_AZ)
        {
            const types::RgAz<double> composite(
                    errorStats->compositeSCP->xErr,
                    errorStats->compositeSCP->yErr);
            const double corr = errorStats->compositeSCP->xyErr;

            errors.mUnmodeledErrorCovar(0, 0) = math::square(composite.rg);
            errors.mUnmodeledErrorCovar(1, 1) = math::square(composite.az);
            errors.mUnmodeledErrorCovar(0, 1) =
                    errors.mUnmodeledErrorCovar(1, 0) =
                            corr * (composite.rg * composite.az);
        }
    }
}
