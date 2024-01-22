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
#include <nitf/Utils.hpp>

using namespace nitf;

template<typename TReturn, typename TFunction>
static inline auto invoke(nitf_Error* error, const char* file, int line, const char* func,
    TReturn catch_return, TFunction f)
{
    try
    {
        return f();
    }
    catch (const except::Exception& ex)
    {
        Utils::error_init(error, ex.getMessage(), file, line, func, NRT_ERR_DECOMPRESSION);
    }
    catch (const std::exception& ex)
    {
        Utils::error_init(error, ex, file, line, func, NRT_ERR_DECOMPRESSION);
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", file, line, func, NRT_ERR_DECOMPRESSION);
    }

    return catch_return;
}

// Ensure the return type is deduced correctly.  NRT_SUCCESS/NRT_FAILURE
// is `int`, not `NITF_BOOL`.
constexpr NITF_BOOL nrt_success = NRT_SUCCESS;
constexpr NITF_BOOL nrt_failure = NRT_FAILURE;

NITF_BOOL DecompressionInterface::adapterStart(
    nitf_DecompressionControl* object,
    nitf_IOInterface* io,
    uint64_t offset,
    uint64_t fileLength,
    nitf_BlockingInfo* blockingDefinition,
    uint64_t* blockMask, 
    nitf_Error* error)
{
    return invoke(error, NRT_CTXT, nrt_failure, [&]()
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
        return nrt_success;
    });
}

uint8_t* DecompressionInterface::adapterReadBlock(
    nitf_DecompressionControl* object,
    uint32_t blockNumber, 
    uint64_t* blockSize, 
    nitf_Error* error)
{
    const auto f = [&]() {
        return static_cast<Decompressor*>(object)->readBlock(blockNumber,
                                                                  blockSize); 
    };
    using retval_t = decltype(f());
    return invoke(error, NRT_CTXT, static_cast<retval_t>(nullptr), f);
}

NITF_BOOL DecompressionInterface::adapterFreeBlock(
    nitf_DecompressionControl* object,
    uint8_t* block,
    nitf_Error* error)
{
    return invoke(error, NRT_CTXT, nrt_failure, [&]()
    {
        static_cast<Decompressor*>(object)->freeBlock(block);
        return nrt_success;
    });
}

NITF_BOOL DecompressionInterface::adapterFreeBlock(
    nitf_DecompressionControl* object,
    std::byte* block,
    nitf_Error* error)
{
    return invoke(error, NRT_CTXT, nrt_failure, [&]()
    {
        static_cast<Decompressor*>(object)->freeBlock(block);
        return nrt_success;
    });
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


