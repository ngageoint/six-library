/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_BLOCKING_INFO_HPP__
#define __NITF_BLOCKING_INFO_HPP__

#include "nitf/System.hpp"
#include "nitf/IOInterface.hpp"
#include "nitf/ImageIO.h"
#include "nitf/ImageSubheader.hpp"
#include "nitf/SubWindow.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 * \file BlockingInfo.hpp
 * \brief Contains wrapper implementation for BlockingInfo
 */

namespace nitf
{

/*!
 *  \class BlockingInfo
 *  \brief The C++ wrapper of the nitf_BlockingInfo
 *
 *  BlockingInfo provides information about an image segment's block
 *  organization.  This structure is obtained by calling the function
 *  ImageReader::getBlockingInfo().  The information in this structure
 *  reflects the correct information.  Due to the implementation of
 *  some compression types, the information in the image segment
 *  header may not be correct.
 */
DECLARE_CLASS(BlockingInfo)
{
public:
    //! Copy constructor
    BlockingInfo(const BlockingInfo & x);

    //! Assignment operator
    BlockingInfo & operator=(const BlockingInfo & x);

    //! Set native object
    BlockingInfo(nitf_BlockingInfo * x);

    //! Constructor
    BlockingInfo();

    //! Destructor
    ~BlockingInfo();

    //! Get the number of blocks per row
    nitf::Uint32 getNumBlocksPerRow() const;

    //! Set the number of blocks per row
    void setNumBlocksPerRow(nitf::Uint32 value);

    //! Get the number of blocks per column
    nitf::Uint32 getNumBlocksPerCol() const;

    //! Set the number of blocks per column
    void setNumBlocksPerCol(nitf::Uint32 value);

    //! Get the number of rows per block
    nitf::Uint32 getNumRowsPerBlock() const;

    //! Set the number of rows per block
    void setNumRowsPerBlock(nitf::Uint32 value);

    //! Get the number of columns per block
    nitf::Uint32 getNumColsPerBlock() const;

    //! Set the number of columns per block
    void setNumColsPerBlock(nitf::Uint32 value);

    //! Get the length
    size_t getLength() const;

    //! Set the length
    void setLength(size_t value);

    //! Print the blocking into to file
    void print(FILE *file);

private:
    nitf_Error error;
};

}
#endif
