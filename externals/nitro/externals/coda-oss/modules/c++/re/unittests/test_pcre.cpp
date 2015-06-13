/* =========================================================================
 * This file is part of re-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * re-c++ is free software; you can redistribute it and/or modify
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

#include <import/re.h>
#include "TestCase.h"

TEST_CASE(testMatches)
{
    re::PCREMatch matches;
    re::PCRE rx("^([^:]+):[ ]*([^\r\n]+)\r\n(.*)");
    TEST_ASSERT(rx.match("Proxy-Connection: Keep-Alive\r\n", matches));
    TEST_ASSERT_EQ(matches.size(), 4);
}

TEST_CASE(testSearch)
{
    re::PCREMatch matches;
    re::PCRE rx("ar");
    rx.searchAll("arabsdsarbjudarc34ardnjfsdveqvare3arfarg", matches);
    TEST_ASSERT_EQ(matches.size(), 7);
}

TEST_CASE(testSub)
{
    re::PCREMatch matches;
    re::PCRE rx("ar");
    std::string subst = rx.sub("Hearo", "ll");
    TEST_ASSERT_EQ(subst, "Hello");
    subst = rx.sub("Hearo Keary!", "ll");
    TEST_ASSERT_EQ(subst, "Hello Kelly!");
}

TEST_CASE(testSplit)
{
    re::PCREMatch matches;
    re::PCRE rx("ar");
    std::vector<std::string> vec;
    rx.split("ONEarTWOarTHREE", vec);
    TEST_ASSERT_EQ(vec.size(), 3);
    TEST_ASSERT_EQ(vec[0], "ONE");
    TEST_ASSERT_EQ(vec[1], "TWO");
    TEST_ASSERT_EQ(vec[2], "THREE");
}

int main(int argc, char* argv[])
{
    TEST_CHECK( testMatches);
    TEST_CHECK( testSearch);
    TEST_CHECK( testSub);
    TEST_CHECK( testSplit);
}
