/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <iostream>
#include <memory>

#include <xml/lite/MinidomParser.h>
#include <six/Init.h>

#include <cphd/PVP.h>

#include "TestCase.h"


namespace
{
TEST_CASE(testSimpleEqualityOperatorTrue)
{
    cphd::Pvp pvp1, pvp2;

    pvp1.txTime.setOffset(0);
    pvp2.txTime.setOffset(0);

    TEST_ASSERT_TRUE((pvp1 == pvp2));
}

TEST_CASE(testAddedParamsEqualityOperatorTrue)
{
    cphd::Pvp pvp1;
    pvp1.setParameter(1, 0, "F8", "AddedParam1");
    pvp1.setParameter(1, 1, "F8", "AddedParam2");

    cphd::Pvp pvp2;
    pvp2.setParameter(1, 0, "F8", "AddedParam1");
    pvp2.setParameter(1, 1, "F8", "AddedParam2");

    TEST_ASSERT_TRUE((pvp1 == pvp2));
}


TEST_CASE(testSimpleEqualityOperatorFalse)
{
    cphd::Pvp pvp1;
    pvp1.fxN1.setOffset(0);

    cphd::Pvp pvp2;
    pvp2.txTime.setOffset(1);

    TEST_ASSERT_TRUE((pvp1 != pvp2));

}

TEST_CASE(testAddedParamsEqualityOperatorFalse)
{
    cphd::Pvp pvp1;
    pvp1.setParameter(1, 0, "F8", "AddedParam1");
    pvp1.setParameter(1, 1, "F8", "AddedParam2");

    cphd::Pvp pvp2;
    pvp2.setParameter(1, 0, "F8", "AddedParam1");

    cphd::Pvp pvp3;
    pvp3.setParameter(1, 0, "F8", "AddedParam1");
    pvp3.setParameter(1, 1, "F8", "AddedParam3");

    TEST_ASSERT_TRUE((pvp1 != pvp2));
    TEST_ASSERT_TRUE((pvp1 != pvp3));
}
}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        TEST_CHECK(testSimpleEqualityOperatorTrue);
        TEST_CHECK(testAddedParamsEqualityOperatorTrue);
        TEST_CHECK(testSimpleEqualityOperatorFalse);
        TEST_CHECK(testAddedParamsEqualityOperatorFalse);
        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }
    return 1;
}
