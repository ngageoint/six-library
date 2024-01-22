/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>

#include <vector>
#include <string>

#include <coda_oss/span.h>
#include <sys/Span.h>

#include "TestCase.h"

 template<typename TContainer, typename TSpan>
 static void testSpanBuffer_(const std::string& testName,
     const TContainer& ints, const TSpan& span)
{
     (void)testName;
    TEST_ASSERT_EQ(ints.size(), span.size());
    TEST_ASSERT_EQ(ints.data(), span.data());

    const size_t dist = std::distance(span.begin(), span.end());
    TEST_ASSERT_EQ(span.size(), dist);

    TEST_ASSERT_EQ(1, span[0]);
    TEST_ASSERT_EQ(1, *(span.begin()));
    TEST_ASSERT_EQ(5, span[4]);

    span[0] = span[4];
    TEST_ASSERT_EQ(5, span[0]);
}
TEST_CASE(testSpanBuffer)
{
    {
        std::vector<int> ints{1, 2, 3, 4, 5};
        const coda_oss::span<int> span(ints.data(), ints.size());
        testSpanBuffer_(testName, ints, span);
    }
}

 template <typename TContainer, typename TSpan>
static void testSpanVector_(const std::string& testName,
                            const TContainer& ints,
                            const TSpan& span)
{
     (void)testName;
    TEST_ASSERT_EQ(ints.size(), span.size());
    TEST_ASSERT_EQ(ints.data(), span.data());

    const size_t dist = std::distance(span.begin(), span.end());
    TEST_ASSERT_EQ(span.size(), dist);

    TEST_ASSERT_EQ(1, span[0]);
    TEST_ASSERT_EQ(1, *(span.begin()));
    TEST_ASSERT_EQ(5, span[4]);
}
TEST_CASE(testSpanVector)
{
    {
        std::vector<int> ints{1, 2, 3, 4, 5};
        const coda_oss::span<int> span(ints.data(), ints.size());
        testSpanVector_(testName, ints, span);
    }
}

TEST_CASE(testGslNarrow)
{
    constexpr int i = INT16_MAX;
    static /*constexpr*/ auto s = gsl::narrow<int16_t>(i); // avoid "conditional expression is constant"
    TEST_ASSERT_EQ(INT16_MAX, s);

    constexpr double d = 3.14;
    static /*constexpr*/ auto v = gsl::narrow_cast<int>(d); // avoid "conditional expression is constant"
    TEST_ASSERT_EQ(3, v);

    TEST_THROWS(gsl::narrow<int>(d));
}

TEST_CASE(test_sys_make_span)
{
    int i = 314;
    int* const p = &i;
    auto s = sys::make_span(p, 1);
    TEST_ASSERT(s.data() == p);
    TEST_ASSERT_EQ(s[0], i);
    s[0] = 123;
    TEST_ASSERT_EQ(i, 123);
    s[0] = 314;

    const int* const q = &i;
    auto cs = sys::make_span(q, 1);
    TEST_ASSERT(cs.data() == q);
    TEST_ASSERT_EQ(cs[0], i);
    //cs[0] = 123; // cs = span<const>
    TEST_ASSERT_EQ(i, 314);

    std::vector<int> v{314};
    s = sys::make_span(v);
    TEST_ASSERT(s.data() == v.data());
    TEST_ASSERT_EQ(s[0], v[0]);
    s[0] = 123;
    TEST_ASSERT_EQ(v[0], 123);
    s[0] = 314;

    const std::vector<int>& u = v;
    cs = sys::make_span(u);
    TEST_ASSERT(cs.data() == u.data());
    TEST_ASSERT_EQ(cs[0], u[0]);
    // cs[0] = 123; // cs = span<const>
    TEST_ASSERT_EQ(u[0], 314);
}

TEST_MAIN(
    TEST_CHECK(testSpanBuffer);
    TEST_CHECK(testSpanVector);
    TEST_CHECK(testGslNarrow);
    TEST_CHECK(test_sys_make_span);
    )
