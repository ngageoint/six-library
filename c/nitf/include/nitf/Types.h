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

#ifndef __NITF_TYPES_H__
#define __NITF_TYPES_H__

/* Enum for the supported version types */
typedef enum _nitf_Version
{
    NITF_VER_20 = 100,
    NITF_VER_21,
    NITF_VER_UNKNOWN
} nitf_Version;

/* These macros check the NITF Version */
#define IS_NITF20(V) ((V) == NITF_VER_20)
#define IS_NITF21(V) ((V) == NITF_VER_21)


typedef void NITF_DATA;
#endif
