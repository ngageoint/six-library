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
#ifndef __SIX_CONVERTING_READ_CONTROL_H__
#define __SIX_CONVERTING_READ_CONTROL_H__

#include <string>

#include <six/Enums.h>
#include <six/ReadControl.h>
#include <six/convert/ConverterProvider.h>
#include <six/convert/ConverterProviderRegistry.h>

namespace six
{
namespace convert
{
/*!
 * \class ConvertingReadControl
 * \brief ReadControl implementation using plugins to read vendor images as SICDs
 *
 * Once constructed, it is used just like NITFReadControl
 */
struct ConvertingReadControl : public ReadControl
{
    /*!
     * Construct ConvertingReadControl
     * \param pluginPathname Path to converter plugins
     */
    ConvertingReadControl(const std::string& pluginPathname);

    /*!
     * Construct ConvertingReadControl
     * \param pluginPathnames Paths to converter plugins
     */
    ConvertingReadControl(const std::vector<std::string>& pluginPathnames);

    ConvertingReadControl(const ConvertingReadControl&) = delete;
    ConvertingReadControl& operator=(const ConvertingReadControl&) = delete;

    /*!
     * Read whether a file has COMPLEX or DERIVED data
     * \param pathname path to file
     * \return datatype of file contents
     */
    virtual DataType getDataType(const std::string& pathname) const;

    /*!
     * Load the file into a container. Once you have done this,
     * you can get the image data back using the interleaved function.
     * \param pathname       Input filepath
     */
    void load(const std::string& pathname)
    {
        load(pathname, std::vector<std::string>());
    }

    /*!
     * Load the file into a container. Once you have done this,
     * you can get the image data back using the interleaved function.
     * \param pathname        Input filepath
     * \param schemaPathnames Not used
     */
    virtual void load(const std::string& pathname,
            const std::vector<std::string>& schemaPaths);

    /*!
     * \return type of file being converted
     */
    virtual std::string getFileType() const;

    /*!
     * Read a region of an image. Pointers to the data will be available
     * from both the return value and the Region object
     *
     * \param region Region to read from image
     * \param imageNumber Index of image to read
     * \return pointer to image data
     */

    virtual UByte* interleaved(Region& region, size_t imageNumber) override;
    virtual void interleaved(Region& region, size_t imageNumber, std::byte*& result) override
    {
        result = reinterpret_cast<std::byte*>(interleaved(region, imageNumber));
    }

    /*!
     * Convenience method to allow reading entire image with no setup
     * \param imageNumber Index of image to read
     * \return pointer to image data
     */
    virtual UByte* interleaved(size_t imageNumber=0);

private:
    const std::vector<std::string> mPluginPathnames;
    ConverterProviderRegistry mRegistry;
    // This is not owned by this class
    // Do not delete
    ConverterProvider* mConverter;
};
}
}

#endif
