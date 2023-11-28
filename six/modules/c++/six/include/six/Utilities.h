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
#pragma once
#ifndef SIX_six_Utilities_h_INCLUDED_
#define SIX_six_Utilities_h_INCLUDED_

#include <vector>
#include <memory>
#include <std/span>
#include <std/cstddef>
#include <std/filesystem>
#include <std/string>

#include <import/io.h>
#include <import/xml/lite.h>
#include <import/str.h>
#include <except/Exception.h>
#include "logging/Logger.h"
#include <sys/Span.h>

#include <scene/sys_Conf.h>
#include "six/Types.h"
#include "six/Enums.h"
#include "six/XMLControlFactory.h"
#include "scene/SceneGeometry.h"
#include "six/ErrorStatistics.h"
#include "six/Init.h"
#include "six/Exports.h"
#include <scene/Utilities.h>


namespace six
{
    struct Data; // forward
    struct ErrorStatistics;

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
inline
double remapZeroTo360(double degree)
{
    return scene::Utilities::remapZeroTo360(degree);
}


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

#define SIX_toString_(T) template<> SIX_SIX_API std::string toString(const T&)

SIX_toString_(float);
SIX_toString_(double);
SIX_toString_(six::Vector3);
SIX_toString_(six::PolyXYZ);

#define SIX_toType_(T) template<> SIX_SIX_API T toType<T>(const std::string&)
#define SIX_toString_toType_(T) SIX_toString_(T); SIX_toType_(T)

SIX_toString_toType_(six::EarthModelType);
SIX_toString_toType_(six::MagnificationMethod);
SIX_toString_toType_(six::DecimationMethod);

SIX_toString_toType_(six::PixelType);

SIX_toString_toType_(six::RadarModeType);

SIX_toString_toType_(six::DateTime);

SIX_toString_(six::DataType);
SIX_toString_toType_(six::OrientationType);
SIX_toString_toType_(six::PolarizationSequenceType);
SIX_toString_toType_(six::PolarizationType);

SIX_toString_toType_(six::DualPolarizationType);
SIX_toString_toType_(six::DemodType);

SIX_toString_toType_(six::ImageFormationType);
SIX_toString_toType_(six::SlowTimeBeamCompensationType);
SIX_toString_toType_(six::ImageBeamCompensationType);
SIX_toString_toType_(six::AutofocusType);
SIX_toString_toType_(six::RMAlgoType);
SIX_toString_toType_(six::SideOfTrackType);

SIX_toString_toType_(six::BooleanType);

SIX_toString_toType_(six::ComplexImagePlaneType);

SIX_toString_toType_(six::ComplexImageGridType);

SIX_toString_toType_(six::FFTSign);

SIX_toString_toType_(six::AppliedType);

SIX_toString_toType_(six::CollectType);

SIX_toString_toType_(six::FrameType);

SIX_toString_(six::LatLonCorners);

#undef SIX_toString_
#undef SIX_toType_
#undef SIX_toString_toType_

// Load the TRE plugins in the given directory
// In most cases this is not needed as XML_DATA_CONTENT is linked statically
void loadPluginDir(const std::string& pluginDir);

/*
 * Used to ensure the PluginRegistry singleton has loaded the XML_DATA_CONTENT
 * handler.  This is used internally by NITFReadControl and NITFWriteControl
 * and should not need to be called directly.
 */
void loadXmlDataContentHandler();
void loadXmlDataContentHandler(FILE* log);


/*
 * Parses the XML in 'xmlStream' and converts it into a Data object
 *
 * \param xmlReg XML registry
 * \param xmlStream Input stream containing XML
 * \param dataType Complex vs. Derived.  If the resulting object is not the
 * expected type, throw.  To avoid this check, set to NOT_SET.
 * \param schemaPaths Schema path(s)
 * \param log Logger
 *
 * \return Data representation of 'xmlStream'
 */
SIX_SIX_API std::unique_ptr<Data> parseData(const XMLControlRegistry& xmlReg,
                              ::io::InputStream& xmlStream, 
                              DataType dataType,
                              const std::vector<std::string>& schemaPaths,
                              logging::Logger& log);
SIX_SIX_API std::unique_ptr<Data> parseData(const XMLControlRegistry& xmlReg,
    ::io::InputStream& xmlStream, DataType dataType,
    const std::vector<std::filesystem::path>*, logging::Logger&);

/*
 * Parses the XML in 'xmlStream' and converts it into a Data object.  Same as
 * above but doesn't require the data type to be known in advance.
 *
 * \param xmlReg XML registry
 * \param xmlStream Input stream containing XML
 * \param schemaPaths Schema path(s)
 * \param log Logger
 *
 * \return Data representation of 'xmlStream'
 */
SIX_SIX_API std::unique_ptr<Data> parseData(const XMLControlRegistry& xmlReg,
                              ::io::InputStream& xmlStream,
                              const std::vector<std::string>& schemaPaths,
                              logging::Logger& log);
SIX_SIX_API std::unique_ptr<Data> parseData(const XMLControlRegistry&, ::io::InputStream&,
    const std::vector<std::filesystem::path>*, logging::Logger&);

/*
 * Parses the XML in 'pathname' and converts it into a Data object.
 *
 * \param xmlReg XML registry
 * \param pathname File containing plain text XML (not a NITF)
 * \param dataType Complex vs. Derived.  If the resulting object is not the
 * expected type, throw.  To avoid this check, set to NOT_SET.
 * \param schemaPaths Schema path(s)
 * \param log Logger
 *
 * \return Data representation of the contents of 'pathname'
 */
SIX_SIX_API std::unique_ptr<Data> parseDataFromFile(const XMLControlRegistry& xmlReg,
    const std::string& pathname,
    DataType dataType,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log);

/*
 * Parses the XML in 'pathname' and converts it into a Data object.  Same as
 * above but doesn't require the data type to be known in advance.
 *
 * \param xmlReg XML registry
 * \param pathname File containing plain text XML (not a NITF)
 * \param schemaPaths Schema path(s)
 * \param log Logger
 *
 * \return Data representation of the contents of 'pathname'
 */
SIX_SIX_API std::unique_ptr<Data> parseDataFromFile(const XMLControlRegistry& xmlReg,
    const std::string& pathname,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log);

/*
 * Parses the XML in 'xmlStr' and converts it into a Data object.
 *
 * \param xmlReg XML registry
 * \param xmlStr XML document as a string
 * \param dataType Complex vs. Derived.  If the resulting object is not the
 * expected type, throw.  To avoid this check, set to NOT_SET.
 * \param schemaPaths Schema path(s)
 * \param log Logger
 *
 * \return Data representation of 'xmlStr'
 */
std::unique_ptr<Data> parseDataFromString(const XMLControlRegistry& xmlReg,
    const std::string& xmlStr,
    DataType dataType,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log);
std::unique_ptr<Data> parseDataFromString(const XMLControlRegistry& xmlReg,
    const std::u8string& xmlStr,
    DataType dataType,
    const std::vector<std::filesystem::path>* pSchemaPaths,
    logging::Logger* pLogger = nullptr);

/*
 * Parses the XML in 'xmlStr' and converts it into a Data object.  Same as
 * above but doesn't require the data type to be known in advance.
 *
 * \param xmlReg XML registry
 * \param xmlStr XML document as a string
 * \param schemaPaths Schema path(s)
 * \param log Logger
 *
 * \return Data representation of 'xmlStr'
 */
std::unique_ptr<Data> parseDataFromString(const XMLControlRegistry& xmlReg,
    const std::string& xmlStr,
    const std::vector<std::string>& schemaPaths,
    logging::Logger& log);
std::unique_ptr<Data> parseDataFromString(const XMLControlRegistry& xmlReg,
    const std::u8string& xmlStr,
    const std::vector<std::filesystem::path>* pSchemaPaths,
    logging::Logger* pLogger = nullptr);

SIX_SIX_API void getErrors(const ErrorStatistics* errorStats,
               const types::RgAz<double>& sampleSpacing,
               scene::Errors& errors);

/*
 * Try to find schema directory from given location.
 * Return empty string on failure
 *
 * \param progname name of executable
 * \return path to schema directory, or empty string
 */
std::string findSchemaPath(const std::string& progname);

class SIX_SIX_API DataParser final
{
    const std::vector<std::filesystem::path>* mpSchemaPaths = nullptr;
    logging::NullLogger mNullLogger;
    logging::Logger& mLog;
    XMLControlRegistry mXmlRegistry;

    // The default is `true` because:
    // * many (most?) other parts of SIX unconditionally set `preserveCharacterData(true)`.
    // * this is new code; if you're using it, you likely want different behavior than that
    //   of existing code; otherwise, why change?
    bool mPreserveCharacterData = true;

public:

    /* Parses the XML and converts it into a ComplexData object.
    * Throws if the underlying type is not complex.
    *
    * \param xmlStream Input stream containing XML
    * \param schemaPaths Schema path(s)
    * \param log Logger
    */
    DataParser(const std::vector<std::filesystem::path>* pSchemaPaths = nullptr, logging::Logger* pLog = nullptr);
    ~DataParser() = default;

    DataParser(const DataParser&) = delete;
    DataParser& operator=(const DataParser&) = delete;
    DataParser(DataParser&&) = delete;
    DataParser& operator=(DataParser&&) = delete;

    /*!
    * If set to true, whitespaces will be preserved in the parsed
    * character data. Otherwise, it will be trimmed.
    */
    void preserveCharacterData(bool preserve);

    template<typename TXMLControlCreator>
    void addCreator()
    {
        mXmlRegistry.addCreator<TXMLControlCreator>();
    }

    /* Parses the XML in 'xmlStream'.
    *
    * \param xmlStream Input stream containing XML
    *
    * \return Data representation of 'xmlStr'
    */
    std::unique_ptr<Data> fromXML(::io::InputStream& xmlStream, const XMLControlRegistry&, DataType) const;
    template<typename TData>
    std::unique_ptr<TData> fromXML(::io::InputStream& xmlStream) const
    {
        auto pData = fromXML(xmlStream, mXmlRegistry, DataType::NOT_SET);
        return std::unique_ptr<TData>(static_cast<TData*>(pData.release()));
    }

    /*
    * Parses the XML in 'pathname'.
    *
    * \param pathname File containing plain text XML (not a NITF)
    *
    * \return Data representation of the contents of 'pathname'
    */
    std::unique_ptr<Data> fromXML(const std::filesystem::path&, const XMLControlRegistry&, DataType) const;

    /*
    * Parses the XML in 'xmlStr'.
    *
    * \param xmlStr XML document as a string
    *
    * \return Data representation of 'xmlStr'
    */
    std::unique_ptr<Data> fromXML(const std::u8string& xmlStr, const XMLControlRegistry&, DataType) const;

    /*!
     *  Additionally performs schema validation --
     */
    std::u8string toXML(const Data&, const XMLControlRegistry&) const;
    std::u8string toXML(const Data&) const;
};

namespace testing
{
    SIX_SIX_API std::filesystem::path findRootDir(const std::filesystem::path& dir);
    SIX_SIX_API std::filesystem::path buildRootDir(const std::filesystem::path& argv0);

    SIX_SIX_API std::filesystem::path getNitfPath(const  std::filesystem::path& filename);
    SIX_SIX_API std::filesystem::path getNitroPath(const  std::filesystem::path& filename);

    SIX_SIX_API std::vector<std::filesystem::path> getSchemaPaths();
    SIX_SIX_API std::filesystem::path getModuleFile(const std::filesystem::path& modulePath, const  std::filesystem::path& filename);
    SIX_SIX_API std::filesystem::path getSampleXmlPath(const std::filesystem::path& module /*"six.sicd"*/, const  std::filesystem::path& filename);
}

}

#endif // SIX_six_Utilities_h_INCLUDED_
