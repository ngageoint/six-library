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
#include <nitf/Utils.hpp>

namespace nitf
{
CustomIO::CustomIO()
{
    set_native_object(createInterface(this));

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
    // By nullptr'ing it out here, and by supporting a passthrough when data
    // comes in nullptr in adapterClose() below, we make the close call safe.
    // Additionally, the nitf_IOInterface_destruct call would try to free our
    // 'data' member via NRT_FREE() if we didn't do this - that would also
    // be exceptionally bad.
    mHandle->get()->data = nullptr;
}

nitf_IOInterface* CustomIO::createInterface(CustomIO* me) noexcept
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

    #ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 26408) // Avoid malloc() and free(), prefer the nothrow version of new with delete (r.10).
    #endif
    auto const impl = static_cast<nitf_IOInterface*>(NITF_MALLOC(sizeof(nitf_IOInterface)));
    #ifdef _MSC_VER
    #pragma warning(pop)
    #endif
    if (impl == nullptr)
    {
        return nullptr;
    }
    memset(impl, 0, sizeof(nitf_IOInterface));

    impl->data = me;
    impl->iface = &iIOHandle;
    return impl;
}

template<typename TTry>
static auto try_catch(TTry f,
    decltype(f()) error_retval, nrt_Error* error, const char* file, int line, const char* func, int level) -> decltype(f())
{
    try
    {
        return f();
    }
    catch (const except::Exception& ex)
    {
        Utils::error_init(error, ex.getMessage(), file, line, func, level);
    }
    catch (const std::exception& ex)
    {
        Utils::error_init(error, ex, file, line, func, level);
    }
    catch (...)
    {
        nrt_Error_init(error, "Unknown error", file, line, func, level);
    }
    return error_retval;
}
template<typename TTry>
static NRT_BOOL try_catch(TTry f,
    nrt_Error* error, const char* file, int line, const char* func, int level)
{
    const auto retval = try_catch([&]()
        {
            f();
            return NRT_SUCCESS;
        },
        NRT_FAILURE, error, file, line, func, level);
    return retval ? true : false;
}

NRT_BOOL CustomIO::adapterRead(NRT_DATA* data,
                               void* buf,
                               size_t size,
                               nrt_Error* error)
{
    return try_catch([&]()
        {
        static_cast<CustomIO*>(data)->readImpl(buf, size);
        }, error, NRT_CTXT, NRT_ERR_READING_FROM_FILE);
}

NRT_BOOL CustomIO::adapterWrite(NRT_DATA* data,
                                const void* buf,
                                size_t size,
                                nrt_Error* error)
{
    return try_catch([&]()
        {
        static_cast<CustomIO*>(data)->writeImpl(buf, size);
        }, error, NRT_CTXT, NRT_ERR_WRITING_TO_FILE);
}

NRT_BOOL CustomIO::adapterCanSeek(NRT_DATA* data,
                                  nrt_Error* error)
{
    return try_catch([&]()
        {
        static_cast<CustomIO*>(data)->canSeekImpl();
        }, error, NRT_CTXT, NRT_ERR_SEEKING_IN_FILE);
}

nrt_Off CustomIO::adapterSeek(NRT_DATA* data,
                              nrt_Off offset,
                              int whence,
                              nrt_Error* error)
{
    return try_catch([&]()
        {
            return static_cast<CustomIO*>(data)->seekImpl(offset, whence);
        }, -1, error, NRT_CTXT, NRT_ERR_SEEKING_IN_FILE);
}

nrt_Off CustomIO::adapterTell(NRT_DATA* data,
                              nrt_Error* error)
{
    return try_catch([&]()
        {
            return static_cast<CustomIO*>(data)->tellImpl();
        }, -1, error, NRT_CTXT, NRT_ERR_SEEKING_IN_FILE);
}

nrt_Off CustomIO::adapterGetSize(NRT_DATA* data,
                                 nrt_Error* error)
{
    return try_catch([&]()
        {
            return static_cast<CustomIO*>(data)->getSizeImpl();
        }, -1, error, NRT_CTXT, NRT_ERR_UNK);
}

int CustomIO::adapterGetMode(NRT_DATA* data,
                             nrt_Error* error)
{
    if (data == nullptr)
    {
        nrt_Error_init(error, "Null pointer reference", NITF_CTXT,
            NITF_ERR_INVALID_OBJECT);
        return -1;
    }

    return try_catch([&]()
        {
            return static_cast<CustomIO*>(data)->getMode();
        }, -1, error, NRT_CTXT, NRT_ERR_UNK);

}

NRT_BOOL CustomIO::adapterClose(NRT_DATA* data,
                                nrt_Error* error)
{
    if (data)
    {
        return try_catch([&]()
            {
            static_cast<CustomIO*>(data)->closeImpl();
            }, error, NRT_CTXT, NRT_ERR_UNK);
    }
    else
    {
        // See destructor for why this is needed
        nrt_Error_init(error, "Handle is nullptr", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        return NRT_FAILURE;
    }
}

void CustomIO::adapterDestruct(NRT_DATA*) noexcept
{
}
}
