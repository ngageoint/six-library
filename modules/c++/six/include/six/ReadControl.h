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
#ifndef __SIX_READ_CONTROL_H__
#define __SIX_READ_CONTROL_H__

#include "six/Types.h"
#include "six/Region.h"
#include "six/Container.h"
#include "six/Options.h"
#include "six/XMLControlFactory.h"
#include <import/logging.h>

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
class ReadControl
{
public:

    //!  Constructor.  Null-set the current container reference
    ReadControl() :
        mContainer(NULL), mLog(NULL), mOwnLog(false), mXMLRegistry(NULL)
    {
        setLogger(NULL);
        setXMLControlRegistry(NULL);
    }

    //!  Destructor doesnt release anything
    virtual ~ReadControl()
    {
        if (mLog && mOwnLog)
            delete mLog;
    }

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
    virtual void load(const std::string& fromFile,
                      const std::vector<std::string>& schemaPaths) = 0;

    /*!
     *  Get a const pointer to the current container.  This
     *  container is not allocated, nor deleted by this class
     */
    const Container* getContainer() const
    {
        return mContainer;
    }

    /*!
     *  Get a non-const pointer to the current container.  This
     *  container is not allocated, nor deleted by this class
     */
    Container* getContainer()
    {
        return mContainer;
    }

    /*!
     *  This function reads in the image area specified by the region.
     *  If you want us to use a work buffer, you can set it through region.
     *
     *  Once read, the image buffer is set in both the region pointer,
     *  and in the return value, for convenience
     */
    virtual UByte* interleaved(Region& region, size_t imageNumber) = 0;

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
    void setLogger(logging::Logger* log, bool ownLog = false)
    {
        if (mLog && mOwnLog && log != mLog)
            delete mLog;
        mLog = log ? log : new logging::NullLogger;
        mOwnLog = log ? ownLog : true;
    }

    void setXMLControlRegistry(const XMLControlRegistry *xmlRegistry)
    {
        mXMLRegistry = xmlRegistry;
        if (!mXMLRegistry)
            mXMLRegistry = &XMLControlFactory::getInstance();
    }

protected:
    Container* mContainer;
    Options mOptions;
    logging::Logger *mLog;
    bool mOwnLog;
    const XMLControlRegistry *mXMLRegistry;

};

}

#endif

