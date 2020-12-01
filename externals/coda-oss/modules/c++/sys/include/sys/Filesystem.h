#pragma once

// For now, these are just wrappers around the CODA-OSS routines ... eventually, this should be replaced
// with C++17's <filesystem>

#include <string>

namespace sys
{
    namespace Filesystem
    {
        // http://en.cppreference.com/w/cpp/filesystem/path
        struct path
        {
            using string_type = std::string;
            using value_type = string_type::value_type;

            // http://en.cppreference.com/w/cpp/filesystem/path/path
            path() noexcept;
            path(const path&);
            path(const string_type&);
            path(const value_type*);

            path& operator/=(const path& p); // http://en.cppreference.com/w/cpp/filesystem/path/append
            path& operator/=(const value_type* p); // http://en.cppreference.com/w/cpp/filesystem/path/append
            path& operator/=(const string_type& p); // http://en.cppreference.com/w/cpp/filesystem/path/append

            void clear() noexcept; // http://en.cppreference.com/w/cpp/filesystem/path/clear

            // http://en.cppreference.com/w/cpp/filesystem/path/native
            const value_type* c_str() const noexcept;
            const string_type& native() const noexcept;
            operator string_type() const;

            std::string string() const; // http://en.cppreference.com/w/cpp/filesystem/path/string

            path parent_path() const; // http://en.cppreference.com/w/cpp/filesystem/path/parent_path
            path filename() const; // http://en.cppreference.com/w/cpp/filesystem/path/filename
            path stem() const; // http://en.cppreference.com/w/cpp/filesystem/path/stem

            bool empty() const noexcept; // http://en.cppreference.com/w/cpp/filesystem/path/empty
            bool is_absolute() const; // http://en.cppreference.com/w/cpp/filesystem/path/is_absrel
            bool is_relative() const; // http://en.cppreference.com/w/cpp/filesystem/path/is_absrel

        private:
            string_type p_;
        };

        path operator/(const path& lhs, const path& rhs); // http://en.cppreference.com/w/cpp/filesystem/path/operator_slash
		bool operator==(const path& lhs, const path& rhs) noexcept; // https://en.cppreference.com/w/cpp/filesystem/path/operator_cmp

        path absolute(const path&); // http://en.cppreference.com/w/cpp/filesystem/absolute
		bool create_directory(const path&); // https://en.cppreference.com/w/cpp/filesystem/create_directory
		path current_path(); // https://en.cppreference.com/w/cpp/filesystem/current_path
		bool remove(const path& p); // https://en.cppreference.com/w/cpp/filesystem/remove
		path temp_directory_path(); // https://en.cppreference.com/w/cpp/filesystem/temp_directory_path

		bool is_regular_file(const path& p); // https://en.cppreference.com/w/cpp/filesystem/is_regular_file
		bool is_directory(const path& p); // https://en.cppreference.com/w/cpp/filesystem/is_directory
        bool exists(const path& p ); // https://en.cppreference.com/w/cpp/filesystem/exists
    }
}

