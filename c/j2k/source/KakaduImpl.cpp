/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#ifdef HAVE_KAKADU_H

#include "j2k/Reader.h"
#include "j2k/Container.h"
#include <string>
#include <kdu_compressed.h>
#include <kdu_block_coding.h>
#include <kdu_sample_processing.h>
#include <iostream>
#include <sstream>


/******************************************************************************/
/* TYPES & DECLARATIONS                                                       */
/******************************************************************************/

NRTPRIV( nrt_Uint32) KakaduContainer_getTilesX(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) KakaduContainer_getTilesY(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) KakaduContainer_getTileWidth(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) KakaduContainer_getTileHeight(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) KakaduContainer_getWidth(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) KakaduContainer_getHeight(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) KakaduContainer_getNumComponents(J2K_USER_DATA *, nrt_Error *);
NRTPRIV( nrt_Uint32) KakaduContainer_getComponentBytes(J2K_USER_DATA *, nrt_Error *);
NRTPRIV(void)        KakaduContainer_destruct(J2K_USER_DATA *);

static j2k_IContainer ContainerInterface = { &KakaduContainer_getTilesX,
                                             &KakaduContainer_getTilesY,
                                             &KakaduContainer_getTileWidth,
                                             &KakaduContainer_getTileHeight,
                                             &KakaduContainer_getWidth,
                                             &KakaduContainer_getHeight,
                                             &KakaduContainer_getNumComponents,
                                             &KakaduContainer_getComponentBytes,
                                             &KakaduContainer_destruct};

NRTPRIV( NRT_BOOL  )     KakaduReader_canReadTiles(J2K_USER_DATA *,  nrt_Error *);
NRTPRIV( nrt_Uint64)     KakaduReader_readTile(J2K_USER_DATA *, nrt_Uint32,
                                               nrt_Uint32, nrt_Uint8 **,
                                               nrt_Error *);
NRTPRIV( nrt_Uint64)     KakaduReader_readRegion(J2K_USER_DATA *, nrt_Uint32,
                                                 nrt_Uint32, nrt_Uint32,
                                                 nrt_Uint32, nrt_Uint8 **,
                                                 nrt_Error *);
NRTPRIV( j2k_Container*) KakaduReader_getContainer(J2K_USER_DATA *, nrt_Error *);
NRTPRIV(void)            KakaduReader_destruct(J2K_USER_DATA *);

static j2k_IReader ReaderInterface = {&KakaduReader_canReadTiles,
                                      &KakaduReader_readTile,
                                      &KakaduReader_readRegion,
                                      &KakaduReader_getContainer,
                                      &KakaduReader_destruct };

/******************************************************************************/
/* IMPLEMENTATION CLASSES                                                     */
/******************************************************************************/

namespace j2k
{
namespace kakadu
{

class IOStream : public kdu_compressed_source
{
public:
    IOStream(nrt_IOInterface *io, bool own = false) :
        mIO(io), mOwn(own), mCapabilities(KDU_SOURCE_CAP_SEQUENTIAL
                | KDU_SOURCE_CAP_SEEKABLE)
    {
        mOffset = nrt_IOInterface_tell(mIO, &mError);
    }

    ~IOStream()
    {
        close();
        if (mOwn)
        {
            nrt_IOInterface_destruct(&mIO);
        }
    }

    virtual int get_capabilities()
    {
        return mCapabilities;
    }

    virtual bool seek(kdu_long offset)
    {
        if (!nrt_IOInterface_seek(mIO, mOffset + offset, NRT_SEEK_SET, &mError))
            return false;
        return true;
    }

    virtual kdu_long get_pos()
    {
        nrt_Off tell = nrt_IOInterface_tell(mIO, &mError);
        if (tell >= 0)
            return tell - mOffset;
        return -1;
    }

    virtual int read(kdu_byte *buf, int num_bytes)
    {
        if (!nrt_IOInterface_read(mIO, (char*)buf, num_bytes, &mError))
            return 0;
        return num_bytes;
    }

    virtual bool close()
    {
        if (mOwn)
            nrt_IOInterface_close(mIO, &mError);
        return true;
    }

    void ownIO()
    {
        mOwn = true;
    }

private:
    nrt_IOInterface *mIO;
    bool mOwn;
    int mCapabilities;
    nrt_Off mOffset;
    nrt_Error mError;
};


class ErrorHandler : public kdu_message
{
public:
    void put_text(const char *s)
    {
        mStream << s;
        throw std::string(s);
    }

    void put_text(const kdu_uint16 *s)
    {
        mStream << s;
    }

    void flush(bool endOfMessage = false)
    {
        if (endOfMessage)
            throw std::string(mStream.str());
    }

protected:
    std::stringstream mStream;
};



void copyLine(nrt_Uint8 *dest, kdu_line_buf &src, int bitsPerSample,
              int outputBytes)
{
    int samples = src.get_width();

    if (src.get_buf32())
    {
        throw std::string("Not implemented");
    }
    else if (src.get_buf16())
    {
        kdu_sample16 *sp = src.get_buf16();
        kdu_int16 val;
        kdu_int16 bitShift = 0;

        switch(outputBytes)
        {
        case 1:
        {
            bitShift = bitsPerSample < 8 ? 8 - bitsPerSample : 0;
            for(; samples > 0; --samples, ++sp, ++dest)
            {
                val = (sp->ival << bitShift) + 128;
                *dest = (kdu_byte)val;
            }
            break;
        }
        case 2:
        {
            /* TODO */
        }
        default:
            throw std::string("Not implemented");
        }
    }
    else
    {
        throw std::string("Unsupported/invalid line buffer");
    }
}


#if 0
void
copyBlock(nrt_Uint8 *dest, kdu_block *block, int bitsPerSample, int outputBytes)
{
    size_t samples = (size_t)block->size.x * block->size.y;
    size_t i;
    kdu_int32 *inPtr = block->sample_buffer;
    kdu_int32 bitShift, val;

    switch(outputBytes)
    {
    case 1:
    {
        bitShift = 32 - bitsPerSample;
        for(; samples > 0; --samples, ++inPtr, dest += outputBytes)
        {
            val = (*inPtr << bitShift) + 128;
            *dest = (kdu_byte)val;
        }
        break;
    }
    default:
        throw std::string("Not yet implemented");
    }

}
#endif


class UserContainer
{
public:

    UserContainer()
    {
    }

    UserContainer(nrt_Uint32 width,
                  nrt_Uint32 height,
                  nrt_Uint32 bands,
                  nrt_Uint32 actualBitsPerPixel,
                  nrt_Uint32 tileWidth,
                  nrt_Uint32 tileHeight,
                  int imageType = J2K_TYPE_UNKNOWN,
                  bool isSigned = false) :
                      mWidth(width),
                      mHeight(height),
                      mComponents(bands),
                      mBitsPerPixel(actualBitsPerPixel),
                      mTileWidth(tileWidth),
                      mTileHeight(tileHeight),
                      mType(imageType),
                      mSigned(isSigned)
    {
    }

    nrt_Uint32 getNumComponents() const
    {
        return mComponents;
    }

    nrt_Uint32 getWidth() const
    {
        return mWidth;
    }

    nrt_Uint32 getHeight() const
    {
        return mHeight;
    }

    nrt_Uint32 getComponentBytes() const
    {
        return (getActualBitsPerPixel() - 1) / 8 + 1;
    }

    nrt_Uint32 getActualBitsPerPixel() const
    {
        return mBitsPerPixel;
    }

    nrt_Uint32 getTileWidth() const
    {
        return mTileWidth;
    }

    nrt_Uint32 getTileHeight() const
    {
        return mTileHeight;
    }

    nrt_Uint32 getTilesX() const
    {
        return mWidth / mTileWidth + (mWidth % mTileWidth == 0 ? 0 : 1);
    }

    nrt_Uint32 getTilesY() const
    {
        return mHeight / mTileHeight + (mHeight % mTileHeight == 0 ? 0 : 1);
    }

    int getType() const
    {
        return mType;
    }

    bool isSigned() const
    {
        return mSigned;
    }


protected:
    friend class Reader;
    nrt_Uint32 mHeight, mWidth, mComponents, mBitsPerPixel;
    nrt_Uint32 mTileWidth, mTileHeight;
    int mType;
    bool mSigned;
};


class Reader
{
public:
    Reader(nrt_IOInterface *io) : mIO(io), mSource(NULL), mContainer(NULL)
    {
        init();
    }
    ~Reader()
    {
        if (mSource)
            delete mSource;
        if (mCodestream)
        {
            mCodestream->destroy();
            delete mCodestream;
        }
        if (mContainer)
            j2k_Container_destruct(&mContainer);
    }

    void ownIO()
    {
        mSource->ownIO();
    }

    UserContainer* getUserContainer() const
    {
        return (UserContainer*)mContainer->data;
    }

    j2k_Container* getContainer() const
    {
        return mContainer;
    }

    nrt_Uint64 readTile(nrt_Uint32 tileX, nrt_Uint32 tileY, nrt_Uint8 **buf)
    {
        UserContainer *container = getUserContainer();
        nrt_Uint32 nComps = container->getNumComponents();
        nrt_Uint32 sampleSize = container->getComponentBytes();
        nrt_Uint64 lineSize = (nrt_Uint64)container->getTileWidth() * sampleSize;
        nrt_Uint64 compSize = lineSize * container->getTileHeight();

        nrt_Uint64 bufSize = compSize * nComps;
        if (buf && !*buf)
        {
            *buf = (nrt_Uint8*)NRT_MALLOC(bufSize);
            if (!*buf)
                throw std::string("Out of memory");
        }

        kdu_coords tileIndex(tileX, tileY);
        kdu_tile tile = mCodestream->open_tile(tileIndex, NULL);

        int tileComponents = tile.get_num_components();
        assert(tileComponents == nComps);

        nrt_Uint8 *bufPtr = *buf;
        for(nrt_Uint32 i = 0; i < nComps; ++i)
        {
            kdu_tile_comp tileComponent = tile.access_component(i);
            kdu_resolution tileRes = tileComponent.access_resolution();
            int bitDepth = tileComponent.get_bit_depth();

            kdu_dims tileSize;
            tileRes.get_dims(tileSize);

            bool shortBuffer = sampleSize <= 2;
            kdu_line_buf line;
            kdu_sample_allocator allocator;
            line.pre_create(&allocator, tileSize.size.x,
                            tileComponent.get_reversible(), shortBuffer);

            kdu_pull_ifc engine;
            if (tileRes.which() == 0)
                engine = kdu_decoder(tileRes.access_subband(LL_BAND),
                                     &allocator, shortBuffer);
            else
                engine = kdu_synthesis(tileRes, &allocator, shortBuffer);
            allocator.finalize();
            line.create();

            for(int y = 0; y < tileSize.size.y; y++ )
            {
                engine.pull(line, true);
                copyLine(bufPtr + y * lineSize, line, bitDepth, sampleSize);
            }
            engine.destroy();

#if 0
            int minBand;
            nrt_Uint32 nBands = tileRes.get_valid_band_indices(minBand);

            for(nrt_Uint32 b = minBand; nBands > 0; --nBands, ++b)
            {
                kdu_subband tileBand = tileRes.access_subband(b);
                kdu_dims validBlocks;
                tileBand.get_valid_blocks(validBlocks);

                kdu_coords idx;
                for(idx.y = 0; idx.y < validBlocks.size.y; ++idx.y)
                {
                    for (idx.x = 0; idx.x < validBlocks.size.x; ++idx.x)
                    {
                        kdu_block *block = tileBand.open_block(idx + validBlocks.pos);
                        kdu_block_decoder decoder;
                        decoder.decode(block);

                        copyBlock(bufPtr, block, bitDepth, sampleSize);
                        bufPtr += block->size.x * block->size.y;
                        tileBand.close_block(block);
                    }
                }
            }
#endif
        }

        tile.close();
        return bufSize;
    }


protected:
    nrt_IOInterface *mIO;
    IOStream *mSource;
    kdu_codestream *mCodestream;
    j2k_Container *mContainer;
    nrt_Error mError;

    void init()
    {
        ErrorHandler errHandler;
        kdu_customize_errors(&errHandler);

        mSource = new IOStream(mIO);

        if (!(mCodestream = new kdu_codestream))
            throw std::string("Unable to create codestream");

        mCodestream->create(mSource);
        mCodestream->apply_input_restrictions(0, 0, 0, 0, NULL,
                                              KDU_WANT_OUTPUT_COMPONENTS);

        // create the j2k_Container
        mContainer = (j2k_Container*) NRT_MALLOC(sizeof(j2k_Container));
        if (!mContainer)
            throw std::string("Unable to create container");
        memset(mContainer, 0, sizeof(j2k_Container));
        mContainer->iface = &ContainerInterface;

        UserContainer *container = new UserContainer;
        mContainer->data = container;

        // cache some metadata
        container->mComponents = mCodestream->get_num_components(true);

        kdu_dims compSize;
        mCodestream->get_dims(-1, compSize, true);
        container->mWidth = (nrt_Uint32)(compSize.size.x);
        container->mHeight = (nrt_Uint32)(compSize.size.y);
        container->mBitsPerPixel = mCodestream->get_bit_depth(-1, true);

        kdu_dims partition;
        mCodestream->get_tile_partition(partition);
        container->mTileWidth = (nrt_Uint32)partition.size.x;
        container->mTileHeight = (nrt_Uint32)partition.size.y;

//        kdu_dims indices;
//        mCodestream->get_valid_tiles(indices);
//        container->mTilesX = indices.size.x;
//        container->mTilesY = indices.size.y;

    }
};

}
}


/******************************************************************************/
/* CONTAINER                                                                  */
/******************************************************************************/

NRTPRIV( nrt_Uint32)
KakaduContainer_getTilesX(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getTilesX();
}

NRTPRIV( nrt_Uint32)
KakaduContainer_getTilesY(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getTilesY();
}

NRTPRIV( nrt_Uint32)
KakaduContainer_getTileWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getTileWidth();
}

NRTPRIV( nrt_Uint32)
KakaduContainer_getTileHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getTileHeight();
}

NRTPRIV( nrt_Uint32)
KakaduContainer_getWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getWidth();
}

NRTPRIV( nrt_Uint32)
KakaduContainer_getHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getHeight();
}

NRTPRIV( nrt_Uint32)
KakaduContainer_getNumComponents(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getNumComponents();
}

NRTPRIV( nrt_Uint32)
KakaduContainer_getComponentBytes(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getComponentBytes();
}

NRTPRIV(void)
KakaduContainer_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
        delete impl;
    }
}

/******************************************************************************/
/* READER                                                                     */
/******************************************************************************/

NRTPRIV( NRT_BOOL)
KakaduReader_canReadTiles(J2K_USER_DATA *data, nrt_Error *error)
{
    return NRT_SUCCESS;
}

NRTPRIV( nrt_Uint64)
KakaduReader_readTile(J2K_USER_DATA *data, nrt_Uint32 tileX, nrt_Uint32 tileY,
                  nrt_Uint8 **buf, nrt_Error *error)
{
    j2k::kakadu::Reader *reader = (j2k::kakadu::Reader*) data;

    try
    {
        return reader->readTile(tileX, tileY, buf);
    }
    catch(std::string &ex)
    {
        nrt_Error_init(error, ex.c_str(), NRT_CTXT, NRT_ERR_INVALID_OBJECT);
        return 0;
    }
}

NRTPRIV( nrt_Uint64)
KakaduReader_readRegion(J2K_USER_DATA *data, nrt_Uint32 x0, nrt_Uint32 y0,
                    nrt_Uint32 x1, nrt_Uint32 y1, nrt_Uint8 **buf,
                    nrt_Error *error)
{
    j2k::kakadu::Reader *reader = (j2k::kakadu::Reader*) data;
    // TODO
    return 0;
}

NRTPRIV( j2k_Container*)
KakaduReader_getContainer(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::Reader *reader = (j2k::kakadu::Reader*) data;
    return reader->getContainer();
}

NRTPRIV(void)
KakaduReader_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
        delete impl;
    }
}

/******************************************************************************/
/******************************************************************************/
/* PUBLIC FUNCTIONS                                                           */
/******************************************************************************/
/******************************************************************************/

NRTAPI(j2k_Reader*) j2k_Reader_open(const char *fname, nrt_Error *error)
{
    j2k_Reader *reader = NULL;
    nrt_IOHandle handle;
    nrt_IOInterface *io = NULL;

    if (!fname)
    {
        nrt_Error_init(error, "NULL filename", NRT_CTXT, NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    handle = nrt_IOHandle_create(fname, NRT_ACCESS_READONLY, NRT_OPEN_EXISTING,
                                 error);
    if (NRT_INVALID_HANDLE(handle))
    {
        nrt_Error_init(error, "Invalid IO handle", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    if (!(io = nrt_IOHandleAdaptor_construct(handle, error)))
        goto CATCH_ERROR;

    if (!(reader = j2k_Reader_openIO(io, error)))
        goto CATCH_ERROR;

    ((j2k::kakadu::Reader*) reader->data)->ownIO();

    return reader;

    CATCH_ERROR:
    {
        if (io)
            nrt_IOInterface_destruct(&io);
        if (reader)
            j2k_Reader_destruct(&reader);
        return NULL;
    }
}

NRTAPI(j2k_Reader*) j2k_Reader_openIO(nrt_IOInterface *io, nrt_Error *error)
{
    j2k_Reader *reader = NULL;
    reader = (j2k_Reader *) NRT_MALLOC(sizeof(j2k_Reader));
    if (!reader)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(reader, 0, sizeof(j2k_Reader));

    try
    {
        if (!(reader->data = new j2k::kakadu::Reader(io)))
            goto CATCH_ERROR;
    }
    catch(std::string& ex)
    {
        nrt_Error_init(error, ex.c_str(), NRT_CTXT, NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    reader->iface = &ReaderInterface;
    return reader;

    CATCH_ERROR:
    {
        if (reader)
        {
            j2k_Reader_destruct(&reader);
        }
        return NULL;
    }
}

NRTAPI(j2k_Container*) j2k_Container_construct(nrt_Uint32 width,
        nrt_Uint32 height,
        nrt_Uint32 bands,
        nrt_Uint32 actualBitsPerPixel,
        nrt_Uint32 tileWidth,
        nrt_Uint32 tileHeight,
        int imageType,
        int isSigned,
        nrt_Error *error)
{
    j2k_Container *container = NULL;
    j2k::kakadu::UserContainer *userContainer = NULL;

    container = (j2k_Container*) NRT_MALLOC(sizeof(j2k_Container));
    if (!container)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(container, 0, sizeof(j2k_Container));

    if (!(container->data = new j2k::kakadu::UserContainer(width, height, bands,
                                                           actualBitsPerPixel,
                                                           tileWidth,
                                                           tileHeight,
                                                           imageType,
                                                           isSigned)))
        goto CATCH_ERROR;

    container->iface = &ContainerInterface;

    return container;

    CATCH_ERROR:
    {
        if (container)
        {
            j2k_Container_destruct(&container);
        }
        return NULL;
    }
}

#endif
