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

#include "io/MMapInputStream.h"

void io::MMapInputStream::open(const std::string& fname, char* flags)
{
    mLength = mOs.getSize(fname);
    //    std::cout << mLength << std::endl;
    mFile = fopen(fname.c_str(), "r");
    if (!mFile)
        throw sys::SystemException(str::Format("Failure while opening file: %s", fname));

    _map();

}

void io::MMapInputStream::close()
{
    _unmap();
    fclose(mFile);
    mFile = nullptr;
}

void io::MMapInputStream::_map()
{
    sys::Handle_T handle = getHandle();
    mData = (sys::byte*)mOs.mapFile(handle, mLength, PROT_READ, MAP_SHARED, 0);
}
void io::MMapInputStream::_unmap()
{
    //     std::cout << "Unmapping file... ";
    mOs.unmapFile(mData, mLength);
    mData = nullptr;
    //    std::cout << "done!" << std::endl;
}

sys::Handle_T io::MMapInputStream::getHandle()
{
    if (!mFile) throw except::NullPointerReference(Ctxt("Uninitialized memory mapped file stream!"));
    return ::fileno(mFile);
}



long io::MMapInputStream::seek(long off)
{
    long where = mMark;
    mMark = off;
    return mMark - where;
}


sys::SSize_T io::MMapInputStream::readImpl(void* buffer, size_t len)
{
    int size = available();
    //    std::cout << "Available: " << size << std::endl;
    //     std::cout << "Len: " << len << std::endl;

    if (len < size)
        size = len;
    memcpy(buffer, &mData[mMark], size);
    mMark += size;
    //    std::cout << "Used: " << size << std::endl;
    return size;
}

