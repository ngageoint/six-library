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


#include <import/nitf.h>

#define BIG "3000000000"
#define SMALLER "300000000"

int main(int argc, char *argv[])
{
    nitf_Int32  i32;
    nitf_Uint32 u32;

    i32 = NITF_ATO32(BIG);
    u32 = NITF_ATO32(BIG);

    printf("First test of NITF_ATO32 i32 %d u32 %u\n", i32, u32);

    i32 = NITF_ATO32(SMALLER);
    u32 = NITF_ATO32(SMALLER);

    printf("Second test of NITF_ATO32 i32 %d u32 %u\n", i32, u32);

    i32 = NITF_ATOU32(BIG);
    u32 = NITF_ATOU32(BIG);

    printf("First test of NITF_ATOU32 i32 %d u32 %u\n", i32, u32);

    i32 = NITF_ATOU32(SMALLER);
    u32 = NITF_ATOU32(SMALLER);

    printf("Second test of NITF_ATOU32 i32 %d u32 %u\n", i32, u32);

    exit(0);
}
