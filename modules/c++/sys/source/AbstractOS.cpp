/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <sys/AbstractOS.h>
#include <sys/Path.h>
#include <sys/DirectoryEntry.h>

namespace sys
{
AbstractOS::AbstractOS()
{
}

AbstractOS::~AbstractOS()
{
}

std::vector<std::string>
AbstractOS::search(const std::vector<std::string>& searchPaths,
                   const std::string& fragment,
                   const std::string& extension,
                   bool recursive) const
{
    std::vector<std::string> elementsFound;

    // add the search criteria
    if (!fragment.empty() && !extension.empty())
    {
        sys::ExtensionPredicate extPred(extension);
        sys::FragmentPredicate fragPred(fragment);

        sys::LogicalPredicate logicPred(false);
        logicPred.addPredicate(&extPred);
        logicPred.addPredicate(&fragPred);

        elementsFound = sys::FileFinder::search(logicPred,
                                                searchPaths,
                                                recursive);
    }
    else if (!extension.empty())
    {
        sys::ExtensionPredicate extPred(extension);
        elementsFound = sys::FileFinder::search(extPred,
                                                searchPaths,
                                                recursive);
    }
    else if (!fragment.empty())
    {
        sys::FragmentPredicate fragPred(fragment);
        elementsFound = sys::FileFinder::search(fragPred,
                                                searchPaths,
                                                recursive);
    }
    return elementsFound;
}

void AbstractOS::remove(const std::string& path) const
{
    if (isDirectory(path))
    {
        // Iterate through each entry in the directory and remove it too
        DirectoryEntry dirEntry(path);
        for (DirectoryEntry::Iterator iter = dirEntry.begin();
                iter != dirEntry.end();
                ++iter)
        {
            const std::string filename(*iter);
            if (filename != "." && filename != "..")
            {
                remove(sys::Path::joinPaths(path, filename));
            }
        }

        // Directory should be empty, so remove it
        removeDirectory(path);
    }
    else
    {
        removeFile(path);
    }
}

bool AbstractOS::getEnvIfSet(const std::string& envVar, std::string& value) const
{
    if (isEnvSet(envVar))
    {
        value = getEnv(envVar);
        return true;
    }
    return false;
}

std::string AbstractOS::getCurrentExecutable(
        const std::string& argvPathname) const
{
    if (argvPathname.empty())
    {
        // If the OS-specific overrides can't find the name,
        // and we don't have an argv[0] to look at,
        // there's nothing we can do.
        return "";
    }

    if (sys::Path::isAbsolutePath(argvPathname))
    {
        return argvPathname;
    }

    const std::string candidatePathname = sys::Path::joinPaths(
            getCurrentWorkingDirectory(), argvPathname);
    if (exists(candidatePathname))
    {
        return candidatePathname;
    }

    // Look for it in PATH
    const std::vector<std::string> pathDirs =
            str::split(getEnv("PATH"), sys::Path::separator());
    for (size_t ii = 0; ii < pathDirs.size(); ++ii)
    {
        const std::string candidatePathname = sys::Path::joinPaths(
                sys::Path::absolutePath(pathDirs[ii]), argvPathname);
        if (exists(candidatePathname))
        {
            return candidatePathname;
        }
    }

    return "";
}

}

