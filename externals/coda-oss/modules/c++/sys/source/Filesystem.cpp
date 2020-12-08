#include "sys/Filesystem.h"

#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <array>
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "sys/Path.h"

namespace fs = sys::Filesystem;

static std::string strerror_(int errnum)
{
#ifdef _MSC_VER
    char errmsg[1024];
    strerror_s(errmsg, 1024, errnum);
    return errmsg;
#else
    return ::strerror(errnum);
#endif
}

static inline std::string make_what(const char* curfile, const int lineNum, const std::string& msg)
{
    std::ostringstream what;
    what << "ERROR: " << curfile << ", Line " << lineNum << ": " << msg;
    return what.str();
}
static inline std::string make_what(const char* curfile, const int lineNum, const std::ostringstream& msg)
{
    return make_what(curfile, lineNum, msg.str());
}
// A macro for conveniently throwing errors.
// Need "throw" to be visible, not hidden inside of a function, so that code-analysis tools can see it.
#define CODA_OSS_Filesystem_THROW_ERR(MSG) throw std::runtime_error(make_what(__FILE__, __LINE__, MSG))

fs::path::path() noexcept
{
}
fs::path::path(const path& p) : p_(p.native())
{
}
fs::path::path(const string_type& p) : p_(p)
{
}
fs::path::path(const value_type* p) : p_(p)
{
}

fs::path& fs::path::operator/=(const path& p)
{
    if (p.is_absolute())
    {
        p_ = p.native();  // "If p.is_absolute() ... replaces the current path with p ... http://en.cppreference.com/w/cpp/filesystem/path/append
    }
    else
    {
        // TODO: there is more to do here ... see http://en.cppreference.com/w/cpp/filesystem/path/append
        p_ = sys::Path::joinPaths(p_, p.native());
    }

    return *this;
}
fs::path& fs::path::operator/=(const string_type& p)
{
    return (*this) /= path(p);  // TODO: call sys::Path::joinPaths() directly?
}
fs::path& fs::path::operator/=(const value_type* p)
{
    return (*this) /= path(p);
}

void fs::path::clear() noexcept
{
    p_.clear();
}

const fs::path::value_type* fs::path::c_str() const noexcept
{
    return native().c_str();
}

const fs::path::string_type& fs::path::native() const noexcept
{
    return p_;
}

fs::path::operator string_type() const
{
    return string();
}

std::string fs::path::string() const
{
    return native();
}

fs::path fs::path::parent_path() const
{
    return sys::Path::splitPath(native()).first;
}

fs::path fs::path::filename() const
{
    return sys::Path::basename(native(), false /*rmvExt*/);
}

fs::path fs::path::stem() const
{
    return sys::Path::basename(native(), true /*rmvExt*/);
}

fs::path fs::path::extension() const
{
    // https://en.cppreference.com/w/cpp/filesystem/path/extension

    // "If the pathname is either . or .., ... then empty path is returned."
    if ((native() == ".") || (native() == ".."))
    {
        return "";
    }

    auto fn = filename().string();
    const auto dot = fn.find(".");

    // "If ... filename() does not contain the . character, then empty path is returned."
    if (dot == std::string::npos)
    {
        return "";
    }

    // "If the first character in the filename is a period, that period is ignored
    // (a filename like '.profile' is not treated as an extension)"
    if (dot == 0)
    {
        fn = fn.substr(1);
    }

    return sys::Path::splitExt(fn).second;
}

bool fs::path::empty() const noexcept
{
    return native().empty();
}

bool fs::path::is_absolute() const
{
    return sys::Path::isAbsolutePath(native());
}
bool fs::path::is_relative() const
{
    return !is_absolute();  // "... the other way round." http://en.cppreference.com/w/cpp/filesystem/path/is_absrel
}

fs::path fs::operator/(const fs::path& lhs, const fs::path& rhs)
{
    return fs::path(lhs) /= rhs;  // "... returns path(lhs) /= rhs." http://en.cppreference.com/w/cpp/filesystem/path/operator_slash
}

fs::path fs::absolute(const path& p)
{
    return sys::Path::absolutePath(p.native());
}

fs::path fs::temp_directory_path()
{
    // https://en.cppreference.com/w/cpp/filesystem/temp_directory_path
    //
    // On POSIX systems, the path may be the one specified in the environment variables TMPDIR, TMP, TEMP, TEMPDIR, and, if none of them are
    // specified, the path "/tmp" is returned.
    //
    // On Windows systems, the path is typically the one returned by GetTempPath
#if defined(_WIN32)

    // https://msdn.microsoft.com/en-us/library/b0084kay.aspx
    // "_WIN32 Defined as 1 when the compilation target is 32 - bit ARM, 64 - bit ARM, x86, or x64.Otherwise, undefined."

    // https://docs.microsoft.com/en-us/windows/desktop/api/fileapi/nf-fileapi-gettemppatha
    std::array<CHAR, MAX_PATH + 2> buf;  // "The maximum possible return value is MAX_PATH+1 (261)."
    const auto result = GetTempPathA(static_cast<DWORD>(buf.size()), buf.data());
    if (result == 0)  // "If the function fails, the return value is zero"
    {
        throw std::runtime_error("GetTempPathA() failed.");  // TODO: std::filesystem_error
    }

    return buf.data();

#else  // assume Linux

    // https://stackoverflow.com/questions/31068/how-do-i-find-the-temp-directory-in-linux
    const std::vector<const char*> tmpdirs{"TMPDIR", "TMP", "TEMP", "TEMPDIR"};
    for (const auto& dir : tmpdirs)
    {
        if (auto result = getenv(dir))
        {
            return result;
        }
    }

    return "/tmp";
#endif
}

bool fs::create_directory(const path& p)
{
    const sys::OS os;
    const bool created = os.makeDirectory(p);
    if (created)
    {
        // If it exists, see if it's a file
        if (os.isFile(p))
        {
            const std::string stat_failed("stat failed: " + p.string());
            const std::string error("Error:\n" + strerror_(errno));
            CODA_OSS_Filesystem_THROW_ERR(stat_failed + "\n" + error);
        }
        else if (!os.isDirectory(p))
        {
            std::stringstream ss;
            ss << "Path '" << p.string() << "' exists and is not a directory.";
            CODA_OSS_Filesystem_THROW_ERR(ss.str());
        }

        // If we got here, the path exists and is a directory, which is what we want
    }
    return created;
}

bool fs::is_regular_file(const path& p)
{
    const sys::OS os;
    return os.isFile(p);
}

bool fs::is_directory(const path& p)
{
    const sys::OS os;
    return os.isDirectory(p);
}

bool fs::exists(const path& p)
{
    const sys::OS os;
    return os.exists(p);
}

bool fs::remove(const path& p)
{
    // https://en.cppreference.com/w/cpp/io/c/remove
    return ::remove(p.c_str()) == 0;  // "0 upon success or non-zero value on error."
}

fs::path fs::current_path()
{
    // https://en.cppreference.com/w/cpp/filesystem/current_path

    // https://stackoverflow.com/questions/2203159/is-there-a-c-equivalent-to-getcwd
    char temp[1024];
    return
#ifdef _WIN32
            _getcwd
#else
            getcwd
#endif
            (temp, sizeof(temp))
            ? std::string(temp)
            : std::string("");
}

bool fs::operator==(const path& lhs, const path& rhs) noexcept
{
    return sys::Path::normalizePath(lhs) == sys::Path::normalizePath(rhs);
}
bool fs::operator!=(const path& lhs, const path& rhs) noexcept
{
    return !(lhs == rhs);
}

std::ostream& fs::operator<<(std::ostream& os, const path& p)
{
    // https://en.cppreference.com/w/cpp/filesystem/path/operator_ltltgtgt
    os << "\"" << p.string() << "\"";
    return os;
}