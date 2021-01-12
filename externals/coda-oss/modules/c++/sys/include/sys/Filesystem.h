#ifndef CODA_OSS_sys_Filesystem_h_INCLUDED_
#define CODA_OSS_sys_Filesystem_h_INCLUDED_
#pragma once

//
// For now, these are just some wrappers around the CODA-OSS routines ...
// eventually, this should be replaced with C++17's <filesystem>.
//
// This does not even TRY to be a complete implementation of std::filesystem.
//

#include <string>
#include <ostream>

#include "Conf.h"

namespace sys
{
// http://en.cppreference.com/w/cpp/filesystem
namespace Filesystem
{
  struct path; // forward
  namespace details
  {
    extern bool Equals(const path& lhs, const path& rhs) noexcept;
    extern std::ostream& Ostream(std::ostream& os, const path& p);
  }

// http://en.cppreference.com/w/cpp/filesystem/path
struct path // N.B. this is an INCOMPLETE implementation!
{
    using string_type = std::string;
    using value_type = string_type::value_type;

    // http://en.cppreference.com/w/cpp/filesystem/path/path
    path() noexcept;
    path(const path&);
    path(const string_type&);
    path(const value_type*);

    path& operator/=(const path&);  // http://en.cppreference.com/w/cpp/filesystem/path/append
    path& operator/=(const value_type*);  // http://en.cppreference.com/w/cpp/filesystem/path/append
    path& operator/=(const string_type&);  // http://en.cppreference.com/w/cpp/filesystem/path/append

    void clear() noexcept;  // http://en.cppreference.com/w/cpp/filesystem/path/clear

    // http://en.cppreference.com/w/cpp/filesystem/path/native
    const value_type* c_str() const noexcept;
    const string_type& native() const noexcept;
    operator string_type() const;

    std::string string() const;  // http://en.cppreference.com/w/cpp/filesystem/path/string

    path parent_path() const;  // http://en.cppreference.com/w/cpp/filesystem/path/parent_path
    path filename() const;  // http://en.cppreference.com/w/cpp/filesystem/path/filename
    path stem() const;  // http://en.cppreference.com/w/cpp/filesystem/path/stem
    path extension() const;  // https://en.cppreference.com/w/cpp/filesystem/path/extension

    bool empty() const noexcept;  // http://en.cppreference.com/w/cpp/filesystem/path/empty
    bool is_absolute() const;  // http://en.cppreference.com/w/cpp/filesystem/path/is_absrel
    bool is_relative() const;  // http://en.cppreference.com/w/cpp/filesystem/path/is_absrel

    friend bool operator==(const path& lhs, const path& rhs) noexcept  // https://en.cppreference.com/w/cpp/filesystem/path/operator_cmp
    {
      return details::Equals(lhs, rhs);
    }
    friend bool operator!=(const path& lhs, const path& rhs) noexcept  // https://en.cppreference.com/w/cpp/filesystem/path/operator_cmp
    {
      return !(lhs == rhs);
    }
    friend std::ostream& operator<<(std::ostream& os, const path& p) // https://en.cppreference.com/w/cpp/filesystem/path/operator_ltltgtgt
    {
      return details::Ostream(os, p);
    }

private:
    string_type p_;
};

path operator/(const path& lhs, const path& rhs);  // http://en.cppreference.com/w/cpp/filesystem/path/operator_slash

path absolute(const path&);  // http://en.cppreference.com/w/cpp/filesystem/absolute
bool create_directory(const path&);  // https://en.cppreference.com/w/cpp/filesystem/create_directory
path current_path();  // https://en.cppreference.com/w/cpp/filesystem/current_path
bool remove(const path& p);  // https://en.cppreference.com/w/cpp/filesystem/remove
path temp_directory_path();  // https://en.cppreference.com/w/cpp/filesystem/temp_directory_path

bool is_regular_file(const path& p);  // https://en.cppreference.com/w/cpp/filesystem/is_regular_file
bool is_directory(const path& p);  // https://en.cppreference.com/w/cpp/filesystem/is_directory
bool exists(const path& p);  // https://en.cppreference.com/w/cpp/filesystem/exists
}
}

#ifndef CODA_OSS_DEFINE_std_filesystem_
    // Some versions of G++ say they're C++17 but don't have <filesystem>
    #if CODA_OSS_cpp17 && __has_include(<filesystem>)  // __has_include is C++17
        #define CODA_OSS_DEFINE_std_filesystem_ -1  // OK to #include <>, below
    #else
        #define CODA_OSS_DEFINE_std_filesystem_ CODA_OSS_AUGMENT_std_namespace  // maybe use our own
    #endif
#endif  // CODA_OSS_DEFINE_std_filesystem_


#if CODA_OSS_DEFINE_std_filesystem_ == 1
    namespace std // This is slightly uncouth: we're not supposed to augment "std".
    {
        namespace filesystem = ::sys::Filesystem;
    }
#elif CODA_OSS_DEFINE_std_filesystem_ == -1 // set above
    #include <filesystem>
#endif // CODA_OSS_DEFINE_std_filesystem_

#endif  // CODA_OSS_sys_Filesystem_h_INCLUDED_
