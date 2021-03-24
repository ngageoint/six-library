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

#include <nitf/CompressionInterface.hpp>

using namespace nitf;

NITF_BOOL CompressionInterface::adapterStart(
    nitf_CompressionControl* object,
    uint64_t offset,
    uint64_t dataLength,
    uint64_t* blockMask,
    uint64_t* padMask, 
    nitf_Error* error)
{
    try
    {
        static_cast<Compressor*>(object)->start(offset, 
                                                     dataLength, 
                                                     blockMask, 
                                                     padMask);
        return NRT_SUCCESS;
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_COMPRESSION);
        return NRT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_COMPRESSION);
        return NRT_FAILURE;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_COMPRESSION);
        return NRT_FAILURE;
    }
}

NITF_BOOL CompressionInterface::adapterWriteBlock(
    nitf_CompressionControl* object, 
    nitf_IOInterface* io,
    const uint8_t* data,
    NITF_BOOL pad,
    NITF_BOOL noData,
    nitf_Error* error)
{
    try
    {
        nitf::IOInterface ioInter(io);
        ioInter.setManaged(true);
        static_cast<Compressor*>(object)->writeBlock(ioInter,
                                                          data, 
                                                          pad, 
                                                          noData);
        return NRT_SUCCESS;
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_COMPRESSION);
        return NRT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_COMPRESSION);
        return NRT_FAILURE;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_COMPRESSION);
        return NRT_FAILURE;
    }
}
NITF_BOOL CompressionInterface::adapterWriteBlock(
    nitf_CompressionControl* object,
    nitf_IOInterface* io,
    const std::byte* data,
    NITF_BOOL pad,
    NITF_BOOL noData,
    nitf_Error* error)
{
    const void* const data_ = data;
    return adapterWriteBlock(object, io, static_cast<const uint8_t*>(data_), pad, noData, error);
}

NITF_BOOL CompressionInterface::adapterEnd(
    nitf_CompressionControl* object,
    nitf_IOInterface* io,
    nitf_Error* error)
{
    try
    {
        nitf::IOInterface ioInter(io);
        ioInter.setManaged(true);
        static_cast<Compressor*>(object)->end(ioInter);
        return NRT_SUCCESS;
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_COMPRESSION);
        return NRT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_COMPRESSION);
        return NRT_FAILURE;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_COMPRESSION);
        return NRT_FAILURE;
    }
}

void CompressionInterface::adapterDestroy(
    nitf_CompressionControl** object) noexcept
{
    if (object != nullptr && *object != nullptr)
    {
        delete static_cast<Compressor*>(*object);
        *object = nullptr;
    }
}
