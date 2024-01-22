/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#ifndef NITRO_nitf_TestingTest_h_INCLUDED_
#define NITRO_nitf_TestingTest_h_INCLUDED_
#pragma once

#include "nitf/System.h"
#include "nitf/FieldDescriptor.h"
#include "nitf/ComponentInfo.h"
#include "nitf/Extensions.h"
#include "nitf/FileSecurity.h"
#include "nitf/Field.h"

NITF_CXX_GUARD

/*
 typedef struct _nitf_testing_Test1 { Field* f1; } nitf_testing_Test1;
 static const nitf_StructFieldDescriptor _nitf_testing_Test1_fields[] = { { NITF_FieldType_Field, "f1", 0} };
 */
NITF_DECLARE_struct_1(testing_Test1a, Field, f1);
NITFAPI(nitf_testing_Test1a*) nitf_testing_Test1a_construct(nitf_Error* error);
NITFAPI(void) nitf_testing_Test1a_destruct(nitf_testing_Test1a** fh);

// Same thing, by hand, to check consistency
typedef struct _nitf_testing_Test1b { nitf_Field* f; } nitf_testing_Test1b;
static const nitf_StructFieldDescriptor nitf_testing_Test1b_fields[] = {
    { NITF_FieldType_Field, "f", offsetof(nitf_testing_Test1b, f) },
};
NITFAPI(nitf_testing_Test1b*) nitf_testing_Test1b_construct(nitf_Error* error);
NITFAPI(void) nitf_testing_Test1b_destruct(nitf_testing_Test1b** fh);

// try out macros
NITF_DECLARE_struct_5(testing_Test5,
    Field, field,
    FileSecurity, fileSecurity,
    ComponentInfo, componentInfo,
    PComponentInfo, pComponentInfo,
    Extensions, extensions);

NITF_CXX_ENDGUARD

#endif // NITRO_nitf_TestingTest_h_INCLUDED_
