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

#include <algorithm>

#include <sys/Path.h>
#include <sys/Filesystem.h>

namespace fs = sys::Filesystem;

namespace sys
{
Path::Path()
{
}

Path::Path(const Path& parent, const std::string& child) :
    mPathName(joinPaths(parent.mPathName, child))
{
}

Path::Path(const std::string& parent, const std::string& child) :
    mPathName(joinPaths(parent, child))
{
}

Path::Path(const std::string& pathName) :
    mPathName(pathName)
{
}

Path& Path::operator=(const Path& path)
{
    if (this != &path)
    {
        mPathName = path.mPathName;
    }
    return *this;
}

Path::Path(const Path& path) :
    mPathName(path.mPathName)
{
}

std::string Path::normalizePath(const std::string& path)
{
    std::string osDelimStr(Path::delimiter());
    std::string delimStr = osDelimStr;

    //if it's not a forward slash, add it as one of the options
    if (delimStr != "/")
        delimStr += "/";

    //get the drive parts, if any -- we will use the drive later
    Path::StringPair driveParts = Path::splitDrive(path);

    std::vector<std::string> parts = str::Tokenizer(path, delimStr);

    int upCount = 0;
    std::deque<std::string> pathDeque;
    for (std::vector<std::string>::iterator it = parts.begin(); it
            != parts.end(); ++it)
    {
        if (*it == ".")
            continue;
        else if (*it == "..")
        {
            //we want to keep the drive, if there is one
            if (pathDeque.size() == 1 && (*pathDeque.begin())
                    == driveParts.first)
                continue;
            if (pathDeque.size() > 0)
                pathDeque.pop_back();
            else
                upCount++;
        }
        else
            pathDeque.push_back(*it);
    }

    //use the OS-specific delimiters
    std::ostringstream out;
    //only apply the beginning up directories if we didn't start at the root (/)
    if (!str::startsWith(path, osDelimStr) && !str::startsWith(path, "/")
            && driveParts.first.empty())
    {
        if (upCount > 0)
            out << "..";
        for (int i = 1; i < upCount; ++i)
            out << osDelimStr << "..";
    }

    //make sure we don't prepend the drive with a delimiter!
    std::deque<std::string>::iterator it = pathDeque.begin();
    if (!driveParts.first.empty())
        out << *it++;
    for (; it != pathDeque.end(); ++it)
        out << osDelimStr << *it;
    return out.str();
}

std::string Path::joinPaths(const std::string& path1,
                                 const std::string& path2)
{
    std::string osDelimStr(Path::delimiter());

    //check to see if path2 is a root path
    if (str::startsWith(path2, osDelimStr) || str::startsWith(path2, "/")
            || !Path::splitDrive(path2).first.empty())
        return path2;

    std::ostringstream out;
    out << path1;
    if (!str::endsWith(path1, osDelimStr) && !str::endsWith(path1, "/"))
        out << osDelimStr;
    out << path2;
    return out.str();
}

std::vector<std::string> Path::separate(const std::string& path)
{
    Path workingPath = path;
    std::vector<std::string> pathList;
    Path::StringPair pair;
    while ((pair = workingPath.split()).first != workingPath.getPath())
    {
        if (!pair.second.empty())
            pathList.push_back(pair.second);
        workingPath = pair.first;
    }

    std::reverse(pathList.begin(), pathList.end());
    return pathList;
}
std::vector<std::string> Path::separate(const std::string& path, bool& isAbsolute)
{
    isAbsolute = isAbsolutePath(path);
    return separate(path);
}

std::string Path::absolutePath(const std::string& path)
{
    std::string osDelimStr(Path::delimiter());

    Path::StringPair driveParts = Path::splitDrive(path);
    if (!str::startsWith(path, osDelimStr) &&
        !str::startsWith(path, "/") &&
        driveParts.first.empty())
    {
        return Path::normalizePath(Path::joinPaths(
            OS().getCurrentWorkingDirectory(), path));
    }
    else
    {
        return Path::normalizePath(path);
    }
}

bool Path::isAbsolutePath(const std::string& path)
{
#if defined(WIN32) || defined(_WIN32)
    return !Path::splitDrive(path).first.empty();
#else
    return (!path.empty() && path[0] == Path::delimiter()[0]);
#endif
}

Path::StringPair Path::splitPath(const std::string& path)
{
    std::string delimStr(Path::delimiter());

    //if it's not a forward slash, add it as one of the options
    if (delimStr != "/")
        delimStr += "/";

    std::string::size_type pos = path.find_last_of(delimStr);
    if (pos == std::string::npos)
        return Path::StringPair("", path);
    else if (!path.empty() && pos == path.length() - 1)
    {
        // Just call ourselves again without the delimiter
        return Path::splitPath(path.substr(0, path.length() - 1));
    }

    std::string::size_type lastRootPos = path.find_last_not_of(delimStr, pos);
    std::string root;
    if (lastRootPos == std::string::npos)
        root = path.substr(0, pos + 1);
    else
        root = path.substr(0, lastRootPos + 1);
    std::string base = path.substr(path.find_first_not_of(delimStr, pos));
    return Path::StringPair(root, base);
}

Path::StringPair Path::splitExt(const std::string& path)
{
    std::string::size_type pos = path.rfind(".");
    if (pos == std::string::npos)
        return Path::StringPair(path, "");
    return Path::StringPair(path.substr(0, pos), path.substr(pos));
}

std::string Path::basename(const std::string& path, bool removeExt)
{
    std::string baseWithExtension = Path::splitPath(path).second;
    if (removeExt)
    {
        return Path::splitExt(baseWithExtension).first;
    }
    return baseWithExtension;

}

Path::StringPair Path::splitDrive(const std::string& path)
{
#if defined(WIN32) || defined(_WIN32)
    std::string::size_type pos = path.find(":");
#else
    std::string::size_type pos = std::string::npos;
#endif

    if (pos == std::string::npos)
        return Path::StringPair("", path);
    return Path::StringPair(path.substr(0, pos + 1), path.substr(pos + 1));
}

const char* Path::delimiter()
{
#if defined(WIN32) || defined(_WIN32)
    return "\\";
#else
    return "/";
#endif
}

const char* Path::separator()
{
#if defined(WIN32) || defined(_WIN32)
    return ";";
#else
    return ":";
#endif
}

std::vector<std::string> Path::list(const std::string& path)
{
    OS os;
    if (!os.exists(path) || !os.isDirectory(path))
    {
        std::ostringstream oss;
        oss << "'" << path
                << "' does not exist or is not a valid directory";
        throw except::Exception(Ctxt(oss.str()));
    }
    std::vector<std::string> listing;
    Directory directory;
    std::string p = directory.findFirstFile(path.c_str());
    while (!p.empty())
    {
        listing.push_back(p);
        p = directory.findNextFile();
    }
    return listing;
}

std::ostream& operator<<(std::ostream& os, const Path& path)
{
    os << path.getPath().c_str();
    return os;
}
std::istream& operator>>(std::istream& is, Path& path)
{
    std::string str;
    is >> str;
    path.reset(str);
    return is;
}

using path_components = std::vector<std::string>;
class separated_path final
{
    path_components components_;

public:
    bool absolute = false;
    separated_path(path_components&& components) : components_(std::move(components)) { }
    separated_path(const path_components& components) : components_(components)  {  }
    void push_back(const std::string& s)
    {
        components_.push_back(s);
    }
    void push_back(std::string&& s)
    {
        components_.push_back(std::move(s));
    }
    const path_components& components() const { return components_; }
};
static separated_path separate_path(const std::string& path)
{
    bool absolute;
    separated_path retval(Path::separate(path, absolute));
    retval.absolute = absolute;
    return retval;
}

static void clean_slashes(std::string& path, bool isAbsolute)
{
    // Directories will consistently have a trailing '/', files won't
    while (str::endsWith(path, Path::delimiter()))
    {
        path = path.substr(0, path.length() - 1);
    }

    // get rid of multiple "//"s
    while (str::startsWith(path, Path::delimiter()))
    {
    path = path.substr(1);
    }
    #ifndef _WIN32 // std::filesystem has (some?) support for UNC paths, but not this code
    if (isAbsolute)
    {
        path = Path::delimiter() + path;
    }
    #endif

    // Do this last so that we have the best chance of finding the path on disk
    if (fs::is_directory(path))
    {
        if (!str::endsWith(path, Path::delimiter()))
        {
            path += Path::delimiter();
        }
    }
    else if (fs::is_regular_file(path))
    {
      while (str::endsWith(path, Path::delimiter()))
        {
            path = path.substr(0, path.length() - 1);
      }
    }

    assert(isAbsolute ? fs::path(path).is_absolute() : fs::path(path).is_relative());
}
std::string Path::merge(const std::vector<std::string>& components, bool isAbsolute)
{
    std::string retval = isAbsolute ? delimiter() : "";
    for (const auto& component : components)
    {
        retval += component + delimiter();
    }

    clean_slashes(retval, isAbsolute);
    return retval;
}
static std::string merge_path(const separated_path& components)
{
    return Path::merge(components.components(), components.absolute);
}

struct ExtractedEnvironmentVariable final
{
    std::string component; // copy of what was passed

    std::string begin; // "foo" of "foo$(BAR)baz"
    std::string variable; // "BAR" of "foo$(BAR)baz"
    std::string op; // for ${FOO@b}, "b"; http://www.gnu.org/savannah-checkouts/gnu/bash/manual/bash.html#Shell-Parameter-Expansion
    std::string end; // "baz" of "foo$(BAR)baz"
};

static ExtractedEnvironmentVariable extractEnvironmentVariable_dollar(std::string component, size_t pos) // make a copy for manipulation
{
    assert(pos != std::string::npos);
    ExtractedEnvironmentVariable retval;
    retval.component = retval.variable = component;  // assume this really isn't an env. var

    retval.begin = component.substr(0, pos);
    str::replace(component, retval.begin + "$", ""); // don't want to find "(" before "$"
    auto paren = component.find('(');
    char paren_match = ')';
    if (paren == std::string::npos)
    {
        paren = component.find('{');
        paren_match = '}';
    }

    if (paren == 0) // ${FOO} or $(FOO)
    {
        const auto paren_match_pos = component.find(paren_match); // "$(FOO)bar)" will get $(FOO), leaving "bar)"
        if ((paren_match_pos != std::string::npos) && (paren_match_pos > paren))
        {
            retval.variable = component.substr(paren + 1, paren_match_pos - 1);
            retval.end = component.substr(paren_match_pos + 1);

            // We're going to support a very specific format for modifiers, just: ${FOO@c}
            // If it's anythng else, assume it's something else
            if (paren_match == '}')  // only "${...", not "$(..."
            {
                const auto at_pos = retval.variable.find('@');
                if ((at_pos != std::string::npos) && (at_pos >= 1) && (at_pos < retval.variable.length()))
                {
                    auto op = retval.variable.substr(at_pos + 1); // at_pos < length(), from above()
                    if (op.length() == 1)  // only single-characters
                    {
                        retval.variable = retval.variable.substr(0, at_pos);
                        retval.op = std::move(op);
                    }
                }
            }

            return retval;
        }
    }

    // not ${FOO} or $(FOO), maybe $FOO-bar ($FOO_BAR is a real name)
    const auto delim = component.find_first_of("-(){}$%");
    if (delim != std::string::npos)
    {
        retval.variable = component.substr(0, delim);
        retval.end = component.substr(delim);
        return retval;
    }

    // must be just "$FOO"
    retval.variable = component;
    return retval;
}

#if _WIN32 // %FOO% only on Windows
static ExtractedEnvironmentVariable extractEnvironmentVariable_percent(std::string component, size_t pos) // make a copy for manipulation
{
    assert(pos != std::string::npos);
    ExtractedEnvironmentVariable retval;
    retval.variable = component;  // assume this really isn't an env. var

    retval.begin = component.substr(0, pos); // foo%BAR%
    str::replace(component, retval.begin + "%", ""); // %FOO%bar% -> foo_bar% for FOO=foo_
    auto percent_pos = component.find('%');
    if (percent_pos == std::string::npos) // "foo%BAR"
    {
        retval.begin.clear();
        return retval;
    }

    retval.variable = component.substr(0, percent_pos);
    retval.end = component.substr(percent_pos+1);
    return retval;
}
#endif // _WIN32

static ExtractedEnvironmentVariable extractEnvironmentVariable(const std::string& component)
{
    // http://www.kitebird.com/csh-tcsh-book/tcsh.pdf
    /* The word or words in a history reference can be edited, or "modified", by following it with one or more modifiers,
        each preceded by a ':':
            h Remove a trailing pathname component, leaving the head.
            t Remove all leading pathname components, leaving the tail.
            r Remove a filename extension '.xxx', leaving the root name.
            e Remove all but the extension.
    */
    // http://www.gnu.org/savannah-checkouts/gnu/bash/manual/bash.html#Shell-Parameter-Expansion
    /*
    ${parameter@operator} The expansion is either a transformation of the value of parameter or information about parameter itself,
    depending on the value of operator. Each operator is a single letter:
    */


    ExtractedEnvironmentVariable retval;
    retval.variable = component; // assume this really isn't an env. var

    const auto dollar_pos = component.find('$');
    if (dollar_pos != std::string::npos)  // foo$BAR -> "foo_bar" for BAR=_bar
    {
        return extractEnvironmentVariable_dollar(component, dollar_pos);
    }

    #if _WIN32 // %FOO% only on Windows
    const auto percent_pos = component.find('%');
    if (percent_pos != std::string::npos)
    {
        return extractEnvironmentVariable_percent(component, percent_pos);
    }
    #endif

    return retval;
}

static std::string apply_edits(const std::string& path, const std::string& op)
{
    // http://www.kitebird.com/csh-tcsh-book/tcsh.pdf
    /* The word or words in a history reference can be edited, or "modified", by following it with one or more modifiers,
        each preceded by a ':':
    */
    if (op.length() == 1)
    {
        const fs::path fspath(path);
        switch (op[0])
        {
	   // h Remove a trailing pathname component, leaving the head.
           case 'h': return fspath.parent_path().string();

	   // t Remove all leading pathname components, leaving the tail.
           case 't': return fspath.filename().string();

	   // r Remove a filename extension '.xxx', leaving the root name.
           case 'r': return fspath.root_path().string();


	   // e Remove all but the extension.
          case 'e':
	  {
	      // CSH "e" doesn't include the "."
	      auto ext = fspath.extension().string();
	      const auto dot_pos = ext.find(".");
	      if (dot_pos == 0)
	      {
	          ext = ext.substr(1);
	      }    	    
	      return ext;
	  }


	  // We're already "off the reservation" by combining BASH and CSH syntax/functionality,
	  // so ... provide access to other std::file_system::path routines too.
	  // https://en.cppreference.com/w/cpp/filesystem/path

	  // root_name()
	  case 'n' : break; // return fspath.root_name().string();

	  // root_directory()
	  case 'd' : break; // return fspath.root_directory().string();

	  // root_path(), 'r' above

	  // relative_path()
	  case 'p' : break; // return fspath.relative_path().string();

	  // parent_path(), 'h' above

	  // Force use of CSH names rather than providing and alisas: 'f' filename()
	  // filename(), 't' above
	  case 'f' : break; // return fspath.filename().string();

	  // stem()
	  case 's' : return fspath.stem().string();

	  // extension(), 'e' above

          default: break;
        }
    }

    return path;
}

static path_components expandEnvironmentVariable(const std::string& component)
{
    const auto extractedEnvVar = extractEnvironmentVariable(component);
    path_components retval;
    if (extractedEnvVar.variable == component)
    {
        // no env-var syntax found; don't even bother with osSplitEnv()
        retval.push_back(component);
        return retval;
    }

    // Can't check whether the expansions exist as this could just be a piece
    // of a longer path: /foo/$BAR/baz/file.txt
    static const sys::OS os;
    std::string value;
    if (!os.getEnvIfSet(extractedEnvVar.variable, value, true /*includeSpecial*/))
    {
        // No value for the purported "environment variable," assume it's just a
        // path with some funky characters: $({})
        retval.push_back(component);
        return retval;
    }
    const auto paths = str::split(value, sys::Path::separator());
    assert(!paths.empty()); // split("abc") should be "abc"

    // Add back the other pieces: "foo$(BAR)baz" -> "foo_bar_baz" for BAR=_bar_
    //
    // The "end" piece could be another env-var: foo$BAR$BAZ
    const auto endExpandedEnvVar = expandEnvironmentVariable(extractedEnvVar.end); // note: recursion

    path_components updated_paths;
    for (const auto& path_ : paths)
    {
        const auto path = apply_edits(path_, extractedEnvVar.op);

        for (const auto& endVar : endExpandedEnvVar)
        {
            auto p = extractedEnvVar.begin + path + endVar;
            updated_paths.push_back(std::move(p));
        }
    }
    return updated_paths;
}

static path_components join(const std::string& v1, const std::string& v2)
{
    // put two strings into a new list
    path_components retval;
    retval.push_back(v1);
    retval.push_back(v2);
    return retval;
}
static path_components join(path_components v1, const std::string& v2)
{
    // add a string onto an existing list
    v1.push_back(v2);
    return v1;
}
template <typename C3, typename C1, typename C2>
C3& joined_cartesian_product(const C1& c1, const C2& c2, C3& result)
{
    for (const auto& v1 : c1)
    {
        for (const auto& v2 : c2)
        {
            // Rather than returning a list of (v1_n, v2_n) pairs, we'll "join" the
            // pair into a list [v1_n, v2_n].  That list will in turn be the input for
            // a subsequent cartesian product where we can just add to the end.
            //
            // Input: [a, b], [1, 2]; "normal" output: [(a, 1), (a, 2), (b, 1), (b, 2)]
            // Input: [(a, 1), (b, 2)], [X, Y]; "normal" output: [((a, 1), X), ((a, 1), Y), ((b, 2), X), ((b, 2), Y)]
            // By turning the pairs into a list, we simply products-of-products (and products-of-products-of-products)
            //    Input: [a, b], [1, 2]; output: [[a, 1], [a, 2], [b, 1], [b, 2]]
            //    Input: [[a, 1], [b, 2]], [X, Y]; output: [[a, 1, X], [a, 1, Y], [b, 2,, X], [b, 2, Y]]
            result.push_back(join(v1, v2));
        }
    }
    return result;
}

struct expanded_component final
{
    std::string component;
    std::vector<std::string> value;
};
static std::vector<expanded_component> expand_components(const separated_path& components)
{
    std::vector<expanded_component> retval;
    for (const auto& component : components.components())
    {
        expanded_component e{component};
        e.value = expandEnvironmentVariable(component);
        assert(e.value.size() >= 1);  // the component itself should always be there

        retval.push_back(std::move(e));
    }
    return retval;
}

// Generate all the different ways the expansions can be combined.
std::vector<path_components> joined_cartesian_product(const expanded_component& ec1, const expanded_component& ec2)
{
    std::vector<path_components> retval;
    return joined_cartesian_product(ec1.value, ec2.value, retval);
}
std::vector<path_components> joined_cartesian_product(const std::vector<path_components>& ec1, const expanded_component& ec2)
{
    std::vector<path_components> retval;
    return joined_cartesian_product(ec1, ec2.value, retval);
}
std::vector<path_components> expand(const std::vector<expanded_component>& expanded_components)
{
    std::vector<path_components> retval;
    if (expanded_components.empty())
    {
        return retval;
    }
    if (expanded_components.size() == 1)
    {
        // cartesian product of <anything> with <empty> is empty; we want <anything> instead
        for (const auto& s : expanded_components[0].value)
        {
            retval.push_back(join(s, ""));
        }
        return retval;
    }

    retval = joined_cartesian_product(expanded_components[0], expanded_components[1]);
    for (size_t i = 2; i < expanded_components.size(); i++)
    {
        retval = joined_cartesian_product(retval, expanded_components[i]);
    }
    return retval;
}

// a single "~" is a bit of a special case
static std::string expandTilde()
{
    static const sys::OS os;
    return os.getSpecialEnv("HOME"); // getSpecialEnv manages $HOME vs. %USERPROFILE%
}

static std::vector<std::string> expandedEnvironmentVariables_(const std::string& path_, bool& specialPath)
{
    // Avoid pathalogical cases where the first env-variable expands to escape or ~
    #if _WIN32
    //constexpr auto escape = R"(\\?\)"; // https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
    constexpr auto escape = R"(\\)"; // none of the Path:: code supports UNC paths: \\server\dir\file.txt, only C:\dir\file.txt
    #else // assuming *nix
    constexpr auto escape = R"(//)";
    #endif
    if (path_.find(escape) == 0)
    {
        specialPath = true;
        return std::vector<std::string>{path_};
    }
    if (path_ == "~")
    {
        specialPath = true;
        return std::vector<std::string>{expandTilde()};
    }
    specialPath = false;

    constexpr auto tilde_slash = "~/"; // ~\ would be goofy on Windows, so only support ~/
    auto path = path_;
    if (path.find(tilde_slash) == 0)
    {
        // Don't have to worry about goofy things like ~ expanding to /home/${FOO}
        // expandTilde() ensures the directory exists.
        str::replace(path, tilde_slash, expandTilde() + "/");
    }

    const auto components = separate_path(path);  // "This splits on both '/' and '\\'."
    const auto expanded_components = expand_components(components);
    const auto all_expansions = expand(expanded_components);

    std::vector<std::string> retval;
    for (const auto& unmerged_path_ : all_expansions)
    {
        separated_path unmerged_path(unmerged_path_);
        unmerged_path.absolute = components.absolute;
        // $PATH doesn't look absolute, but /usr/bin is
        if (!unmerged_path.absolute && !unmerged_path_.empty())
        {
            unmerged_path.absolute = fs::path(unmerged_path_.front()).is_absolute();
        }
        path = merge_path(unmerged_path);
        retval.push_back(std::move(path));
    }
    return retval;
}
std::vector<std::string> Path::expandedEnvironmentVariables(const std::string& path)
{
    bool unused_specialPath;
    return expandedEnvironmentVariables_(path, unused_specialPath);
}

static bool path_matches_type(const std::string &path, sys::Filesystem::FileType type)
{
    if ((type == Filesystem::FileType::Regular) && Filesystem::is_regular_file(path))
    {
        return true;
    }
    if ((type== Filesystem::FileType::Directory) && Filesystem::is_directory(path))
    {
        return true;
    }
    return false;
}

static std::string expandEnvironmentVariables_(const std::string& path,
                                               bool checkIfExists, sys::Filesystem::FileType* pType = nullptr)
{
    bool specialPath;
    const auto expanded_paths = expandedEnvironmentVariables_(path, specialPath);
    if (specialPath)
    {
        assert(expanded_paths.size() == 1);

        // more handling for "~"; it's a directory, not a file
        if (path == "~")
        {
            if ((pType != nullptr) && (*pType == Filesystem::FileType::Regular))
            {
                return ""; // looking for files, "~" can't be it
            }
        }
        return expanded_paths[0];
    }

    for (const auto& expanded_path : expanded_paths)
    {
        // If the type matches, we're done
        if (pType != nullptr)
        {
            assert(checkIfExists);
            if (path_matches_type(expanded_path, *pType))
            {
                return expanded_path;
            }
        }
        else
        {
            assert(pType == nullptr);
            if (!checkIfExists)
            {
                return expanded_path; // not checking for existence, just grab the first one
            }
            if (Filesystem::exists(expanded_path))
            {
                return expanded_path;
            }
        }
    }
    return "";
}
std::string Path::expandEnvironmentVariables(const std::string& path, bool checkIfExists)
{
    return expandEnvironmentVariables_(path, checkIfExists);
}
std::string Path::expandEnvironmentVariables(const std::string& path, sys::Filesystem::FileType type)
{
    bool unused_checkIfExists = true;
    return expandEnvironmentVariables_(path, unused_checkIfExists, &type);
}

}
