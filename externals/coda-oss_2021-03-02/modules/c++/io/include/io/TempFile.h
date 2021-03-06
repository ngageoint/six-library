/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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


#ifndef __IO_TEMPFILE_H__
#define __IO_TEMPFILE_H__

#include <sys/OS.h>

namespace io
{
/*!
 * RAII object for a temporary file that gets deleted
 * upon object destruction
 */
struct TempFile
{
    /*!
     * Constructor for TempFile object. Provided a directory,
     * this will find a random, unused filename, and create a file
     * with that in in the directory.
     *
     * \param dirname The directory in which to create the file
     */
    TempFile(const std::string& dirname=".");
    ~TempFile();
    /*!
     * Get pathname of temporary file.
     *
     * \return The pathname of the created file
     */
    inline std::string pathname() const
    {
        return mPathname;
    }

    TempFile(const TempFile&) = delete;
    TempFile& operator=(const TempFile&) = delete;

private:
    const sys::OS mOS;
    const std::string mPathname;
};
}

#endif
