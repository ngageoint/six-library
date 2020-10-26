/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <import/sys.h>

#include <import/six.h>
#include <import/six/sicd.h>
#include "TestCase.h"

TEST_CASE(valid_six_50x50)
{
#ifndef _WIN32
    // not setup on *ix ... yet
    TEST_ASSERT_TRUE(true);
    return;
#endif

    std::string inputPathname; // sicd_50x50.nitf
    TEST_ASSERT_TRUE(sys::OS().getEnvIfSet("SIX_UNIT_TEST_NITF_PATH_", inputPathname));

    // create an XML registry
    // The reason to do this is to avoid adding XMLControlCreators to the
    // XMLControlFactory singleton - this way has more fine-grained control
    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX,
        new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

    // this validates the DES of the input against the best available schema
    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);

    std::vector<std::string> schemaPaths;
    reader.load(inputPathname, schemaPaths);
    mem::SharedPtr<six::Container> container = reader.getContainer();
    TEST_ASSERT_EQ(container->getNumData(), 1);
    for (size_t jj = 0; jj < container->getNumData(); ++jj)
    {
        std::unique_ptr<six::Data> data;
        data.reset(container->getData(jj)->clone());

        TEST_ASSERT(data->getDataType() == six::DataType::COMPLEX);

        const auto& classification = data->getClassification();
        TEST_ASSERT_TRUE(classification.isUnclassified());

        // UTF-8 characters in 50x50.nitf
        const std::string classificationText("NON CLASSIFI\xc3\x89 / UNCLASSIFIED"); // UTF-8 "NON CLASSIFIÉ / UNCLASSIFIED"
        const auto actual = classification.getLevel();
        TEST_ASSERT_EQ(actual, classificationText);
    }
}

TEST_MAIN(
    TEST_CHECK(valid_six_50x50);
)

