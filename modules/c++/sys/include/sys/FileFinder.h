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

#ifndef __SYS_FILE_FINDER_H__
#define __SYS_FILE_FINDER_H__

#include "sys/OS.h"
#include <functional>

namespace sys
{

/**
 * Predicate interface for filenames
 */
struct FilePredicate: std::unary_function<std::string, bool>
{
    virtual ~FilePredicate() {}
    virtual bool operator()(const std::string& obj) = 0;
};

/**
 * Predicate that matches files only (no directories)
 */
struct FileOnlyPredicate: public FilePredicate
{
    virtual ~FileOnlyPredicate() {}
    virtual bool operator()(const std::string& filename);
};

/**
 * Predicate interface for filtering files with a specific extension
 * This method will not match '.xxx.yyy' type patterns, since the 
 * splitting routines will only find '.yyy'.  See re::GlobFilePredicate
 * for a more useful finder.
 */
class ExtensionPredicate: public FileOnlyPredicate
{
public:
    ExtensionPredicate(std::string ext, bool ignoreCase = true);
    bool operator()(const std::string& filename);
private:
    std::string mExt;
    bool mIgnoreCase;
};

class MultiFilePredicate: public FilePredicate
{
public:
    MultiFilePredicate(bool orOperator = true);
    virtual ~MultiFilePredicate();

    virtual bool operator()(const std::string& filename);
    MultiFilePredicate& addPredicate(FilePredicate* filter, bool ownIt = false);
protected:
    bool mOrOperator;
    typedef std::pair<FilePredicate*, bool> PredicatePair;
    std::vector<PredicatePair> mPredicates;
};


/**
 * \class FileFinder
 *
 * The FileFinder class allows you to search for files/directories in a
 * clean way.
 */
class FileFinder
{
public:
    FileFinder(){}
    FileFinder(const std::vector<std::string>& searchPaths);
    ~FileFinder();

    /**
     * Add a search path
     */
    FileFinder& addSearchPath(std::string path);

    /**
     * Add a predicate/filter to use when searching
     */
    FileFinder& addPredicate(FilePredicate* filter, bool ownIt = false);

    /**
     * Perform the search
     * \return a std::vector<std::string> of paths that match
     */
    std::vector<std::string> findFiles(bool recursive = false) const;

protected:
    typedef std::pair<FilePredicate*, bool> PredicatePair;
    sys::OS mOS;
    std::vector<std::string> mPaths;
    std::vector<PredicatePair> mPredicates;
};


}

#endif
