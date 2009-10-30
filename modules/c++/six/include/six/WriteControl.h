/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include "six/Types.h"
#include "six/Region.h"
#include "six/Container.h"
#include "six/Options.h"
namespace six
{

//!  A vector of InputStream objects (one per SICD, N per SIDD)
typedef std::vector<io::InputStream*> SourceList;

//!  A vector of Buffer objects (one per SICD, N per SIDD)
typedef std::vector<UByte*> BufferList;

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
class WriteControl
{
protected:
    Container* mContainer;
    Options mOptions;
public:

    //!  Global byte swap option.  Normally, you should leave this up to us
    static const char OPT_BYTE_SWAP[];

    /*!
     *  Buffer size to use when writing.  This is just a preference, and
     *  may be ignored by an implementation file.
     */
    static const char OPT_BUFFER_SIZE[];

    //!  Constructor.  Null-sets the Container
    WriteControl() :
        mContainer(NULL)
    {
    }

    //!  Destructor.  Does not release any memory
    virtual ~WriteControl()
    {
    }

    /*!
     *  Initialize sets the underlying Container pointer to the
     *  call parameter.  It sets up the data model for a write.  This
     *  may mean different things to different implementations.  No
     *  data is written until the save() method is called
     *
     *  \param container Container to bind to
     */
    virtual void initialize(Container* container) = 0;

    /*!
     *  Save a list of InputStream sources.  This should always be
     *  size one for a SICD (as should the Container), but it might
     *  contain multiple elements for certain types of SIDD files
     *
     *  \param sources A vector of InputStream items, one per target image
     *  \param toFile file name to write out
     */
    virtual void save(SourceList& sources, std::string toFile) = 0;

    /*!
     *  Save a list of memory sources.  This should always be
     *  size one for a SICD (as should the Container), but it might
     *  contain multiple elements for certain types of SIDD files
     *
     *  \param sources A vector of memory buffers, one per target image
     *  \param toFile file name to write out
     */
    virtual void save(BufferList& sources, std::string toFile) = 0;

    /*!
     *  Utility for Writing out one InputStream only.
     *
     */
    virtual void save(io::InputStream* source, std::string toFile) 
    {
	SourceList sources;
	sources.push_back(source);
	save(sources, toFile);
    }

    /*!
     *  Utility for Writing out one buffer image only.
     *
     */

    virtual void save(UByte* buffer, std::string toFile)
    {
	BufferList sources;
	sources.push_back( buffer );
	save(sources, toFile);
    }

    /*!
     *  const pointer to Container.  This object is not owned by the
     *  WriteControl
     */
    const Container* getContainer()
    {
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

};

}
#endif
