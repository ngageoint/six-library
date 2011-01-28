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

#include "nrt/System.h"

NRTPROT(nrt_Uint16) nrt_System_swap16(nrt_Uint16 ins)
{
    nrt_Uint16 outs;
    unsigned char *ibytep = (unsigned char *) &ins;
    unsigned char *obytep = (unsigned char *) &outs;
    obytep[1] = ibytep[0];
    obytep[0] = ibytep[1];
    return outs;
}

NRTPROT(nrt_Uint32) nrt_System_swap32(nrt_Uint32 inl)
{
    nrt_Uint32 outl;
    unsigned char *ibytep = (unsigned char *) &inl;
    unsigned char *obytep = (unsigned char *) &outl;
    obytep[3] = ibytep[0];
    obytep[2] = ibytep[1];
    obytep[1] = ibytep[2];
    obytep[0] = ibytep[3];
    return outl;
}

NRTPROT(nrt_Uint32) nrt_System_swap64c(nrt_Uint64 inl)
{
    nrt_Uint64 outl;
    unsigned char *ibytep = (unsigned char *) &inl;
    unsigned char *obytep = (unsigned char *) &outl;
    obytep[7] = ibytep[4];
    obytep[6] = ibytep[5];
    obytep[5] = ibytep[6];
    obytep[4] = ibytep[7];

    obytep[3] = ibytep[0];
    obytep[2] = ibytep[1];
    obytep[1] = ibytep[2];
    obytep[0] = ibytep[3];
    return outl;
}

NRTPROT(nrt_Uint64) nrt_System_swap64(nrt_Uint64 inl)
{
    nrt_Uint64 outl;
    unsigned char *ibytep = (unsigned char *) &inl;
    unsigned char *obytep = (unsigned char *) &outl;

    obytep[7] = ibytep[0];
    obytep[6] = ibytep[1];
    obytep[5] = ibytep[2];
    obytep[4] = ibytep[3];
    obytep[3] = ibytep[4];
    obytep[2] = ibytep[5];
    obytep[1] = ibytep[6];
    obytep[0] = ibytep[7];
    return outl;
}
