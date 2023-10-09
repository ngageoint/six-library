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

#include <sstream>
#include <io/FileUtils.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>

/*!
 *  Copy a file or directory permissions and ownership.
 *
 *  NOTE: This is not exposed to the user because windows 
 *        functionality is unsupported. Once this is
 *        implemented copyPermissions() and setPermissions() 
 *        functions should be promoted to the header.
 *
 *  \param src      - source location
 *  \param dest     - destination location
 *  \return True upon success, false if failure
 */
void copyPermissions(const std::string& src, 
                     const std::string& dest)
{
#ifndef _WIN32
    // set up permissions on unix --
    // copy the source's permissions
    struct stat statBuf;
    if (stat(src.c_str(), &statBuf) == -1)
    {
        throw except::Exception(Ctxt(
            "Copy Failed: Could not obtain the statistics of the input"));
    }

    // set the file with the appropriate access --
    // permissions should exactly match that of the source file
    // NOTE: this will be ignored on Windows-based mapped file systems
    if (chmod(dest.c_str(), statBuf.st_mode) == -1)
    {
        throw except::Exception(Ctxt(
            "Copy Failed: Could not set the permissions of the output"));
    }

    // set the file ownership --
    // ownership should exactly match that of the source file
    // NOTE: ownership can only change to groups the user belongs to
    //       and will fail otherwise
    if (chown(dest.c_str(), statBuf.st_uid, statBuf.st_gid) == -1)
    {
        throw except::Exception(Ctxt(
            "Copy Failed: Could not set the ownership of the output"));
    }
#else
    UNREFERENCED_PARAMETER(src);
    UNREFERENCED_PARAMETER(dest);
#endif
}

void io::copy(const std::string& path, 
              const std::string& newPath,
              size_t blockSize)
{
    //! list will find '.' and '..' in the directory
    const std::string item = sys::Path::splitPath(path).second;
    if (item == "." || item == "..")
    {
        return;
    }

    sys::OS os;
    if(os.isDirectory(path))
    {
        std::string destDir = sys::Path::joinPaths(newPath, item);

        // make the destination directory if it doesn't exist
        if (!os.exists(destDir))
        {
            os.makeDirectory(destDir);
        }
        copyPermissions(path, destDir);

        // recursively copy directories
        std::vector<std::string> contents = sys::Path::list(path);
        for (size_t ii = 0; ii < contents.size(); ++ii)
        {
            std::string srcFile  = sys::Path::joinPaths(path, contents[ii]);
            io::copy(srcFile, destDir, blockSize);
        }
    }
    else
    {
        std::string newFile = sys::Path::joinPaths(newPath, item);
        io::FileInputStream fis(path);
        io::FileOutputStream fos(newFile);
        const sys::SSize_T numBytes = fis.streamTo(fos);
        fis.close();
        fos.close();

        copyPermissions(path, newFile);

        if (numBytes < 0)
        {
            std::ostringstream oss;
            oss << "Copy Failed: Could not copy source [" <<
                path << "] to destination [" <<
                newFile << "]";
            throw except::Exception(Ctxt(oss.str()));
        }
    }
}

std::string io::FileUtils::createFile(std::string dirname,
        std::string filename, bool overwrite)
{
    sys::OS os;

    if (!os.exists(dirname))
        throw except::IOException(Ctxt(str::Format("Directory does not exist: %s",
                                            dirname.c_str())));

    str::trim(filename);

    bool emptyName = filename.empty();
    //to protect against full paths being passed in
    filename = emptyName ? filename : sys::Path::basename(filename);

    std::string outFilename = sys::Path::joinPaths(dirname, filename);
    if (emptyName || (os.exists(outFilename) && !overwrite))
    {
        std::string ext = "";
        if (!emptyName)
        {
            sys::Path::StringPair nameExt = sys::Path::splitExt(filename);
            filename = nameExt.first;
            ext = nameExt.second;

            int count = 0;
            while (os.exists(outFilename))
            {
                std::string base = filename + "-" + str::toString(++count);
                outFilename = sys::Path::joinPaths(dirname, base);
                if (!ext.empty())
                {
                    outFilename += ext;
                }
            }
        }
        else
        {
            //just create a temp filename
            outFilename = os.getTempName(dirname);
        }
        //now, touch it
        io::FileUtils::touchFile(outFilename);
    }
    return outFilename;
}

void io::FileUtils::touchFile(std::string filename)
{
    sys::OS os;
    if (os.exists(filename))
    {
        io::FileOutputStream f(filename, sys::File::EXISTING
                | sys::File::WRITE_ONLY);
        f.close();
    }
    else
    {
        io::FileOutputStream f(filename, sys::File::CREATE
                | sys::File::TRUNCATE);
        f.close();
    }
}

void io::FileUtils::forceMkdir(std::string dirname)
{
    sys::OS os;
    if (os.exists(dirname))
    {
        if (!os.isDirectory(dirname))
            throw except::IOException(Ctxt(
                    "Cannot create directory - file already exists"));
    }
    else
    {
        if (!os.makeDirectory(dirname))
            throw except::IOException(Ctxt("Cannot create directory"));
    }
}
