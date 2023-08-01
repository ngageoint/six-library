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

#ifndef _WIN32

#include "sys/AbstractOS.h"
#include "sys/Conf.h"
#include <dirent.h>
#include <sys/utsname.h>

namespace sys
{
struct OSUnix final : public AbstractOS
{
    OSUnix() = default;
    virtual ~OSUnix() = default;

    virtual std::string getPlatformName() const override;

    virtual std::string getNodeName() const override;

    /*!
     *  Get the path delimiter for this operating system.
     *  For windows, this will be two slashes \\
     *  For unix it will be one slash /
     *  \return The path delimiter
     */
    virtual const char* getDelimiter() const override
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
    virtual bool exists(const std::string& path) const override;

    /*!
     *  Move file with this path name to the newPath
     *  \return True upon success, false if failure
     */
    virtual bool move(const std::string& path,
                      const std::string& newPath) const override;

    /*!
     *  Does this path resolve to a file?
     *  \param path The path
     *  \return True if it does, false if not
     */
    virtual bool isFile(const std::string& path) const override;

    /*!
     *  Does this path resolve to a directory?
     *  \param path The path
     *  \return True if it does, false if not
     */
    virtual bool isDirectory(const std::string& path) const override;

    virtual bool makeDirectory(const std::string& path) const override;


    virtual Pid_T getProcessId() const override;


    /*!
     *  Retrieve the current working directory.
     *  \return The current working directory
     */
    virtual std::string getCurrentWorkingDirectory() const override;

    /*!
     *  Change the current working directory.
     *  \return true if the directory was changed, otherwise false.
     */
    virtual bool changeDirectory(const std::string& path) const override;

    /*!
     *  Get a suitable temporary file name
     *  \return The file name
     */
    virtual std::string getTempName(const std::string& path = ".",
                                    const std::string& prefix = "TMP") const override;

    /*!
     *  Return the size in bytes of a file
     *  \return The file size
     */
    virtual sys::Off_T getSize(const std::string& path) const override;

    /**
     * Returns the last modified time of the file/directory
     */
    virtual sys::Off_T getLastModifiedTime(const std::string& path) const override;

    /*!
     *  This is a system independent sleep function.
     *  Be careful using timing calls with threads
     *  \todo I heard usleep is deprecated, and I should
     *  use nanosleep
     *  \param milliseconds The params
     */
    virtual void millisleep(int milliseconds) const override;

    virtual std::string operator[](const std::string& s) const override;

    /*!
     *  Get an environment variable
     */
    virtual std::string getEnv(const std::string& s) const override;

    /*!
     * Returns true if environment variable is set, false otherwise
     */
    virtual bool isEnvSet(const std::string& s) const override;

    /*!
     *  Set an environment variable
     */
    virtual void setEnv(const std::string& var,
                        const std::string& val,
                        bool overwrite) override;

    /*!
     * Unset an environment variable
     */
    virtual void unsetEnv(const std::string& var) override;

    virtual std::string getDSOSuffix() const override;

    /*!
     * \return the number of logical CPUs present on the machine
     *         (includes hyperthreading)
     */
    virtual size_t getNumCPUs() const override;

    /*!
     * \return the number of logical CPUs available. This will be
     *         affected by pinning (e.g. numactl/taskset), and will
     *         always be <= getNumCPUs.
     */
    virtual size_t getNumCPUsAvailable() const override;

    /*!
     * \return the number of physical CPUs present on the machine
     *         (excludes hyperthreading)
     */
    virtual size_t getNumPhysicalCPUs() const override;

    /*!
     * \return the number of physical CPUs available. This will be
     *         affected by pinning (e.g. numactl/taskset), and will
     *         always be <= getNumPhysicalCPUs.
     */
    virtual size_t getNumPhysicalCPUsAvailable() const override;

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
                                  std::vector<int>& htCPUs) const override;

    /*!
     * Figure out what SIMD instrunctions are available.  Keep in mind these
     * are RUN-TIME, not compile-time, checks.
     */
    SIMDInstructionSet getSIMDInstructionSet() const override;

    /*!
     *  Create a symlink, pathnames can be either absolute or relative
     */
    virtual void createSymlink(const std::string& origPathname,
                               const std::string& symlinkPathname) const override;

    /*!
     * Remove a symlink, pathname can be absolute or relative
     */
    virtual void removeSymlink(const std::string& symlinkPathname) const override;

    /*!
     *  Get the total RAM and available RAM on the system in megabytes
     */
    virtual void getMemInfo(size_t& totalPhysMem, size_t& freePhysMem) const override;

    /*!
     *  Get the absolute path to the current executable
     */
    virtual std::string getCurrentExecutable(
            const std::string& argvPathname="") const override;

protected:
    /*!
     *  Remove file with this pathname
     */
    virtual void removeFile(const std::string& pathname) const override;

    /*!
     *  Remove directory with this pathname
     *  NOTE: This will throw if the directory is not empty
     */
    virtual void removeDirectory(const std::string& pathname) const override;
};

struct DirectoryUnix final : public AbstractDirectory
{
    DirectoryUnix() = default;
    virtual ~DirectoryUnix()
    {
        close();
    }
    void close() override;
    std::string findFirstFile(const std::string& dir) override;
    std::string findNextFile() override;
    DIR* mDir = nullptr;
};

}

#endif
#endif
