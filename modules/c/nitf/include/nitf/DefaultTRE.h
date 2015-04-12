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

#ifndef __NITF_DEFAULT_TRE_H__
#define __NITF_DEFAULT_TRE_H__

#include "nitf/System.h"
#include "nitf/TRE.h"
#include "nitf/TREUtils.h"

NITF_CXX_GUARD

struct _nitf_Record;
/*!
 *  \fn nitf_DefaultTRE_handler
 *  \brief The default TRE handler is what gets run if no handler
 *  is defined
 *
 *  When the reader finds a TRE, it first checks the plug-in registry
 *  to see if it has a known plug-in handler.  If not, this function
 *  is called instead.  The default handler behavior is simply to
 *  store the raw data in a string in the hash table.
 *
 *  \param io The io handle situated at the beginning of the TRE
 *  \param tre The tre to use
 *  \param error The structure to populate if an error occurs
 *  \return The status
 *
 */
NITFAPI(nitf_TREHandler*) nitf_DefaultTRE_handler(nitf_Error * error);



NITF_CXX_ENDGUARD

#endif

