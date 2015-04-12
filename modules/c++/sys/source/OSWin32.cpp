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


#if defined(WIN32)

#include <sstream>
#include "sys/OSWin32.h"
#include "sys/File.h"


std::string sys::OSWin32::getPlatformName() const
{
    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&info))
        throw sys::SystemException("While performing GetVersionEx()");

    std::string platform;
    if (info.dwPlatformId == VER_PLATFORM_WIN32s)
    {
        platform = "Windows on Win32 3.1";
    }
    else if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
        platform = "Windows on Win32 95";
    }
    else if (info.dwPlatformId = VER_PLATFORM_WIN32_NT)
    {
        platform = "Windows on Win32 NT";
    }
    else
    {
        platform = "Unknown Windows OS";
    }
    return FmtX("%s: %d.%d [build: %d], %s", platform.c_str(),
                info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber,
                info.szCSDVersion);
}

std::string sys::OSWin32::getNodeName() const
{
    char buffer[512];
    size_t size = 512;
    if (GetComputerName(buffer, (LPDWORD)&size))
        return std::string(buffer, size);
    else return std::string("Unknown");
}

bool sys::OSWin32::exists(const std::string& path) const
{
    const DWORD what = GetFileAttributes(path.c_str());

    if (what == INVALID_FILE_ATTRIBUTES)
    {
        const DWORD errCode = GetLastError();
        if (errCode != ERROR_FILE_NOT_FOUND && errCode != ERROR_PATH_NOT_FOUND)
        {
            char* err = NULL;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                          FORMAT_MESSAGE_FROM_SYSTEM,
                          NULL, errCode,
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPTSTR) &err, 0, NULL);
            throw except::Exception(Ctxt(
                "Problem while testing file existence for " + path +
                " with Error: " + std::string(err)));
        }
        return false;
    }

    return true;
}

sys::Pid_T sys::OSWin32::getProcessId() const
{
    return GetCurrentProcessId();
}

void sys::OSWin32::removeFile(const std::string& pathname) const
{
    if (DeleteFile(pathname.c_str()) != true)
    {
        sys::Err err;
        std::ostringstream oss;
        oss << "Failure removing file [" <<  pathname << 
            "] with error [" << err.toString() << "]";

        throw except::Exception(Ctxt(oss.str()));
    }
}

void sys::OSWin32::removeDirectory(const std::string& pathname) const
{
    if (RemoveDirectory(pathname.c_str()) != true)
    {
        sys::Err err;
        std::ostringstream oss;
        oss << "Failure removing directory [" <<  pathname << 
            "] with error [" << err.toString() << "]";

        throw except::Exception(Ctxt(oss.str()));
    }
}

bool sys::OSWin32::move(const std::string& path, 
                        const std::string& newPath) const
{
    return (MoveFile(path.c_str(), newPath.c_str())) ? (true) : (false);
}



bool sys::OSWin32::isFile(const std::string& path) const
{
    //  I'm not 100% sure.  So I'm checking
    //  1) Exists
    //  2) Not Directory
    //  3) Not Archive - we aren't doing that...
    const DWORD what = GetFileAttributes(path.c_str());
    return (what != INVALID_FILE_ATTRIBUTES &&
            !(what & FILE_ATTRIBUTE_DIRECTORY));
}


bool sys::OSWin32::isDirectory(const std::string& path) const
{
    const DWORD what = GetFileAttributes(path.c_str());
    return (what != INVALID_FILE_ATTRIBUTES &&
            (what & FILE_ATTRIBUTE_DIRECTORY));
}

bool sys::OSWin32::makeDirectory(const std::string& path) const
{
    return (CreateDirectory(path.c_str(), NULL)) ? (true): (false);
}

std::string sys::OSWin32::getCurrentWorkingDirectory() const
{
    char buffer[MAX_PATH + 1];
    DWORD where = GetCurrentDirectory(MAX_PATH + 1, buffer);
    if (where == 0)
        throw sys::SystemException("In getCurrentWorkingDirectory()");
    std::string __str(buffer, where);
    return __str;
}

bool sys::OSWin32::changeDirectory(const std::string& path) const
{
    return SetCurrentDirectory(path.c_str()) ? true : false;
}

std::string sys::OSWin32::getTempName(const std::string& path,
                                      const std::string& prefix) const
{
    char buffer[MAX_PATH];
    if (GetTempFileName(path.c_str(),
                        prefix.c_str(),
                        0, buffer) == 0) return std::string("");
    return std::string(buffer);
}

sys::Off_T sys::OSWin32::getSize(const std::string& path) const
{
    return sys::File(path).length();
}

sys::Off_T sys::OSWin32::getLastModifiedTime(const std::string& path) const
{
    return sys::File(path).lastModifiedTime();
}

void sys::OSWin32::millisleep(int milliseconds) const
{
    Sleep(milliseconds);
}

std::string sys::OSWin32::getDSOSuffix() const
{
    return "dll";
}

std::string sys::OSWin32::operator[](const std::string& s) const
{
    return getEnv(s);
}


std::string sys::OSWin32::getEnv(const std::string& s) const
{
    std::string result;
    DWORD size = GetEnvironmentVariable(s.c_str(), NULL, 0);
    if (size == 0)
        throw sys::SystemException(Ctxt(FmtX(
            "Unable to get windows environment variable %s", s.c_str())));

    // If we can use a normal size buffer, lets not bother to malloc

    char *buffer = new char[size + 1];
    DWORD retVal = GetEnvironmentVariable(s.c_str(), buffer, size);
    result = buffer;
    delete [] buffer;

    return result;
}

void sys::OSWin32::setEnv(const std::string& var, 
                          const std::string& val,
                          bool overwrite)
{
    BOOL ret = SetEnvironmentVariable(var.c_str(), val.c_str());
    if(!ret)
        throw sys::SystemException(Ctxt(FmtX(
            "Unable to set windows environment variable %s", var.c_str())));
}

size_t sys::OSWin32::getNumCPUs() const
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
}

void sys::DirectoryWin32::close()
{
    if (mHandle != INVALID_HANDLE_VALUE)
        ::FindClose(mHandle);
    mHandle = INVALID_HANDLE_VALUE;
}

std::string sys::DirectoryWin32::findFirstFile(const std::string& dir)
{
    std::string plusWC = dir;
    plusWC += std::string("\\*");
    mHandle = ::FindFirstFile(plusWC.c_str(), &mFileData);
    if (mHandle == INVALID_HANDLE_VALUE) 
        return "";
    return mFileData.cFileName;
}

std::string sys::DirectoryWin32::findNextFile()
{
    if (::FindNextFile(mHandle, &mFileData) == 0)
        return "";
    return mFileData.cFileName;
}


#endif
