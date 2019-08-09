/* ==========================================================================
* This file is part of cphd-c++
* ==========================================================================
*
* (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

TEST_CASE(testEqualityOperatorTrue)
{

    cphd::Pvp pvp1;
    cphd::Pvp pvp2;
    pvp1.TxTime.size = 1;
    pvp1.TxTime.offset = 0;
    pvp1.TxTime.format = "F8";

    pvp2.TxTime.size = 1;
    pvp2.TxTime.offset = 0;
    pvp2.TxTime.format = "F8";

    if(pvp1 != pvp2)
    {
        std::cout<<"NOT SAME\n";
    }
    else
    {
        std::cout<<"SAME\n";
    }
    TEST_ASSERT_TRUE((pvp1 == pvp2));

}


TEST_CASE(testEqualityOperatorFalse)
{

    cphd::Pvp pvp1, pvp2;
    pvp1.FXN1.size = 1;
    pvp1.FXN1.offset = 0;
    pvp1.FXN1.format = "F8";

    pvp2.TxTime.size = 1;
    pvp2.TxTime.offset = 0;
    pvp2.TxTime.format = "F8";

    TEST_ASSERT_TRUE((pvp1 != pvp2));

}

}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        TEST_CHECK(testEqualityOperatorTrue);
        TEST_CHECK(testEqualityOperatorFalse);
        // TEST_CHECK(testCanReadHeaderWithoutBreaking);
        // TEST_CHECK(testRoundTripHeader);
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

