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

#include <string.h>
#include <nitf/CustomIO.hpp>

namespace nitf
{
CustomIO::CustomIO() :
    IOInterface(createInterface(this))
{
    setManaged(false);
}

CustomIO::~CustomIO()
{
    // This is messy but I don't think there's a cleaner way to do this.
    // After this destructor fires, IOInterface's destructor will fire which
    // will eventually call IOInterfaceDestructor::operator() which will
    // in turn try to call nitf_IOInterface_close() followed by
    // nitf_IOInterface_destruct().  But at that point CustomIO no longer
    // exists, so the data variable passed into these functions is invalid.
    // By NULL'ing it out here, and by supporting a passthrough when data
    // comes in NULL in adapterClose() below, we make the close call safe.
    // Additionally, the nitf_IOInterface_destruct call would try to free our
    // 'data' member via NRT_FREE() if we didn't do this - that would also
    // be exceptionally bad.
    mHandle->get()->data = NULL;
}

nitf_IOInterface* CustomIO::createInterface(CustomIO* me)
{
    static nrt_IIOInterface iIOHandle = {
        &CustomIO::adapterRead,
        &CustomIO::adapterWrite,
        &CustomIO::adapterCanSeek,
        &CustomIO::adapterSeek,
        &CustomIO::adapterTell,
        &CustomIO::adapterGetSize,
        &CustomIO::adapterGetMode,
        &CustomIO::adapterClose,
        &CustomIO::adapterDestruct
    };

    nitf_IOInterface* const impl =
            (nitf_IOInterface *)NITF_MALLOC(sizeof(nitf_IOInterface));
    if (impl == NULL)
    {
        return NULL;
    }
    memset(impl, 0, sizeof(nitf_IOInterface));

    impl->data = me;
    impl->iface = &iIOHandle;
    return impl;
}

NRT_BOOL CustomIO::adapterRead(NRT_DATA* data,
                               void* buf,
                               size_t size,
                               nrt_Error* error)
{
    try
    {
        reinterpret_cast<CustomIO*>(data)->readImpl(buf, size);
        return NRT_SUCCESS;
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_READING_FROM_FILE);
        return NRT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_READING_FROM_FILE);
        return NRT_FAILURE;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_READING_FROM_FILE);
        return NRT_FAILURE;
    }
}

NRT_BOOL CustomIO::adapterWrite(NRT_DATA* data,
                                const void* buf,
                                size_t size,
                                nrt_Error* error)
{
    try
    {
        reinterpret_cast<CustomIO*>(data)->writeImpl(buf, size);
        return NRT_SUCCESS;
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_WRITING_TO_FILE);
        return NRT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_WRITING_TO_FILE);
        return NRT_FAILURE;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_WRITING_TO_FILE);
        return NRT_FAILURE;
    }
}

NRT_BOOL CustomIO::adapterCanSeek(NRT_DATA* data,
                                  nrt_Error* error)
{
    try
    {
        reinterpret_cast<CustomIO*>(data)->canSeekImpl();
        return NRT_SUCCESS;
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
        return NRT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
        return NRT_FAILURE;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
        return NRT_FAILURE;
    }
}

nrt_Off CustomIO::adapterSeek(NRT_DATA* data,
                              nrt_Off offset,
                              int whence,
                              nrt_Error* error)
{
    try
    {
        return reinterpret_cast<CustomIO*>(data)->seekImpl(offset, whence);
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
        return -1;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
        return -1;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
        return -1;
    }
}

nrt_Off CustomIO::adapterTell(NRT_DATA* data,
                              nrt_Error* error)
{
    try
    {
        return reinterpret_cast<CustomIO*>(data)->tellImpl();
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
        return -1;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
        return -1;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_SEEKING_IN_FILE);
        return -1;
    }
}

nrt_Off CustomIO::adapterGetSize(NRT_DATA* data,
                                 nrt_Error* error)
{
    try
    {
        return reinterpret_cast<CustomIO*>(data)->getSizeImpl();
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_UNK);
        return -1;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_UNK);
        return -1;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_UNK);
        return -1;
    }
}

int CustomIO::adapterGetMode(NRT_DATA* data,
                             nrt_Error* error)
{
    try
    {
        return reinterpret_cast<CustomIO*>(data)->getMode();
    }
    catch (const except::Exception& ex)
    {
        nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                       NRT_ERR_UNK);
        return -1;
    }
    catch (const std::exception& ex)
    {
        nrt_Error_init(error, ex.what(), NRT_CTXT,
                       NRT_ERR_UNK);
        return -1;
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", NRT_CTXT,
                       NRT_ERR_UNK);
        return -1;
    }
}

NRT_BOOL CustomIO::adapterClose(NRT_DATA* data,
                                nrt_Error* error)
{
    if (data)
    {
        try
        {
            reinterpret_cast<CustomIO*>(data)->closeImpl();
            return NRT_SUCCESS;
        }
        catch (const except::Exception& ex)
        {
            nrt_Error_init(error, ex.getMessage().c_str(), NRT_CTXT,
                           NRT_ERR_UNK);
            return NRT_FAILURE;
        }
        catch (const std::exception& ex)
        {
            nrt_Error_init(error, ex.what(), NRT_CTXT,
                           NRT_ERR_UNK);
            return NRT_FAILURE;
        }
        catch (...)
        {
            nrt_Error_init(error, "Unknown error", NRT_CTXT,
                           NRT_ERR_UNK);
            return NRT_FAILURE;
        }
    }
    else
    {
        // See destructor for why this is needed
        nrt_Error_init(error, "Handle is NULL", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        return NRT_FAILURE;
    }
}

void CustomIO::adapterDestruct(NRT_DATA* data)
{
}
}
