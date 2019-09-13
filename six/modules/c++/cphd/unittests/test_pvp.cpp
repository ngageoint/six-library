/* ==========================================================================
* This file is part of cphd-c++
* ==========================================================================
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

    pvp1.txTime.setData(1, 0, "F8");
    pvp2.txTime.setData(1, 0, "F8");

    TEST_ASSERT_TRUE((pvp1 == pvp2));
}

TEST_CASE(testAddedParamsEqualityOperatorTrue)
{
    cphd::Pvp pvp1, pvp2;

    pvp1.addedPVP.push_back(cphd::APVPType());
    pvp1.addedPVP.push_back(cphd::APVPType());
    pvp1.addedPVP[0].setName("AddedParam1");
    pvp1.addedPVP[1].setName("AddedParam2");

    pvp2.addedPVP.push_back(cphd::APVPType());
    pvp2.addedPVP.push_back(cphd::APVPType());
    pvp2.addedPVP[0].setName("AddedParam1");
    pvp2.addedPVP[1].setName("AddedParam2");

    TEST_ASSERT_TRUE((pvp1 == pvp2));
}


TEST_CASE(testSimpleEqualityOperatorFalse)
{

    cphd::Pvp pvp1, pvp2;
    pvp1.fxN1.reset(new cphd::PVPType());
    pvp1.fxN1->setData(1, 0, "F8");
    pvp2.txTime.setData(1, 0, "F8");

    TEST_ASSERT_TRUE((pvp1 != pvp2));

}

TEST_CASE(testAddedParamsEqualityOperatorFalse)
{
    cphd::Pvp pvp1;
    cphd::Pvp pvp2;
    cphd::Pvp pvp3;

    pvp1.addedPVP.push_back(cphd::APVPType());
    pvp1.addedPVP.push_back(cphd::APVPType());
    pvp1.addedPVP[0].setName("AddedParam1");
    pvp1.addedPVP[1].setName("AddedParam2");

    pvp2.addedPVP.push_back(cphd::APVPType());
    pvp2.addedPVP.push_back(cphd::APVPType());
    pvp2.addedPVP[0].setName("AddedParam1");

    pvp3.addedPVP.push_back(cphd::APVPType());
    pvp3.addedPVP.push_back(cphd::APVPType());
    pvp3.addedPVP[0].setName("AddedParam1");
    pvp3.addedPVP[1].setName("AddedParam3");

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

