/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2025, Arka Group, L.P.
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

#include <six/csm/SIXPlugin.h>
#include <six/sicd/ComplexXMLControl.h>

#include <std/filesystem>

#include "utilities.h"

// CSM includes
#include <NitfIsd.h>
#include <Plugin.h>
#include <RasterGM.h>

#include "TestCase.h"

namespace fs = std::filesystem;

class TestHarness
{
public:
    static TestHarness& getInstance()
    {
        if (!mInstance.get())
            mInstance = std::unique_ptr<TestHarness>(new TestHarness());

        return *mInstance.get();
    }

    std::string schemaPath()
    {
        return mSchemaPath;
    }

    const csm::Plugin& plugin()
    {
        return *mPlugin;
    }

    fs::path find(std::string filename)
    {
        return sys::test::findGITModuleFile("croppedNitfs", "SICD", filename);
    }

private:
    TestHarness()
    {
        six::CSM::SIXPlugin::_publicInterface();

        static const auto sicdModulePath = std::filesystem::path("six") /
                "modules" / "c++" / "six.sicd" / "conf" / "schema";
        static const auto sicdRootSchemaDir =
                sys::test::findGITModuleFile(
                        "six",
                        sicdModulePath,
                        "SICD_schema_V1.0.0_2011_08_31.xsd")
                        .parent_path();

        static const auto siddModulePath = std::filesystem::path("six") /
                "modules" / "c++" / "six.sidd" / "conf" / "schema";
        static const auto siddRootSchemaDir =
                sys::test::findGITModuleFile("six",
                                             siddModulePath,
                                             "SICommonTypes_V1.0.xsd")
                        .parent_path();

        std::vector<std::filesystem::path> schemaPaths{sicdRootSchemaDir,
                                                       siddRootSchemaDir};

        mSchemaPath = sicdRootSchemaDir;

        csm::Plugin::setDataDirectory(mSchemaPath);

        // Make sure we found it
        csm::PluginList pluginList = csm::Plugin::getList();

        if (pluginList.size() != 1)
        {
            throw except::Exception(Ctxt("Expected 1 plugin but found " +
                                         str::toString(pluginList.size())));
        }

        mPlugin = *pluginList.begin();

        if (mPlugin->getPluginName() != "SIX")
        {
            throw except::Exception(Ctxt("Unexpected plugin name '" +
                                         mPlugin->getPluginName() + "'"));
        }
    }

private:
    const csm::Plugin* mPlugin;
    std::string mSchemaPath;
    static std::unique_ptr<TestHarness> mInstance;
};

std::unique_ptr<TestHarness> TestHarness::mInstance;

TEST_CASE(testPluginParams)
{
    TestHarness& harness = TestHarness::getInstance();
    const csm::Plugin& plugin = harness.plugin();

    TEST_ASSERT_EQ(plugin.getPluginName(), "SIX");
    TEST_ASSERT_EQ(plugin.getManufacturer(), "Arka");
    TEST_ASSERT_EQ(plugin.getReleaseDate(), "20250301");
    TEST_ASSERT_EQ(plugin.getNumModels(), 2);

    TEST_ASSERT_EQ(plugin.getModelName(0), "SICD_SENSOR_MODEL");
    TEST_ASSERT_EQ(plugin.getModelFamily(0), "GeometricRaster");

    TEST_ASSERT_EQ(plugin.getModelName(1), "SIDD_SENSOR_MODEL");
    TEST_ASSERT_EQ(plugin.getModelFamily(1), "GeometricRaster");
}

TEST_CASE(testFromISD)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    for (int i = 0; i < 2; i++)
    {
        std::unique_ptr<csm::Isd> isd;

        if (i == 0)
        {
            isd = std::unique_ptr<csm::Isd>(new csm::Isd(sicd));
        }
        else
        {
            six::XMLControlRegistry xmlRegistry;
            six::NITFReadControl reader;
            std::unique_ptr<six::sicd::ComplexData> complexData;

            // Read in the SICD XML
            xmlRegistry.addCreator<six::sicd::ComplexXMLControl>();

            reader.setXMLControlRegistry(&xmlRegistry);

            reader.load(sicd,
                        std::vector<std::string>(1, harness.schemaPath()));
            complexData = six::sicd::Utilities::getComplexData(reader);

            isd = constructIsd(sicd, reader, complexData.get(), xmlRegistry);
        }

        TEST_ASSERT(
                plugin.canModelBeConstructedFromISD(*isd, "SICD_SENSOR_MODEL"));

        std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
                plugin.constructModelFromISD(*isd, "SICD_SENSOR_MODEL")));

        /*std::pair<csm::ImageCoord, csm::ImageCoord> vir =
                model->getValidImageRange();*/

        csm::ImageCoord ul = model->getImageStart();
        csm::ImageVector size = model->getImageSize();
        TEST_ASSERT_EQ(ul.line, 100);
        TEST_ASSERT_EQ(ul.samp, 100);
        TEST_ASSERT_EQ(size.line, 5);
        TEST_ASSERT_EQ(size.line, 5);

        /*csm::EcefCoord refpt = model->getReferencePoint();
        csm::ImageCoord ic = model->groundToImage(refpt);*/
    }
}

TEST_CASE(testFromState)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sicd);

    TEST_ASSERT(plugin.canISDBeConvertedToModelState(isd, "SICD_SENSOR_MODEL"));
    std::string state = plugin.convertISDToModelState(isd, "SICD_SENSOR_MODEL");

    TEST_ASSERT_EQ(plugin.getModelNameFromModelState(state),
                   "SICD_SENSOR_MODEL");
    TEST_ASSERT(
            plugin.canModelBeConstructedFromState("SICD_SENSOR_MODEL", state));

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromState(state)));
}

TEST_CASE(testModelState)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sicd);

    TEST_ASSERT(plugin.canISDBeConvertedToModelState(isd, "SICD_SENSOR_MODEL"));
    std::string state = plugin.convertISDToModelState(isd, "SICD_SENSOR_MODEL");

    TEST_ASSERT_EQ(plugin.getModelNameFromModelState(state),
                   "SICD_SENSOR_MODEL");
    TEST_ASSERT(
            plugin.canModelBeConstructedFromState("SICD_SENSOR_MODEL", state));

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromState(state)));

    TEST_ASSERT_EQ(model->getModelState(), state);

    std::unique_ptr<csm::RasterGM> model2(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SICD_SENSOR_MODEL")));
    TEST_ASSERT_EQ(model2->getModelState(), state);

    model2->replaceModelState(state);
    TEST_ASSERT_EQ(model2->getModelState(), state);
}

TEST_CASE(testAdjParamsState)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sicd);

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SICD_SENSOR_MODEL")));

    TEST_ASSERT_EQ(model->getNumParameters(), 7);
    for (int i = 0; i < 7; i++)
        model->setParameterValue(i, 11 * i);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model->getParameterValue(i), 11 * i);

    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            model->setParameterCovariance(i, j, i*2.3 + j*0.11);
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            TEST_ASSERT_EQ(model->getParameterCovariance(i, j), i*2.3 + j*0.11);

    for (int i = 0; i < 7; i++)
        model->setParameterType(i, csm::param::REAL);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model->getParameterType(i), csm::param::REAL);

    for (int i = 0; i < 7; i++)
        model->setParameterType(i, csm::param::FIXED);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model->getParameterType(i), csm::param::FIXED);

    for (int i = 0; i < 7; i++)
        model->setParameterType(i, csm::param::FICTITIOUS);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model->getParameterType(i), csm::param::FICTITIOUS);

    for (int i = 0; i < 7; i++)
        model->setParameterType(i, csm::param::NONE);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model->getParameterType(i), csm::param::NONE);

    // round-trip the model through the state string
    std::unique_ptr<csm::RasterGM> model2(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromState(model->getModelState())));

    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model2->getParameterValue(i), 11 * i);
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            TEST_ASSERT_EQ(model2->getParameterCovariance(i, j), i*2.3 + j*0.11);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model2->getParameterType(i), csm::param::NONE);
}

// Test imageToGround projections using modified adjustable parameters
TEST_CASE(testAdjParams1)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sicd);

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SICD_SENSOR_MODEL")));

    csm::ImageCoordCovar ic(4000, 4000, 1.5, 0.3, 1.2);
    double height(300);
    double heightCovar(1);
    csm::EcefCoordCovar gc = model->imageToGround(ic, height, heightCovar);

    for (int i = 0; i < 7; i++)
    {
        double origValue = model->getParameterValue(i);
        double adj = (i < 3) ? 1 : 0.1;
        model->setParameterValue(i, origValue + adj);

        csm::EcefCoordCovar gcAdj =
                model->imageToGround(ic, height, heightCovar);
        TEST_ASSERT_NOT_EQ(gcAdj.x, gc.x);
        TEST_ASSERT_NOT_EQ(gcAdj.y, gc.y);
        TEST_ASSERT_NOT_EQ(gcAdj.z, gc.z);
        for (int j = 0; j < 9; j++)
            TEST_ASSERT_NOT_EQ(gcAdj.covariance[j], gc.covariance[j]);

        model->setParameterValue(i, origValue);
        csm::EcefCoordCovar gcRestored =
                model->imageToGround(ic, height, heightCovar);
        TEST_ASSERT_EQ(gcRestored.x, gc.x);
        TEST_ASSERT_EQ(gcRestored.y, gc.y);
        TEST_ASSERT_EQ(gcRestored.z, gc.z);
        for (int j = 0; j < 9; j++)
            TEST_ASSERT_EQ(gcRestored.covariance[j], gc.covariance[j]);
    }
}

// Test groundToImage projections using modified adjustable parameters
TEST_CASE(testAdjParams2)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sicd);

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SICD_SENSOR_MODEL")));

    csm::EcefCoordCovar gc(
            4.19186e+06, 901641, 4.70669e+06, 3, 1, 0.5, 4, 0.3, 2);
    csm::ImageCoordCovar ic = model->groundToImage(gc);

    for (int i = 0; i < 7; i++)
    {
        double origValue = model->getParameterValue(i);
        double adj = (i < 3) ? 1 : 0.1;
        model->setParameterValue(i, origValue + adj);

        csm::ImageCoordCovar icAdj = model->groundToImage(gc);
        TEST_ASSERT_NOT_EQ(icAdj.line, ic.line);
        TEST_ASSERT_NOT_EQ(icAdj.samp, ic.samp);
        for (int j = 0; j < 4; j++)
            TEST_ASSERT_NOT_EQ(icAdj.covariance[j], ic.covariance[j]);

        model->setParameterValue(i, origValue);
        csm::ImageCoordCovar icRestored = model->groundToImage(gc);
        TEST_ASSERT_EQ(icRestored.line, ic.line);
        TEST_ASSERT_EQ(icRestored.samp, ic.samp);
        for (int j = 0; j < 4; j++)
            TEST_ASSERT_EQ(icRestored.covariance[j], ic.covariance[j]);
    }
}

TEST_CASE(testImageIdentifier)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sicd);

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SICD_SENSOR_MODEL")));

    model->setImageIdentifier("test identifier");
    TEST_ASSERT_EQ(model->getImageIdentifier(), "test identifier");

    std::unique_ptr<csm::RasterGM> model2(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromState(model->getModelState())));
    TEST_ASSERT_EQ(model2->getImageIdentifier(), "test identifier");
}

int main(int argc, char* argv[])
{
    TEST_CHECK(testPluginParams);
    TEST_CHECK(testFromISD);
    TEST_CHECK(testFromState);
    TEST_CHECK(testModelState);
    TEST_CHECK(testAdjParamsState);
    TEST_CHECK(testAdjParams1);
    TEST_CHECK(testAdjParams2);
    TEST_CHECK(testImageIdentifier);
}
