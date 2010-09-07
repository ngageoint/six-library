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

#ifndef __NITF_WRITE_HANDLER_H__
#define __NITF_WRITE_HANDLER_H__

#include "nrt/WriteHandler.h"

typedef NRT_IWRITEHANDLER_WRITE         NITF_IWRITEHANDLER_WRITE;
typedef NRT_IWRITEHANDLER_DESTRUCT      NITF_IWRITEHANDLER_DESTRUCT;
typedef nrt_IWriteHandler               nitf_IWriteHandler;
typedef nrt_WriteHandler                nitf_WriteHandler;
#define nitf_WriteHandler_destruct      nrt_WriteHandler_destruct

#endif
