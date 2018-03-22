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
#ifdef WIN32
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
#ifdef WIN32
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
#ifdef WIN32
    return "\\";
#else
    return "/";
#endif
}

const char* Path::separator()
{
#ifdef WIN32
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
}
