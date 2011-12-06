#include "nrt/IOInterface.h"

NRT_CXX_GUARD typedef struct _IOHandleControl
{
    nrt_IOHandle handle;
    int mode;
} IOHandleControl;

typedef struct _BufferIOControl
{
    char *buf;
    size_t size;
    size_t mark;
    size_t bytesWritten;
    NRT_BOOL ownBuf;
} BufferIOControl;

NRTAPI(NRT_BOOL) nrt_IOInterface_read(nrt_IOInterface * io, char *buf,
                                      size_t size, nrt_Error * error)
{
    return io->iface->read(io->data, buf, size, error);
}

NRTAPI(NRT_BOOL) nrt_IOInterface_write(nrt_IOInterface * io, const char *buf,
                                       size_t size, nrt_Error * error)
{
    return io->iface->write(io->data, buf, size, error);
}

NRTAPI(NRT_BOOL) nrt_IOInterface_canSeek(nrt_IOInterface * io,
                                         nrt_Error * error)
{
    return io->iface->canSeek(io->data, error);
}

NRTAPI(nrt_Off) nrt_IOInterface_seek(nrt_IOInterface * io, nrt_Off offset,
                                     int whence, nrt_Error * error)
{
    if (!nrt_IOInterface_canSeek(io, error) && offset != 0)
    {
        nrt_Error_init(error, "IO Interface does not support seeking", NRT_CTXT,
                       NRT_ERR_INVALID_OBJECT);
        return (nrt_Off) - 1;
    }
    return io->iface->seek(io->data, offset, whence, error);
}

NRTAPI(nrt_Off) nrt_IOInterface_tell(nrt_IOInterface * io, nrt_Error * error)
{
    return io->iface->tell(io->data, error);
}

NRTAPI(nrt_Off) nrt_IOInterface_getSize(nrt_IOInterface * io, nrt_Error * error)
{
    return io->iface->getSize(io->data, error);
}

NRTAPI(int) nrt_IOInterface_getMode(nrt_IOInterface * io, nrt_Error * error)
{
    return io->iface->getMode(io->data, error);
}

NRTAPI(NRT_BOOL) nrt_IOInterface_close(nrt_IOInterface * io, nrt_Error * error)
{
    return io->iface->close(io->data, error);

}

NRTAPI(void) nrt_IOInterface_destruct(nrt_IOInterface ** io)
{
    if (*io)
    {
        if ((*io)->iface)
        {
            if ((*io)->data)
            {
                (*io)->iface->destruct((*io)->data);
                free((*io)->data);
                (*io)->data = NULL;
            }
            (*io)->iface = NULL;
        }
        NRT_FREE(*io);
        *io = NULL;
    }
}

NRTPRIV(NRT_BOOL) IOHandleAdapter_read(NRT_DATA * data, char *buf, size_t size,
                                       nrt_Error * error)
{
    IOHandleControl *control = (IOHandleControl *) data;
    return nrt_IOHandle_read(control->handle, buf, size, error);
}

NRTPRIV(NRT_BOOL) IOHandleAdapter_write(NRT_DATA * data, const char *buf,
                                        size_t size, nrt_Error * error)
{
    IOHandleControl *control = (IOHandleControl *) data;
    return nrt_IOHandle_write(control->handle, buf, size, error);
}

NRTPRIV(NRT_BOOL) IOHandleAdapter_canSeek(NRT_DATA * data, nrt_Error * error)
{
    return NRT_SUCCESS;
}

NRTPRIV(nrt_Off) IOHandleAdapter_seek(NRT_DATA * data, nrt_Off offset,
                                      int whence, nrt_Error * error)
{
    IOHandleControl *control = (IOHandleControl *) data;
    return nrt_IOHandle_seek(control->handle, offset, whence, error);
}

NRTPRIV(nrt_Off) IOHandleAdapter_tell(NRT_DATA * data, nrt_Error * error)
{
    IOHandleControl *control = (IOHandleControl *) data;
    return nrt_IOHandle_tell(control->handle, error);
}

NRTPRIV(nrt_Off) IOHandleAdapter_getSize(NRT_DATA * data, nrt_Error * error)
{
    IOHandleControl *control = (IOHandleControl *) data;
    return nrt_IOHandle_getSize(control->handle, error);
}

NRTPRIV(int) IOHandleAdapter_getMode(NRT_DATA * data, nrt_Error * error)
{
    IOHandleControl *control = (IOHandleControl *) data;
    return control->mode;
}

NRTPRIV(NRT_BOOL) IOHandleAdapter_close(NRT_DATA * data, nrt_Error * error)
{
    IOHandleControl *control = (IOHandleControl *) data;
    if (control && control->handle && !NRT_INVALID_HANDLE(control->handle))
    {
        nrt_IOHandle_close(control->handle);

        // Invalidate the handle.  Otherwise, if a caller closes this once,
        // then opens another IOHandle that the OS happens to give the same
        // underlying handle value to, then the caller closes this again,
        // they will actually close the second IOHandle.
        control->handle = NRT_INVALID_HANDLE_VALUE;
    }
    return NRT_SUCCESS;
}

NRTPRIV(void) IOHandleAdapter_destruct(NRT_DATA * data)
{
}

NRTPRIV(NRT_BOOL) BufferAdapter_read(NRT_DATA * data, char *buf, size_t size,
                                     nrt_Error * error)
{
    BufferIOControl *control = (BufferIOControl *) data;

    if (size > control->size - control->mark)
    {
        nrt_Error_init(error, "Invalid size requested - EOF", NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NRT_FAILURE;
    }

    if (size > 0)
    {
        memcpy(buf, (char *) (control->buf + control->mark), size);
        control->mark += size;
    }
    return NRT_SUCCESS;
}

NRTPRIV(NRT_BOOL) BufferAdapter_write(NRT_DATA * data, const char *buf,
                                      size_t size, nrt_Error * error)
{
    BufferIOControl *control = (BufferIOControl *) data;

    if (size > control->size - control->mark)
    {
        nrt_Error_init(error, "Invalid size requested - EOF", NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NRT_FAILURE;
    }

    if (size > 0)
    {
        memcpy((char *) (control->buf + control->mark), buf, size);
        control->mark += size;
        if (control->mark > control->bytesWritten)
        {
            control->bytesWritten = control->mark;
        }
    }
    return NRT_SUCCESS;
}

NRTPRIV(NRT_BOOL) BufferAdapter_canSeek(NRT_DATA * data, nrt_Error * error)
{
    return NRT_SUCCESS;
}

NRTPRIV(nrt_Off) BufferAdapter_seek(NRT_DATA * data, nrt_Off offset, int whence,
                                    nrt_Error * error)
{
    BufferIOControl *control = (BufferIOControl *) data;

    if (whence == NRT_SEEK_SET)
    {
        if (offset >= (nrt_Off) control->size)
        {
            nrt_Error_init(error, "Invalid offset requested - EOF", NRT_CTXT,
                           NRT_ERR_MEMORY);
            return -1;
        }
        control->mark = (size_t) offset;
    }
    else if (whence == NRT_SEEK_CUR)
    {
        if (offset >= (nrt_Off) control->size - control->mark)
        {
            nrt_Error_init(error, "Invalid offset requested - EOF", NRT_CTXT,
                           NRT_ERR_MEMORY);
            return -1;
        }
        control->mark += (size_t) offset;
    }
    else
    {
        nrt_Error_init(error, "Invalid/unsupported seek directive", NRT_CTXT,
                       NRT_ERR_MEMORY);
        return -1;
    }
    return control->mark;
}

NRTPRIV(nrt_Off) BufferAdapter_tell(NRT_DATA * data, nrt_Error * error)
{
    BufferIOControl *control = (BufferIOControl *) data;
    return (nrt_Off) control->mark;
}

NRTPRIV(nrt_Off) BufferAdapter_getSize(NRT_DATA * data, nrt_Error * error)
{
    BufferIOControl *control = (BufferIOControl *) data;
    return (nrt_Off) control->bytesWritten;
}

NRTPRIV(int) BufferAdapter_getMode(NRT_DATA * data, nrt_Error * error)
{
    return NRT_ACCESS_READWRITE;
}

NRTPRIV(NRT_BOOL) BufferAdapter_close(NRT_DATA * data, nrt_Error * error)
{
    /* nothing */
}

NRTPRIV(void) BufferAdapter_destruct(NRT_DATA * data)
{
    BufferIOControl *control = (BufferIOControl *) data;
    if (control && control->buf && control->ownBuf)
    {
        NRT_FREE(control->buf);
        control->buf = NULL;
    }
}

NRTAPI(nrt_IOInterface *) nrt_IOHandleAdapter_construct(nrt_IOHandle handle,
                                                        int accessMode,
                                                        nrt_Error * error)
{
    static nrt_IIOInterface iIOHandle = {
        &IOHandleAdapter_read,
        &IOHandleAdapter_write,
        &IOHandleAdapter_canSeek,
        &IOHandleAdapter_seek,
        &IOHandleAdapter_tell,
        &IOHandleAdapter_getSize,
        &IOHandleAdapter_getMode,
        &IOHandleAdapter_close,
        &IOHandleAdapter_destruct
    };
    nrt_IOInterface *impl = NULL;
    IOHandleControl *control = NULL;

    if (!(impl = (nrt_IOInterface *) NRT_MALLOC(sizeof(nrt_IOInterface))))
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(nrt_IOInterface));

    if (!(control = (IOHandleControl *) NRT_MALLOC(sizeof(IOHandleControl))))
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(control, 0, sizeof(IOHandleControl));
    control->handle = handle;
    control->mode = accessMode;

    impl->data = (NRT_DATA *) control;
    impl->iface = &iIOHandle;
    return impl;

    CATCH_ERROR:
    {
        if (impl)
            nrt_IOInterface_destruct(&impl);
        return NULL;
    }
}

NRTAPI(nrt_IOInterface *) nrt_IOHandleAdapter_open(const char *fname,
                                                   int accessFlags,
                                                   int creationFlags,
                                                   nrt_Error * error)
{
    nrt_IOHandle handle = nrt_IOHandle_create(fname, accessFlags, creationFlags,
                                              error);
    if (NRT_INVALID_HANDLE(handle))
    {
        // Save off the original error message (i.e. that the file doesn't
        // exist or has invalid permissions)
        // TODO: Would really like to use one of the nrt_Error print functions
        char origMessage[NRT_MAX_EMESSAGE + 1];
        strcpy(origMessage, error->message);

        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                        "Invalid IO handle (%s)", origMessage);
        return NULL;
    }

    return nrt_IOHandleAdapter_construct(handle, accessFlags, error);
}

NRTAPI(nrt_IOInterface *) nrt_BufferAdapter_construct(char *buf, size_t size,
                                                      NRT_BOOL ownBuf,
                                                      nrt_Error * error)
{
    static nrt_IIOInterface bufferInterface = {
        &BufferAdapter_read,
        &BufferAdapter_write,
        &BufferAdapter_canSeek,
        &BufferAdapter_seek,
        &BufferAdapter_tell,
        &BufferAdapter_getSize,
        &BufferAdapter_getMode,
        &BufferAdapter_close,
        &BufferAdapter_destruct
    };
    nrt_IOInterface *impl = NULL;
    BufferIOControl *control = NULL;

    if (!(impl = (nrt_IOInterface *) NRT_MALLOC(sizeof(nrt_IOInterface))))
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(nrt_IOInterface));

    if (!(control = (BufferIOControl *) NRT_MALLOC(sizeof(BufferIOControl))))
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(control, 0, sizeof(BufferIOControl));
    control->buf = buf;
    control->size = size;
    control->ownBuf = ownBuf;

    impl->data = (NRT_DATA *) control;
    impl->iface = &bufferInterface;
    return impl;

    CATCH_ERROR:
    {
        if (impl)
            nrt_IOInterface_destruct(&impl);
        return NULL;
    }
}

NRT_CXX_ENDGUARD
