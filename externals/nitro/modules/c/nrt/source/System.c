/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

NRTPROT(uint16_t) nrt_System_swap16(uint16_t ins)
{
    uint16_t outs;
    unsigned char *ibytep = (unsigned char *) &ins;
    unsigned char *obytep = (unsigned char *) &outs;
    obytep[1] = ibytep[0];
    obytep[0] = ibytep[1];
    return outs;
}

NRTPROT(uint32_t) nrt_System_swap32(uint32_t inl)
{
    uint32_t outl;
    unsigned char *ibytep = (unsigned char *) &inl;
    unsigned char *obytep = (unsigned char *) &outl;
    obytep[3] = ibytep[0];
    obytep[2] = ibytep[1];
    obytep[1] = ibytep[2];
    obytep[0] = ibytep[3];
    return outl;
}

NRTPROT(uint64_t) nrt_System_swap64c(uint64_t inl)
{
    uint64_t outl;
    const unsigned char* const ibytep = (const unsigned char* )&inl;
    unsigned char* const obytep = (unsigned char* )&outl;

    /* 4 byte real */
    obytep[7] = ibytep[4];
    obytep[6] = ibytep[5];
    obytep[5] = ibytep[6];
    obytep[4] = ibytep[7];

    /* 4 byte imaginary */
    obytep[3] = ibytep[0];
    obytep[2] = ibytep[1];
    obytep[1] = ibytep[2];
    obytep[0] = ibytep[3];
    return outl;
}

NRTPROT(uint64_t) nrt_System_swap64(uint64_t inl)
{
    uint64_t outl;
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
