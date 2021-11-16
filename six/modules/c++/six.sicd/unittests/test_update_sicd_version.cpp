/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include "TestCase.h"
#include <six/sicd/Utilities.h>
#include <six/sicd/SICDVersionUpdater.h>

TEST_CASE(testInvalidVersions)
{
    six::sicd::ComplexData complexData;
    complexData.setVersion("1.1.0");
    logging::NullLogger log;

    // Invalid SICD version should throw
    TEST_EXCEPTION(six::sicd::SICDVersionUpdater(complexData, "1.1.7", log));

    // No-op should throw
    TEST_EXCEPTION(six::sicd::SICDVersionUpdater(complexData, "1.1.0", log));

    // Downgrade should throw
    TEST_EXCEPTION(six::sicd::SICDVersionUpdater(complexData, "1.0.0", log));

    // Don't support 0.3.1 (yet?)
    TEST_EXCEPTION(six::sicd::SICDVersionUpdater(complexData, "0.3.1", log));
}

TEST_CASE(test110To120)
{
    six::sicd::ComplexData complexData;
    complexData.setVersion("1.1.0");
    logging::NullLogger log;
    six::sicd::SICDVersionUpdater(complexData, "1.2.0", log).update();

    // AFAIK, this is the only difference...
    TEST_ASSERT_EQ(complexData.getVersion(), "1.2.0");
}

TEST_CASE(testTransitiveUpdate)
{
    six::sicd::ComplexData complexData;
    complexData.setVersion("1.0.0");
    logging::NullLogger log;
    six::sicd::SICDVersionUpdater(complexData, "1.1.0", log).update();

    // This is still the only notable difference
    TEST_ASSERT_EQ(complexData.getVersion(), "1.1.0");

    // We should still have a new processing block for this "empty"
    // version update
    TEST_ASSERT(complexData.imageFormation.get());
    TEST_ASSERT_EQ(complexData.imageFormation->processing.size(), static_cast<size_t>(1));
    const auto& processing = complexData.imageFormation->processing[0];
    TEST_ASSERT_FALSE(processing.type.empty());
    TEST_ASSERT_EQ(processing.applied, six::AppliedType::IS_TRUE);
}

TEST_CASE(testUpdateDistRefLinePoly)
{
    logging::NullLogger log;
    six::sicd::ComplexData complexData;

    // 0.4.0 -> 0.4.1 or 0.5.0 should initialize the polynomial
    complexData.setVersion("0.4.0");
    complexData.rma.reset(new six::sicd::RMA());
    complexData.rma->rmat.reset(new six::sicd::RMAT());

    six::sicd::SICDVersionUpdater(complexData, "0.5.0", log).update();
    TEST_ASSERT_FALSE(six::Init::isUndefined(complexData.rma->rmat->distRefLinePoly));

    // Jumping from 0.4.0 to 1.0.0 should leave it uninitialized
    complexData.setVersion("0.4.0");
    complexData.rma.reset(new six::sicd::RMA());
    complexData.rma->rmat.reset(new six::sicd::RMAT());

    six::sicd::SICDVersionUpdater(complexData, "1.0.0", log).update();
    TEST_ASSERT_TRUE(six::Init::isUndefined(complexData.rma->rmat->distRefLinePoly));
}

TEST_CASE(testWarningParameters)
{
    logging::NullLogger log;
    six::sicd::ComplexData complexData;

    // Add in an initial processing step to make sure we deal
    // with it properly
    complexData.imageFormation->processing.push_back(six::sicd::Processing());

    complexData.setVersion("0.4.0");

    six::sicd::SICDVersionUpdater(complexData, "1.0.0", log).update();

    // Make sure original Processing is still there
    TEST_ASSERT_EQ(complexData.imageFormation->processing.size(), static_cast<size_t>(2));
    TEST_ASSERT(complexData.imageFormation->processing[0].parameters.empty());

    // We should have a new processing block with parameters for
    // reported warnings
    const auto& processing = complexData.imageFormation->processing[1];
    TEST_ASSERT_FALSE(processing.type.empty());
    TEST_ASSERT_EQ(processing.applied, six::AppliedType::IS_TRUE);

    const auto& parameters = processing.parameters;
    TEST_ASSERT_FALSE(parameters.empty());
}

TEST_MAIN((void)argv; (void)argc;
    // These tests should suffice to cover all of the "tricky"
    // logic in the implementation.  Any remaining bugs would
    // most likely be due to misreading the standards.
    TEST_CHECK(testInvalidVersions);
    TEST_CHECK(test110To120);
    TEST_CHECK(testTransitiveUpdate);
    TEST_CHECK(testUpdateDistRefLinePoly);
    TEST_CHECK(testWarningParameters);
    )