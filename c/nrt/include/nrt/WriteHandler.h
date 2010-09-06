/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#ifndef __NRT_WRITE_HANDLER_H__
#define __NRT_WRITE_HANDLER_H__

#include "nrt/Error.h"
#include "nrt/System.h"
#include "nrt/IOInterface.h"

NRT_CXX_GUARD

/*
 *  Function pointer for writing.
 *  \param data     The ancillary "helper" data
 *  \param io       The output interface
 *  \param error    populated on error
 */
typedef NRT_BOOL(*NRT_IWRITEHANDLER_WRITE) (NRT_DATA *data,
                                              nrt_IOInterface* output,
                                              nrt_Error *error);


/*
 *  Function pointer for destructing the data structure
 *  \param data     The ancillary "helper" data
 */
typedef void (*NRT_IWRITEHANDLER_DESTRUCT) (NRT_DATA *);


/*!
 *  \struct nrt_IWriteHandler
 *  \brief The "write handler" interface, which handles writing data
 */
typedef struct _NRT_IWriteHandler
{
    NRT_IWRITEHANDLER_WRITE write;
    NRT_IWRITEHANDLER_DESTRUCT destruct;
} nrt_IWriteHandler;


typedef struct _NRT_WriteHandler
{
    nrt_IWriteHandler *iface;
    NRT_DATA *data;
}nrt_WriteHandler;


NRTAPI(void) nrt_WriteHandler_destruct(nrt_WriteHandler ** writeHandler);



NRT_CXX_ENDGUARD

#endif
