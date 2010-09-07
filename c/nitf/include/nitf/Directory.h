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

#ifndef __NITF_DIRECTORY_H__
#define __NITF_DIRECTORY_H__

#include "nrt/Directory.h"

typedef nrt_Directory                   nitf_Directory;
#define nitf_Directory_construct        nrt_Directory_construct
#define nitf_Directory_findFirstFile    nrt_Directory_findFirstFile
#define nitf_Directory_findNextFile     nrt_Directory_findNextFile
#define nitf_Directory_destruct         nrt_Directory_destruct
#define nitf_Directory_exists           nrt_Directory_exists

#endif
