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

#include <nitf/DecompressionInterface.hpp>
#include <nitf/BlockingInfo.hpp>

using namespace nitf;

NITF_BOOL DecompressionInterface::adapterStart(
    nitf_DecompressionControl* object,
    nitf_IOInterface* io,
    uint64_t offset,
    uint64_t fileLength,
    nitf_BlockingInfo* blockingDefinition,
    uint64_t* blockMask, 
    nitf_Error* error)
{
    try
    {
        nitf::IOInterface ioInter(io);
        ioInter.setManaged(true);
        nitf::BlockingInfo blockInfo(blockingDefinition);
        blockInfo.setManaged(true);
        static_cast<Decompressor*>(object)->start(ioInter,
                                                       offset, 
                                                       fileLength, 
                                                       blockInfo,
                                                       blockMask);
        return NRT_SUCCESS;
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_DECOMPRESSION);
        return NRT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_DECOMPRESSION);
        return NRT_FAILURE;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_DECOMPRESSION);
        return NRT_FAILURE;
    }
}

uint8_t* DecompressionInterface::adapterReadBlock(
    nitf_DecompressionControl* object,
    uint32_t blockNumber, 
    uint64_t* blockSize, 
    nitf_Error* error)
{
    try
    {
        return static_cast<Decompressor*>(object)->readBlock(blockNumber,
                                                                  blockSize); 
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_DECOMPRESSION);
        return nullptr;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_DECOMPRESSION);
        return nullptr;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_DECOMPRESSION);
        return nullptr;
    }
}

NITF_BOOL DecompressionInterface::adapterFreeBlock(
    nitf_DecompressionControl* object,
    std::byte* block,
    nitf_Error* error)
{
    try
    {
        static_cast<Decompressor*>(object)->freeBlock(block);
        return NRT_SUCCESS;
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_DECOMPRESSION);
        return NRT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_DECOMPRESSION);
        return NRT_FAILURE;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_DECOMPRESSION);
        return NRT_FAILURE;
    }
}

void DecompressionInterface::adapterDestroy(
    nitf_DecompressionControl** object) noexcept
{
    if (object != nullptr && *object != nullptr)
    {
        delete static_cast<Decompressor*>(*object);
        *object = nullptr;
    }
}


