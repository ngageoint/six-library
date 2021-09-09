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

#include <assert.h>

#include <iostream>
#include <vector>
#include <sstream>

#if defined(WIN32) || defined(_WIN32)

#include "sys/OSWin32.h"
#include "sys/File.h"

std::string sys::OSWin32::getPlatformName() const
{
    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    #pragma warning(push)
    #pragma warning(disable : 4996)  // '...' : was declared deprecated
    if (!GetVersionEx(&info))
        throw sys::SystemException("While performing GetVersionEx()");
    #pragma warning(pop)

    std::string platform;
    if (info.dwPlatformId == VER_PLATFORM_WIN32s)
    {
        platform = "Windows on Win32 3.1";
    }
    else if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
        platform = "Windows on Win32 95";
    }
    else if (info.dwPlatformId == VER_PLATFORM_WIN32_NT)
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
    if (DeleteFile(pathname.c_str()) != TRUE)
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
    if (RemoveDirectory(pathname.c_str()) != TRUE)
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
    // MOVEFILE_REPLACE_EXISTING - forcefully move the file
    // MOVEFILE_WRITE_THROUGH    - report status after performing a flush
    return (MoveFileEx(path.c_str(),
                       newPath.c_str(),
                       MOVEFILE_REPLACE_EXISTING |
                       MOVEFILE_WRITE_THROUGH)) ? (true) : (false);
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
                        0, buffer) == 0)
    {
        throw except::Exception(Ctxt("Unable to create a temporary file"));
    }
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

static std::string getEnv(const std::string& s)
{
    const DWORD size = GetEnvironmentVariable(s.c_str(), NULL, 0);
    if (size == 0)
    {
        throw sys::SystemException(Ctxt(
            "Unable to get windows environment variable " + s));
    }
    std::vector<char> buffer(size + 1);
    const DWORD retVal = GetEnvironmentVariable(s.c_str(), &buffer[0], size);
    // Win32 API weirdness -- see https://msdn.microsoft.com/en-us/libary/windows/desktop/ms683188%28v=vs.85%29.aspx
    // When less then the size of the buffer is allocated, it returns the required size, including the null character
    // Otherwise, it returns the size of the variable, not including the null character
    if (retVal + 1 != size)
    {
        throw sys::SystemException(Ctxt(
           "Environment variable size does not match allocated size for " + s));
    }
    if (retVal == 0)
    {
        throw sys::SystemException(Ctxt(
               "Environment variable size changed unexpectedly to zero \
                following buffer allocation " + s));
    }
    return buffer.data();
}

static const char* getenv_(const std::string& s)
{
    #pragma warning(push)
    #pragma warning(disable: 4996) // '...': This function or variable may be unsafe. Consider using _dupenv_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
    return getenv(s.c_str());
    #pragma warning(pop)
}

std::string sys::OSWin32::getEnv(const std::string& s) const
{
    std::string retval = ::getEnv(s);
    assert(retval == getenv_(s));
    return retval;
}

bool sys::OSWin32::isEnvSet(const std::string& s) const
{
    const DWORD size = GetEnvironmentVariable(s.c_str(), NULL, 0);
    if (size != 0)
    {
        return true;
    }
    return getenv_(s) != nullptr;
}

static void setEnv(const std::string& var,
                          const std::string& val)
{
    const BOOL ret = SetEnvironmentVariable(var.c_str(), val.c_str());
    if (!ret)
    {
        throw sys::SystemException(Ctxt(
            "Unable to set windows environment variable " + var));
    }
    const auto s = var + "=" + val;
    _putenv(s.c_str());
}
void sys::OSWin32::setEnv(const std::string& var,
                          const std::string& val,
			  bool overwrite)
{
    if (overwrite || !isEnvSet(var))
    {
        ::setEnv(var, val);
    }
}

void sys::OSWin32::unsetEnv(const std::string& var)
{
    const BOOL ret = SetEnvironmentVariable(var.c_str(), NULL);
    if (!ret)
    {
        throw sys::SystemException(Ctxt("Unable to unset windows environment variable " + var));
    }
    const auto s = var + "=";
    _putenv(s.c_str());
}

size_t sys::OSWin32::getNumCPUs() const
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
}

size_t sys::OSWin32::getNumCPUsAvailable() const
{
    throw except::NotImplementedException(
        Ctxt("Windows getNumCPUsAvailable not yet implemented."));
}

size_t sys::OSWin32::getNumPhysicalCPUs() const
{
    // TODO Need to use GetLogicalProcessorInformationEx.
    //      See reference implementation at
    //      https://devblogs.microsoft.com/oldnewthing/?p=2823
    throw except::NotImplementedException(
        Ctxt("Windows getNumPhysicalCPUs not yet implemented."));
}

size_t sys::OSWin32::getNumPhysicalCPUsAvailable() const
{
    throw except::NotImplementedException(
        Ctxt("Windows getNumPhysicalCPUsAvailable not yet implemented."));
}

void sys::OSWin32::getAvailableCPUs(std::vector<int>& /*physicalCPUs*/,
                                    std::vector<int>& /*htCPUs*/) const
{
    throw except::NotImplementedException(
        Ctxt("Windows getAvailableCPUs not yet implemented."));
}

void sys::OSWin32::createSymlink(const std::string& origPathname,
                                 const std::string& symlinkPathname) const
{
    // NTDDI_WINXPSP3 = 0x05010300
    // NTDDI_VISTA    = 0x06000000
    // Note that we can't actually just use the defined constants reliably
    // below as Visual Studio 2008 did not have NTDDI_VISTA defined in it
    // If you want to configure on a Windows 7 machine to support a build that
    // will run on a Windows XP machine, you want to configure with the
    // following flags (except again you may need to substitute the constants
    // with the actual values)
    // --with-cflags="/DNTDDI_VERSION=NTDDI_WINXPSP3 /D_WIN32_WINNT=_WIN32_WINNT_WINXP"
    // --with-cxxflags="/DNTDDI_VERSION=NTDDI_WINXPSP3 /D_WIN32_WINNT=_WIN32_WINNT_WINXP"
#ifdef NTDDI_VERSION
#if NTDDI_VERSION >= 0x06000000
    if(!CreateSymbolicLink(const_cast<char*>(symlinkPathname.c_str()),
                           const_cast<char*>(origPathname.c_str()), true))
    {
        throw sys::SystemException(Ctxt(
            "Call to CreateSymbolicLink() has failed"));
    }
#else
    throw sys::SystemException(Ctxt(
        "Windows version does not support symlinks"));
#endif
#else
    // Don't think this should occur
    throw sys::SystemException(Ctxt(
            "NTDDI_VERSION macro not set to check Windows version"));
#endif
}

void sys::OSWin32::removeSymlink(const std::string& symlinkPathname) const
{
    if (RemoveDirectory(symlinkPathname.c_str()) != TRUE)
    {
        sys::Err err;
        std::ostringstream oss;
        oss << "Failure removing symlink [" <<  symlinkPathname <<
            "] with error [" << err.toString() << "]";

        throw except::Exception(Ctxt(oss.str()));
    }
}

void sys::OSWin32::getMemInfo(size_t& totalPhysMem, size_t& freePhysMem) const
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if(GlobalMemoryStatusEx(&memInfo))
    {
        totalPhysMem = memInfo.ullTotalPhys;
        freePhysMem = memInfo.ullAvailPhys;

        // convert to megabytes
        totalPhysMem /= (1024*1024);
        freePhysMem /= (1024*1024);
    }
    else
    {
        throw sys::SystemException(Ctxt(
            "Call to GlobalMemoryStatusEx() has failed"));
    }
}

std::string sys::OSWin32::getCurrentExecutable(
        const std::string& argvPathname_) const
{
    //XP doesn't always null-terminate the buffer, so taking some precautions
    char buffer[MAX_PATH + 2];
    memset(buffer, 0, MAX_PATH + 2);

    size_t bytesRead = GetModuleFileName(NULL, buffer, MAX_PATH + 1);

    if (bytesRead == MAX_PATH + 1 || bytesRead == 0)
    {
        // Path may be up to 32,767 characters, so take a more manual
        // approach instead of guess-and-checking our way up
        const auto argvPathname = AbstractOS::getArgvPathname(argvPathname_);
        return AbstractOS::getCurrentExecutable(argvPathname);
    }
    return std::string(buffer);
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
