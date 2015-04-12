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

#ifndef __NITF_WRITE_HANDLER_H__
#define __NITF_WRITE_HANDLER_H__

#include "nitf/System.h"

NITF_CXX_GUARD

/*
 *  Function pointer for writing.
 *  \param data     The ancillary "helper" data
 *  \param io       The output interface
 *  \param error    populated on error
 */
typedef NITF_BOOL(*NITF_IWRITEHANDLER_WRITE)(NITF_DATA *data,
        nitf_IOInterface* output, nitf_Error *error);

/*
 *  Function pointer for destructing the data structure
 *  \param data     The ancillary "helper" data
 */
typedef void (*NITF_IWRITEHANDLER_DESTRUCT)(NITF_DATA *);

/*!
 *  \struct nitf_IWriteHandler
 *  \brief The "write handler" interface, which handles writing data
 */
typedef struct _nitf_IWriteHandler
{
    NITF_IWRITEHANDLER_WRITE write;
    NITF_IWRITEHANDLER_DESTRUCT destruct;
} nitf_IWriteHandler;

typedef struct _nitf_WriteHandler
{
    nitf_IWriteHandler *iface;
    NITF_DATA *data;
} nitf_WriteHandler;

NITFAPI(void) nitf_WriteHandler_destruct(nitf_WriteHandler **writeHandler);

NITF_CXX_ENDGUARD

#endif
