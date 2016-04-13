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


#ifndef __SYS_WIN32_OS_H__
#define __SYS_WIN32_OS_H__

#include "sys/AbstractOS.h"

#if defined(WIN32)

/*!
 *  \file OSWin32.h
 *  \brief This file provides definitions for the windows layer
 *
 *  This file provides the definitions for the windows layer of
 *  file manipulation abstraction, etc.
 */

namespace sys
{

/*!
 *  \class OSWin32
 *  \brief The abstraction definition layer for windows
 *
 *  This class is the abstraction layer as defined for 
 *  the windows operating system.
 */
class OSWin32 : public AbstractOS
{
public:
    OSWin32()
    {}
    virtual ~OSWin32()
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
        return "\\";
    }

    /*!
     *  Determine the username
     *  \return The username
     */ 
    //virtual std::string getUsername() const;


    /*!
     *  Does this path exist?
     *  NOTE: On Windows a path specifying a directory should not
     *  end in '\\' or '/' or it will always fail this check
     *  \param path The path to check for
     *  \return True if it does, false otherwise
     */
    virtual bool exists(const std::string& path) const;

    /*!
     *  Move file with this path name to the newPath
     *  Note: this will move (rename) either a file or a directory
     * (including its children) either in the same directory or across
     *  directories. The one caveat is that it will fail on directory
     *  moves when the destination is on a different volume.
     * 
     *  \return True upon success, false if failure
     */
    virtual bool move(const std::string& path, 
                      const std::string& newPath) const;

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
     *  \todo Throw if nothing exists??
     */
    virtual bool isDirectory(const std::string& path) const;

    /*!
     *  Create a directory with for the path specified
     *  \param path The path to create
     *  \return True on success, false on failure (since
     *  you may only create if no such exists)
     */
    virtual bool makeDirectory(const std::string& path) const;

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


    virtual Pid_T getProcessId() const;

    /*!
     *  Get a suitable temporary file name
     *  \return The file name
     *
     */
    virtual std::string getTempName(const std::string& path = ".",
                                    const std::string& prefix = "TMP") const;
    /*!
     *  Return the size in bytes of a file
     *  \return The file size
     */
    virtual sys::Off_T getSize(const std::string& path) const;

    virtual sys::Off_T getLastModifiedTime(const std::string& path) const;

    virtual std::string getDSOSuffix() const;

    /*!
     *  This is a system independent sleep function.
     *  Be careful using timing calls with threads
     *  \param milliseconds The params
     */
    virtual void millisleep(int milliseconds) const;

    virtual std::string operator[](const std::string& s) const;

    /*!
     *  Get an environment variable
     */
    virtual std::string getEnv(const std::string& s) const;

    /*!
     * Returns true if environment variable is set, false otherwise
     */
    virtual bool isEnvSet(const std::string& s) const;

    /*!
     *  Set an environment variable
     */
    virtual void setEnv(const std::string& var, 
                        const std::string& val,
                        bool overwrite);

    /*!
     * Unset an environment variable
     */
    virtual void unsetEnv(const std::string& var);

    virtual size_t getNumCPUs() const;

    /*!
     *  Create a symlink, pathnames can be either absolute or relative
     */
    virtual void createSymlink(const std::string& origPathname, 
                               const std::string& symlinkPathname) const;

    /*!
     * Remove a symlink, pathname can be absolute or relative
     */
    virtual void removeSymlink(const std::string& symlinkPathname) const;

    /*!
     *  Get the total RAM and available RAM on the system in megabytes
     */
    virtual void getMemInfo(size_t& totalPhysMem, size_t& freePhysMem) const;

protected:
    /*!
     *  Remove file with this pathname
     */
    virtual void removeFile(const std::string& pathname) const;

    /*!
     *  Remove directory with this pathname
     *  NOTE: This will throw if the directory is not empty
     */
    virtual void removeDirectory(const std::string& pathname) const;
};



class DirectoryWin32 : public AbstractDirectory
{
public:
    DirectoryWin32() : mHandle(INVALID_HANDLE_VALUE)
    {}
    virtual ~DirectoryWin32()
    {
        close();
    }
    virtual void close();

    virtual std::string findFirstFile(const std::string& dir);

    virtual std::string findNextFile();

    HANDLE mHandle;
    WIN32_FIND_DATA mFileData;

};

}

#endif
#endif
