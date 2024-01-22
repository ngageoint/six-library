/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#ifndef __IO_ROTATING_FILE_STREAMS_H__
#define __IO_ROTATING_FILE_STREAMS_H__

#include "config/Exports.h"
#include <import/sys.h>
#include "io/CountingStreams.h"

namespace io
{

/**
 * An OutputStream that keeps track of the number of bytes written to the stream.
 */
struct CODA_OSS_API RotatingFileOutputStream : public CountingOutputStream
{
    RotatingFileOutputStream(const std::string& filename,
                             unsigned long maxBytes = 0,
                             size_t backupCount = 0, int creationFlags =
                                     sys::File::CREATE | sys::File::TRUNCATE);

    virtual ~RotatingFileOutputStream() = default;
    RotatingFileOutputStream(const RotatingFileOutputStream&) = delete;
    RotatingFileOutputStream& operator=(const RotatingFileOutputStream&) = delete;

    using CountingOutputStream::write;

    virtual void write(const void* buffer, size_t len) override;

protected:
    std::string mFilename;
    unsigned long mMaxBytes;
    size_t mBackupCount;

    virtual bool shouldRollover(sys::Size_T len);
    virtual void doRollover();

};

}

#endif
