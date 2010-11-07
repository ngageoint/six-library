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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cgm/NITFWriteHandler.h" 


typedef struct _WriteHandlerImpl
{
    cgm_Metafile *mf;
} WriteHandlerImpl;


/*
 *  Private read implementation for file source.
 */
NITFPRIV(NITF_BOOL) WriteHandler_write
    (NITF_DATA * data, nitf_IOInterface* io, nitf_Error * error)
{
    WriteHandlerImpl *impl = NULL;
    cgm_MetafileWriter *writer = NULL;
    NITF_BOOL status;

    /* cast it to the structure we know about */
    impl = (WriteHandlerImpl *) data;
    
    /* create a new MetafileWriter */
    writer = cgm_MetafileWriter_construct(error);
    if (!writer)
        return NITF_FAILURE;
    
    /* write the CGM! */
    status = cgm_MetafileWriter_write(writer, impl->mf, io, error);
    
    /* done with the writer, so destroy it */
    cgm_MetafileWriter_destruct(&writer);
        
    return status;
}


NITFPRIV(void) WriteHandler_destruct(NITF_DATA * data)
{
    if (data)
    {
        NITF_FREE(((WriteHandlerImpl*)data));
    }
}


nitf_WriteHandler* cgm_NITFWriteHandler_construct(
        cgm_Metafile *mf, nitf_Error *error)
{
    nitf_WriteHandler *writeHandler = NULL;
    WriteHandlerImpl *impl = NULL;
    
    /* make the interface */
    static nitf_IWriteHandler iWriteHandler = {
	&WriteHandler_write,
        &WriteHandler_destruct
    };
    
    /* construct the persisent one */
    impl = (WriteHandlerImpl *) NITF_MALLOC(sizeof(WriteHandlerImpl));
    if (!impl)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    impl->mf = mf;

    writeHandler = (nitf_WriteHandler *) NITF_MALLOC(sizeof(nitf_WriteHandler));
    if (!writeHandler)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    writeHandler->data = impl;
    writeHandler->iface = &iWriteHandler;

    /* return successfully */
    return writeHandler;
    
  CATCH_ERROR:
    if (writeHandler)
        nitf_WriteHandler_destruct(&writeHandler);
    if (impl)
        NITF_FREE(impl);
    return NULL;
}
