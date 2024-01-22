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

#include "nitf/TestingTest.hpp"

nitf::testing::Test1a::Test1a(const Test1a& x)
{
    *this = x;
}

nitf::testing::Test1a& nitf::testing::Test1a::operator=(const Test1a& x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

nitf::testing::Test1a::Test1a(native_t* x)
{
    setNative(x);
    getNativeOrThrow();
}

nitf::testing::Test1a::Test1a() noexcept(false)  : Test1a(nitf_testing_Test1a_construct(&error))
{
    setManaged(false);
}

#define getField_(name) nitf::Field(getNativeOrThrow()->name)
//#define getField_(name) fromNativeOffset<Field>(*this, nitf_offsetof(name));

nitf::Field nitf::testing::Test1a::getF1() const
{
    return getField_(f1);
}

std::vector<nitf::FieldDescriptor> nitf::testing::Test1a::getDescriptors() const
{
    return getFieldDescriptors(nitf_testing_Test1a_fields);
}

nitf::testing::Test1b::Test1b(const Test1b& x)
{
    *this = x;
}

nitf::testing::Test1b& nitf::testing::Test1b::operator=(const Test1b& x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

nitf::testing::Test1b::Test1b(native_t* x)
{
    setNative(x);
    getNativeOrThrow();
}

nitf::testing::Test1b::Test1b() noexcept(false) : Test1b(nitf_testing_Test1b_construct(&error))
{
    setManaged(false);
}


