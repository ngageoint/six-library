/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#ifndef __IO_FILE_UTILS_H__
#define __IO_FILE_UTILS_H__

#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>

namespace io
{

/*!
 *  Copy a file or directory to a new path. 
 *  Source and destination cannot be the same location
 *
 *  \param path      - source location
 *  \param newath    - destination location
 *  \param blockSize - files are copied in blocks (1MB default)
 *                     NOTE: This is a placeholder and is unused currently
 *  \return True upon success, false if failure
 */
void copy(const std::string& path, 
          const std::string& newPath,
          size_t blockSize = 1048576);

/*!
 *  Move file with this path name to the newPath
 *  \return True upon success, false if failure
 */
inline void move(const std::string& path, 
                 const std::string& newPath)
{
    sys::OS os;
    if (os.move(path, newPath) == false)
    {
        std::ostringstream oss;
        oss << "Move Failed: Could not move source [" <<
            path << "] to destination [" <<
            newPath << "]";
        throw except::Exception(Ctxt(oss.str()));
    }
}

/*!
 *  Remove file with this path name
 *  \return True upon success, false if failure
 */
inline void remove(const std::string& path)
{
    sys::OS os;
    os.remove(path);
}


/**
 * Static file manipulation utilities.
 */
class FileUtils
{
public:
    /**
     * Creates a file in the given directory. It will try to use the filename
     * given. If overwrite is false, it will create a filename based on the
     * given one. If the given filename is empty, a temporary name is used.
     */
    static std::string createFile(std::string dirname, std::string filename =
            std::string(""), bool overwrite = true);

    static void touchFile(std::string filename);

    static void forceMkdir(std::string dirname);

private:
    //private constructor
    FileUtils()
    {
    }
};

}

#endif
