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

#ifndef __SYS_FILE_FINDER_H__
#define __SYS_FILE_FINDER_H__

#include <functional>
#include <vector>
#include <string>
#include <utility>

#include "sys/Filesystem.h"

namespace sys
{

/**
 * Predicate interface for all entries
 */
struct FilePredicate
{
    using argument_type = std::string;
    using result_type = bool;

    virtual ~FilePredicate() {}
    virtual bool operator()(const std::string& entry) const = 0;
};

/**
 * Predicate interface for existance
 */
struct ExistsPredicate : FilePredicate
{
    virtual ~ExistsPredicate() {}
    virtual bool operator()(const std::string& entry) const;
};

/**
 * Predicate that matches files only (no directories)
 */
struct FileOnlyPredicate: public FilePredicate
{
    virtual ~FileOnlyPredicate() {}
    virtual bool operator()(const std::string& entry) const;
};

/**
 * Predicate that matches directories only (no files)
 */
struct DirectoryOnlyPredicate: public FilePredicate
{
    virtual ~DirectoryOnlyPredicate() {}
    virtual bool operator()(const std::string& entry) const;
};

/**
 * Predicate that matches directories only (no files)
 */
struct FragmentPredicate : public FilePredicate
{
public:
    FragmentPredicate(const std::string& fragment, bool ignoreCase = true);
    bool operator()(const std::string& entry) const;

private:
    std::string mFragment;
    bool mIgnoreCase;

};


/**
 * Predicate interface for filtering files with a specific extension
 * This method will not match '.xxx.yyy' type patterns, since the 
 * splitting routines will only find '.yyy'.  See re::RegexPredicate
 * for a more useful finder.
 */
class ExtensionPredicate: public FileOnlyPredicate
{
public:
    ExtensionPredicate(const std::string& ext, bool ignoreCase = true);
    bool operator()(const std::string& filename) const;

private:
    std::string mExt;
    bool mIgnoreCase;
};

/**
 * Predicate that does logical not of another predicate (ie !)
 */
class NotPredicate : public FilePredicate
{
public:
    NotPredicate(FilePredicate* filter, bool ownIt = false);
    virtual ~NotPredicate();

    virtual bool operator()(const std::string& entry) const;

protected:
    typedef std::pair<FilePredicate*, bool> PredicatePair;
    PredicatePair mPredicate;
};


/**
 *  The LogicalPredicate class allows you to chain many 
 *  predicates using the logical && or ||
 */
class LogicalPredicate : public FilePredicate
{
public:
    LogicalPredicate(bool orOperator = true);
    virtual ~LogicalPredicate();

    sys::LogicalPredicate& addPredicate(FilePredicate* filter, 
                                        bool ownIt = false);

    virtual bool operator()(const std::string& entry) const;

protected:
    bool mOrOperator;
    typedef std::pair<FilePredicate*, bool> PredicatePair;
    std::vector<PredicatePair> mPredicates;
};

/**
 * \class FileFinder
 *
 *  The FileFinder class allows you to search for 
 *  files/directories in a clean way.
 */
class FileFinder
{
public:
    FileFinder() {}
    ~FileFinder() {}

    /**
     * Perform the search
     * \return a std::vector<std::string> of paths that match
     */
    static std::vector<std::string> search(
        const FilePredicate& filter,
        const std::vector<std::string>& searchPaths, 
        bool recursive = false);
};

}

#endif
