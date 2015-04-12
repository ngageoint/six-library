/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
            std::string(""), bool overwrite = true) throw (except::IOException);

    static void touchFile(std::string filename) throw (except::IOException);

private:
    //private constructor
    FileUtils()
    {
    }
};

}

#endif
