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
    NITF_PLUGIN_DECOMPRESSION_KEY, #_DECOMPRESSION_ID, NULL\
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
        return NULL;\
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
                                  nitf_Uint64 offset,
                                  nitf_Uint64 fileLength,
                                  nitf_BlockingInfo* blockingDefinition,
                                  nitf_Uint64* blockMask, 
                                  nitf_Error* error);

    static nitf_Uint8* adapterReadBlock(nitf_DecompressionControl* object,
                                        nitf_Uint32 blockNumber, 
                                        nitf_Uint64* blockSize, 
                                        nitf_Error* error);

    static NITF_BOOL adapterFreeBlock(nitf_DecompressionControl* object,
                                      nitf_Uint8* block, 
                                      nitf_Error* error);

    static void adapterDestroy(nitf_DecompressionControl** object);

};

/*!
 *  \class Compressor
 *  \brief This is the c++ interface for nitf_CompressionControl
 */
class Decompressor
{
public:
    Decompressor() {}
    virtual ~Decompressor() {}

    virtual void start(nitf::IOInterface& io,
                       nitf::Uint64 offset,
                       nitf::Uint64 fileLength,
                       nitf::BlockingInfo& blockingDefinition,
                       nitf::Uint64* blockMask) = 0;

    virtual nitf_Uint8* readBlock(nitf::Uint32 blockNumber, 
                                  nitf::Uint64* blockSize) = 0;

    virtual void freeBlock(nitf::Uint8* block) = 0;
};

}

#endif
