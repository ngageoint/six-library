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


#ifndef __SYS_OS_UNIX_H__
#define __SYS_OS_UNIX_H__

#if !(defined(WIN32) || defined(_WIN32))

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
     *  Move file with this path name to the newPath
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
    virtual std::string getTempName(const std::string& path = ".",
                                    const std::string& prefix = "TMP") const;

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

    virtual std::string getDSOSuffix() const;

    /*!
     * \return the number of logical CPUs present on the machine
     *         (includes hyperthreading)
     */
    virtual size_t getNumCPUs() const;

    /*!
     * \return the number of logical CPUs available. This will be
     *         affected by pinning (e.g. numactl/taskset), and will
     *         always be <= getNumCPUs.
     */
    virtual size_t getNumCPUsAvailable() const;

    /*!
     * \return the number of physical CPUs present on the machine
     *         (excludes hyperthreading)
     */
    virtual size_t getNumPhysicalCPUs() const;

    /*!
     * \return the number of physical CPUs available. This will be
     *         affected by pinning (e.g. numactl/taskset), and will
     *         always be <= getNumPhysicalCPUs.
     */
    virtual size_t getNumPhysicalCPUsAvailable() const;

    /*!
     * Divide the available CPUs (pinned with numactl/taskset) into
     * a set of physical CPUs and a set of hyperthreaded CPUs. Note
     * that there is no real distinction between CPUs that share a core,
     * and the separation here is not unique. However, there will only ever
     * be 1 entry per core in the physical CPUs list, while the remainder
     * of CPUs present in the core will be assigned to the htCPU list.
     *
     * \param[out] physicalCPUs List of physical CPUs. Size of
     *                          getNumPhysicalCPUsAvailable().
     * \param[out] htCPUs List of CPUs that share a core with a CPU in
     *                    'physicalCPUs'. Size of
     *                    getNumCPUsAvailable() - getNumPhysicalCPUsAvailable().
     */
    virtual void getAvailableCPUs(std::vector<int>& physicalCPUs,
                                  std::vector<int>& htCPUs) const;

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

    /*!
     *  Get the absolute path to the current executable
     */
    virtual std::string getCurrentExecutable(
            const std::string& argvPathname="") const;

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


class DirectoryUnix : public AbstractDirectory
{
public:
    DirectoryUnix() : mDir(nullptr)
    {}
    virtual ~DirectoryUnix()
    {
        close();
    }
    virtual void close();
    virtual std::string findFirstFile(const std::string& dir);
    virtual std::string findNextFile();
    DIR* mDir;

};

}

#endif
#endif
