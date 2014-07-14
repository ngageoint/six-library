/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_UTILITIES_H__
#define __SIX_UTILITIES_H__

#include <sys/Conf.h>
#include <except/Exception.h>
#include "six/Types.h"
#include "six/Data.h"
#include "six/XMLControlFactory.h"
#include "logging/Logger.h"
#include "scene/SceneGeometry.h"
#include "six/ErrorStatistics.h"
#include "six/Init.h"
#include <import/io.h>
#include <import/xml/lite.h>
#include <import/str.h>
#include <vector>
#include <memory>

namespace six
{

/*!
 *  Remaps angles into [0:360]
 *
 *  Others ranges are explicitly unsupported because
 *  of remapping issues, and that angles outside those
 *  ranges generally indicate errors. For instance a
 *  Latitude value outside of the [-90:90] range would
 *  be considered an error.
 *
 *  Unsupported ranges are:
 *      [0:90]
 *      [-90:90]
 *      [-180:180]
 */
double remapZeroTo360(double degree);


// TODO eventually replace enum toString/toType methods below directly in the
// Enums.h header - it is currently being generated so we won't change it yet

template<typename T> std::string toString(const T& value)
{
    if (six::Init::isUndefined(value))
    {
        throw six::UninitializedValueException(
            Ctxt("Attempted use of uninitialized value"));
    }

    return str::toString<T>(value);
}

template<typename T> T toType(const std::string& s)
{
    return str::toType<T>(s);
}

template<> std::string toString(const float& value);
template<> std::string toString(const double& value);
template<> six::EarthModelType
        toType<six::EarthModelType>(const std::string& s);
template<> six::MagnificationMethod
        toType<six::MagnificationMethod>(const std::string& s);
template<> std::string toString(const six::MagnificationMethod& value);
template<> six::MagnificationMethod
        toType<six::MagnificationMethod>(const std::string& s);
template<> six::DecimationMethod
        toType<six::DecimationMethod>(const std::string& s);
template<> std::string toString(const six::DecimationMethod& value);

template<> six::PixelType toType<six::PixelType>(const std::string& s);
template<> std::string toString(const six::PixelType& value);

template<> six::RadarModeType toType<six::RadarModeType>(const std::string& s);
template<> std::string toString(const six::RadarModeType& value);

template<> six::DateTime toType<six::DateTime>(const std::string& s);
template<> std::string toString(const six::DateTime& value);

template<> std::string toString(const six::DataType& value);
template<> std::string toString(const six::EarthModelType& value);
template<> six::OrientationType
        toType<six::OrientationType>(const std::string& s);
template<> std::string toString(const six::OrientationType& value);
template<> six::PolarizationSequenceType
        toType<six::PolarizationSequenceType>(const std::string& s);
template<> std::string toString(const six::PolarizationSequenceType& value);
template<> six::PolarizationType
        toType<six::PolarizationType>(const std::string& s);
template<> std::string toString(const six::PolarizationType& value);

template<> std::string toString(const six::DualPolarizationType& value);
template<> six::DualPolarizationType
        toType<six::DualPolarizationType>(const std::string& s);
template<> six::DemodType toType<six::DemodType>(const std::string& s);
template<> std::string toString(const six::DemodType& value);

template<> std::string toString(const six::ImageFormationType& value);
template<> six::ImageFormationType
        toType<six::ImageFormationType>(const std::string& s);
template<> std::string toString(const six::SlowTimeBeamCompensationType& value);
template<> six::SlowTimeBeamCompensationType toType<
        six::SlowTimeBeamCompensationType>(const std::string& s);
template<> std::string toString(const six::ImageBeamCompensationType& value);
template<> six::ImageBeamCompensationType
toType<six::ImageBeamCompensationType>(const std::string& s);
template<> std::string toString(const six::AutofocusType& value);
template<> six::AutofocusType toType<six::AutofocusType>(const std::string& s);
template<> std::string toString(const six::RMAlgoType& value);
template<> six::RMAlgoType toType<six::RMAlgoType>(const std::string& s);
template<> std::string toString(const six::SideOfTrackType& value);

template<> six::BooleanType toType<six::BooleanType>(const std::string& s);
template<> std::string toString(const six::BooleanType& value);

template<> six::SideOfTrackType
        toType<six::SideOfTrackType>(const std::string& s);

template<> six::ComplexImagePlaneType
        toType<six::ComplexImagePlaneType>(const std::string& s);
template<> std::string toString(const six::ComplexImagePlaneType& value);

template<> six::ComplexImageGridType
        toType<six::ComplexImageGridType>(const std::string& s);
template<> std::string toString(const six::ComplexImageGridType& value);

template<> six::FFTSign toType<six::FFTSign>(const std::string& s);
template<> std::string toString(const six::FFTSign& value);

template<> six::AppliedType toType<six::AppliedType>(const std::string& s);
template<> std::string toString(const six::AppliedType& value);

template<> six::CollectType toType<six::CollectType>(const std::string& s);
template<> std::string toString(const six::CollectType& value);

template<> six::FrameType toType<six::FrameType>(const std::string& s);
template<> std::string toString(const six::FrameType& value);

template<> std::string toString(const six::LatLonCorners& corners);

// Load the TRE plugins in the given directory
void loadPluginDir(const std::string& pluginDir);

std::auto_ptr<Data> parseData(const XMLControlRegistry& xmlReg, 
                              ::io::InputStream& xmlStream, 
                              DataType dataType,
                              const std::vector<std::string>& schemaPaths,
                              logging::Logger& log);

}

#endif

