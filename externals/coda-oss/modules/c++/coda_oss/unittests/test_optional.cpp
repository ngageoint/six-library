/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "coda_oss_TestCase.h"

#include <array>
#include <ostream>

#include "coda_oss/optional.h"

namespace str
{
template <typename T>
std::string toString(const coda_oss::optional<T>& value)
{
    return std::to_string(value.value());
}
}

namespace
{
template <typename T>
std::ostream& operator<<(std::ostream& s, const coda_oss::optional<T>& opt)
{
    if (opt.has_value())
    {
        s << *opt;
    }
    else
    {
        s << "[no value]";
    }
    return s;
}
#if CODA_OSS_cpp17
template <typename T>
std::ostream& operator<<(std::ostream& s, const std::optional<T>& opt)
{
    if (opt.has_value())
    {
        s << *opt;
    }
    else
    {
        s << "[no value]";
    }
    return s;
}
#endif
    
template<typename TOptional>
static void testOptional_(const std::string& testName, const TOptional& opt)
{
    const coda_oss::optional<int> null;
    TEST_ASSERT_FALSE(null.has_value());
    TEST_ASSERT_NOT_EQ(null, 314);
    TEST_ASSERT_GREATER(314, null);
    TEST_ASSERT_GREATER_EQ(314, null);
    TEST_ASSERT_LESSER(null, 314);
    TEST_ASSERT_LESSER_EQ(null, 314);

    TEST_ASSERT_TRUE(opt.has_value());
    TEST_ASSERT_EQ(opt, opt);
    TEST_ASSERT_EQ(314, opt);
    TEST_ASSERT_GREATER_EQ(opt, opt);
    TEST_ASSERT_LESSER_EQ(opt, opt);

    TEST_ASSERT_GREATER(opt, 313);
    TEST_ASSERT_GREATER_EQ(opt, 314);
    TEST_ASSERT_GREATER_EQ(opt, 313);
    TEST_ASSERT_LESSER(313, opt);
    TEST_ASSERT_LESSER_EQ(314, opt);
    TEST_ASSERT_LESSER_EQ(313, opt);

    const auto other = opt;
    TEST_ASSERT_EQ(opt, other);

    // assignment
    TOptional another;
    another = opt;
    TEST_ASSERT_EQ(opt, another);
    another = 314;
    TEST_ASSERT_EQ(314, another);
}

TEST_CASE(test_sys_Optional)
{
    const coda_oss::optional<int> null;
    TEST_ASSERT_FALSE(null.has_value());

    {
        coda_oss::optional<int> opt;
        TEST_ASSERT_FALSE(opt.has_value());
        TEST_ASSERT_EQ(null, opt);
        opt = 314;
        testOptional_(testName, opt);
    }
    {
        coda_oss::optional<int> opt = 314;
        testOptional_(testName, opt);
    }
    {
        auto opt = coda_oss::make_optional<int>(314);
        testOptional_(testName, opt);
    }
}

TEST_CASE(test_std_optional)
{
    TEST_ASSERT(!testName.empty());
#if CODA_OSS_cpp17
    const std::optional<int> null;
    TEST_ASSERT_FALSE(null.has_value());

    {
        std::optional<int> opt;
        TEST_ASSERT_FALSE(opt.has_value());
        TEST_ASSERT_EQ(null, opt);
        opt = 314;
        testOptional_(testName, opt);
    }
    {
        std::optional<int> opt = 314;
        testOptional_(testName, opt);
    }
    {
        auto opt = std::make_optional<int>(314);
        testOptional_(testName, opt);
    }
#endif
}
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(test_sys_Optional);
    TEST_CHECK(test_std_optional);
    return 0;
}
