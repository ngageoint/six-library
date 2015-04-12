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

#include "sys/Path.h"

sys::Path::Path()
{
}

sys::Path::Path(const Path& parent, std::string child)
{
    mPathName = joinPaths(parent.mPathName, child);
}

sys::Path::Path(std::string parent, std::string child)
{
    mPathName = joinPaths(parent, child);
}

sys::Path::Path(std::string pathName) :
    mPathName(pathName)
{
}

sys::Path& sys::Path::operator=(const sys::Path& path)
{
    if (this != &path)
    {
        mPathName = path.mPathName;
    }
    return *this;
}

sys::Path::Path(const sys::Path& path)
{
    mPathName = path.mPathName;
}

sys::Path::~Path()
{
}

std::string sys::Path::normalizePath(const std::string& path)
{
    std::string osDelimStr(sys::Path::delimiter());
    std::string delimStr = osDelimStr;

    //if it's not a forward slash, add it as one of the options
    if (delimStr != "/")
        delimStr += "/";

    //get the drive parts, if any -- we will use the drive later
    sys::Path::StringPair driveParts = sys::Path::splitDrive(path);

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

std::string sys::Path::joinPaths(const std::string& path1,
                                 const std::string& path2)
{
    std::string osDelimStr(sys::Path::delimiter());

    //check to see if path2 is a root path
    if (str::startsWith(path2, osDelimStr) || str::startsWith(path2, "/")
            || !sys::Path::splitDrive(path2).first.empty())
        return path2;

    std::ostringstream out;
    out << path1;
    if (!str::endsWith(path1, osDelimStr) && !str::endsWith(path1, "/"))
        out << osDelimStr;
    out << path2;
    return out.str();
}

std::string sys::Path::absolutePath(const std::string& path)
{
    std::string osDelimStr(sys::Path::delimiter());

    sys::Path::StringPair driveParts = sys::Path::splitDrive(path);
    if (!str::startsWith(path, osDelimStr) && !str::startsWith(path, "/")
            && driveParts.first.empty())
        return sys::Path::normalizePath(
                                        sys::Path::joinPaths(
                                                             sys::OS().getCurrentWorkingDirectory(),
                                                             path));
    return sys::Path::normalizePath(path);
}

sys::Path::StringPair sys::Path::splitPath(const std::string& path)
{
    std::string delimStr(sys::Path::delimiter());

    //if it's not a forward slash, add it as one of the options
    if (delimStr != "/")
        delimStr += "/";

    std::string::size_type pos = path.find_last_of(delimStr);
    if (pos == std::string::npos)
        return sys::Path::StringPair("", path);
    else if (!path.empty() && pos == path.length() - 1)
    {
        // Just call ourselves again without the delimiter
        return sys::Path::splitPath(path.substr(0, path.length() - 1));
    }

    std::string::size_type lastRootPos = path.find_last_not_of(delimStr, pos);
    std::string root;
    if (lastRootPos == std::string::npos)
        root = path.substr(0, pos + 1);
    else
        root = path.substr(0, lastRootPos + 1);
    std::string base = path.substr(path.find_first_not_of(delimStr, pos));
    return sys::Path::StringPair(root, base);
}

sys::Path::StringPair sys::Path::splitExt(const std::string& path)
{
    std::string::size_type pos = path.rfind(".");
    if (pos == std::string::npos)
        return sys::Path::StringPair(path, "");
    return sys::Path::StringPair(path.substr(0, pos), path.substr(pos));
}

std::string sys::Path::basename(const std::string& path, bool removeExt)
{
    std::string baseWithExtension = sys::Path::splitPath(path).second;
    if (removeExt)
        return splitExt(baseWithExtension).first;
    return baseWithExtension;

}

sys::Path::StringPair sys::Path::splitDrive(const std::string& path)
{
#ifdef WIN32
    std::string::size_type pos = path.find(":");
#else
    std::string::size_type pos = std::string::npos;
#endif

    if (pos == std::string::npos)
        return sys::Path::StringPair("", path);
    return sys::Path::StringPair(path.substr(0, pos + 1), path.substr(pos + 1));
}

const char* sys::Path::delimiter()
{
#ifdef WIN32
    return "\\";
#else
    return "/";
#endif
}

const char* sys::Path::separator()
{
#ifdef WIN32
    return ";";
#else
    return ":";
#endif
}

std::vector<std::string> sys::Path::list() const
{
    if (!mOS.exists(mPathName) || !mOS.isDirectory(mPathName))
    {
        std::ostringstream oss;
        oss << "'" << mPathName
                << "' does not exist or is not a valid directory";
        throw except::Exception(Ctxt(oss.str()));
    }
    std::vector<std::string> listing;
    sys::Directory directory;
    const char* p = directory.findFirstFile(mPathName.c_str());
    while (p != NULL)
    {
        listing.push_back(p);
        p = directory.findNextFile();
    }
    return listing;
}

std::ostream& operator<<(std::ostream& os, const sys::Path& path)
{
    os << path.getPath().c_str();
    return os;
}
std::istream& operator>>(std::istream& is, sys::Path& path)
{
    std::string str;
    is >> str;
    path.reset(str);
    return is;
}
