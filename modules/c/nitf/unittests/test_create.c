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
#include "Test.h"

TEST_CASE(testCreate)
{
    nitf_IOHandle handle;
    nitf_Error error;

    handle = nitf_IOHandle_create("test_create.ntf", NITF_ACCESS_READONLY, NITF_CREATE, &error);
    TEST_ASSERT(!NITF_INVALID_HANDLE(handle));

    nitf_IOHandle_close(handle);
}

int main(int argc, char **argv)
{
    CHECK(testCreate);
    return 0;
}
