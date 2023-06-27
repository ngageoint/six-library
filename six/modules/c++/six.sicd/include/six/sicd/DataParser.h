/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef SIX_six_sicd_DataParser_h_INCLUDED_
#define SIX_six_sicd_DataParser_h_INCLUDED_

#include <memory>
#include <vector>
#include <std/filesystem>
#include <std/string>

#include <io/InputStream.h>
#include <logging/NullLogger.h>

#include "six/Utilities.h"
#include "six/XMLControlFactory.h"
#include "six/sicd/ComplexData.h"

namespace six
{
namespace sicd
{
class DataParser final
{
    six::DataParser mDataParser;
    XMLControlRegistry mXmlRegistry;

public:

    /* Parses the XML and converts it into a ComplexData object.
    * Throws if the underlying type is not complex.
    *
    * \param xmlStream Input stream containing XML
    * \param schemaPaths Schema path(s)
    * \param log Logger
    */
    DataParser(const std::vector<std::filesystem::path>* pSchemaPaths = nullptr, logging::Logger* pLog = nullptr);
    DataParser(const std::vector<std::filesystem::path>& schemaPaths, logging::Logger* pLog = nullptr)
        : DataParser(&schemaPaths, pLog) { }
    DataParser(logging::Logger& log, const std::vector<std::filesystem::path>* pSchemaPaths = nullptr)
        : DataParser(pSchemaPaths, &log) { }
    DataParser(const std::vector<std::filesystem::path>& schemaPaths, logging::Logger& log)
        : DataParser(schemaPaths, &log) { }

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

    /* Parses the XML in 'xmlStream'.
    *
    * \param xmlStream Input stream containing XML
    *
    * \return Data representation of 'xmlStr'
    */
    std::unique_ptr<ComplexData> fromXML(::io::InputStream& xmlStream) const;

    /*
     * Parses the XML in 'pathname'.
     *
     * \param pathname File containing plain text XML (not a NITF)
     *
     * \return Data representation of the contents of 'pathname'
     */
    std::unique_ptr<ComplexData> fromXML(const std::filesystem::path&) const;

    /*
     * Parses the XML in 'xmlStr'.
     *
     * \param xmlStr XML document as a string
     *
     * \return Data representation of 'xmlStr'
     */
    std::unique_ptr<ComplexData> fromXML(const std::u8string& xmlStr) const;

    /*
     * Converts 'data' back into a formatted XML string
     *
     * \param data Representation of SICD data
     *
     * \return XML string representation of 'data'
     */
    std::u8string toXML(const ComplexData&) const;

};
}
}
#endif // SIX_six_sicd_DataParser_h_INCLUDED_
