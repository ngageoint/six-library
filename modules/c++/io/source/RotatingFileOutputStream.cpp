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

#include "io/RotatingFileOutputStream.h"
#include "io/FileOutputStream.h"

io::RotatingFileOutputStream::RotatingFileOutputStream(
                                                       const std::string& filename,
                                                       unsigned long maxBytes,
                                                       size_t backupCount,
                                                       int creationFlags) :
    io::CountingOutputStream(new io::FileOutputStream(filename, creationFlags), true),
    mMaxBytes(maxBytes), mBackupCount(backupCount)
{
    mFilename = filename; // doing this in initializer list causes ASAN diagnostic on Windows ... VS bug?

    mByteCount = ((io::FileOutputStream*) mProxy.get())->tell();
    if (shouldRollover(0))
        doRollover();
}

bool io::RotatingFileOutputStream::shouldRollover(sys::Size_T len)
{
    if (mMaxBytes > 0)
    {
        // first check if we are at the beginning of the file
        // if one log message overflows our max bytes, we'll just write it
        // the other option is to not write it at all - at least this way we
        // track it
        if (mByteCount == 0 && len > mMaxBytes)
            return false;

        // otherwise, if this message puts us over, we rollover
        if (mByteCount + len > mMaxBytes)
            return true;
    }
    return false;
}

void io::RotatingFileOutputStream::doRollover()
{
    io::FileOutputStream* fos = (io::FileOutputStream*) mProxy.get();
    fos->close();
    sys::OS os;

    if (mBackupCount > 0)
    {
        for (size_t i = mBackupCount - 1; i > 0; --i)
        {
            std::stringstream curName;
            curName << mFilename << "." << i;
            std::stringstream nextName;
            nextName << mFilename << "." << (i + 1);
            if (os.exists(curName.str()))
            {
                if (os.exists(nextName.str()))
                {
                    os.remove(nextName.str());
                }
                os.move(curName.str(), nextName.str());
            }
        }
        std::string curName = mFilename + ".1";
        if (os.exists(curName))
            os.remove(curName);
        os.move(mFilename, curName);
    }
    mProxy.reset(new io::FileOutputStream(mFilename, sys::File::CREATE));
    mByteCount = 0;
}

void io::RotatingFileOutputStream::write(const void* buffer, size_t len)
{
    if (shouldRollover(len))
        doRollover();
    io::CountingOutputStream::write(buffer, len);
}
