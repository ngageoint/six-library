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

#ifndef __NITF_DECOMPRESSION_INTERFACE_HPP__
#define __NITF_DECOMPRESSION_INTERFACE_HPP__
#pragma once

#include <nitf/coda-oss.hpp>
#include <nitf/ImageSubheader.hpp>
#include <nitf/IOInterface.hpp>
#include <nitf/BlockingInfo.hpp>
#include <nitf/ImageIO.h>


/*!
 *  This is a macro for quickly exposing hooks to a c++ layer 
 *  Decompressor object. The idea here is to setup everything in
 *  one shot. All the user has to do is create a 
 *  nitf_DecompressionInterface 'open' method, and declare the
 *  identifier for decompression type.
 */
#define NITF_CREATE_CUSTOM_DECOMPRESSION(_DECOMPRESSION_ID, \
                                         _DECOMPRESSION_DLL_NAME, \
                                         _DECOMPRESSION_ADAPTER_OPEN_FUNC) \
NITF_CXX_GUARD \
static const char* _DECOMPRESSION_ID##_ident[] = \
{\
    NITF_PLUGIN_DECOMPRESSION_KEY, #_DECOMPRESSION_ID, nullptr\
};\
\
static nitf_DecompressionInterface _DECOMPRESSION_ID##_INTERFACE_TABLE = {\
    &_DECOMPRESSION_ADAPTER_OPEN_FUNC,\
    &::nitf::DecompressionInterface::adapterStart,\
    &::nitf::DecompressionInterface::adapterReadBlock,\
    &::nitf::DecompressionInterface::adapterFreeBlock,\
    &::nitf::DecompressionInterface::adapterDestroy};\
\
NITFAPI(const char**) _DECOMPRESSION_DLL_NAME##_init(nitf_Error *error)\
{\
    return _DECOMPRESSION_ID##_ident;\
}\
\
NITFAPI(void) _DECOMPRESSION_ID##_cleanup(void)\
{\
}\
\
NITFAPI(void*) _DECOMPRESSION_ID##_construct(char* decompressionType,\
                                             nitf_Error* error)\
{\
    if (strcmp(decompressionType, #_DECOMPRESSION_ID) != 0)\
    {\
        nitf_Error_init(error,\
                        "Unsupported decompression type",\
                        NITF_CTXT,\
                        NITF_ERR_DECOMPRESSION);\
        return nullptr;\
    }\
    return &_DECOMPRESSION_ID##_INTERFACE_TABLE;\
}\
NITF_CXX_ENDGUARD

namespace nitf
{

/*!
 *  \class DecompressionInterface
 *  \brief Decompression object controls all innerworkings 
 *         during decompression, and gives a c++ api for getting
 *         things done.
 */
class DecompressionInterface
{
public:

    //! These are canned methods which turn around 
    //  and call the nitf_DecompressionControl of your choice
    static NITF_BOOL adapterStart(nitf_DecompressionControl* object,
                                  nitf_IOInterface* io,
                                  uint64_t offset,
                                  uint64_t fileLength,
                                  nitf_BlockingInfo* blockingDefinition,
                                  uint64_t* blockMask, 
                                  nitf_Error* error);

    static uint8_t* adapterReadBlock(nitf_DecompressionControl* object,
                                        uint32_t blockNumber, 
                                        uint64_t* blockSize, 
                                        nitf_Error* error);

    static NITF_BOOL adapterFreeBlock(nitf_DecompressionControl* object,
                                      uint8_t* block, 
                                      nitf_Error* error);
    static NITF_BOOL adapterFreeBlock(nitf_DecompressionControl* object,
                                      std::byte* block, 
                                      nitf_Error* error);

    static void adapterDestroy(nitf_DecompressionControl** object) noexcept;

};

/*!
 *  \class Compressor
 *  \brief This is the c++ interface for nitf_CompressionControl
 */
struct Decompressor
{
    Decompressor() = default;
    virtual ~Decompressor() {}

    virtual void start(nitf::IOInterface& io,
                       uint64_t offset,
                       uint64_t fileLength,
                       nitf::BlockingInfo& blockingDefinition,
                       uint64_t* blockMask) = 0;

    virtual uint8_t* readBlock(uint32_t blockNumber,
                                  uint64_t* blockSize) = 0;

    virtual void freeBlock(uint8_t* block) = 0;
    virtual void freeBlock(std::byte* block)
    {
        void* block_ = block;
        freeBlock(static_cast<uint8_t*>(block_));
    }
};

}

#endif