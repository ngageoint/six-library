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

#include "six/sicd/ComplexData.h"

namespace six
{
namespace sicd
{
class DataParser final
{
    const std::vector<std::filesystem::path>* mpSchemaPaths = nullptr;
    logging::NullLogger mNullLogger;
    logging::Logger& mLog;
    bool mPreserveCharacterData = false;

public:

    /* Parses the XML and converts it into a ComplexData object.
    * Throws if the underlying type is not complex.
    *
    * \param xmlStream Input stream containing XML
    * \param schemaPaths Schema path(s)
    * \param log Logger
    */
    DataParser(const std::vector<std::filesystem::path>* pSchemaPaths = nullptr, logging::Logger* pLog = nullptr, bool preserveCharacterData = false);
    DataParser(const std::vector<std::filesystem::path>& schemaPaths, logging::Logger* pLog = nullptr, bool preserveCharacterData = false)
        : DataParser(&schemaPaths, pLog, preserveCharacterData) { }
    DataParser(const std::vector<std::filesystem::path>& schemaPaths, logging::Logger& log, bool preserveCharacterData = false)
        : DataParser(schemaPaths, &log, preserveCharacterData) { }

    ~DataParser() = default;

    DataParser(const DataParser&) = delete;
    DataParser& operator=(const DataParser&) = delete;
    DataParser(DataParser&&) = default;
    DataParser& operator=(DataParser&&) = default;

    /* Parses the XML in 'xmlStream'.
    *
    * \param xmlStream Input stream containing XML
    *
    * \return Data representation of 'xmlStr'
    */
    std::unique_ptr<ComplexData> parseData(::io::InputStream& xmlStream);

    /*
     * Parses the XML in 'pathname'.
     *
     * \param pathname File containing plain text XML (not a NITF)
     *
     * \return Data representation of the contents of 'pathname'
     */
    std::unique_ptr<ComplexData> parseData(const std::filesystem::path&);

    /*
     * Parses the XML in 'xmlStr'.
     *
     * \param xmlStr XML document as a string
     *
     * \return Data representation of 'xmlStr'
     */
    std::unique_ptr<ComplexData> parseData(const std::u8string& xmlStr);
};
}
}
#endif // SIX_six_sicd_DataParser_h_INCLUDED_
