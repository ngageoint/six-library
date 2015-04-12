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

#ifdef WIN32

#include "sys/File.h"

void sys::File::create(const std::string& str,
        int accessFlags,
        int creationFlags)
{
    // If the truncate bit is on AND the file does exist,
    // we need to set the mode to TRUNCATE_EXISTING
    if ((creationFlags & sys::File::TRUNCATE) && sys::OS().exists(str) )
    {
        creationFlags = TRUNCATE_EXISTING;
    }
    else
    {
        creationFlags = ~sys::File::TRUNCATE & creationFlags;
    }

    mHandle = CreateFile(str.c_str(),
            accessFlags,
            FILE_SHARE_READ, NULL,
            creationFlags,
            FILE_ATTRIBUTE_NORMAL, NULL);

    if (mHandle == SYS_INVALID_HANDLE)
    {
        throw sys::SystemException(Ctxt("Error opening file: [%s]", str.c_str()));
    }
    mPath = str;
}

void sys::File::readInto(char *buffer, Size_T size)
{
    /****************************
     *** Variable Declarations ***
     ****************************/
    DWORD bytesRead = 0; /* Number of bytes read during last read operation */
    DWORD totalBytesRead = 0; /* Total bytes read thus far */

    /* Make the next read */
    if (!ReadFile(mHandle, buffer, size, &bytesRead, 0))
    {
        throw sys::SystemException(Ctxt("Error reading from file"));
    }
}

void sys::File::writeFrom(const char *buffer, Size_T size)
{
    DWORD actuallyWritten = 0;

    do
    {
        /* Keep track of the bytes we read */
        DWORD bytesWritten;
        /* Write the data */
        BOOL ok = WriteFile(mHandle, buffer, size, &bytesWritten, NULL);
        if (!ok)
        {
            /* If the function failed, we want to get the last error */
            throw sys::SystemException(Ctxt("Writing from file"));
        }
        /* Otherwise, we want to accumulate this write until we are done */
        actuallyWritten += bytesWritten;
    }
    while (actuallyWritten < size);
}

sys::Off_T sys::File::seekTo(sys::Off_T offset, int whence)
{
    /* Ahhh!!! */
    LARGE_INTEGER largeInt;
    LARGE_INTEGER toWhere;
    largeInt.QuadPart = offset;
    if (!SetFilePointerEx(mHandle, largeInt, &toWhere, whence))
        throw sys::SystemException(Ctxt("SetFilePointer failed"));

    return (sys::Off_T) toWhere.QuadPart;
}

sys::Off_T sys::File::length()
{
    DWORD highOff;
    DWORD ret = GetFileSize(mHandle, &highOff);
    sys::Uint64_T off = highOff;
    return (sys::Off_T)(off << 32) + ret;
}

sys::Off_T sys::File::lastModifiedTime()
{
    FILETIME creationTime, lastAccessTime, lastWriteTime;
    BOOL ret = GetFileTime(mHandle, &creationTime,
            &lastAccessTime, &lastWriteTime);
    if (ret)
    {
        ULARGE_INTEGER uli;
        uli.LowPart = lastWriteTime.dwLowDateTime;
        uli.HighPart = lastWriteTime.dwHighDateTime;
        ULONGLONG stInMillis(uli.QuadPart/10000);
        return (sys::Off_T)stInMillis;
    }
    throw sys::SystemException(Ctxt(
                    FmtX("Error getting last modified time for path %s",
                            mPath.c_str())));
}

void sys::File::close()
{
    CloseHandle(mHandle);
    mHandle = SYS_INVALID_HANDLE;
}

#endif
