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
#include "sys/FileFinder.h"

#include <iterator>
#include <stdexcept>
#include <tuple> // std::ignore

#include "sys/DirectoryEntry.h"
#include "sys/Path.h"

bool sys::ExistsPredicate::operator()(const std::string& entry) const
{
    return sys::Path(entry).exists();
}

bool sys::FileOnlyPredicate::operator()(const std::string& entry) const
{
    return sys::Path(entry).isFile();
}

bool sys::DirectoryOnlyPredicate::operator()(const std::string& entry) const
{
    return sys::Path(entry).isDirectory();
}

sys::FragmentPredicate::FragmentPredicate(const std::string& fragment,
                                          bool ignoreCase) :
    mFragment(fragment), mIgnoreCase(ignoreCase)
{
}

bool sys::FragmentPredicate::operator()(const std::string& entry) const
{
    if (mIgnoreCase)
    {
        std::string base = entry;
        str::lower(base);

        std::string match = mFragment;
        str::lower(match);

        return str::contains(base, match);
    }
    else
        return str::contains(entry, mFragment);
}


sys::ExtensionPredicate::ExtensionPredicate(const std::string& ext, 
                                            bool ignoreCase) :
    mExt(ext), mIgnoreCase(ignoreCase)
{
}

bool sys::ExtensionPredicate::operator()(const std::string& filename) const
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
    else
        return ext == mExt;
}

sys::NotPredicate::NotPredicate(FilePredicate* filter, bool ownIt) :
    mPredicate(sys::NotPredicate::PredicatePair(filter, ownIt))
{
}

sys::NotPredicate::~NotPredicate()
{
    if (mPredicate.second && mPredicate.first)
    {
        FilePredicate* tmp = mPredicate.first;
        mPredicate.first = NULL;
        delete tmp;
    }
}

bool sys::NotPredicate::operator()(const std::string& entry) const
{
    return !(*mPredicate.first)(entry);
}

sys::LogicalPredicate::LogicalPredicate(bool orOperator) :
    mOrOperator(orOperator)
{
}

sys::LogicalPredicate::~LogicalPredicate()
{
    for (size_t i = 0; i < mPredicates.size(); ++i)
    {
        sys::LogicalPredicate::PredicatePair& p = mPredicates[i];
        if (p.first && p.second)
        {
            sys::FilePredicate* tmp = p.first;
            p.first = NULL;
            delete tmp;
        }
    }
}

bool sys::LogicalPredicate::operator()(const std::string& entry) const
{
    bool ok = !mOrOperator;
    for (size_t i = 0, n = mPredicates.size(); i < n && ok != mOrOperator; ++i)
    {
        const sys::LogicalPredicate::PredicatePair& p = mPredicates[i];
        if (mOrOperator)
            ok |= (p.first && (*p.first)(entry));
        else
            ok &= (p.first && (*p.first)(entry));
    }
    return ok;
}

sys::LogicalPredicate& sys::LogicalPredicate::addPredicate(
    FilePredicate* filter,
    bool ownIt)
{
    mPredicates.push_back(
        sys::LogicalPredicate::PredicatePair(
            filter, ownIt));
    return *this;
}

std::vector<std::string> sys::FileFinder::search(
    const FilePredicate& filter,
    const std::vector<std::string>& searchPaths, 
    bool recursive)
{
    // turn it into a list so we can queue additional entries
    std::list < std::string > paths;
    std::copy(searchPaths.begin(), searchPaths.end(), 
              std::back_inserter(paths));

    std::vector <std::string> files;
    size_t numInputPaths = searchPaths.size();
    for (size_t pathIdx = 0; !paths.empty(); ++pathIdx)
    {
        sys::Path path(paths.front());
        paths.pop_front();

        //! check if it exists
        if (path.exists())
        {
            // check if this meets the criteria -- 
            // we only need one to add it
            if (filter(path.getPath()))
            {
                files.push_back(path.getPath());
            }

            // if it's a directory we need to search its contents
            if (path.isDirectory())
            {
                // If its an original directory or we are recursively searching
                if (pathIdx < numInputPaths || recursive)
                {
                    sys::DirectoryEntry d(path.getPath());
                    for (sys::DirectoryEntry::Iterator p = d.begin(); 
                         p != d.end(); ++p)
                    {
                        std::string fname(*p);
                        if (fname != "." && fname != "..")
                        {
                            // add it to the list
                            paths.push_back(sys::Path::joinPaths(path.getPath(),
                                                                 fname));
                        }
                    }
                }
            }

        }
    }
    return files;
}

static coda_oss::filesystem::path findFirst(const sys::FilePredicate& pred, const coda_oss::filesystem::path& startingDirectory)
{
    auto dir = startingDirectory;
    while (true)
    {
        const std::vector<std::string> searchPaths{dir.string()};
        const auto results = sys::FileFinder::search(pred, searchPaths, true /*recursive*/);
        if (results.size() == 1)
        {
            return results[0];
        }
        if (results.size() > 1)
        {
            throw std::logic_error("Found the same file at multiple locations: " + searchPaths[0]);
        }

        if (is_directory(dir / ".git"))
        {
            throw std::logic_error("Won't traverse above .git directory at: " + dir.string());
        }
        dir = dir.parent_path();
    }
}
coda_oss::filesystem::path sys::findFirstFile(const coda_oss::filesystem::path& startingDirectory, const coda_oss::filesystem::path& filename)
{
    struct FileExistsPredicate final : public FileOnlyPredicate
    {
        coda_oss::filesystem::path name_;
        FileExistsPredicate(const coda_oss::filesystem::path& name) : name_(name) { }
        bool operator()(const std::string& entry) const override
        {
            const auto p =entry / name_;
            return coda_oss::filesystem::is_regular_file(p);
        }
    };
    const FileExistsPredicate pred(filename);
    return findFirst(pred, startingDirectory);
}
coda_oss::filesystem::path sys::findFirstDirectory(const coda_oss::filesystem::path& startingDirectory, const coda_oss::filesystem::path& dir)
{
    struct DirectoryExistsPredicate final : public DirectoryOnlyPredicate
    {
        coda_oss::filesystem::path name_;
        DirectoryExistsPredicate(const coda_oss::filesystem::path& name) : name_(name) { }
        bool operator()(const std::string& entry) const override
        {
            const auto p =entry / name_;
            return coda_oss::filesystem::is_directory(p);
        }
    };
    const DirectoryExistsPredicate pred(dir);
    return findFirst(pred, startingDirectory);
}















coda_oss::filesystem::path sys::test::findRootDirectory(const coda_oss::filesystem::path& p, const std::string& rootName,
        std::function<bool(const coda_oss::filesystem::path&)> isRoot)
{
    const auto isRootDirectory = [&](const coda_oss::filesystem::path& p) { return is_directory(p) && isRoot(p); };

    // Does the given path look good?
    if (isRootDirectory(p))
    {
        return p;
    }

    // Nope, maybe the directory we're interested in is here
    if (!rootName.empty())
    {
        auto root = p / rootName;
        if (isRootDirectory(root))
        {
            return root;
        }

        // We put other code in an "externals" directory; try that.
        const auto externals = p / "externals";
        if (is_directory(externals))
        {
            root = externals / rootName;
            if (isRootDirectory(root))
            {
                return root;
            }
        }
    }

    // Once we're at a .git directory, we have to go down, not up ... or fail.
    if (!is_directory(p / ".git"))
    {
        return findRootDirectory(p.parent_path(), rootName, isRoot);
    }
    
    // TODO: since we're in the "FileFinder" module, maybe try a bit harder to find "rootName"?
    throw std::invalid_argument("Can't find '" + rootName + "' root directory");
}

static const sys::OS os;
static inline std::string Configuration()  // "Configuration" is typically "Debug" or "Release"
{
    return os.getSpecialEnv("Configuration");
}
static inline std::string Platform()
{
    return os.getSpecialEnv("Platform");  // e.g., "x64" on Windows
}

static coda_oss::filesystem::path findCMakeRoot(const coda_oss::filesystem::path& path, const coda_oss::filesystem::path& dir)
{
   	static const auto platform_and_configuration = ::Platform() + "-" + ::Configuration(); // "x64-Debug"
    const auto pred = [&](const coda_oss::filesystem::path& p)
    {
        if (p.filename() == platform_and_configuration)
        {
            return p.parent_path().filename() == dir;
        }

        // "x64-Debug" is common on Windows from Visual Studio, things
        // may be a bit different on Linux.
        const auto CMakeCache_txt = p / "CMakeCache.txt";
        const auto CMakeFiles = p / "CMakeFiles";
        if (is_regular_file(CMakeCache_txt) && is_directory(CMakeFiles))
        {
            // looks promising ... _deps and modules directories are there, call it good.
            const auto deps = p / "_deps";
            const auto modules = p / "modules";
            return is_directory(deps) && is_directory(modules);
        }

        return false;
    };
	return sys::test::findRootDirectory(path, "", pred);
}

coda_oss::filesystem::path findCMake_Root(const coda_oss::filesystem::path& path,
    const std::string& build, const std::string& install)
{
    // Calling these directories "build" and "install" for clarity, even though they may be
    // "install" and "build" (or maybe even something else).
    
    // .../out/build/x64-Debug
    try
    {
        return findCMakeRoot(path, build);
    }
    catch (const std::invalid_argument&)
    {
    }

    // Might be given a path to something in "install" ...
    std::clog << "path: " << path << '\n';
    const auto configAndPlatformDir = findCMakeRoot(path, install);  // should be, e.g., "x64-Debug"
    const auto installDir = configAndPlatformDir.parent_path();
    if (installDir.filename() == install)
    {
        auto retval = installDir.parent_path() / build;
        std::clog << "root: " << retval << '\n';
        if (is_directory(retval))
        {
            return retval;
        }
    }
    return findCMakeRoot(path, build);  // throw an exception
}
coda_oss::filesystem::path sys::test::findCMakeBuildRoot(const coda_oss::filesystem::path& path)
{
    auto retval = findCMake_Root(path, "build", "install");
    std::clog << "findCMakeBuildRoot(): " << retval << "\n";
    return retval;
}
coda_oss::filesystem::path sys::test::findCMakeInstallRoot(const coda_oss::filesystem::path& path)
{
    auto retval = findCMake_Root(path, "install", "build");
    std::clog << "findCMakeInstallRoot(): " << retval << "\n";
    return retval;
}

bool sys::test::isCMakeBuild(const coda_oss::filesystem::path& path)
{
    try
    {
        std::ignore = sys::test::findCMakeBuildRoot(path);
        return true;
    }
    catch (const std::invalid_argument&)
    {
        return false;
    }
}