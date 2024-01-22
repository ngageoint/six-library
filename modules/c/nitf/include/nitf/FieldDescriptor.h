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

#ifndef NITRO_nitf_FieldDescriptor_h_INCLUDED_
#define NITRO_nitf_FieldDescriptor_h_INCLUDED_
#pragma once

#include <stddef.h>

//
// Support a simple "reflection" scheme.
//

/* Enum for the types of fields in structs */
typedef enum nitf_StructFieldType_
{
    NITF_FieldType_Field, // nitf_Field*
    NITF_FieldType_FileSecurity, // nitf_FileSecurity*
    NITF_FieldType_ComponentInfo, // nitf_ComponentInfo*
    NITF_FieldType_PComponentInfo, // nitf_ComponentInfo** aka nitf_PComponentInfo*
    NITF_FieldType_Extensions, // nitf_Extensions*
} nitf_StructFieldType;

// A "descriptor" of a field
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#endif
typedef struct nitf_StructFieldDescriptor_
{
    nitf_StructFieldType type;
    const char* name;
    size_t offset;
} nitf_StructFieldDescriptor;
#if _MSC_VER
#pragma warning(pop)
#endif

// A bunch of ugly macros to generate two things: the struct itself and a list of descriptors the fields.
// The end result of NITF_DECLARE_struct_1_(Test, Field, f) is:
//   typedef struct _nitf_Test { Field* f; };
//   static const nitf_StructFieldDescriptor nitf_Test_fields[] = { { NITF_FieldType_Field, "f", 0} };

// Generates `{ NITF_FieldType_Field, "f", 0}` from NITF_StructFieldDescriptor_value(Field, Test, f)
#define NITF_StructFieldDescriptor_value_(type, s, m) {type, #m, offsetof(s, m) }
#define NITF_StructFieldDescriptor_value(type, s, m) NITF_StructFieldDescriptor_value_(NITF_FieldType_##type, nitf_##s, m)

// The overall structure of what we want to generate.
#define NITF_DECLARE_struct_(name, fields, descriptors) typedef struct _##nitf_##name { fields; } nitf_##name; \
    static const nitf_StructFieldDescriptor nitf_##name##_fields[] = { descriptors }

// These are gnarly :-( And with a lot of repitition :-( :-(
//
// Each N macro peels off the first two arguments 't' and 'f' (type and field)
// and then passes the rest to the N-1 macro via __VA_ARGS__.

#define NITF_DECLARE_struct_1_(t, f) nitf_##t* f /* nitf_Field* f */
#define NITF_StructFieldDescriptor_value_1_(name, t, f) NITF_StructFieldDescriptor_value(t, name, f) /* { NITF_FieldType_Field, "f", 0} */
#define NITF_DECLARE_struct_1(name, ...) NITF_DECLARE_struct_(name, \
    NITF_DECLARE_struct_1_(__VA_ARGS__), NITF_StructFieldDescriptor_value_1_(name, __VA_ARGS__))

#define NITF_expand_(v) v
#define NITF_DECLARE_struct_n(n, name, ...) NITF_DECLARE_struct_(name, \
    NITF_expand_(NITF_DECLARE_struct_##n##_(__VA_ARGS__)), \
    NITF_expand_(NITF_StructFieldDescriptor_value_##n##_(name, __VA_ARGS__)))

#define NITF_DECLARE_struct_2_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_1_(__VA_ARGS__))
#define NITF_StructFieldDescriptor_value_2_( name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_1_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_2(name, ...) NITF_DECLARE_struct_n(2, name, __VA_ARGS__)

#define NITF_DECLARE_struct_3_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_2_(__VA_ARGS__))
#define NITF_StructFieldDescriptor_value_3_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_2_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_3(name, ...) NITF_DECLARE_struct_n(3, name, __VA_ARGS__)

#define NITF_DECLARE_struct_4_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_3_(__VA_ARGS__))
#define NITF_StructFieldDescriptor_value_4_( name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_3_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_4(name, ...) NITF_DECLARE_struct_n(4, name, __VA_ARGS__)

#define NITF_DECLARE_struct_5_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_4_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_5_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_4_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_5(name, ...) NITF_DECLARE_struct_n(5, name, __VA_ARGS__)

#define NITF_DECLARE_struct_6_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_5_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_6_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_5_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_6(name, ...) NITF_DECLARE_struct_n(6, name, __VA_ARGS__)

#define NITF_DECLARE_struct_7_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_6_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_7_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_6_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_7(name, ...) NITF_DECLARE_struct_n(7, name, __VA_ARGS__)

#define NITF_DECLARE_struct_8_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_7_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_8_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_7_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_8(name, ...) NITF_DECLARE_struct_n(8, name, __VA_ARGS__)

#define NITF_DECLARE_struct_9_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_8_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_9_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_8_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_9(name, ...) NITF_DECLARE_struct_n(9, name, __VA_ARGS__)

#define NITF_DECLARE_struct_10_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_9_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_10_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_9_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_10(name, ...) NITF_DECLARE_struct_n(10, name, __VA_ARGS__)

#define NITF_DECLARE_struct_11_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_10_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_11_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_10_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_11(name, ...) NITF_DECLARE_struct_n(11, name, __VA_ARGS__)

#define NITF_DECLARE_struct_12_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_11_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_12_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_11_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_12(name, ...) NITF_DECLARE_struct_n(12, name, __VA_ARGS__)

#define NITF_DECLARE_struct_13_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_12_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_13_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_12_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_13(name, ...) NITF_DECLARE_struct_n(13, name, __VA_ARGS__)

#define NITF_DECLARE_struct_14_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_13_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_14_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_13_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_14(name, ...) NITF_DECLARE_struct_n(14, name, __VA_ARGS__)

#define NITF_DECLARE_struct_15_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_14_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_15_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_14_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_15(name, ...) NITF_DECLARE_struct_n(15, name, __VA_ARGS__)

#define NITF_DECLARE_struct_16_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_15_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_16_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_15_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_16(name, ...) NITF_DECLARE_struct_n(16, name, __VA_ARGS__)

#define NITF_DECLARE_struct_17_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_16_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_17_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_16_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_17(name, ...) NITF_DECLARE_struct_n(17, name, __VA_ARGS__)

#define NITF_DECLARE_struct_18_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_17_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_18_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_17_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_18(name, ...) NITF_DECLARE_struct_n(18, name, __VA_ARGS__)

#define NITF_DECLARE_struct_19_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_18_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_19_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_18_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_19(name, ...) NITF_DECLARE_struct_n(19, name, __VA_ARGS__)

#define NITF_DECLARE_struct_20_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_19_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_20_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_19_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_20(name, ...) NITF_DECLARE_struct_n(20, name, __VA_ARGS__)

#define NITF_DECLARE_struct_21_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_20_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_21_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_20_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_21(name, ...) NITF_DECLARE_struct_n(21, name, __VA_ARGS__)

#define NITF_DECLARE_struct_22_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_21_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_22_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_21_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_22(name, ...) NITF_DECLARE_struct_n(22, name, __VA_ARGS__)

#define NITF_DECLARE_struct_23_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_22_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_23_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_22_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_23(name, ...) NITF_DECLARE_struct_n(23, name, __VA_ARGS__)

#define NITF_DECLARE_struct_24_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_23_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_24_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_23_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_24(name, ...) NITF_DECLARE_struct_n(24, name, __VA_ARGS__)

#define NITF_DECLARE_struct_25_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_24_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_25_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_24_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_25(name, ...) NITF_DECLARE_struct_n(25, name, __VA_ARGS__)

#define NITF_DECLARE_struct_26_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_25_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_26_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_25_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_26(name, ...) NITF_DECLARE_struct_n(26, name, __VA_ARGS__)

#define NITF_DECLARE_struct_27_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_26_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_27_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_26_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_27(name, ...) NITF_DECLARE_struct_n(27, name, __VA_ARGS__)

#define NITF_DECLARE_struct_28_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_27_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_28_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_27_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_28(name, ...) NITF_DECLARE_struct_n(5, name, __VA_ARGS__)

#define NITF_DECLARE_struct_29_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_28_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_29_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_28_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_29(name, ...) NITF_DECLARE_struct_n(29, name, __VA_ARGS__)

#define NITF_DECLARE_struct_30_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_29_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_30_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_29_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_30(name, ...) NITF_DECLARE_struct_n(30, name, __VA_ARGS__)

#define NITF_DECLARE_struct_31_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_30_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_31_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_30_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_31(name, ...) NITF_DECLARE_struct_n(31, name, __VA_ARGS__)

#define NITF_DECLARE_struct_32_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_31_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_32_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_31_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_32(name, ...) NITF_DECLARE_struct_n(32, name, __VA_ARGS__)

#define NITF_DECLARE_struct_33_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_32_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_33_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_32_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_33(name, ...) NITF_DECLARE_struct_n(33, name, __VA_ARGS__)

#define NITF_DECLARE_struct_34_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_33_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_34_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_33_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_34(name, ...) NITF_DECLARE_struct_n(34, name, __VA_ARGS__)

#define NITF_DECLARE_struct_35_(t, f, ...) nitf_##t* f; NITF_expand_(NITF_DECLARE_struct_34_( __VA_ARGS__))
#define NITF_StructFieldDescriptor_value_35_(name, t, f, ...) NITF_StructFieldDescriptor_value(t, name, f), \
    NITF_expand_(NITF_StructFieldDescriptor_value_34_(name, __VA_ARGS__))
#define NITF_DECLARE_struct_35(name, ...) NITF_DECLARE_struct_n(35, name, __VA_ARGS__)

#endif // NITRO_nitf_FieldDescriptor_h_INCLUDED_

