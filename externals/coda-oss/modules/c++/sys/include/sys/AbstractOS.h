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

#ifndef __SYS_ABSTRACT_OS_H__
#define __SYS_ABSTRACT_OS_H__

#include <vector>
#include <string>
#include "sys/Conf.h"
#include "sys/FileFinder.h"
#include "sys/SystemException.h"
#include "str/Tokenizer.h"

/*!
 *  \file
 *  \brief This class is the interface for an OS Abstraction layer.
 *  
 *  The class contains the interface for the abstraction layer for
 *  an operating system independent layer.
 *
 */
namespace sys
{
/*!
 *  \class AbstractOS
 *  \brief Interface for system independent function calls
 *
 *  The AbstractOS class defines the base for a system
 *  independent layer of function calls.
 */
class AbstractOS
{
public:
    AbstractOS();

    virtual ~AbstractOS();

    /*!
     *  Get the name of the platform this was compiled for
     *  This is done by retrieving the 'target' variable
     *  in configure and forcing it to be defined for sys
     *  
     *  \return The name of the platform.
     *
     */
    virtual std::string getPlatformName() const = 0;

    /*!
     *  Get the name of the host machine
     *
     *  \return the name of the host machine
     */
    virtual std::string getNodeName() const = 0;

    /*!
     *  Get the path delimiter for this operating system.
     *  For windows, this will be two slashes \\
     *  For unix it will be one slash /
     *  \return The path delimiter
     */
    virtual const char* getDelimiter() const = 0;

    /*!
     *  Search recursively for some fragment with the directory.
     *  This will return both directories and files with the 
     *  fragment, unless the user specifies the extension.
     *
     *  \param elementsFound  All retrieved enumerations
     *  \param fragment       The fragment to search for
     *  \param extension      extensions should only be used for files
     *  \param pathList       The path list (colon delimited)
     */
    std::vector<std::string>
    search(const std::vector<std::string>& searchPaths,
           const std::string& fragment = "",
           const std::string& extension = "",
           bool recursive = true) const;

    /*!
     *  Does this path exist?
     *  \param path The path to check for
     *  \return True if it does, false otherwise
     */
    virtual bool exists(const std::string& path) const = 0;

    /*!
     *  Remove file with this path name
     *  This method will not remove symlinks, use removeSymlink()
     */
    virtual void remove(const std::string& path) const;

    /*!
     *  Move file with this path name to the newPath
     *  \return True upon success, false if failure
     */
    virtual bool move(const std::string& path, 
                      const std::string& newPath) const = 0;

    /*!
     *  Does this path resolve to a file?
     *  \param path The path
     *  \return True if it does, false if not
     */
    virtual bool isFile(const std::string& path) const = 0;

    /*!
     *  Does this path resolve to a directory?
     *  \param path The path
     *  \return True if it does, false if not
     */
    virtual bool isDirectory(const std::string& path) const = 0;

    /*!
     *  Create a directory with for the path specified
     *  \param path The path to create
     *  \return True on success, false on failure (since
     *  you may only create if no such exists)
     */
    virtual bool makeDirectory(const std::string& path) const = 0;

    /*!
     *  Retrieve the current working directory.
     *  \return The current working directory
     */
    virtual std::string getCurrentWorkingDirectory() const = 0;

    /*!
     *  Change the current working directory.
     *  \return true if the directory was changed, otherwise false.
     */
    virtual bool changeDirectory(const std::string& path) const = 0;

    /*!
     *  Get a suitable temporary file name
     *  \return The file name
     *
     */
    virtual std::string getTempName(const std::string& path = "", 
                                    const std::string& prefix = "") const = 0;

    /*!
     *  Return the size in bytes of a file
     *  \return The file size
     */
    virtual sys::Off_T getSize(const std::string& path) const = 0;

    /*!
     * Return the last modified time of a file
     * \return The last modified time, in millis
     */
    virtual sys::Off_T getLastModifiedTime(const std::string& path) const = 0;

    /*!
     *  This is a system independent sleep function.
     *  Be careful using timing calls with threads
     *  \param milliseconds The params
     */
    virtual void millisleep(int milliseconds) const = 0;

    /*!
     *  Get an environment variable
     */
    virtual std::string operator[](const std::string& s) const = 0;

    /*!
     *  Get an environment variable
     */
    virtual std::string getEnv(const std::string& s) const = 0;

    /*!
     *  Set an environment variable
     */
    virtual void setEnv(const std::string& var, 
                        const std::string& val,
                        bool overwrite) = 0;

    virtual Pid_T getProcessId() const = 0;

    virtual std::string getDSOSuffix() const = 0;

    virtual size_t getNumCPUs() const = 0;

    /*!
     *  Create a symlink, pathnames can be either absolute or relative
     */
    virtual void createSymlink(const std::string& origPathname, 
                               const std::string& symlinkPathname) const = 0;

    /*!
     * Remove a symlink, pathname can be absolute or relative
     */
    virtual void removeSymlink(const std::string& symlinkPathname) const = 0;

    /*!
     *  Get the total RAM and available RAM on the system in megabytes
     */
    virtual void getMemInfo(size_t& totalPhysMem, size_t& freePhysMem) const = 0;

protected:
    /*!
     *  Remove file with this pathname
     */
    virtual void removeFile(const std::string& pathname) const = 0;

    /*!
     *  Remove directory with this pathname
     *  NOTE: This will throw if the directory is not empty
     */
    virtual void removeDirectory(const std::string& pathname) const = 0;
};

class AbstractDirectory
{
public:
    AbstractDirectory()
    {
    }
    virtual ~AbstractDirectory()
    {
    }
    virtual void close() = 0;
    virtual std::string findFirstFile(const std::string& dir) = 0;
    virtual std::string findNextFile() = 0;

};

}

#endif

