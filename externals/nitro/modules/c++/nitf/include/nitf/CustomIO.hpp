/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_CUSTOM_IO_HPP__
#define __NITF_CUSTOM_IO_HPP__

#include <nitf/IOInterface.hpp>

namespace nitf
{
struct CustomIO : public IOInterface
{
    CustomIO();
    ~CustomIO();

protected:
    virtual void readImpl(void* buf, size_t size) = 0;

    virtual void writeImpl(const void* buf, size_t size) = 0;

    virtual bool canSeekImpl() const = 0;

    virtual nitf::Off seekImpl(nitf::Off offset, int whence) = 0;

    virtual nitf::Off tellImpl() const = 0;

    virtual nitf::Off getSizeImpl() const = 0;

    virtual int getModeImpl() const = 0;

    virtual void closeImpl() = 0;

private:
    static
    nitf_IOInterface* createInterface(CustomIO* me) noexcept;

    static
    NRT_BOOL adapterRead(NRT_DATA* data, void* buf, size_t size, nrt_Error* error);

    static
    NRT_BOOL adapterWrite(NRT_DATA* data, const void* buf, size_t size, nrt_Error* error);

    static
    NRT_BOOL adapterCanSeek(NRT_DATA* data, nrt_Error* error);

    static
    nrt_Off adapterSeek(NRT_DATA* data, nrt_Off offset, int whence, nrt_Error* error);

    static
    nrt_Off adapterTell(NRT_DATA* data, nrt_Error* error);

    static
    nrt_Off adapterGetSize(NRT_DATA* data, nrt_Error* error);

    static
    int adapterGetMode(NRT_DATA* data, nrt_Error* error);

    static
    NRT_BOOL adapterClose(NRT_DATA* data, nrt_Error* error);

    static
    void adapterDestruct(NRT_DATA* data) noexcept;
};
}

#endif
