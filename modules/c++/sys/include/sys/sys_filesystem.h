/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2020, Maxar Technologies, Inc.
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
 * License along with this program; If not, http://www.gnu.org/licenses/.
 *
 */
#ifndef CODA_OSS_sys_sys_filesystem_h_INCLUDED_
#define CODA_OSS_sys_sys_filesystem_h_INCLUDED_
#pragma once

//
// For now, these are just some wrappers around the CODA-OSS routines ...
// eventually, this should be replaced with C++17's <filesystem>.
//
// This does not even TRY to be a complete implementation of std::filesystem.
//

#include <string>
#include <ostream>
#include <cstdint>

#include "sys/CPlusPlus.h"
#include "coda_oss/namespace_.h" // get something in the "coda_oss" namespace
#include "config/Exports.h"

namespace sys // should be in coda_oss/, but implementation needs sys::Path
{
// http://en.cppreference.com/w/cpp/filesystem
namespace filesystem // not "Filesystem", make it easy to use code in other namespaces
{
  struct path; // forward
  namespace details
  {
      CODA_OSS_API bool Equals(const path& lhs, const path& rhs) noexcept;
      CODA_OSS_API std::ostream& Ostream(std::ostream& os, const path& p);
  }

  // https://en.cppreference.com/w/cpp/filesystem/file_type
  enum class file_type // match C++17 spelling for easier transition from our C++11 work-around
  {
      none = 0,
      not_found = 1,
      regular,
      directory,
      unknown 
  };

// http://en.cppreference.com/w/cpp/filesystem/path
struct CODA_OSS_API path final // N.B. this is an INCOMPLETE and NON-STANDARD implementation!
{
    // character type used by the native encoding of the filesystem: char on POSIX, wchar_t on Windows
    #ifdef _WIN32
    using value_type = wchar_t;
    #else
    using value_type = char;
    #endif
    using string_type = std::basic_string<value_type>;

    // http://en.cppreference.com/w/cpp/filesystem/path/path
    path() noexcept;
    path(const path&);
    path(const string_type&);
    template<typename TSource>
    path(const TSource& source)
    {
      *this = source;
    }
    ~path() = default;

    path& operator=(const path&) = default;
    path& operator=(path&&) = default;
    template<typename TSource>
    path& operator=(const TSource& source)
    {
        p_ = to_native(source);
	return *this;
    }

    path& operator/=(const path&);  // http://en.cppreference.com/w/cpp/filesystem/path/append
    template <typename TSource>
    path& operator/=(const TSource& source)  // http://en.cppreference.com/w/cpp/filesystem/path/append
    {
        return (*this) /= path(to_native(source));
    }
    void clear() noexcept;  // http://en.cppreference.com/w/cpp/filesystem/path/clear

    // http://en.cppreference.com/w/cpp/filesystem/path/native
    const value_type* c_str() const noexcept;
    const string_type& native() const noexcept;
    operator string_type() const;

    std::string string() const;  // http://en.cppreference.com/w/cpp/filesystem/path/string

    path root_path() const; // https://en.cppreference.com/w/cpp/filesystem/path/root_path
    path parent_path() const;  // http://en.cppreference.com/w/cpp/filesystem/path/parent_path
    path filename() const;  // http://en.cppreference.com/w/cpp/filesystem/path/filename
    path stem() const;  // http://en.cppreference.com/w/cpp/filesystem/path/stem
    path extension() const;  // https://en.cppreference.com/w/cpp/filesystem/path/extension

    bool empty() const noexcept;  // http://en.cppreference.com/w/cpp/filesystem/path/empty
    bool is_absolute() const;  // http://en.cppreference.com/w/cpp/filesystem/path/is_absrel
    bool is_relative() const;  // http://en.cppreference.com/w/cpp/filesystem/path/is_absrel

    CODA_OSS_API friend bool operator==(const path& lhs, const path& rhs) noexcept  // https://en.cppreference.com/w/cpp/filesystem/path/operator_cmp
    {
      return details::Equals(lhs, rhs);
    }
    CODA_OSS_API friend bool operator!=(const path& lhs, const path& rhs) noexcept  // https://en.cppreference.com/w/cpp/filesystem/path/operator_cmp
    {
      return !(lhs == rhs);
    }
    CODA_OSS_API friend std::ostream& operator<<(std::ostream& os, const path& p) // https://en.cppreference.com/w/cpp/filesystem/path/operator_ltltgtgt
    {
      return details::Ostream(os, p);
    }

private:
    string_type p_;
    static string_type to_native(const std::string& s);
};

CODA_OSS_API path operator/(const path& lhs, const path& rhs);  // http://en.cppreference.com/w/cpp/filesystem/path/operator_slash

CODA_OSS_API path absolute(const path&);  // http://en.cppreference.com/w/cpp/filesystem/absolute
CODA_OSS_API bool create_directory(const path&);  // https://en.cppreference.com/w/cpp/filesystem/create_directory
CODA_OSS_API path current_path();  // https://en.cppreference.com/w/cpp/filesystem/current_path
CODA_OSS_API bool remove(const path& p);  // https://en.cppreference.com/w/cpp/filesystem/remove
CODA_OSS_API path temp_directory_path();  // https://en.cppreference.com/w/cpp/filesystem/temp_directory_path

CODA_OSS_API bool is_regular_file(const path& p);  // https://en.cppreference.com/w/cpp/filesystem/is_regular_file
CODA_OSS_API bool is_directory(const path& p);  // https://en.cppreference.com/w/cpp/filesystem/is_directory
CODA_OSS_API bool exists(const path& p);  // https://en.cppreference.com/w/cpp/filesystem/exists
CODA_OSS_API std::uintmax_t file_size(const path& p); // https://en.cppreference.com/w/cpp/filesystem/file_size

}
}

#endif  // CODA_OSS_sys_sys_filesystem_h_INCLUDED_
