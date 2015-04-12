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

#include <iterator>
#include "sys/FileFinder.h"
#include "sys/DirectoryEntry.h"
#include "sys/Path.h"

bool sys::FileOnlyPredicate::operator()(const std::string& filename)
{
    return sys::Path(filename).isFile();
}

sys::ExtensionPredicate::ExtensionPredicate(std::string ext, bool ignoreCase) :
    mExt(ext), mIgnoreCase(ignoreCase)
{
}

bool sys::ExtensionPredicate::operator()(const std::string& filename)
{
    if (!sys::FileOnlyPredicate::operator()(filename))
        return false;

    std::string ext = sys::Path::splitExt(filename).second;
    if (mIgnoreCase)
    {
        std::string matchExt = mExt;
        str::lower(matchExt);
        str::lower(ext);
        return ext == matchExt;
    }
    return ext == mExt;
}

sys::MultiFilePredicate::MultiFilePredicate(bool orOperator) :
    mOrOperator(orOperator)
{
}
sys::MultiFilePredicate::~MultiFilePredicate()
{
    for (size_t i = 0, n = mPredicates.size(); i < n; ++i)
    {
        sys::MultiFilePredicate::PredicatePair& p = mPredicates[i];
        if (p.first && p.second)
            delete p.first;
    }
}

bool sys::MultiFilePredicate::operator()(const std::string& filename)
{
    bool ok = !mOrOperator;
    for (size_t i = 0, n = mPredicates.size(); i < n && ok != mOrOperator; ++i)
    {
        sys::MultiFilePredicate::PredicatePair& p = mPredicates[i];
        if (mOrOperator)
            ok |= (p.first && (*p.first)(filename));
        else
            ok &= (p.first && (*p.first)(filename));
    }
    return ok;
}

sys::MultiFilePredicate& sys::MultiFilePredicate::addPredicate(
                                                               FilePredicate* filter,
                                                               bool ownIt)
{
    mPredicates.push_back(sys::MultiFilePredicate::PredicatePair(filter, ownIt));
    return *this;
}

sys::FileFinder::FileFinder(const std::vector<std::string>& searchPaths) :
    mPaths(searchPaths)
{
}
sys::FileFinder::~FileFinder()
{
    for (size_t i = 0, n = mPredicates.size(); i < n; ++i)
    {
        sys::FileFinder::PredicatePair& p = mPredicates[i];
        if (p.first && p.second)
            delete p.first;
    }
}

sys::FileFinder& sys::FileFinder::addSearchPath(std::string path)
{
    mPaths.push_back(path);
    return *this;
}

sys::FileFinder& sys::FileFinder::addPredicate(sys::FilePredicate* filter,
                                               bool ownIt)
{
    mPredicates.push_back(sys::FileFinder::PredicatePair(filter, ownIt));
    return *this;
}

std::vector<std::string> sys::FileFinder::findFiles(bool recursive) const
{
    std::list < std::string > paths;
    std::copy(mPaths.begin(), mPaths.end(), std::back_inserter(paths));
    std::vector < std::string > files;
    size_t numInputPaths = mPaths.size();

    for (size_t pathIdx = 0; !paths.empty(); ++pathIdx)
    {
        sys::Path path(paths.front());
        paths.pop_front();

        if (path.isDirectory())
        {
            if (pathIdx < numInputPaths || recursive)
            {
                sys::DirectoryEntry d(path.getPath());
                for (sys::DirectoryEntry::Iterator p = d.begin(); p != d.end(); ++p)
                {
                    std::string fname(*p);
                    if (fname != "." && fname != "..")
                    {
                        paths.push_back(sys::Path::joinPaths(path.getPath(),
                                                             fname));
                    }
                }
            }
        }
        else if (path.exists())
        {
            bool addIt = true;
            for (size_t i = 0, n = mPredicates.size(); addIt && i < n; ++i)
            {
                const sys::FileFinder::PredicatePair& p = mPredicates[i];
                if (p.first)
                    addIt = (*p.first)(path.getPath());
            }
            if (addIt)
                files.push_back(path.getPath());
        }
    }
    return files;
}

