/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include "sys/File.h"

#ifndef WIN32

#include <unistd.h>
#include <string.h>
#include <errno.h>

void sys::File::create(const std::string& str, int accessFlags,
        int creationFlags) throw (sys::SystemException)
{

    if (accessFlags & sys::File::WRITE_ONLY)
        creationFlags |= sys::File::TRUNCATE;
    mHandle = open(str.c_str(), accessFlags | creationFlags, _SYS_DEFAULT_PERM);

    if (mHandle < 0)
    {
        throw sys::SystemException(Ctxt(FmtX("Error opening file [%d]: [%s]",
                                             mHandle, str.c_str())));
    }
    mPath = str;
}

void sys::File::readInto(char *buffer, Size_T size)
        throw (sys::SystemException)
{
    SSize_T bytesRead = 0;
    Size_T totalBytesRead = 0;
    int i;

    /* make sure the user actually wants data */
    if (size == 0)
        return;

    for (i = 1; i <= _SYS_MAX_READ_ATTEMPTS; i++)
    {
        bytesRead = ::read(mHandle, buffer + totalBytesRead, size
                - totalBytesRead);

        switch (bytesRead)
        {
        case -1: /* Some type of error occured */
            switch (errno)
            {
            case EINTR:
            case EAGAIN: /* A non-fatal error occured, keep trying */
                break;

            default: /* We failed */
                throw sys::SystemException("While reading from file");
            }
            break;

        case 0: /* EOF (unexpected) */
            throw sys::SystemException(Ctxt("Unexpected end of file"));

        default: /* We made progress */
            totalBytesRead += bytesRead;

        } /* End of switch */

        /* Check for success */
        if (totalBytesRead == size)
        {
            return;
        }
    }
    throw sys::SystemException(Ctxt("Unknown read state"));
}

void sys::File::writeFrom(const char *buffer, Size_T size)
        throw (sys::SystemException)
{
    Size_T bytesActuallyWritten = 0;

    do
    {
        const SSize_T bytesThisRead = ::write(mHandle,
                                              buffer + bytesActuallyWritten,
                                              size - bytesActuallyWritten);
        if (bytesThisRead == -1)
        {
            throw sys::SystemException(Ctxt("Writing to file"));
        }
        bytesActuallyWritten += bytesThisRead;
    }
    while (bytesActuallyWritten < size);
}

sys::Off_T sys::File::seekTo(sys::Off_T offset, int whence)
        throw (sys::SystemException)
{
    sys::Off_T off = ::lseek(mHandle, offset, whence);
    if (off == (sys::Off_T) - 1)
        throw sys::SystemException(Ctxt("Seeking in file"));
    return off;
}

sys::Off_T sys::File::length() throw (sys::SystemException)
{
    struct stat buf;
    int rval = fstat(mHandle, &buf);
    if (rval == -1)
        throw sys::SystemException(Ctxt("Error querying file attributes"));
    return buf.st_size;
}

sys::Off_T sys::File::lastModifiedTime() throw (sys::SystemException)
{
    struct stat buf;
    int rval = fstat(mHandle, &buf);
    if (rval == -1)
        throw sys::SystemException(Ctxt("Error querying file attributes"));
    return (sys::Off_T) buf.st_mtime * 1000;
}

void sys::File::flush()
{
    if (::fsync(mHandle) != 0)
    {
        const int errnum = errno;
        throw sys::SystemException(Ctxt(
            "Error flushing file " + mPath + " (" + ::strerror(errnum) + ")"));
    }
}

void sys::File::close()
{
    ::close(mHandle);
    mHandle = SYS_INVALID_HANDLE;
}

#endif

