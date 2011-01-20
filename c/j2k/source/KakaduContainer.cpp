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

#include "j2k/Container.h"
#include <string>
#include <kdu_compressed.h>
#include <kdu_block_coding.h>
#include <kdu_sample_processing.h>
#include <iostream>
#include <sstream>

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
    UserContainer(nrt_IOInterface *io) : mIO(io), mSource(NULL)
    {
        init();
    }
    ~UserContainer()
    {
        if (mSource)
            delete mSource;
        if (mCodestream)
        {
            mCodestream->destroy();
            delete mCodestream;
        }
    }

    void ownIO()
    {
        mSource->ownIO();
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
        return mBytes;
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
        return mTilesX;
    }

    nrt_Uint32 getTilesY() const
    {
        return mTilesY;
    }

    nrt_Uint64 readTile(nrt_Uint32 tileX, nrt_Uint32 tileY, nrt_Uint8 **buf)
    {
        nrt_Uint32 nComps = getNumComponents();
        nrt_Uint32 sampleSize = getComponentBytes();
        nrt_Uint64 lineSize = (nrt_Uint64)getTileWidth() * sampleSize;
        nrt_Uint64 compSize = lineSize * getTileHeight();

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
    nrt_Uint32 mComponents, mHeight, mWidth, mBytes, mTileWidth, mTileHeight;
    nrt_Uint32 mTilesX, mTilesY;
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

        // cache some metadata
        mComponents = mCodestream->get_num_components(true);

        kdu_dims compSize;
        mCodestream->get_dims(-1, compSize, true);
        mWidth = (nrt_Uint32)(compSize.size.x);
        mHeight = (nrt_Uint32)(compSize.size.y);
        mBytes = (mCodestream->get_bit_depth(-1, true) - 1) / 8 + 1;

        kdu_dims partition;
        mCodestream->get_tile_partition(partition);
        mTileWidth = (nrt_Uint32)partition.size.x;
        mTileHeight = (nrt_Uint32)partition.size.y;

        kdu_dims indices;
        mCodestream->get_valid_tiles(indices);
        mTilesX = indices.size.x;
        mTilesY = indices.size.y;
    }
};

}
}


NRTPRIV( NRT_BOOL)
Kakadu_canReadTiles(J2K_USER_DATA *data, nrt_Error *error)
{
    return NRT_SUCCESS;
}

NRTPRIV( nrt_Uint32)
Kakadu_getTilesX(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getTilesX();
}

NRTPRIV( nrt_Uint32)
Kakadu_getTilesY(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getTilesY();
}

NRTPRIV( nrt_Uint32)
Kakadu_getTileWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getTileWidth();
}

NRTPRIV( nrt_Uint32)
Kakadu_getTileHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getTileHeight();
}

NRTPRIV( nrt_Uint32)
Kakadu_getWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getWidth();
}

NRTPRIV( nrt_Uint32)
Kakadu_getHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getHeight();
}

NRTPRIV( nrt_Uint32)
Kakadu_getNumComponents(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getNumComponents();
}

NRTPRIV( nrt_Uint32)
Kakadu_getComponentBytes(J2K_USER_DATA *data, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    return impl->getComponentBytes();
}

NRTPRIV( nrt_Uint64)
Kakadu_readTile(J2K_USER_DATA *data, nrt_Uint32 tileX, nrt_Uint32 tileY,
                  nrt_Uint8 **buf, nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;

    try
    {
        return impl->readTile(tileX, tileY, buf);
    }
    catch(std::string &ex)
    {
        nrt_Error_init(error, ex.c_str(), NRT_CTXT, NRT_ERR_INVALID_OBJECT);
        return 0;
    }
}

NRTPRIV( nrt_Uint64)
Kakadu_readRegion(J2K_USER_DATA *data, nrt_Uint32 x0, nrt_Uint32 y0,
                    nrt_Uint32 x1, nrt_Uint32 y1, nrt_Uint8 **buf,
                    nrt_Error *error)
{
    j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
    // TODO
    return 0;
}

NRTPRIV(void)
Kakadu_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        j2k::kakadu::UserContainer *impl = (j2k::kakadu::UserContainer*) data;
        delete impl;
    }
}


NRTAPI(j2k_Container*) j2k_Container_open(const char *fname, nrt_Error *error)
{
    j2k_Container *container = NULL;
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

    if (!(container = j2k_Container_openIO(io, error)))
        goto CATCH_ERROR;

    ((j2k::kakadu::UserContainer*) container->data)->ownIO();

    return container;

    CATCH_ERROR:
    {
        if (io)
            nrt_IOInterface_destruct(&io);
        if (container)
            j2k_Container_destruct(&container);
        return NULL;
    }
}

NRTAPI(j2k_Container*) j2k_Container_openIO(nrt_IOInterface *io, nrt_Error *error)
{
    static j2k_IContainer containerInterface =
    { &Kakadu_canReadTiles, &Kakadu_getTilesX,
      &Kakadu_getTilesY, &Kakadu_getTileWidth, &Kakadu_getTileHeight,
      &Kakadu_getWidth, &Kakadu_getHeight, &Kakadu_getNumComponents,
      &Kakadu_getComponentBytes, &Kakadu_readTile, &Kakadu_readRegion,
      &Kakadu_destruct };

    j2k_Container *container = NULL;
    container = (j2k_Container *) NRT_MALLOC(sizeof(j2k_Container));
    if (!container)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(container, 0, sizeof(j2k_Container));

    try
    {
        if (!(container->data = new j2k::kakadu::UserContainer(io)))
            goto CATCH_ERROR;
    }
    catch(std::string& ex)
    {
        nrt_Error_init(error, ex.c_str(), NRT_CTXT, NRT_ERR_UNK);
        goto CATCH_ERROR;
    }

    container->iface = &containerInterface;
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
