/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022 , Maxar Technologies, Inc.
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

#ifndef NITRO_nitf_TestingTest_hpp_INCLUDED_
#define NITRO_nitf_TestingTest_hpp_INCLUDED_
#pragma once

#include <string>

#include "nitf/TestingTest.h"
#include "nitf/System.hpp"
#include "nitf/Field.hpp"
#include "nitf/Object.hpp"
#include "nitf/FieldDescriptor.hpp"

namespace nitf
{
    namespace testing
    {
        // wrap the test classes, ala FileHeader, etc.
        DECLARE_CLASS_IN(Test1a, nitf_testing)
        {
        public:
            Test1a() noexcept(false);
            ~Test1a() = default;
            Test1a(const Test1a & x);
            Test1a& operator=(const Test1a & x);

            //! Set native object
            using native_t = nitf_testing_Test1a;
            Test1a(native_t * x);

            nitf::Field getF1() const;
            std::string f1() const
            {
                return getF1(); // nitf::Field implicitly converts to std::string
            }
            void setF1(const std::string & v) { getF1().set(v); }

            std::vector<FieldDescriptor> getDescriptors() const;

        private:
            mutable nitf_Error error{};
        };

        DECLARE_CLASS_IN(Test1b, nitf_testing)
        {
        public:
            Test1b() noexcept(false);
            ~Test1b() = default;
            Test1b(const Test1b & x);
            Test1b& operator=(const Test1b & x);

            //! Set native object
            using native_t = nitf_testing_Test1b;
            Test1b(native_t * x);

        private:
            mutable nitf_Error error{};
        };
    }
}
#endif // NITRO_nitf_TestingTest_hpp_INCLUDED_
