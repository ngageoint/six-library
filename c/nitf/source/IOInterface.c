#include "nitf/IOInterface.h"

NITF_CXX_GUARD

NITFAPI(NITF_BOOL) nitf_IOInterface_read(nitf_IOInterface* io,
                                         char *buf, size_t size,
                                         nitf_Error *error)
{
    return io->iface->read(io->data, buf, size, error);
}

NITFAPI(NITF_BOOL) nitf_IOInterface_write(nitf_IOInterface* io,
                                          const char *buf,
                                          size_t size,
                                          nitf_Error* error)
{
    return io->iface->write(io->data, buf, size, error);
}

NITFAPI(nitf_Off) nitf_IOInterface_seek(nitf_IOInterface* io,
                                        nitf_Off offset,
                                        int whence,
                                        nitf_Error* error)
{
    return io->iface->seek(io->data, offset, whence, error);
}

NITFAPI(nitf_Off) nitf_IOInterface_tell(nitf_IOInterface* io,
                                     nitf_Error* error)
{
    return io->iface->tell(io->data, error);
}


NITFAPI(nitf_Off) nitf_IOInterface_getSize(nitf_IOInterface* io,
                                        nitf_Error* error)
{
    return io->iface->getSize(io->data, error);
}


NITFAPI(NITF_BOOL) nitf_IOInterface_close(nitf_IOInterface* io,
                                          nitf_Error* error)
{
    return io->iface->close(io->data, error);

}
NITFAPI(void) nitf_IOInterface_destruct(nitf_IOInterface** io)
{
    if (*io)
    {
        if ( (*io)->iface)
        {
            (*io)->iface->destruct( (*io)->data );
            (*io)->data = NULL;
            (*io)->iface = NULL;
            NITF_FREE( *io );
        }
        *io = NULL;
    }
}

NITFPRIV(NITF_BOOL) IOHandleAdaptor_read(NITF_DATA* data,
                                         char* buf,
                                         size_t size,
                                         nitf_Error* error)
{
    return nitf_IOHandle_read( (nitf_IOHandle)(long)data,
                               buf, size, error);

}
NITFPRIV(NITF_BOOL) IOHandleAdaptor_write(NITF_DATA* data,
                                          const char* buf,
                                          size_t size,
                                          nitf_Error* error)
{
    return nitf_IOHandle_write( (nitf_IOHandle)(long)data,
                                buf, size, error);
}


NITFPRIV(nitf_Off) IOHandleAdaptor_seek(NITF_DATA* data,
                                        nitf_Off offset,
                                        int whence,
                                        nitf_Error* error)
{
    return nitf_IOHandle_seek( (nitf_IOHandle)(long)data,
                               offset, whence, error);
}


NITFPRIV(nitf_Off) IOHandleAdaptor_tell(NITF_DATA* data,
                                     nitf_Error* error)
{
    return nitf_IOHandle_tell( (nitf_IOHandle)(long)data,
                               error);
}

NITFPRIV(nitf_Off) IOHandleAdaptor_getSize(NITF_DATA* data,
                                        nitf_Error* error)
{
    return nitf_IOHandle_getSize( (nitf_IOHandle)(long)data,
                                  error);
}

NITFPRIV(NITF_BOOL) IOHandleAdaptor_close(NITF_DATA* data,
                                     nitf_Error* error)
{
    nitf_IOHandle_close((nitf_IOHandle)(long)data);
    return NITF_SUCCESS;
}

NITFPRIV(void) IOHandleAdaptor_destruct(NITF_DATA* data)
{
}


NITFAPI(nitf_IOInterface*)
nitf_IOHandleAdaptor_construct(nitf_IOHandle handle, nitf_Error* error)
{
    static nitf_IIOInterface iIOHandle =
    {
        &IOHandleAdaptor_read,
        &IOHandleAdaptor_write,
        &IOHandleAdaptor_seek,
        &IOHandleAdaptor_tell,
        &IOHandleAdaptor_getSize,
        &IOHandleAdaptor_close,
        &IOHandleAdaptor_destruct
    };
    nitf_IOInterface* impl =
        (nitf_IOInterface*)NITF_MALLOC(sizeof(nitf_IOInterface));

    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    impl->data = (NITF_DATA*)(long)handle;
    impl->iface = &iIOHandle;
    return impl;
}

NITF_CXX_ENDGUARD
