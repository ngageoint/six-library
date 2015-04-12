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


#ifndef __SYS_OS_UNIX_H__
#define __SYS_OS_UNIX_H__

#if !defined(WIN32)

#include "sys/AbstractOS.h"
#include "sys/Conf.h"
#include <dirent.h>
#include <sys/utsname.h>

namespace sys
{
class OSUnix : public AbstractOS
{
public:
    OSUnix()
    {}
    virtual ~OSUnix()
    {}

    virtual std::string getPlatformName() const;

    virtual std::string getNodeName() const;

    /*!
     *  Get the path delimiter for this operating system.
     *  For windows, this will be two slashes \\
     *  For unix it will be one slash /
     *  \return The path delimiter
     */
    virtual const char* getDelimiter() const
    {
        return "/";
    }

    /*!
     *  Search for *fragment*filter recursively over path
     *  \param exhaustiveEnumerations  All retrieved enumerations
     *  \param fragment The fragment to search for
     *  \param filter What to use as an extension
     *  \param pathList  The path list (colon delimited)
     */
    virtual void search(std::vector<std::string>& exhaustiveEnumerations,
                        const std::string& fragment,
                        std::string filter = "",
                        std::string pathList = ".") const;

    /*!
     *  Gets the username for windows users
     *  \return The string name of the user
     */ 
    //        virtual std::string getUsername() const;

    /*!
     *  Does this path exist?
     *  \param path The path to check for
     *  \return True if it does, false otherwise
     */
    virtual bool exists(const std::string& path) const;

    /*!
     *  Remove file with this path name
     *  \return True upon success, false if failure
     */
    virtual bool remove(const std::string& path) const;

    /*!
     *  Move file with this path name to the newPath
     *  \return True upon success, false if failure
     */
    virtual bool move(const std::string& path, const std::string& newPath) const;

    /*!
     *  Does this path resolve to a file?
     *  \param path The path
     *  \return True if it does, false if not
     */
    virtual bool isFile(const std::string& path) const;

    /*!
     *  Does this path resolve to a directory?
     *  \param path The path
     *  \return True if it does, false if not
     */
    virtual bool isDirectory(const std::string& path) const;

    virtual bool makeDirectory(const std::string& path) const;


    virtual Pid_T getProcessId() const;


    /*!
     *  Retrieve the current working directory.
     *  \return The current working directory
     */
    virtual std::string getCurrentWorkingDirectory() const;

    /*!
     *  Change the current working directory.
     *  \return true if the directory was changed, otherwise false.
     */
    virtual bool changeDirectory(const std::string& path) const;

    /*!
     *  Get a suitable temporary file name
     *  \return The file name
     */
    virtual std::string getTempName(std::string path = ".",
                                    std::string prefix = "TMP") const;

    /*!
     *  Return the size in bytes of a file
     *  \return The file size
     */
    virtual sys::Off_T getSize(const std::string& path) const;

    /**
     * Returns the last modified time of the file/directory
     */
    virtual sys::Off_T getLastModifiedTime(const std::string& path) const;

    /*!
     *  This is a system independent sleep function.
     *  Be careful using timing calls with threads
     *  \todo I heard usleep is deprecated, and I should
     *  use nanosleep
     *  \param milliseconds The params
     */
    virtual void millisleep(int milliseconds) const;

    virtual std::string operator[](const std::string& s) const;

    /*!
     *  Get an environment variable
     */
    virtual std::string getEnv(const std::string& s) const;

    /*!
     *  Set an environment variable
     */
    virtual void setEnv(const std::string& var, 
			const std::string& val,
			bool overwrite);

    virtual std::string getDSOSuffix() const;

};


class DirectoryUnix : public AbstractDirectory
{
public:
    DirectoryUnix() : mDir(NULL)
    {}
    virtual ~DirectoryUnix()
    {
        close();
    }
    virtual void close();
    virtual const char* findFirstFile(const char* dir);
    virtual const char* findNextFile();
    DIR* mDir;

};

}

#endif
#endif
