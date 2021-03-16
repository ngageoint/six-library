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

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include <sstream>
#include <vector>
#include <set>
#include <fstream>

#include "sys/Conf.h"

#if !(defined(WIN32) || defined(_WIN32))

#include <unistd.h>

#if defined(__APPLE__)

#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>

#endif

#include "sys/OSUnix.h"
#include "sys/File.h"
#include "sys/ScopedCPUAffinityUnix.h"
#include "str/Tokenizer.h"


namespace
{
std::string readLink(const std::string& pathname)
{
    char buffer[PATH_MAX];

    // readlink does not null-terminate anything
    memset(buffer, 0, PATH_MAX);

    if (readlink(pathname.c_str(), buffer, PATH_MAX) == -1)
    {
        throw except::Exception(Ctxt(strerror(errno)));
    }

    return std::string(buffer);
}

class CharWrapper
{
public:
    CharWrapper(char* array):
        mArray(array)
    {
    }
    ~CharWrapper()
    {
        free(mArray);
    }
    inline const char* get() const
    {
        return mArray;
    }
private:
    char* mArray;
};

std::set<std::string> get_unique_thread_siblings()
{
    // Our goal is to count the number of unique entries that occur
    // in the files /sys/devices/system/cpu/cpu*/topology/thread_siblings_list
    const sys::Path sysCPUPath("/sys/devices/system/cpu");
    if (!sysCPUPath.isDirectory())
    {
        throw except::Exception(
                Ctxt("Expected dir /sys/devices/system/cpu does not exist"));
    }

    const std::vector<std::string> searchPaths(1, sysCPUPath.getPath());
    const std::vector<std::string> subDirs =
        sys::FileFinder::search(
            sys::DirectoryOnlyPredicate(),
            searchPaths,
            false);

    std::set<std::string> unique_ts;
    for (std::vector<std::string>::const_iterator ii = subDirs.begin();
         ii != subDirs.end();
         ++ii)
    {
        const sys::Path tsPath(*ii, "topology/thread_siblings_list");
        if (tsPath.exists())
        {
            std::ifstream tsIFS(tsPath.getPath().c_str());
            if (!tsIFS.is_open())
            {
                std::ostringstream msg;
                msg << "Unable to open thread siblings file "
                    << tsPath.getPath();
                throw except::Exception(Ctxt(msg.str()));
            }

            std::string tsContents;
            tsIFS >> tsContents;
            tsIFS.close();

            unique_ts.insert(tsContents);
        }
    }

    return unique_ts;
}
}

std::string sys::OSUnix::getPlatformName() const
{
    struct utsname name;
    if (uname(&name) == -1)
        throw sys::SystemException("Uname failed");

    return FmtX("%s (%s): %s [build: %s]", name.sysname, name.machine,
                name.release, name.version);
}

std::string sys::OSUnix::getNodeName() const
{
    struct utsname name;
    if (uname(&name) == -1)
        throw sys::SystemException("Uname failed");

    return std::string(name.nodename);
}


bool sys::OSUnix::exists(const std::string& path) const
{
    struct stat info;
    if (stat(path.c_str(), &info) == -1)
        return false;
    return true;
}

void sys::OSUnix::removeFile(const std::string& pathname) const
{
    if (::unlink(pathname.c_str()) != 0)
    {
        sys::Err err;
        std::ostringstream oss;
        oss << "Failure removing file [" <<  pathname <<
            "] with error [" << err.toString() << "]";

        throw except::Exception(Ctxt(oss.str()));
    }
}

void sys::OSUnix::removeDirectory(const std::string& pathname) const
{
    if (::rmdir(pathname.c_str()) != 0)
    {
        sys::Err err;
        std::ostringstream oss;
        oss << "Failure removing directory [" <<  pathname <<
            "] with error [" << err.toString() << "]";

        throw except::Exception(Ctxt(oss.str()));
    }
}

bool sys::OSUnix::move(const std::string& path,
                       const std::string& newPath) const
{
    return (::rename(path.c_str(), newPath.c_str()) == 0);
}

sys::Pid_T sys::OSUnix::getProcessId() const
{
    return ::getpid();
}

bool sys::OSUnix::makeDirectory(const std::string& path) const
{
    if (::mkdir(path.c_str(), 0777) == 0)
        return true;
    return false;
}

bool sys::OSUnix::isFile(const std::string& path) const
{
    struct stat info;
    if (stat(path.c_str(), &info) == -1)
        return false;
//        throw sys::SystemException("Stat failed");
    return (S_ISREG(info.st_mode)) ? (true) : (false);
}

bool sys::OSUnix::isDirectory(const std::string& path) const
{
    struct stat info;
    if (stat(path.c_str(), &info) == -1)
        return false;
//        throw sys::SystemException("Stat failed");
    return (S_ISDIR(info.st_mode)) ? (true) : (false);
}

std::string sys::OSUnix::getCurrentWorkingDirectory() const
{
    char buffer[PATH_MAX];
    if (!getcwd(buffer, PATH_MAX))
        throw sys::SystemException("Getcwd() failed");
    return std::string(buffer);
}

bool sys::OSUnix::changeDirectory(const std::string& path) const
{
    return chdir(path.c_str()) == 0 ? true : false;
}

std::string sys::OSUnix::getTempName(const std::string& path,
                                     const std::string& prefix) const
{
    std::string name;
#if defined(_USE_MKSTEMP) || defined(__linux__) || defined(__linux) || defined(linux__)
    std::string pathname(path);
    pathname += "/" + prefix + "XXXXXX";
    std::vector<char> fullPath(pathname.size() + 1);
    strcpy(&fullPath[0], pathname.c_str());
    int ret = mkstemp(&fullPath[0]);
    if (ret == -1) name = "";
    else
    {
        name = &fullPath[0];
    }
#else
    CharWrapper tempname = tempnam(path.c_str(), prefix.c_str());
    if (tempname.get() == NULL)
        name = "";
    else
    {
        name = tempname.get();
        sys::File (name, sys::File::WRITE_ONLY, sys::File::CREATE);
    }
#endif
    if (name.empty())
    {
        throw except::Exception(Ctxt("Unable to create a temporary file"));
    }
    return name;
}

sys::Off_T sys::OSUnix::getSize(const std::string& path) const
{
    return sys::File(path).length();
}

sys::Off_T sys::OSUnix::getLastModifiedTime(const std::string& path) const
{
    return sys::File(path).lastModifiedTime();
}

void sys::OSUnix::millisleep(int milliseconds) const
{
    usleep(milliseconds * 1000);
}
std::string sys::OSUnix::getDSOSuffix() const
{
    return "so";
}

std::string sys::OSUnix::operator[](const std::string& s) const
{
    return getEnv(s);
}

std::string sys::OSUnix::getEnv(const std::string& s) const
{
    const char* envVal = getenv(s.c_str());
    if (envVal == NULL)
        throw sys::SystemException(
            Ctxt("Unable to get unix environment variable " + s));
    return std::string(envVal);
}

bool sys::OSUnix::isEnvSet(const std::string& s) const
{
    const char* envVal = getenv(s.c_str());
    return (envVal != NULL);
}

void sys::OSUnix::setEnv(const std::string& var,
                         const std::string& val,
                         bool overwrite)
{
    int ret;

#if CODA_OSS_POSIX2001_SOURCE
    // https://man7.org/linux/man-pages/man3/setenv.3.html
    ret = setenv(var.c_str(), val.c_str(), overwrite);
#else
    // putenv() will overwrite the value if it already exists, so if we don't
    // want to overwrite, we do nothing when getenv() indicates the variable's
    // already set
    if (overwrite || getenv(var.c_str()) == NULL)
    {
        // putenv() isn't guaranteed to make a copy of the string, so we need
        // to allocate it and let it leak.  Ugh.
        char* const strBuffer = new char[var.length() + 1 + val.length() + 1];
        ::sprintf(strBuffer, "%s=%s", var.c_str(), val.c_str());
        ret = putenv(strBuffer);
    }
    else
    {
        ret = 0;
    }
#endif
    if(ret != 0)
    {
        throw sys::SystemException(Ctxt(
                "Unable to set unix environment variable " + var));
    }
}

void sys::OSUnix::unsetEnv(const std::string& var)
{
    const int ret = unsetenv(var.c_str());
    // by definition, unsetenv does not consider a missing environment variable
    // to be an error condition, so this should only throw if the environment
    // variable could not be changed
    if (ret == -1)
    {
      throw sys::SystemException(Ctxt("Unable to unset unix environment variable " + var));
    }
}

size_t sys::OSUnix::getNumCPUs() const
{
    return static_cast<size_t>(ScopedCPUMaskUnix::getNumOnlineCPUs());
}

size_t sys::OSUnix::getNumCPUsAvailable() const
{
    const ScopedCPUAffinityUnix mask;
    return CPU_COUNT_S(mask.getSize(), mask.getMask());
}

size_t sys::OSUnix::getNumPhysicalCPUs() const
{
    return get_unique_thread_siblings().size();
}

size_t sys::OSUnix::getNumPhysicalCPUsAvailable() const
{
    std::vector<int> physicalCPUs;
    std::vector<int> htCPUs;
    getAvailableCPUs(physicalCPUs, htCPUs);
    return physicalCPUs.size();
}

void sys::OSUnix::getAvailableCPUs(std::vector<int>& physicalCPUs,
                                   std::vector<int>& htCPUs) const
{
    physicalCPUs.clear();
    htCPUs.clear();

    // Obtain scheduling affinity for all CPUs (including hyperthreading)
    const ScopedCPUAffinityUnix mask;

    // Cross-reference the thread siblings with active CPUs
    // and separate into physical CPUs and HT CPUs. At the hardware level there
    // is no distinction as to which CPU is the physical and which is the HT.
    // So, we'll just say that the first masked CPU ID encountered for a core
    // is the physical CPU, and the remainder of masked CPU IDs are the HT CPUs.
    const std::set<std::string> unique_ts(get_unique_thread_siblings());
    std::set<std::string>::const_iterator tsStr;
    for (tsStr = unique_ts.begin(); tsStr != unique_ts.end(); ++tsStr)
    {
        bool foundPhysical = false;
        const str::Tokenizer::Tokens cpuIDs = str::Tokenizer(*tsStr, ",");
        for (str::Tokenizer::Tokens::const_iterator cpu = cpuIDs.begin();
             cpu != cpuIDs.end();
             ++cpu)
        {
            const int cpuInt = str::toType<int>(*cpu);
            if (CPU_ISSET_S(cpuInt, mask.getSize(), mask.getMask()))
            {
                if (foundPhysical)
                {
                    htCPUs.push_back(cpuInt);
                }
                else
                {
                    physicalCPUs.push_back(cpuInt);
                    foundPhysical = true;
                }
            }
        }
    }
}

void sys::OSUnix::createSymlink(const std::string& origPathname,
                                const std::string& symlinkPathname) const
{
    if(symlink(origPathname.c_str(), symlinkPathname.c_str()))
    {
        throw sys::SystemException(Ctxt(
                "Symlink creation has failed"));
    }
}

void sys::OSUnix::removeSymlink(const std::string& symlinkPathname) const
{
	if (::unlink(symlinkPathname.c_str()) != 0)
	{
		sys::Err err;
		std::ostringstream oss;
		oss << "Failure removing symlink [" <<  symlinkPathname <<
			"] with error [" << err.toString() << "]";

		throw except::Exception(Ctxt(oss.str()));
	}
}

size_t sysconfCaller(int name)
{
    long long returnVal = sysconf(name);
    if(returnVal == -1){
        throw sys::SystemException(Ctxt(
                "Call to sysconf() has failed"));
    }
    return returnVal;
}

void sys::OSUnix::getMemInfo(size_t &totalPhysMem, size_t &freePhysMem) const
{
    // Unfortunately sysctl is the best way to do this on OSX,
    // but sysctl is deprecated in favor of sysconf on linux
#if defined(__APPLE__)
    long long physMem = 0;
    size_t size = sizeof(physMem);
    int status = sysctlbyname("hw.memsize", &physMem, &size, 0, 0);
    if(status)
    {
        throw sys::SystemException(Ctxt("Call to sysctl() has failed"));
    }

    mach_port_t            machPort = mach_host_self();
    mach_msg_type_number_t count     = HOST_VM_INFO_COUNT;
    vm_size_t              pageSize = 0;
    vm_statistics_data_t   vmstat;

    if(KERN_SUCCESS != host_statistics(machPort, HOST_VM_INFO,
                (host_info_t) &vmstat, &count))
    {
        throw sys::SystemException(Ctxt("Call to host_statistics() has failed"));
    }

    if(KERN_SUCCESS != host_page_size(machPort, &pageSize))
    {
        throw sys::SystemException(Ctxt("Call to host_page_size has failed"));
    }

    long long freeBytes = vmstat.free_count * pageSize;

    totalPhysMem = physMem / 1024 / 1024;
    freePhysMem = freeBytes / 1024 / 1024;

#else
    long long pageSize = sysconfCaller(_SC_PAGESIZE);
    long long totalNumPages = sysconfCaller(_SC_PHYS_PAGES);
    long long availNumPages = sysconfCaller(_SC_AVPHYS_PAGES);

    totalPhysMem = (pageSize*totalNumPages/1024)/1024;
    freePhysMem = (pageSize*availNumPages/1024)/1024;

#endif
}

std::string sys::OSUnix::getCurrentExecutable(
        const std::string& argvPathname_) const
{
    std::vector<std::string> possibleSymlinks;

    // Linux
    possibleSymlinks.push_back(sys::Path::joinPaths(
            sys::Path::delimiter()[0] + std::string("proc"),
            sys::Path::joinPaths("self", "exe")));

    // Solaris
    possibleSymlinks.push_back(sys::Path::joinPaths(
            sys::Path::delimiter()[0] + std::string("proc"),
            sys::Path::joinPaths("self",
            sys::Path::joinPaths("path", "a.out"))));

    for (size_t ii = 0; ii < possibleSymlinks.size(); ++ii)
    {
        const std::string pathname = possibleSymlinks[ii];
        if (!isFile(pathname))
        {
            continue;
        }
        const std::string executableName = readLink(pathname);

        if (isFile(executableName))
        {
            return executableName;
        }
    }

    const auto argvPathname = AbstractOS::getArgvPathname(argvPathname_);
    return AbstractOS::getCurrentExecutable(argvPathname);
}

void sys::DirectoryUnix::close()
{
    if (mDir)
    {
        closedir( mDir);
        mDir = NULL;
    }
}
std::string sys::DirectoryUnix::findFirstFile(const std::string& dir)
{
    // First file is always . on Unix
    mDir = ::opendir(dir.c_str());
    if (mDir == NULL)
        return "";
    return findNextFile();
}

std::string sys::DirectoryUnix::findNextFile()
{
    struct dirent* entry = NULL;
    entry = ::readdir(mDir);
    if (entry == NULL)
        return "";
    return entry->d_name;
}

#endif
