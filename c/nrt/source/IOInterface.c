#include "nrt/IOInterface.h"

NRT_CXX_GUARD

NRTAPI(NRT_BOOL) nrt_IOInterface_read(nrt_IOInterface* io,
                                         char *buf, size_t size,
                                         nrt_Error *error)
{
    return io->iface->read(io->data, buf, size, error);
}

NRTAPI(NRT_BOOL) nrt_IOInterface_write(nrt_IOInterface* io,
                                          const char *buf,
                                          size_t size,
                                          nrt_Error* error)
{
    return io->iface->write(io->data, buf, size, error);
}

NRTAPI(nrt_Off) nrt_IOInterface_seek(nrt_IOInterface* io,
                                        nrt_Off offset,
                                        int whence,
                                        nrt_Error* error)
{
    return io->iface->seek(io->data, offset, whence, error);
}

NRTAPI(nrt_Off) nrt_IOInterface_tell(nrt_IOInterface* io,
                                     nrt_Error* error)
{
    return io->iface->tell(io->data, error);
}


NRTAPI(nrt_Off) nrt_IOInterface_getSize(nrt_IOInterface* io,
                                        nrt_Error* error)
{
    return io->iface->getSize(io->data, error);
}


NRTAPI(NRT_BOOL) nrt_IOInterface_close(nrt_IOInterface* io,
                                          nrt_Error* error)
{
    return io->iface->close(io->data, error);

}
NRTAPI(void) nrt_IOInterface_destruct(nrt_IOInterface** io)
{
    if (*io)
    {
        if ( (*io)->iface)
        {
            (*io)->iface->destruct( (*io)->data );
            (*io)->data = NULL;
            (*io)->iface = NULL;
            NRT_FREE( *io );
        }
        *io = NULL;
    }
}

NRTPRIV(NRT_BOOL) IOHandleAdaptor_read(NRT_DATA* data,
                                         char* buf,
                                         size_t size,
                                         nrt_Error* error)
{
    return nrt_IOHandle_read( (nrt_IOHandle)(long)data,
                               buf, size, error);

}
NRTPRIV(NRT_BOOL) IOHandleAdaptor_write(NRT_DATA* data,
                                          const char* buf,
                                          size_t size,
                                          nrt_Error* error)
{
    return nrt_IOHandle_write( (nrt_IOHandle)(long)data,
                                buf, size, error);
}


NRTPRIV(nrt_Off) IOHandleAdaptor_seek(NRT_DATA* data,
                                        nrt_Off offset,
                                        int whence,
                                        nrt_Error* error)
{
    return nrt_IOHandle_seek( (nrt_IOHandle)(long)data,
                               offset, whence, error);
}


NRTPRIV(nrt_Off) IOHandleAdaptor_tell(NRT_DATA* data,
                                     nrt_Error* error)
{
    return nrt_IOHandle_tell( (nrt_IOHandle)(long)data,
                               error);
}

NRTPRIV(nrt_Off) IOHandleAdaptor_getSize(NRT_DATA* data,
                                        nrt_Error* error)
{
    return nrt_IOHandle_getSize( (nrt_IOHandle)(long)data,
                                  error);
}

NRTPRIV(NRT_BOOL) IOHandleAdaptor_close(NRT_DATA* data,
                                     nrt_Error* error)
{
    nrt_IOHandle_close((nrt_IOHandle)(long)data);
    return NRT_SUCCESS;
}

NRTPRIV(void) IOHandleAdaptor_destruct(NRT_DATA* data)
{
}


NRTAPI(nrt_IOInterface*)
nrt_IOHandleAdaptor_construct(nrt_IOHandle handle, nrt_Error* error)
{
    static nrt_IIOInterface iIOHandle =
    {
        &IOHandleAdaptor_read,
        &IOHandleAdaptor_write,
        &IOHandleAdaptor_seek,
        &IOHandleAdaptor_tell,
        &IOHandleAdaptor_getSize,
        &IOHandleAdaptor_close,
        &IOHandleAdaptor_destruct
    };
    nrt_IOInterface* impl =
        (nrt_IOInterface*)NRT_MALLOC(sizeof(nrt_IOInterface));

    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                        NRT_ERR_MEMORY);
        return NULL;
    }
    impl->data = (NRT_DATA*)(long)handle;
    impl->iface = &iIOHandle;
    return impl;
}

NRT_CXX_ENDGUARD
