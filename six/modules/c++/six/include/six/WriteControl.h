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
#ifndef __SIX_WRITE_CONTROL_H__
#define __SIX_WRITE_CONTROL_H__

#include <memory>
#include <std/span>
#include <vector>
#include <std/filesystem>
#include <complex>

#include "six/Types.h"
#include "six/Region.h"
#include "six/Container.h"
#include "six/Options.h"
#include "six/XMLControlFactory.h"
#include "six/Logger.h"
#include <import/logging.h>

namespace six
{

//!  A vector of InputStream objects (one per SICD, N per SIDD)
typedef std::vector<io::InputStream*> SourceList;

//!  A vector of Buffer objects (one per SICD, N per SIDD)
typedef std::vector<const UByte*> BufferList;

//!  Same as above but used in overloadings to help the compiler out when
//   it's convenient for the caller to put non-const pointers in the vector
typedef std::vector<UByte*> NonConstBufferList;

/*!
 *  \class WriteControl
 *  \brief Defines the interface for a SICD/SIDD writer
 *
 *  This class defines the interface for reading and writing a SICD
 *  or SIDD file.  It leaves the details up to the derived implementations.
 *
 *  As with the ReadControl, the WriteControl relies on two derived
 *  classes to handle the container format, one for NITF and one for GeoTIFF.
 *  SICD products can only be written as NITFs.
 *
 *  SIDD products may have a container populated with more than one SIDD
 *  item, as well as more than one SICD item (see Container).  This interface
 *  allows that data model to be transmitted to the container file format.
 *
 */
struct WriteControl
{
    //!  Global byte swap option.  Normally, you should leave this up to us
    static const char OPT_BYTE_SWAP[];

    /*!
     *  Buffer size to use when writing.  This is just a preference, and
     *  may be ignored by an implementation file.
     */
    static const char OPT_BUFFER_SIZE[];

    // Control multi-threading for AMP8I_PHS8I conversion in six::sicd::ImageData.
    // A negative means no multithreading, 0 will have "the system" guess at
    // an appropriate cutoff (a hardcoded value based on testing).  Any other
    // positive value is the number of pixels to process in each thread; it should
    // be fairly large to make-up for the overhead of threading.
    static constexpr char AMP8I_PHS8I_CUTOFF[] = "AMP8I_PHS8I_cutoff";
    static constexpr ptrdiff_t AMP8I_PHS8I_DEFAULT_CUTOFF = 0; // to_AMP8I_PHS8I() is too slow w/o multi-threading

    //!  Constructor.  Null-sets the Container
    WriteControl() noexcept(false) :
        mLogger(mLog, mOwnLog, nullptr)
    {
        setLogger(nullptr);
        setXMLControlRegistry(nullptr);
    }

    //!  Destructor.  Does not release any memory
    virtual ~WriteControl() noexcept {}

    WriteControl(const WriteControl&) = delete;
    WriteControl& operator=(const WriteControl&) = delete;

    /*!
     *  Initialize sets the underlying Container pointer to the
     *  call parameter.  It sets up the data model for a write.  This
     *  may mean different things to different implementations.  No
     *  data is written until the save() method is called
     *
     *  \param container Container to bind to
     */
    virtual void initialize(std::shared_ptr<Container> container) = 0;

    /*!
     *  Save a list of InputStream sources.  This should always be
     *  size one for a SICD (as should the Container), but it might
     *  contain multiple elements for certain types of SIDD files
     *
     *  \param sources A vector of InputStream items, one per target image
     *  \param toFile file name to write out
     *  \param schemaPaths Directories or files of schema locations
     */
    void save(const SourceList& sources, const std::string& toFile)
    {
        save(sources, toFile, std::vector<std::string>());
    }

    virtual void save(const SourceList& sources, const std::string& toFile,
                      const std::vector<std::string>& schemaPaths) = 0;

    /*!
     *  Save a list of memory sources.  This should always be
     *  size one for a SICD (as should the Container), but it might
     *  contain multiple elements for certain types of SIDD files
     *
     *  \param sources A vector of memory buffers, one per target image
     *  \param toFile file name to write out
     *  \param schemaPaths Directories or files of schema locations
     */
    void save(const BufferList& sources, const std::string& toFile)
    {
        save(sources, toFile, std::vector<std::string>());
    }
    virtual void save(const BufferList& sources, const std::string& toFile,
                      const std::vector<std::string>& schemaPaths) = 0;

    // For convenience since the compiler can't implicitly convert
    // std::vector<T*> to std::vector<const T*>
    void save(const NonConstBufferList& sources, const std::string& toFile)
    {
        save(convertBufferList(sources), toFile);
    }

    void save(const NonConstBufferList& sources,
              const std::string& toFile,
              const std::vector<std::string>& schemaPaths)
    {
        save(convertBufferList(sources), toFile, schemaPaths);
    }

    /*!
     *  Utility for Writing out one InputStream only.
     */
    void save(io::InputStream* source, const std::string& toFile)
    {
        save(source, toFile, std::vector<std::string>());
    }
    void save(io::InputStream* source, const std::string& toFile,
              const std::vector<std::string>& schemaPaths)
    {
        SourceList sources;
        sources.push_back(source);
        save(sources, toFile, schemaPaths);
    }

    /*!
     *  Utility for Writing out one buffer image only.
     */
    void save(const UByte* buffer, const std::string& toFile)
    {
        save(buffer, toFile, std::vector<std::string>());
    }
    void save(const std::byte* buffer, const std::string& toFile)
    {
        const void* buffer_ = buffer;
        save(static_cast<const UByte*>(buffer_), toFile);
    }
    void save(const UByte* buffer, const std::string& toFile,
              const std::vector<std::string>& schemaPaths)
    {
        BufferList sources;
        sources.push_back(buffer);
        save(sources, toFile, schemaPaths);
    }
    void save(const std::byte* buffer, const std::string& toFile,
              const std::vector<std::string>& schemaPaths)
    {
        const void* buffer_ = buffer;
        save(static_cast<const UByte*>(buffer_), toFile, schemaPaths);
    }

    /*!
     * shared pointer to Container
     */
    std::shared_ptr<Container> getContainer()
    {
        return mContainer;
    }

    /*!
     *  shared const pointer to Container.
     */
    std::shared_ptr<const Container> getContainer() const
    {
        std::shared_ptr<const Container> retval = mContainer;
        return mContainer;
    }

    /*!
     *  Reports back what kind of file format the derived implementation
     *  is supporting (currently only NITF and GeoTIFF)
     *  \return A string name
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

    virtual void setXMLControlRegistry(const XMLControlRegistry* xmlRegistry)
    {
        setXMLControlRegistryImpl(xmlRegistry);
    }

    //! \return XML registry being used by the writer
    const XMLControlRegistry* getXMLControlRegistry() const
    {
        return mXMLRegistry;
    }

    BufferList convertBufferList(const NonConstBufferList& buffers)
    {
        BufferList retval;
        for (const auto& buffer : buffers)
        {
            const void* buffer_ = buffer;
            retval.push_back(static_cast<BufferList::value_type>(buffer_));
        }
        return retval;
    }

protected:
    void setXMLControlRegistryImpl(const XMLControlRegistry* xmlRegistry)
    {
        mXMLRegistry = xmlRegistry;
        if (!mXMLRegistry)
            mXMLRegistry = &XMLControlFactory::getInstance();
    }
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

