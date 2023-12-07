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

#include <iomanip>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <logging/NullLogger.h>
#include <math/Utilities.h>
#include <nitf/PluginRegistry.hpp>
#include <sys/FileFinder.h>
#include <sys/Path.h>

#include "six/Init.h"
#include "six/Utilities.h"
#include "six/XMLControl.h"
#include "six/Data.h"
#include <six/XmlLite.h>

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
        assign(sensorCovar, 1, 4, error.p2 * error.v2 * corrCoefs.p2v2);
        assign(sensorCovar, 1, 5, error.p2 * error.v3 * corrCoefs.p2v3);
        assign(sensorCovar, 2, 3, error.p3 * error.v1 * corrCoefs.p3v1);
        assign(sensorCovar, 2, 4, error.p3 * error.v2 * corrCoefs.p3v2);
        assign(sensorCovar, 2, 5, error.p3 * error.v3 * corrCoefs.p3v3);
    }
}
}

using namespace six;

template <>
BooleanType six::toType<BooleanType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    str::lower(type);
    if (type == "true" || type == "1" || type == "yes")
        return BooleanType::IS_TRUE;
    if (type == "false" || type == "0" || type == "no")
        return BooleanType::IS_FALSE;
    return BooleanType::NOT_SET;
}

template <>
std::string six::toString<float>(const float& value)
{
    if (six::Init::isUndefined(value))
    {
        throw six::UninitializedValueException(
                Ctxt("Attempted use of uninitialized float value"));
    }

    std::ostringstream os;
    constexpr size_t precision = std::numeric_limits<float>::max_digits10;
    os << std::uppercase << std::scientific << std::setprecision(precision)
       << value;
    std::string strValue = os.str();

    // remove any + in scientific notation to meet SICD XML standard
    const size_t plusPos = strValue.find("+");
    if (plusPos != std::string::npos)
    {
        strValue.erase(plusPos, 1);
    }
    return strValue;
}

template <>
std::string six::toString<double>(const double& value)
{
    if (six::Init::isUndefined(value))
    {
        throw six::UninitializedValueException(
                Ctxt("Attempted use of uninitialized double value"));
    }

    std::ostringstream os;
    constexpr size_t precision = std::numeric_limits<double>::max_digits10;
    os << std::uppercase << std::scientific << std::setprecision(precision)
       << value;
    std::string strValue = os.str();

    // remove any + in scientific notation to meet SICD XML standard
    const size_t plusPos = strValue.find("+");
    if (plusPos != std::string::npos)
    {
        strValue.erase(plusPos, 1);
    }
    return strValue;
}

template <>
std::string six::toString<BooleanType>(const BooleanType& value)
{
    return str::toString<bool>(value == BooleanType::IS_TRUE);
}

template <>
std::string six::toString(const six::Vector3& v)
{
    std::ostringstream os;
    os << "(X:" << v[0] << " Y:" << v[1] << " Z:" << v[2] << ")";
    return os.str();
}

template <>
std::string six::toString(const six::PolyXYZ& p)
{
    std::ostringstream os;
    for (size_t i = 0; i < p.size(); i++)
    {
        os << toString(p[i]) << "*y^" << i << ((i != p.size()) ? " + " : "")
           << "\n";
    }
    return os.str();
}

template <>
DateTime six::toType<DateTime>(const std::string& dateTime)
{
    try
    {
        // Try an XML format type
        // NOTE: There may or may not be a Z on the end of this
        //       The spec specifies that there "should" be to indicate UTC,
        //       but Timeline/CollectStart seems to frequently be produced
        //       without it
        if (dateTime.length() >= 19)
        {
            const bool trimStr = (dateTime[dateTime.length() - 1] == 'Z');

            return DateTime(trimStr ? dateTime.substr(0, dateTime.length() - 1)
                                    : dateTime,
                            "%Y-%m-%dT%H:%M:%S");
        }
    }
    catch (const except::Exception&)
    {
    }

    try
    {
        // Try a NITF 2.1 format
        if (dateTime.length() >= 14)
            return DateTime(dateTime.substr(0, 14), "%Y%m%d%H%M%S");
    }
    catch (const except::Exception&)
    {
    }

    try
    {
        // Try a NITF 2.1 format - just date
        if (dateTime.length() >= 10)
            return DateTime(dateTime.substr(0, 10), "%Y-%m-%d");
    }
    catch (const except::Exception&)
    {
    }

    try
    {
        // Try a simple 8-char format
        if (dateTime.length() == 8)
            return DateTime(dateTime.substr(0, 8), "%Y-%m-%d");
    }
    catch (const except::Exception&)
    {
    }

    // should we really do this?
    return DateTime();
}

template <>
std::string six::toString(const DateTime& dateTime)
{
    char date[256];
    date[255] = 0;
    dateTime.format("%Y-%m-%dT%H:%M:%.6SZ", date, 255);
    std::string strDate(date);
    return strDate;
}

template<typename T>
inline T toType_(std::string s, const except::Exception& ex)
{
    str::trim(s);
    const auto result = T::toType(s, std::nothrow);
    auto retval = nitf::details::value(result, ex); // throw our exception rather than a default one
    if (retval == T::NOT_SET)
    {
        throw ex;
    }
    return retval;
}
template<typename T>
inline T toType_(std::string s)
{
    str::trim(s);
    const auto result = T::toType(s, std::nothrow);
    if (result.has_value())
    {
        return *result;
    }
    return T::NOT_SET;
}

template<typename T>
inline std::string toString_(const T& t, const except::Exception& ex)
{
    if (t == T::NOT_SET)
    {
        throw ex;
    }
    const auto result = t.toString(std::nothrow);
    return nitf::details::value(result, ex); // throw our exception rather than a default one
}

template <>
std::string six::toString(const RadarModeType& type)
{
    auto result = toString_(type, except::Exception(Ctxt("Radar mode not set!")));
    static const auto strDYNAMIC_STRIPMAP = RadarModeType(RadarModeType::DYNAMIC_STRIPMAP).toString();
    if (result == strDYNAMIC_STRIPMAP)
    {
        return "DYNAMIC STRIPMAP"; // no "_"
    }
    return result;
}
template <>
RadarModeType six::toType<RadarModeType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "DYNAMIC STRIPMAP") // no "_"
        return RadarModeType::DYNAMIC_STRIPMAP;

    static const auto strDYNAMIC_STRIPMAP = RadarModeType(RadarModeType::DYNAMIC_STRIPMAP).toString();
    if (type == strDYNAMIC_STRIPMAP)
    {
        return RadarModeType::NOT_SET; // "DYNAMIC_STRIPMAP" (with '_') doesn't convert
    }
    return toType_<RadarModeType>(type);
}

template <>
std::string six::toString(const DataType& type)
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

template <>
PixelType six::toType<PixelType>(const std::string& s)
{
    auto p = PixelType::toType(s);
    if (p == PixelType::NOT_SET)
        throw except::Exception(Ctxt(str::Format("Type not understood [%s]", s)));
    return p;
}

template <>
std::string six::toString(const PixelType& type)
{
    if (type == PixelType::NOT_SET)
    {
        throw except::Exception(Ctxt("Unsupported pixel type"));
    }
    return type.toString();
}

// There's a lot of boiler-plate code that can be hidden behind a few macros
#define SIX_define_six_toType_(T) template <> T six::toType<T>(const std::string& s) { return toType_<T>(s); }
#define SIX_define_six_toTypeEx_(T, message) template <> T six::toType<T>(const std::string& s) { \
    return toType_<T>(s,  except::Exception(Ctxt(message + s + "'"))); }
#define SIX_define_six_toString_(T, message) template <> std::string six::toString(const T& s) { \
    return toString_(s, except::Exception(Ctxt(message))); }

#define SIX_define_six_toType_toString_(T, message) SIX_define_six_toType_(T); SIX_define_six_toString_(T, message)
#define SIX_define_six_toTypeEx_toString_(T, toTypeMessage, toStringMessage) \
    SIX_define_six_toTypeEx_(T, toTypeMessage); SIX_define_six_toString_(T, toStringMessage)
#define SIX_define_six_toType_toString(T) SIX_define_six_toType_toString_(T, "Unsupported " #T)
#define SIX_define_six_toTypeEx_toString(T) SIX_define_six_toTypeEx_toString_(T, "Unsupported " #T, "Unsupported " #T)

SIX_define_six_toType_toString(MagnificationMethod);
SIX_define_six_toType_toString(DecimationMethod);

template <>
EarthModelType six::toType<EarthModelType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "WGS_84")
        return EarthModelType::WGS84;
    return EarthModelType::NOT_SET;
}
template <>
std::string six::toString(const EarthModelType& t)
{
    switch (t)
    {
    case EarthModelType::WGS84:
        return "WGS_84";
    default:
        throw except::Exception(Ctxt("Unsupported earth model"));
    }
}

SIX_define_six_toTypeEx_toString(OrientationType); // "Unsupported orientation type '", "Unsupported orientation"
SIX_define_six_toTypeEx_toString(DemodType); // "Unsupported demod type '", "Unsupported demod type"
SIX_define_six_toTypeEx_toString(ImageFormationType); // "Unsupported image formation type '", "Unsupported image formation type"
SIX_define_six_toTypeEx_toString(SlowTimeBeamCompensationType); // "Unsupported slow time beam compensation type  '", "Unsupported slow time beam compensation type"
SIX_define_six_toTypeEx_toString(ImageBeamCompensationType); // "Unsupported image beam compensation type  '", "Unsupported image beam compensation type"
SIX_define_six_toTypeEx_toString(AutofocusType); // "Unsupported autofocus type  '", "Unsupported autofocus type"
SIX_define_six_toTypeEx_toString(RMAlgoType); // "Unsupported RM algorithm type  '", "Unsupported RM algorithm type"

template <>
SideOfTrackType six::toType<SideOfTrackType>(const std::string& s)
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
template <>
std::string six::toString(const SideOfTrackType& t)
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

SIX_define_six_toTypeEx_toString(ComplexImagePlaneType); // "Unsupported complex image plane   '", "Unsupported complex image plane"
SIX_define_six_toTypeEx_toString(ComplexImageGridType); // "Unsupported complex image grid '", "Unsupported complex image grid"

template <>
FFTSign six::toType<FFTSign>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "-1")
    {
        return FFTSign::NEG;
    }
    if (type == "+1" || type == "1")
    {
        // NOTE: The SICD Volume 1 spec says only "+1" and "-1" are allowed,
        //       and while the schema uses those same strings, it sets the
        //       type to xs:int so that "1" will pass schema validation.  Some
        //       producers do use "1" so for simplicity just support it here.
        return FFTSign::POS;
    }
    if (type == "0")
    {
        return FFTSign::NOT_SET;
    }
    throw except::Exception(Ctxt("Unsupported fft sign '" + s + "'"));
}

template <>
std::string six::toString(const FFTSign& value)
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

template <>
AppliedType six::toType<AppliedType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    // TODO: Needs correction -- added alternate value to match schema
    // from 03/17/2009.
    if (type == "APPLIED" || type == "APPILED")
        return AppliedType::IS_TRUE;
    if (type == "NOT_APPLIED")
        return AppliedType::IS_FALSE;
    throw except::Exception(Ctxt("Unsupported applied type '" + s + "'"));
}

template <>
std::string six::toString(const AppliedType& value)
{
    switch (value)
    {
    case AppliedType::IS_TRUE:
        // TODO: Needs correction -- support alternate value to match
        // schema from 03/17/2009.
        //        return "APPILED";
        return "APPLIED";  // fixed in 2010-07-08 version of schema
    case AppliedType::IS_FALSE:
        return "NOT_APPLIED";
    default:
        throw except::Exception(Ctxt("Unsupported applied type"));
    }
}

SIX_define_six_toTypeEx_toString(CollectType); // "Unsupported collect type '", "Unsupported collect type"

template <>
std::string six::toString(const six::FrameType& value)
{
    switch (value.mValue)
    {
    case FrameType::ECF:
        return "ECF";
    case FrameType::RIC_ECF:
        return "RIC_ECF";
    case FrameType::RIC_ECI:
        return "RIC_ECI";
    case FrameType::NOT_SET:
    default:
        throw except::Exception(Ctxt("Unsupported frame type"));
    }
}
template <>
six::FrameType six::toType<six::FrameType>(const std::string& s)
{
    std::string type(s);
    str::trim(type);
    if (type == "ECF")
        return FrameType::ECF;
    if (type == "RIC_ECF")
        return FrameType::RIC_ECF;
    if (type == "RIC_ECI")
        return FrameType::RIC_ECI;
    throw except::Exception(Ctxt("Unsupported frame type '" + s + "'"));
}

template <>
std::string six::toString(const six::LatLonCorners& corners)
{
    // Print the 4 corners as a 5-point polygon (last point is the first point
    // repeated).  These are printed as lat/lon pairs with no separator other
    // than that each value always contain a leading +/- sign.  Lat values get
    // 2 leading digits and lon values get 3.  Both get 8 decimal digits.

    // BASE_WIDTH = sign + at least 2 leading digits + decimal point +
    // decimal digits
    constexpr size_t NUM_TRAILING_DIGITS = 8;
    constexpr size_t BASE_WIDTH = 1 + 2 + 1 + NUM_TRAILING_DIGITS;

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

void six::loadXmlDataContentHandler(FILE* log)
{
    // This can generate output from implicitConstruct() complaining about NITF_PLUGIN_PATH
    // not being set; often the warning is benign and is just confusing.  Provide a way to turn
    // it off (FILE* log = NULL) without upsetting existing code.
    if (!nitf::PluginRegistry::treHandlerExists("XML_DATA_CONTENT", log))
    {
        nitf::PluginRegistry::registerTREHandler(XML_DATA_CONTENT_init,
                                                 XML_DATA_CONTENT_handler);
    }
}
void six::loadXmlDataContentHandler()
{
    loadXmlDataContentHandler(stderr); // existing/legacy behavior
}

std::unique_ptr<Data> six::parseData(const XMLControlRegistry& xmlReg,
    ::io::InputStream& xmlStream,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log)
{
    return parseData(xmlReg, xmlStream, DataType::NOT_SET, schemaPaths, log);
}
std::unique_ptr<Data> six::parseData(const XMLControlRegistry& xmlReg,
    ::io::InputStream& xmlStream,
    const std::vector<std::filesystem::path>* pSchemaPaths,
    logging::Logger& log)
{
    DataParser dataParser(pSchemaPaths, &log);
    dataParser.preserveCharacterData(false); // existing behavior
    return dataParser.fromXML(xmlStream, xmlReg, DataType::NOT_SET);
}

inline std::unique_ptr<Data> fromXML_(const xml::lite::Document& doc, XMLControl& xmlControl, const std::vector<std::string>& schemaPaths)
{
    return std::unique_ptr<Data>(xmlControl.fromXML(&doc, schemaPaths));
}
inline std::unique_ptr<Data> fromXML_(const xml::lite::Document& doc, XMLControl& xmlControl, const std::vector<std::filesystem::path>* pSchemaPaths)
{
    return xmlControl.fromXML(doc, pSchemaPaths);
}

static auto parseInputStream(::io::InputStream& xmlStream, bool preserveCharacterData = false)
{
    six::MinidomParser xmlParser;
    xmlParser.preserveCharacterData(preserveCharacterData);
    try
    {
        xmlParser.parse(xmlStream);
    }
    catch (const except::Throwable& ex)
    {
        throw except::Exception(ex, Ctxt("Invalid XML data"));
    }
    return xmlParser;
}

template<typename TReturn, typename TSchemaPaths>
TReturn six_parseData(const XMLControlRegistry& xmlReg,
                                   six::MinidomParser& xmlParser,
                                   DataType dataType,
                                   const TSchemaPaths& schemaPaths,
                                   logging::Logger& log)
{
    const auto& doc = getDocument(xmlParser);

    //! Check the root localName for the XML type
    std::string xmlType = doc.getRootElement()->getLocalName();
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
    const std::unique_ptr<XMLControl> xmlControl(xmlReg.newXMLControl(xmlDataType, &log));
    return fromXML_(doc, *xmlControl, schemaPaths);
}
std::unique_ptr<Data> six::parseData(const XMLControlRegistry& xmlReg,
    ::io::InputStream& xmlStream,
    DataType dataType,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log)
{
    auto xmlParser = parseInputStream(xmlStream);
    return six_parseData<std::unique_ptr<Data>>(xmlReg, xmlParser, dataType, schemaPaths, log);
}
std::unique_ptr<Data> six::parseData(const XMLControlRegistry& xmlReg,
    ::io::InputStream& xmlStream,
    DataType dataType,
    const std::vector<std::filesystem::path>* pSchemaPaths,
    logging::Logger& log)
{
    DataParser dataParser(pSchemaPaths, &log);
    dataParser.preserveCharacterData(false); // existing behavior
    return dataParser.fromXML(xmlStream, xmlReg, dataType);
}

std::unique_ptr<Data>  six::parseDataFromFile(const XMLControlRegistry& xmlReg,
    const std::string& pathname,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log)
{
    return parseDataFromFile(xmlReg, pathname, DataType::NOT_SET, schemaPaths, log);
}
std::unique_ptr<Data> six::parseDataFromFile(
        const XMLControlRegistry& xmlReg,
        const std::string& pathname,
        DataType dataType,
        const std::vector<std::string>& schemaPaths,
        logging::Logger& log)
{
    io::FileInputStream inStream(pathname);
    return parseData(xmlReg, inStream, dataType, schemaPaths, log);
}

std::unique_ptr<Data> six::parseDataFromString(const XMLControlRegistry& xmlReg,
    const std::u8string& xmlStr,
    const std::vector<std::filesystem::path>* pSchemaPaths,
    logging::Logger* pLogger)
{
    DataParser dataParser(pSchemaPaths, pLogger);
    dataParser.preserveCharacterData(false); // existing behavior
    return dataParser.fromXML(xmlStr, xmlReg, DataType::NOT_SET);
}
std::unique_ptr<Data> six::parseDataFromString(const XMLControlRegistry& xmlReg,
    const std::string& xmlStr,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log)
{
    return parseDataFromString(xmlReg, xmlStr, DataType::NOT_SET, schemaPaths, log);
}

std::unique_ptr<Data> six::parseDataFromString(
        const XMLControlRegistry& xmlReg,
        const std::u8string& xmlStr,
        DataType dataType,
        const std::vector<std::filesystem::path>* pSchemaPaths,
        logging::Logger* pLogger)
{
    DataParser dataParser(pSchemaPaths, pLogger);
    dataParser.preserveCharacterData(false); // existing behavior
    return dataParser.fromXML(xmlStr, xmlReg, dataType);
}
std::unique_ptr<Data> six::parseDataFromString(const XMLControlRegistry& xmlReg,
    const std::string& xmlStr,
    DataType dataType,
    const std::vector<std::string>& schemaPaths_,
    logging::Logger& log)
{
    const auto schemaPaths = sys::convertPaths(schemaPaths_);
    auto result = parseDataFromString(xmlReg, str::u8FromNative(xmlStr), dataType, &schemaPaths, &log);
    return std::unique_ptr<Data>(result.release());
}

std::string six::findSchemaPath(const std::string& progname)
{
    sys::OS os;
    std::string currentDir = os.getCurrentExecutable(progname);

    // Arbitrary depth to prevent infinite loop in case
    // of weird project structure
    constexpr size_t MAX_DEPTH = 5;
    size_t levelsTraversed = 0;

    std::string schemaPath;
    while (levelsTraversed < MAX_DEPTH)
    {
        currentDir =
                sys::Path::absolutePath(sys::Path::joinPaths(currentDir, ".."));
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
            double rangeBias = 0.0;
            if (components->radarSensor)
            {
                const RadarSensor& radarSensor(*components->radarSensor);

                if (has_value(radarSensor.rangeBiasDecorr))
                {
                    const auto& rangeBiasDecorr = value(radarSensor.rangeBiasDecorr);
                    errors.mRangeCorrCoefZero = rangeBiasDecorr.corrCoefZero;
                    errors.mRangeDecorrRate = rangeBiasDecorr.decorrRate;
                }

                rangeBias = value(radarSensor.rangeBias);
            }

            if (components->posVelError)
            {
                const PosVelError& posVelError(*components->posVelError);
                errors.mFrameType = posVelError.frame;

                getSensorCovariance(posVelError,
                                    rangeBias,
                                    errors.mSensorErrorCovar);

                if (has_value(posVelError.positionDecorr))
                {
                    const auto& positionDecorr = value(posVelError.positionDecorr);
                    errors.mPositionCorrCoefZero = positionDecorr.corrCoefZero;
                    errors.mPositionDecorrRate = positionDecorr.decorrRate;
                }
            }

            if (components->ionoError)
            {
                const six::IonoError& ionoError(*components->ionoError);
                if (has_value(ionoError.ionoRangeVertical))
                {
                    errors.mIonoErrorCovar(0, 0) =
                        math::square(value(ionoError.ionoRangeVertical));
                }
                if (has_value(ionoError.ionoRangeRateVertical))
                {
                    errors.mIonoErrorCovar(1, 1) =
                        math::square(value(ionoError.ionoRangeRateVertical));
                }
                if (has_value(ionoError.ionoRangeVertical) && has_value(ionoError.ionoRangeRateVertical))
                {
                    errors.mIonoErrorCovar(0, 1) = errors.mIonoErrorCovar(1, 0) =
                        value(ionoError.ionoRangeVertical) *
                        value(ionoError.ionoRangeRateVertical) *
                        value(ionoError.ionoRgRgRateCC);
                }
            }

            if (components->tropoError)
            {
                errors.mTropoErrorCovar(0, 0) = math::square(
                        value(components->tropoError->tropoRangeVertical));
            }
        }

        if (const auto compositeSCP = errorStats->compositeSCP.get())
        {
            if (compositeSCP->scpType == CompositeSCP::RG_AZ)
            {
                const types::RgAz<double> composite(compositeSCP->xErr, compositeSCP->yErr);
                const double corr = compositeSCP->xyErr;

                auto& unmodeledErrorCovar = errors.mUnmodeledErrorCovar;
                unmodeledErrorCovar(0, 0) = math::square(composite.rg);
                unmodeledErrorCovar(1, 1) = math::square(composite.az);
                unmodeledErrorCovar(0, 1) = unmodeledErrorCovar(1, 0) = corr * (composite.rg * composite.az);
            }
        }
        else if (has_value(errorStats->unmodeled))
        {
            const auto& unmodeled = value(errorStats->unmodeled);

            auto& unmodeledErrorCovar = errors.mUnmodeledErrorCovar;
            auto&& Xrow = unmodeled.Xrow;
            auto&& Ycol = unmodeled.Ycol;
            auto&& XrowYcol = unmodeled.XrowYcol;

            // From Bill: Here is the mapping from the UnmodeledError to the 2x2 covariance matrix:
            //    [0][0] = Xrow; [1][1] = Ycol; 
            //    [1][0] = [0][1] = XrowYcol * Xrow * Ycol
            unmodeledErrorCovar(0, 0) = Xrow;
            unmodeledErrorCovar(1, 1) = Ycol;
            unmodeledErrorCovar(0, 1) = unmodeledErrorCovar(1, 0) = XrowYcol * Xrow * Ycol;
        }
    }
}

static bool is_six_root(const std::filesystem::path& dir)
{
    const auto six = dir / "six";
    const auto six_sln = dir / "six.sln";
    return is_directory(six) && is_regular_file(six_sln);
}
std::filesystem::path six::testing::findRootDir(const std::filesystem::path& dir)
{
    // <dir>/six.sln
    if (is_six_root(dir))
    {
        return dir;
    }

    // <dir>/externals/six/six.sln
    const auto externals = dir / "externals" / "six"; // not "six-library"
    if (is_six_root(externals))
    {
        return externals;
    }
    
    const auto parent = dir.parent_path();
    return findRootDir(parent);
}

std::filesystem::path six::testing::buildRootDir(const std::filesystem::path& argv0)
{
    auto platform = sys::Platform; // "conditional expression is constant"
    if (platform == sys::PlatformType::Windows)
    {
        // On Windows ... in Visual Studio or stand-alone?
        if ((argv0.filename() == "Test.exe") || (argv0.filename() == "testhost.exe")) // Visual Studio
        {
            const auto cwd = std::filesystem::current_path();
            const auto root_dir = cwd.parent_path().parent_path();
            return root_dir;
        }
    }

    // Linux or stand-alone
    return six::testing::findRootDir(argv0);
}

std::filesystem::path six::testing::getNitroPath(const std::filesystem::path& filename)
{
    static const auto unittests = std::filesystem::path("modules") / "c++" / "nitf" / "unittests";
    return sys::test::findGITModuleFile("nitro", unittests, filename);
}

std::filesystem::path six::testing::getModuleFile(const std::filesystem::path& modulePath, const  std::filesystem::path& filename)
{
    return sys::test::findGITModuleFile("six", modulePath, filename);
}

std::filesystem::path six::testing::getNitfPath(const std::filesystem::path& filename)
{
    static const auto tests_nitf = std::filesystem::path("six") / "modules" / "c++" / "six" / "tests" / "nitf";
    return getModuleFile(tests_nitf, filename);
}

std::vector<std::filesystem::path> six::testing::getSchemaPaths()
{ 
    static const auto modulePath = std::filesystem::path("six") / "modules" / "c++" / "six.sicd" / "conf" / "schema";
    static const auto filename = getModuleFile(modulePath, "SICD_schema_V1.3.0.xsd");
    static const auto schemaPath = filename.parent_path();
    return std::vector<std::filesystem::path> { schemaPath };
}

std::filesystem::path six::testing::getSampleXmlPath(const std::filesystem::path& moduleName, const  std::filesystem::path& filename)
{
    const auto modulePath = std::filesystem::path("six") / "modules" / "c++" / moduleName;
    return getModuleFile(modulePath, filename);
}

six::DataParser::DataParser(const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLog)
    : mpSchemaPaths(pSchemaPaths),
    mLog(pLog == nullptr ? mNullLogger : *pLog)
{
}

std::unique_ptr<six::Data> six::DataParser::DataParser::fromXML(::io::InputStream& xmlStream,
    const XMLControlRegistry& xmlReg, DataType dataType) const
{
    auto xmlParser = parseInputStream(xmlStream, mPreserveCharacterData);
    return six_parseData<std::unique_ptr<Data>>(xmlReg, xmlParser, dataType, mpSchemaPaths, mLog);
}

std::unique_ptr<six::Data> six::DataParser::DataParser::fromXML(const std::filesystem::path& pathname, 
    const XMLControlRegistry& xmlReg, DataType dataType) const
{
    io::FileInputStream inStream(pathname.string());
    return fromXML(inStream, xmlReg, dataType);
}

std::unique_ptr<six::Data> six::DataParser::DataParser::fromXML(const std::u8string& xmlStr,
    const XMLControlRegistry& xmlReg, DataType dataType) const
{
    io::U8StringStream inStream;
    inStream.write(xmlStr);
    return fromXML(inStream, xmlReg, dataType);
}

std::u8string  six::DataParser::DataParser::toXML(const Data& data, const XMLControlRegistry& xmlReg) const
{
    return ::six::toValidXMLString(data, mpSchemaPaths, &mLog, &xmlReg);
}
std::u8string  six::DataParser::DataParser::toXML(const Data& data) const
{
    return toXML(data, mXmlRegistry);
}

void six::DataParser::DataParser::preserveCharacterData(bool preserve)
{
    mPreserveCharacterData = preserve;
}
