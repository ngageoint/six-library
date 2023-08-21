/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * zip-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_zip_GZipOutputStream_h_INCLUDED_
#define CODA_OSS_zip_GZipOutputStream_h_INCLUDED_

#include "config/Exports.h"

#include "zip/Types.h"

namespace zip
{
/*!
 *  \class GZipOutputStream
 *  \brief IO wrapper for zlib API
 *
 */
class CODA_OSS_API GZipOutputStream : public io::OutputStream
{
    gzFile mFile;
public:
    //!  Constructor requires initialization
    GZipOutputStream(const std::string& file);

    /*!
     *  Write len (or less) bytes into the gzip stream.
     *  If an error occurs, this function throws.  If
     *  the underlying gzip stream ever returns zero bytes
     *  the call returns.
     *
     */
    virtual void write(const void* buffer, size_t len) override;

    /*!
     *  Close the gzip stream.  You must call this
     *  afterward (it is not done automatically).
     */
    virtual void close() override;
};
}

#endif  // CODA_OSS_zip_GZipOutputStream_h_INCLUDED_