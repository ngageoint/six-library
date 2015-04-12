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

#include "io/FileUtils.h"
#include "io/FileOutputStream.h"

std::string io::FileUtils::createFile(std::string dirname,
                                      std::string filename, bool overwrite)
                                                                            throw (except::IOException)
{
    sys::OS os;

    if (!os.exists(dirname))
        throw except::IOException(Ctxt(FmtX("Directory does not exist: %s",
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

void io::FileUtils::touchFile(std::string filename) throw (except::IOException)
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
