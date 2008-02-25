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

#include <import/nitf.h>


int main()
{
    nitf_Error error;
    nitf_Int32 int32 = 16801;
    nitf_Value* vbin = nitf_Value_construct(NITF_VAL_INT, &int32, 4, 4, &error);
    printf("%d\n", int32);
    if (!nitf_Value_get(vbin, &int32, NITF_VAL_INT, 4, &error))
    {
        nitf_Error_print(&error, stdout, "Blah");
        exit(EXIT_FAILURE);
    }
    printf("VBIN: [%d]\n", int32);

    return 0;
}
