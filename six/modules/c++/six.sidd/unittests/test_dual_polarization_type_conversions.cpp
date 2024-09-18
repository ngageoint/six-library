/* =========================================================================
* This file is part of six.sidd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six-c++ is free software; you can redistribute it and/or modify
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

#include <vector>

#include <import/str.h>

#include "TestCase.h"
#include <six/Enums.h>
#include <six/sidd/Utilities.h>

TEST_CASE(DualPolarization)
{
    auto&& map = six::PolarizationType::string_to_value_();
    for (auto&& tx : map)
    {
        const auto txType = six::PolarizationType::toType(tx.first);
        TEST_ASSERT_EQ(tx.second, txType);
        if ((txType == six::PolarizationType::NOT_SET) || (txType == six::PolarizationType::UNKNOWN))
        {
            continue;
        }
        for (auto&& rcv : map)
        {
            const auto rcvType = six::PolarizationType::toType(rcv.first);
            TEST_ASSERT_EQ(rcv.second, rcvType);
            if ((rcvType == six::PolarizationType::NOT_SET) || (rcvType == six::PolarizationType::UNKNOWN))
            {
                continue;
            }
            auto strType = txType.toString() + "_" + rcvType.toString();

            auto fromToType = six::DualPolarizationType::toType(strType);

            auto asPair = six::sidd::Utilities::convertDualPolarization(fromToType);
            TEST_ASSERT_EQ(asPair.first.toString(), txType.toString());
            TEST_ASSERT_EQ(asPair.second.toString(), rcvType.toString());
        }
    }
}

TEST_MAIN(
    TEST_CHECK(DualPolarization);
    )
