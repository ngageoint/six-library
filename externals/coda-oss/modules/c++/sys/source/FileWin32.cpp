/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifdef _WIN32

#include <limits>
#include <cmath>
#include "sys/File.h"

_SYS_HANDLE_TYPE sys::File::createFile(const coda_oss::filesystem::path& str_, int accessFlags, int creationFlags) noexcept
{
    const auto str = str_.string();

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

    const auto dwDesiredAccess = static_cast<DWORD>(accessFlags);
    const auto dwCreationDisposition = static_cast<DWORD>(creationFlags);
    return CreateFile(str.c_str(),
                         dwDesiredAccess,
                         FILE_SHARE_READ,
                         nullptr /*lpSecurityAttributes*/,
                         dwCreationDisposition,
                         FILE_ATTRIBUTE_NORMAL,
                         static_cast<HANDLE>(nullptr) /*hTemplateFile*/);
}
void sys::File::create(const std::string& str,
                       int accessFlags,
                       int creationFlags)
{
    create(std::nothrow, str, accessFlags, creationFlags);
    if (mHandle == INVALID_HANDLE_VALUE)
    {
        throw sys::SystemException(Ctxt(str::Format("Error opening file: [%s]", str)));
    }
}

void sys::File::readInto(void* buffer, size_t size)
{
    static const size_t MAX_READ_SIZE = std::numeric_limits<DWORD>::max();
    size_t bytesRead = 0;
    size_t bytesRemaining = size;

    sys::byte* bufferPtr = static_cast<sys::byte*>(buffer);

    while (bytesRead < size)
    {
        // Determine how many bytes to read
        const DWORD bytesToRead = static_cast<DWORD>(
                std::min(MAX_READ_SIZE, bytesRemaining));

        // Read from file
        DWORD bytesThisRead = 0;
        if (!ReadFile(mHandle,
                      bufferPtr + bytesRead,
                      bytesToRead,
                      &bytesThisRead,
                      nullptr))
        {
            throw sys::SystemException(Ctxt("Error reading from file"));
        }
        else if (bytesThisRead == 0)
        {
            //! ReadFile does not fail when finding the EOF --
            //  instead it reports 0 bytes read, so this stops an infinite loop
            //  from Unexpected EOF
            throw sys::SystemException(Ctxt("Unexpected end of file"));
        }

        bytesRead += bytesThisRead;
        bytesRemaining -= bytesThisRead;
    }
}

void sys::File::writeFrom(const void* buffer, size_t size)
{
    static const size_t MAX_WRITE_SIZE = std::numeric_limits<DWORD>::max();
    size_t bytesRemaining = size;
    size_t bytesWritten = 0;

    const sys::byte* bufferPtr = static_cast<const sys::byte*>(buffer);

    while (bytesWritten < size)
    {
        // Determine how many bytes to write
        const DWORD bytesToWrite = static_cast<DWORD>(
            std::min(MAX_WRITE_SIZE, bytesRemaining));

        // Write the data
        DWORD bytesThisWrite = 0;
        if (!WriteFile(mHandle,
                       bufferPtr + bytesWritten,
                       bytesToWrite,
                       &bytesThisWrite,
                       nullptr))
        {
            throw sys::SystemException(Ctxt("Writing from file"));
        }

        // Accumulate this write until we are done
        bytesRemaining -= bytesThisWrite;
        bytesWritten += bytesThisWrite;
    }
}

sys::Off_T sys::File::seekTo(sys::Off_T offset, int whence)
{
    /* Ahhh!!! */
    LARGE_INTEGER largeInt;
    largeInt.QuadPart = offset;
    LARGE_INTEGER newFilePointer;
    const auto dwMoveMethod = static_cast<DWORD>(whence);
    if (SetFilePointerEx(mHandle, largeInt, &newFilePointer, dwMoveMethod) == 0)
    {
        const auto dwLastError = GetLastError();
        throw sys::SystemException(Ctxt("SetFilePointer failed: GetLastError() = " + std::to_string(dwLastError)));
    }

    return static_cast<sys::Off_T>(newFilePointer.QuadPart);
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
                    str::Format("Error getting last modified time for path %s", mPath)));
}

void sys::File::flush()
{
    if (!FlushFileBuffers(mHandle))
    {
        throw sys::SystemException(Ctxt("Error flushing file " + mPath));
    }
}

void sys::File::close()
{
    CloseHandle(mHandle);
    mHandle = SYS_INVALID_HANDLE;
}

#endif
