/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
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

#include <assert.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "config/compiler_extensions.h"
#include "sys/Path.h"
#include "str/Manip.h"

sys::File sys::make_File(const coda_oss::filesystem::path& path, int accessFlags, int creationFlags)
{
    sys::File retval(std::nothrow, path, accessFlags, creationFlags);
    if (retval.isOpen())
    {
        return retval;
    }

    const auto checkIfExists = (creationFlags & sys::File::EXISTING) == sys::File::EXISTING;
    auto expanded = sys::Path::expandEnvironmentVariables(path.string(), checkIfExists);
    if (expanded.empty())
    {
        expanded = path.string(); // Throw exception with non-empty path.
    }
    return sys::File(expanded, accessFlags, creationFlags);
}

sys::File sys::make_File(const coda_oss::filesystem::path& parent, const coda_oss::filesystem::path& name,
        int accessFlags, int creationFlags)
{
    sys::File retval(std::nothrow, parent, name, accessFlags, creationFlags);
    if (retval.isOpen())
    {
        return retval;
    }

    const auto expanded_parent = sys::Path::expandEnvironmentVariables(parent.string(), coda_oss::filesystem::file_type::directory);
    // 'name' probably won't work without 'parent' so no need to checkIfExists
    const auto expanded_name =  sys::Path::expandEnvironmentVariables(name.string(), false /*checkIfExists*/);

    // let the File constructor deal with combining the expanded paths as well as checking for existence.
    return sys::File(expanded_parent, expanded_name, accessFlags, creationFlags);
}

#ifdef _WIN32
// '...': This function or variable may be unsafe. Consider using _sopen_s instead.
static FILE* fopen_(const std::string& fname, const std::string& mode)
{
    FILE* retval = nullptr;
    const auto result = fopen_s(&retval, fname.c_str(), mode.c_str());
    if (result != 0) // "Zero if successful; ..."
    {
        return nullptr;
    }
    return retval;
}
#else
static inline FILE* fopen_(const std::string& fname, const std::string& mode)
{
    return fopen(fname.c_str(), mode.c_str());
}
#endif

FILE* sys::fopen(const coda_oss::filesystem::path& fname, const std::string& mode)
{
    // Call  sys::expandEnvironmentVariables() if the initial fopen() fails.
    auto retval = fopen_(fname.string(), mode);
    if (retval != nullptr)
    {
        return retval;
    }

    const auto r_pos = mode.find('r');
    const auto checkIfExists = r_pos != mode.npos;
    const auto expanded = sys::Path::expandEnvironmentVariables(fname.string(), checkIfExists);
    if (expanded.empty())
    {
        return nullptr; // no need to even try fopen()
    }
    return fopen_(expanded, mode);
}

#ifdef _WIN32
#define CODA_OSS_open ::_open
#else
#define CODA_OSS_open ::open
#endif

static inline int open_(const std::string& pathname, int flags)
{
    const auto p = pathname.c_str();
    CODA_OSS_disable_warning_push
    #ifdef _MSC_VER
    #pragma warning(disable: 4996) // '...': This function or variable may be unsafe. Consider using _sopen_s instead.
    #endif
    return CODA_OSS_open(p, flags);
    CODA_OSS_disable_warning_pop
}
int sys::open(const coda_oss::filesystem::path& path, int flags)
{
    // Call  sys::expandEnvironmentVariables() if the initial open() fails.
    const auto retval = open_(path.string(), flags);
    if (retval > -1)  // "On error, -1 is returned ..."
    {
        return retval;
    }

    constexpr bool checkIfExists = false; // TODO: look for O_CREAT ?
    const auto expanded = sys::Path::expandEnvironmentVariables(path.string(), checkIfExists);
    if (expanded.empty())
    {
        return retval;  // no need to even try another open()
    }
    return open_(expanded, flags);
}

static inline int open_(const std::string& pathname, int flags, int mode)
{
    const auto p = pathname.c_str();
    CODA_OSS_disable_warning_push
    #ifdef _MSC_VER
    #pragma warning(disable: 4996) // '...': This function or variable may be unsafe. Consider using _sopen_s instead.
    #endif
    return CODA_OSS_open(p, flags, mode);
    CODA_OSS_disable_warning_pop
}
int sys::open(const coda_oss::filesystem::path& path, int flags, int mode)
{
    // Call  sys::expandEnvironmentVariables() if the initial open() fails.
    const auto retval = open_(path.string(), flags, mode);
    if (retval > -1)  // "On error, -1 is returned ..."
    {
        return retval;
    }

    constexpr bool checkIfExists = false; // TODO: look for O_CREAT ?
    const auto expanded = sys::Path::expandEnvironmentVariables(path.string(), checkIfExists);
    if (expanded.empty())
    {
        return retval;  // no need to even try another open()
    }
    return open_(expanded, flags, mode);
}
#undef CODA_OSS_open

#ifdef _WIN32
#define CODA_OSS_close ::_close
#else
#define CODA_OSS_close ::close
#endif
int sys::close(int fd)
{
  return CODA_OSS_close(fd);
}
#undef CODA_OSS_close

#ifdef _WIN32
#define CODA_OSS_stat_ ::_stat
#else
#define CODA_OSS_stat_ ::stat
#endif
static inline int stat_(const std::string& pathname, struct CODA_OSS_stat  &buffer)
{
    const auto p = pathname.c_str();
    CODA_OSS_disable_warning_push
    #ifdef _MSC_VER
    #pragma warning(disable: 4996) // '...': This function or variable may be unsafe. Consider using _sopen_s instead.
    #endif
    return CODA_OSS_stat_(p, &buffer);
    CODA_OSS_disable_warning_pop
}
#undef CODA_OSS_stat_
int sys::stat(const coda_oss::filesystem::path& path, struct CODA_OSS_stat &buffer)
{
    // Call  sys::expandEnvironmentVariables() if the initial stat() fails.
    const auto retval = stat_(path.string(), buffer);
    if (retval > -1)  // "On error, -1 is returned ..."
    {
        return retval;
    }

    constexpr bool checkIfExists = true;
    const auto expanded = sys::Path::expandEnvironmentVariables(path.string(), checkIfExists);
    if (expanded.empty())
    {
        return retval;  // no need to even try another stat()
    }
    return stat_(expanded, buffer);
}

void sys::open(std::ifstream& ifs, const coda_oss::filesystem::path& path, std::ios_base::openmode mode)
{
    // Call  sys::expandEnvironmentVariables() if the initial open() fails.
    ifs.open(path.string(), mode);
    if (ifs.is_open())
    {
        return;
    }

    const auto checkIfExists = (mode & std::ios_base::in) == std::ios_base::in;
    auto expanded = sys::Path::expandEnvironmentVariables(path.string(), checkIfExists);
    if (expanded.empty())
    {
        expanded = path.string();  // Throw exception with non-empty path.
    }
    ifs.open(expanded, mode);
}
std::ifstream sys::make_ifstream(const coda_oss::filesystem::path& path, std::ios_base::openmode mode)
{
    std::ifstream retval;
    open(retval, path, mode);
    return retval;
}
