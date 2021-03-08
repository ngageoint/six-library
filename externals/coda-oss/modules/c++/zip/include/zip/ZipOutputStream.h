/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * zip-c++ is free software; you can redistribute it and/or modify
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

#ifndef __ZIP_ZIP_OUTPUT_STREAM_H__
#define __ZIP_ZIP_OUTPUT_STREAM_H__

#include <string>
#include <zip.h>
#include <sys/Conf.h>
#include <io/OutputStream.h>

namespace zip
{
/*
 *  \class ZipOutputStream
 *  \brief Creates a zip file which can hold a variable amount of files
 *         in a user defined directory structure.
 */
class ZipOutputStream: public io::OutputStream
{
public:
    /*
     *  \func Constructor
     *  \brief Sets up the internal structure of the class.
     *
     *  \param pathname The path and filename of the zip.
     */
    ZipOutputStream(const std::string& pathname);

    /*
     *  \func createFileInZip
     *  \brief Creates a new file within the zip which can be written to.
     *
     *  \pathname The directory path and filename as it will appear in the zip.
     *  \comment An optional comment.
     *  \password An optional password for the file.
     */
    void createFileInZip(const std::string& pathname,
                         const std::string& comment = "",
                         const std::string& password = "");

    /*
     *  \func closeFileInZip
     *  \brief Closes a file that was opened by createFileInZip. This will
     *         throw if a file was not created.
     */
    void closeFileInZip();

    /*
     *  \func write
     *  \brief Convenience function which will create, write, and close a file.
     *
     *  \inputPathname The path to the file that you want added to the zip.
     *  \zipPathname The path and file name you want the inputPathname to 
     *               appear as in the zip file.
     */
    void write(const std::string& inputPathname,
               const std::string& zipPathname);

    virtual void write(const void* buffer, size_t len);

    virtual void close();

private:
    zipFile mZip;
};
}

#endif
