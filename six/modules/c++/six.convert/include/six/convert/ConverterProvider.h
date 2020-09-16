/* =========================================================================
 * This file is part of six.convert-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.convert-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_CONVERTER_PROVIDER_H__
#define __SIX_CONVERTER_PROVIDER_H__

#include <string>
#include <vector>

#include <six/sicd/ComplexData.h>
#include <sys/Conf.h>
#include <types/RowCol.h>

namespace six
{
namespace convert
{
/*!
 * \class ConverterProvider
 * Interface for plugins that convert external vendor formats to SICDs
 */
class ConverterProvider
{
public:
    virtual ~ConverterProvider() {}

    //! Return true if converter supports given file
    virtual bool supports(const std::string& pathname) const = 0;

    //! Initialize converter
    virtual void load(const std::string& pathname) = 0;

    //! Parse loaded file and return a ComplexData object
    virtual std::unique_ptr<six::sicd::ComplexData> convert() = 0;

    /*!
     * Read given region from image
     * \param startingLocations Starting row and column
     * \param dims Number of rows and columns to reade
     * \param buffer Pre-allocated buffer for storing image data. Should be
     * `getDataSizeInBytes()` bytes long
     */
    virtual void readData(const types::RowCol<size_t>& startingLocations,
            const types::RowCol<size_t>& dims, void* buffer) = 0;

    /*!
     * Read entire image
     * Use getDataSizeInBytes() to determine buffer size
     * \param buffer Pre-allocated buffer for storing image data. Should be
     * `getDataSizeInBytes()` bytes long
     */
    virtual void readData(void* buffer) = 0;

    /*!
     * \return size of image data in bytes
     */
    virtual size_t getDataSizeInBytes() const = 0;

    /*
     * \return type of file being converted
     */
    virtual std::string getFileType() const = 0;
};
}
}

#endif

