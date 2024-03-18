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
#ifndef __SIX_READ_CONTROL_H__
#define __SIX_READ_CONTROL_H__

#include <memory>
#include <std/filesystem>
#include <algorithm>

#include <mem/ScopedArray.h>
#include <import/logging.h>
#include <sys/Path.h>

#include "six/Types.h"
#include "six/Region.h"
#include "six/Container.h"
#include "six/Options.h"
#include "six/XMLControlFactory.h"
#include "six/Logger.h"
#include "six/Exports.h"

namespace six
{

/*!
 *  \class ReadControl
 *  \brief Interface for reading a SICD/SIDD from a container format
 *
 *  Abstract Base Class for readers to implement.  The ReadControl
 *  is responsible for loading and reading SICD/SIDD files.
 *
 *  The class is initialized with the load() method, at which point,
 *  meta-data is read in.
 *
 *  The interleaved() function loads a Region into memory, abstracting
 *  any file format details.  For example, if the file is a segmented
 *  NITF (i.e., the array size was larger than 10GB), the user can
 *  still request pixels from anywhere in that array, without having
 *  to know that the NITF is split due to technical complications.
 *
 */
struct SIX_SIX_API ReadControl
{
    //!  Constructor.  Null-set the current container reference
    ReadControl() noexcept(false) : mLogger(mLog, mOwnLog, nullptr)
    {
        setLogger(nullptr);
        setXMLControlRegistry(nullptr);
    }

    //!  Destructor doesnt release anything
    virtual ~ReadControl() noexcept = default;

    ReadControl(const ReadControl&) = delete;
    ReadControl& operator=(const ReadControl&) = delete;

    /*!
     *  This function takes in a file and tells you what kind of
     *  container is in it (SICD or SIDD)
     */
    virtual DataType getDataType(const std::string& fromFile) const = 0;

    /*!
     *  Load the SICD or SIDD file into a container.  Once you have
     *  done this, you can get the image data back using the interleaved
     *  function
     *  \param fromFile    Input filepath
     *  \param schemaPaths Directories or files of schema locations
     */
    void load(const std::string& fromFile)
    {
        load(fromFile, std::vector<std::string>());
    }
    virtual void load(const std::string& fromFile, const std::vector<std::string>& schemaPaths) = 0;
    virtual void load(const std::filesystem::path& fromFile, const std::vector<std::filesystem::path>* pSchemaPaths)
    {
        std::vector<std::string> schemaPaths_;
        if (pSchemaPaths != nullptr)
        {
            schemaPaths_ = sys::convertPaths(*pSchemaPaths);
        }
        load(fromFile.string(), schemaPaths_);
    }

    /*!
     *  Get a const shared pointer to the current container.
     */
    std::shared_ptr<const Container> getContainer() const
    {
        std::shared_ptr<const Container> retval = mContainer;
        return retval;
    }

    /*!
     *  Get a non-const pointer to the current container.
     */
    std::shared_ptr<Container> getContainer()
    {
        return mContainer;
    }

    /*!
     *  This function reads in the image area specified by the region.
     *  If you want us to use a work buffer, you can set it through region.
     *
     *  To simply read the entire image, pass a default-constructed
     *  Region as the first parameter
     *
     *  Once read, the image buffer is set in both the region pointer,
     *  and in the return value, for convenience
     *
     *  For safety, prefer the overload below.
     */
    virtual UByte* interleaved(Region& region, size_t imageNumber) = 0;
    virtual void  interleaved(Region& region, size_t imageNumber, std::byte*& result)
    {
        void* result_ = interleaved(region, imageNumber);
        result = static_cast<std::byte*>(result_);
    }

    /*!
     *  This function reads in the image area specified by the region.
     *  If you want us to use a work buffer, you can set it through region.
     *  If the region has a work buffer set, ownership will pass to
     *  the buffer parameter, so be careful about doing this.
     *
     *  To simply read the entire image, pass a default-constructed
     *  Region as the first parameter.
     *
     *  Once read, the image buffer is set in the passed buffer, and
     *  also the region pointer and the return value for convenience.
     *
     *  The method will attempt to read the data as whatever type
     *  you call it with. So using a buffer of floats to read integer
     *  data would be a mistake.
     *
     * \param region Region stores row and cols to be read
     * \param imageNumber Index of the image to read
     * \param buffer Scoped array that holds the memory for the read-in image.
     * This will be allocated by this function.
     *
     * \return Buffer of image data.  This is simply equal to buffer.get() and
     * is provided as a convenience.
     */
    template<typename T>
    T* interleaved(Region& region, size_t imageNumber,
        std::unique_ptr<T[]>& buffer)
    {
        buffer.reset(reinterpret_cast<T*>(interleaved(region, imageNumber)));
        return buffer.get();
    }

    /*!
     *  Get the file type.  For SICD, this will only include "NITF", but
     *  for SIDD, there will be subclassing for "NITF" and "GeoTIFF"
     *
     */
    virtual std::string getFileType() const = 0;

    /*!
     *  Get a const view of the Options map.  These are override
     *  parameters that are specific to a specific ReadControl implementation.
     */
    const Options& getOptions() const
    {
        return mOptions;
    }

    /*!
     *  Get a non-const view of the Options map.  These are override
     *  parameters that are specific to a specific ReadControl implementation.
     */
    Options& getOptions()
    {
        return mOptions;
    }

    /*!
     * Sets the logger to use internally
     */
    template<typename TLogger>
    void setLogger(TLogger&& logger)
    {
        mLogger.setLogger(std::forward<TLogger>(logger));
    }
    void setLogger(logging::Logger* logger, bool ownLog)
    {
        mLogger.setLogger(logger, ownLog);
    }

    void setXMLControlRegistry(const XMLControlRegistry *xmlRegistry)
    {
        mXMLRegistry = xmlRegistry;
        if (!mXMLRegistry)
            mXMLRegistry = &getXMLControlFactory();
    }
    void setXMLControlRegistry(const XMLControlRegistry& xmlRegistry)
    {
        setXMLControlRegistry(&xmlRegistry);
    }

protected:
    std::shared_ptr<Container> mContainer;
    Options mOptions;
    logging::Logger* mLog = nullptr;
    bool mOwnLog = false;
    const XMLControlRegistry* mXMLRegistry = nullptr;

private:
    Logger mLogger;
};

}

#endif

