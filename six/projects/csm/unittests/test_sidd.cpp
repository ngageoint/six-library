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

#include <six/Utilities.h>
#include <six/csm/SIXPlugin.h>
#include <six/sidd/DerivedXMLControl.h>
#include <str/Encoding.h>

#include <std/filesystem>

#include "utilities.h"

// CSM includes
#include <NitfIsd.h>
#include <Plugin.h>
#include <RasterGM.h>

#include "TestCase.h"

namespace fs = std::filesystem;

namespace
{
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
        return sys::test::findGITModuleFile("croppedNitfs", "SIDD", filename);
    }

    std::string fakeSiddXmlData(std::string siddVersion, std::string productName)
    {
        // Fake sidd metadata that still should work through projection.  Note
        // that this data may not be internally consistent but it is fine for
        // limited testing purposes.

        std::string siddOpen =
                R"(<SIDD xmlns="urn:SIDD:)" + siddVersion + R"(" xmlns:si="urn:SICommon:1.0" xmlns:sfa="urn:SFA:1.2.0" xmlns:ism="urn:us:gov:ic:ism:13">)";
        std::string productCreation =
                R"(<ProductCreation><ProcessorInformation><Application>application</Application><ProcessingDateTime>2024-01-01T00:00:00.000000Z</ProcessingDateTime><Site>site</Site></ProcessorInformation><Classification ism:DESVersion="13" ism:ISMCATCESVersion="1" ism:resourceElement="true" ism:createDate="2024-01-01" ism:compliesWith="USGov" ism:classification="U" ism:ownerProducer="USA"/>)"
                R"(<ProductName>)" + productName + R"(</ProductName><ProductClass>Detected Image</ProductClass></ProductCreation>)";
        std::string display =
                R"(<Display><PixelType>MONO8I</PixelType><NumBands>1</NumBands>)"
                R"(<NonInteractiveProcessing band="1"><ProductGenerationOptions><DataRemapping><LUTName>PEDF</LUTName><Predefined><DatabaseName>PEDF</DatabaseName></Predefined></DataRemapping></ProductGenerationOptions><RRDS><DownsamplingMethod>MAX PIXEL</DownsamplingMethod></RRDS></NonInteractiveProcessing>)"
                R"(<InteractiveProcessing band="1">)"
                R"(<GeometricTransform><Scaling><AntiAlias><FilterName>Antialiasing</FilterName><FilterKernel><Predefined><DatabaseName>BILINEAR</DatabaseName></Predefined></FilterKernel><Operation>CONVOLUTION</Operation></AntiAlias><Interpolation><FilterName>Interpolation</FilterName><FilterBank><Predefined><DatabaseName>LAGRANGE</DatabaseName></Predefined></FilterBank><Operation>CORRELATION</Operation></Interpolation></Scaling><Orientation><ShadowDirection>DOWN</ShadowDirection></Orientation></GeometricTransform>)"
                R"(<SharpnessEnhancement><ModularTransferFunctionEnhancement><FilterName>MTFE</FilterName><FilterBank><Predefined><FilterFamily>0</FilterFamily><FilterMember>5</FilterMember></Predefined></FilterBank><Operation>CONVOLUTION</Operation></ModularTransferFunctionEnhancement></SharpnessEnhancement>)"
                R"(<DynamicRangeAdjustment><AlgorithmType>AUTO</AlgorithmType><BandStatsSource>1</BandStatsSource><DRAParameters><Pmin>0</Pmin><Pmax>0.995</Pmax><EminModifier>0</EminModifier><EmaxModifier>0</EmaxModifier></DRAParameters></DynamicRangeAdjustment>)"
                R"(</InteractiveProcessing>)"
                R"(</Display>)";
        std::string geoData =
                R"(<GeoData><EarthModel>WGS_84</EarthModel><ImageCorners>)"
                R"(<ICP index="1:FRFC"><si:Lat>0.01</si:Lat><si:Lon>-0.01</si:Lon></ICP>)"
                R"(<ICP index="2:FRLC"><si:Lat>0.01</si:Lat><si:Lon>0.01</si:Lon></ICP>)"
                R"(<ICP index="3:LRLC"><si:Lat>-0.01</si:Lat><si:Lon>0.01</si:Lon></ICP>)"
                R"(<ICP index="4:LRFC"><si:Lat>-0.01</si:Lat><si:Lon>-0.01</si:Lon></ICP>)"
                R"(</ImageCorners><ValidData size="4">)"
                R"(<Vertex index="1"><si:Lat>0.01</si:Lat><si:Lon>-0.01</si:Lon></Vertex>)"
                R"(<Vertex index="2"><si:Lat>0.01</si:Lat><si:Lon>0.01</si:Lon></Vertex>)"
                R"(<Vertex index="3"><si:Lat>-0.01</si:Lat><si:Lon>0.01</si:Lon></Vertex>)"
                R"(<Vertex index="4"><si:Lat>-0.01</si:Lat><si:Lon>-0.01</si:Lon></Vertex>)"
                R"(</ValidData></GeoData>)";
        std::string measurement =
                R"(<Measurement>)"
                R"(<PlaneProjection><ReferencePoint name="SCP"><si:ECEF><si:X>6378137</si:X><si:Y>0</si:Y><si:Z>0</si:Z></si:ECEF><si:Point><si:Row>500</si:Row><si:Col>500</si:Col></si:Point></ReferencePoint><SampleSpacing><si:Row>0.8</si:Row><si:Col>0.8</si:Col></SampleSpacing><TimeCOAPoly order1="0" order2="0"><si:Coef exponent1="0" exponent2="0">10</si:Coef></TimeCOAPoly><ProductPlane><RowUnitVector><si:X>0.9185676191359099</si:X><si:Y>0.015449014240503411</si:Y><si:Z>0.39496184250378258</si:Z></RowUnitVector><ColUnitVector><si:X>-0.31872818071659126</si:X><si:Y>0.61991268778351927</si:Y><si:Z>0.7170220403461145</si:Z></ColUnitVector></ProductPlane></PlaneProjection>)"
                R"(<PixelFootprint><si:Row>1000</si:Row><si:Col>1000</si:Col></PixelFootprint>)"
                R"(<ARPPoly><si:X order1="1"><si:Coef exponent1="0">6383137</si:Coef><si:Coef exponent1="1">0</si:Coef></si:X><si:Y order1="1"><si:Coef exponent1="0">-5000</si:Coef><si:Coef exponent1="1">0</si:Coef></si:Y><si:Z order1="1"><si:Coef exponent1="0">-2500</si:Coef><si:Coef exponent1="1">1000</si:Coef></si:Z></ARPPoly>)"
                R"(<ValidData size="4">)"
                R"(<Vertex index="1"><si:Row>0</si:Row><si:Col>0</si:Col></Vertex>)"
                R"(<Vertex index="2"><si:Row>0</si:Row><si:Col>1000</si:Col></Vertex>)"
                R"(<Vertex index="3"><si:Row>1000</si:Row><si:Col>1000</si:Col></Vertex>)"
                R"(<Vertex index="4"><si:Row>1000</si:Row><si:Col>0</si:Col></Vertex>)"
                R"(</ValidData></Measurement>)";
        std::string exploitationFeatures =
                R"(<ExploitationFeatures>)"
                R"(<Collection identifier=")" + productName + R"(">)"
                R"(<Information><SensorName>sensor name</SensorName><RadarMode><si:ModeType>SPOTLIGHT</si:ModeType></RadarMode><CollectionDateTime>2016-01-01T00:00:00.000000Z</CollectionDateTime><CollectionDuration>20</CollectionDuration>)"
                R"(<Polarization><TxPolarization>H</TxPolarization><RcvPolarization>H</RcvPolarization></Polarization>)"
                R"(</Information>)"
                R"(<Geometry><Azimuth>0</Azimuth><Slope>30</Slope><Squint>90</Squint><Graze>30</Graze><Tilt>0</Tilt><DopplerConeAngle>90</DopplerConeAngle></Geometry>)"
                R"(<Phenomenology><Shadow><si:Angle>90</si:Angle><si:Magnitude>2</si:Magnitude></Shadow><Layover><si:Angle>270</si:Angle><si:Magnitude>0.5</si:Magnitude></Layover><MultiPath>270</MultiPath><GroundTrack>0</GroundTrack></Phenomenology></Collection>)"
                R"(<Product>)"
                R"(<Resolution><si:Row>1.0</si:Row><si:Col>1.0</si:Col></Resolution><Ellipticity>1.0</Ellipticity>)"
                R"(<Polarization><TxPolarizationProc>H</TxPolarizationProc><RcvPolarizationProc>H</RcvPolarizationProc></Polarization><North>90</North>)"
                R"(</Product>)"
                R"(</ExploitationFeatures>)";
        std::string siddClose =
                R"(</SIDD>)";

        return siddOpen + productCreation + display + geoData + measurement + exploitationFeatures + siddClose;
    }

    std::unique_ptr<six::sidd::DerivedData> fakeDerivedData(
            std::string siddVersion)
    {
        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator<six::sidd::DerivedXMLControl>();

        std::string xml = fakeSiddXmlData(siddVersion, "iid1");

        logging::NullLogger logger;
        auto data = six::parseDataFromString(xmlRegistry,
                                             xml,
                                             six::DataType::DERIVED,
                                             {schemaPath()},
                                             logger);

        return std::unique_ptr<six::sidd::DerivedData>(
                reinterpret_cast<six::sidd::DerivedData*>(data.release()));
    }

    std::string desHdr()
    {
        // It only needs to pass inspection as a SIDD.  The version numbers
        // specified here are not used within the plugins.
        std::string desHeader =
                R"(DEXML_DATA_CONTENT         01UUS                                                                                      )"
                R"(                                                                              077399999XML     2024-09-17T16:29:59Z   )"
                R"(                                     SIDD Volume 1 Design & Implementation Description Document  3.0       2021-11-30T)"
                R"(00:00:00Zurn:SIDD:3.0.0                                                                                               )"
                R"(           +00.00000000+000.00000000+00.00000000+000.00000000+00.00000000+000.00000000+00.00000000+000.00000000+00.000)"
                R"(00000+000.00000000                                                                                                    )"
                R"(                                                                                                                      )"
                R"(                                                                                                                      )"
                R"(                             )";
        return desHeader;
    }

    csm::Nitf21Isd isdFromDerived(
            std::unique_ptr<six::sidd::DerivedData>& derivedData)
    {
        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator<six::sidd::DerivedXMLControl>();

        csm::Des des;
        des.setSubHeader(desHdr());
        des.setData(str::to_native(
                six::toXMLString(derivedData.get(), &xmlRegistry)));

        csm::Nitf21Isd isd;
        isd.addFileDes(des);
        isd.addImage(csm::Image("IMSIDD001001", {}));

        return isd;
    }

    std::unique_ptr<csm::RasterGM> modelFromDerived(
            std::unique_ptr<six::sidd::DerivedData>& derivedData)
    {
        csm::Nitf21Isd isd = isdFromDerived(derivedData);
        std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
                mPlugin->constructModelFromISD(isd, "SIDD_SENSOR_MODEL")));
        return model;
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

        mSchemaPath = siddRootSchemaDir.string();

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

template <long unsigned int N>
void matrixCondition(const math::linear::MatrixMxN<N, N>& m,
                     double& cond,
                     bool& posDefinite)
{
    const math::linear::Eigenvalue<double> Veig(m);
    const math::linear::Vector<double> eigs = Veig.getRealEigenvalues();
    std::vector<double> eigsVec(eigs.get(), eigs.get() + eigs.size());
    std::sort(eigsVec.begin(), eigsVec.end());

    cond = std::abs(eigsVec[eigsVec.size() - 1] / eigsVec[0]);
    posDefinite = eigsVec[eigsVec.size() - 1] > 0;
}
}

std::unique_ptr<TestHarness> TestHarness::mInstance;

TEST_CASE(testPluginParams)
{
    TestHarness& harness = TestHarness::getInstance();
    const csm::Plugin& plugin = harness.plugin();

    TEST_ASSERT_EQ(plugin.getPluginName(), "SIX");
    TEST_ASSERT_EQ(plugin.getManufacturer(), "Arka");
    TEST_ASSERT_EQ(plugin.getReleaseDate(), "20250301");
    TEST_ASSERT_EQ(plugin.getNumModels(), 3);

    TEST_ASSERT_EQ(plugin.getModelName(0), "SICD_SENSOR_MODEL");
    TEST_ASSERT_EQ(plugin.getModelFamily(0), "GeometricRaster");

    TEST_ASSERT_EQ(plugin.getModelName(1), "SIDD_SENSOR_MODEL");
    TEST_ASSERT_EQ(plugin.getModelFamily(1), "GeometricRaster");

    TEST_ASSERT_EQ(plugin.getModelName(2), "SIDD_POLY_SENSOR_MODEL");
    TEST_ASSERT_EQ(plugin.getModelFamily(2), "GeometricRaster");
}

TEST_CASE(testIID1FormatCheck)
{
    TestHarness& harness = TestHarness::getInstance();
    const csm::Plugin& plugin = harness.plugin();

    auto derivedData = harness.fakeDerivedData("3.0.0");
    auto standardIsd = harness.isdFromDerived(derivedData);
    csm::Des des = standardIsd.fileDess().at(0);

    csm::Nitf21Isd isd = csm::Nitf21Isd();
    isd.addImage(csm::Image("IMSIDD001001", {}));
    isd.addFileDes(des);
    plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL");

    std::vector<std::string> invalidIID1s = {
        "IM          ",
        "IMSIDD      ",
        "IMSIDD001   ",
        "IMSIDD000001",
    };

    for (std::string invalidIID1 : invalidIID1s)
    {
        isd = csm::Nitf21Isd();
        isd.addImage(csm::Image(invalidIID1, {}));
        isd.addFileDes(des);
        TEST_SPECIFIC_EXCEPTION(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL"), csm::Error);
    }
}

TEST_CASE(testImageIndexMapping)
{
    TestHarness& harness = TestHarness::getInstance();
    const csm::Plugin& plugin = harness.plugin();

    //auto derivedData = harness.fakeDerivedData("3.0.0");
    //auto standardIsd = harness.isdFromDerived(derivedData);
    //csm::Des des = standardIsd.fileDess().at(0);
    csm::Des des1(harness.desHdr(), harness.fakeSiddXmlData("3.0.0", "iid1"));
    csm::Des des2(harness.desHdr(), harness.fakeSiddXmlData("3.0.0", "iid2"));

    // 1 image, 1 DES
    csm::Nitf21Isd isd = csm::Nitf21Isd();
    isd.addImage(csm::Image("IMSIDD001001", {}));
    isd.addFileDes(des1);

    isd.addParam("IMAGE_INDEX", "0");
    plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL");

    isd.clearAllParams();
    isd.addParam("IMAGE_INDEX", "1");
    TEST_SPECIFIC_EXCEPTION(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL"), csm::Error);

    isd.clearAllParams();
    isd.addParam("IMAGE_INDEX", "text");
    TEST_SPECIFIC_EXCEPTION(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL"), csm::Error);

    // 2 images, 2 DES's
    isd = csm::Nitf21Isd();
    isd.addImage(csm::Image("IMSIDD001001", {}));
    isd.addImage(csm::Image("IMSIDD002001", {}));
    isd.addFileDes(des1);
    isd.addFileDes(des2);

    TEST_ASSERT_EQ(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")->getImageIdentifier(), "iid1");

    isd.addParam("IMAGE_INDEX", "1");
    TEST_ASSERT_EQ(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")->getImageIdentifier(), "iid2");
    //TODO: test image id or something else to prove we're looking at the correct DES

    isd.clearAllParams();
    isd.addParam("IMAGE_INDEX", "2");
    TEST_SPECIFIC_EXCEPTION(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL"), csm::Error);

    // 5 images, 2 DES's
    isd = csm::Nitf21Isd();
    isd.addImage(csm::Image("IMSIDD001001", {}));
    isd.addImage(csm::Image("IMSIDD001002", {}));
    isd.addImage(csm::Image("IMSIDD002001", {}));
    isd.addImage(csm::Image("IMSIDD002002", {}));
    isd.addImage(csm::Image("IMSIDD002003", {}));
    isd.addFileDes(des1);
    isd.addFileDes(des2);

    TEST_ASSERT_EQ(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")->getImageIdentifier(), "iid1");

    isd.addParam("IMAGE_INDEX", "1");
    TEST_ASSERT_EQ(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")->getImageIdentifier(), "iid1");

    isd.clearAllParams();
    isd.addParam("IMAGE_INDEX", "2");
    TEST_ASSERT_EQ(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")->getImageIdentifier(), "iid2");

    isd.clearAllParams();
    isd.addParam("IMAGE_INDEX", "3");
    TEST_ASSERT_EQ(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")->getImageIdentifier(), "iid2");

    isd.clearAllParams();
    isd.addParam("IMAGE_INDEX", "4");
    TEST_ASSERT_EQ(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")->getImageIdentifier(), "iid2");

    isd.clearAllParams();
    isd.addParam("IMAGE_INDEX", "5");
    TEST_SPECIFIC_EXCEPTION(plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL"), csm::Error);
}

void testCommon(std::string& testName, csm::RasterGM& model)
{
    // TODO: add checks for getValidImageRange() if we can identify what the
    // return should look like
    /*std::pair<csm::ImageCoord, csm::ImageCoord> vir =
            model->getValidImageRange();*/

    csm::ImageCoord ul = model.getImageStart();
    csm::ImageVector size = model.getImageSize();
    TEST_ASSERT_EQ(ul.line, 0);
    TEST_ASSERT_EQ(ul.samp, 0);
    TEST_ASSERT_EQ(size.line, 5);
    TEST_ASSERT_EQ(size.line, 5);

    // TODO: add more testing using other csm model methods
    /*csm::EcefCoord refpt = model->getReferencePoint();
    csm::ImageCoord ic = model->groundToImage(refpt);*/
}

TEST_CASE(testFromFilenameISD)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sidd = harness.find("cropped_sidd.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd(sidd.string());

    TEST_ASSERT(plugin.canModelBeConstructedFromISD(isd, "SIDD_SENSOR_MODEL"));

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")));

    testCommon(testName, *model);
}

TEST_CASE(testFromNitf21ISD)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sidd = harness.find("cropped_sidd.nitf");
    const csm::Plugin& plugin = harness.plugin();

    six::XMLControlRegistry xmlRegistry;
    six::NITFReadControl reader;
    std::unique_ptr<six::sidd::DerivedData> derivedData;

    // Read in the SIDD XML
    xmlRegistry.addCreator<six::sidd::DerivedXMLControl>();

    reader.setXMLControlRegistry(&xmlRegistry);

    reader.load(sidd.string(),
                std::vector<std::string>(1, harness.schemaPath()));
    derivedData.reset(static_cast<six::sidd::DerivedData*>(reader.getContainer()->getData(0)->clone()));

    auto isd =
            constructIsd(sidd.string(), reader, xmlRegistry);

    TEST_ASSERT(plugin.canModelBeConstructedFromISD(*isd, "SIDD_SENSOR_MODEL"));

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(*isd, "SIDD_SENSOR_MODEL")));

    testCommon(testName, *model);
}

TEST_CASE(testFromState)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sidd = harness.find("cropped_sidd.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sidd.string());

    TEST_ASSERT(plugin.canISDBeConvertedToModelState(isd, "SIDD_SENSOR_MODEL"));
    std::string state = plugin.convertISDToModelState(isd, "SIDD_SENSOR_MODEL");

    TEST_ASSERT_EQ(plugin.getModelNameFromModelState(state),
                   "SIDD_SENSOR_MODEL");
    TEST_ASSERT(
            plugin.canModelBeConstructedFromState("SIDD_SENSOR_MODEL", state));

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromState(state)));
}

void checkCovarianceMatrix(std::string& testName, csm::RasterGM& model)
{
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_NOT_EQ(model.getParameterCovariance(i, i), 0.);

    math::linear::MatrixMxN<7, 7> covar;
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            covar(i, j) = model.getParameterCovariance(i, j);

    double cond;
    bool posDefinite;
    matrixCondition(covar, cond, posDefinite);
    TEST_ASSERT(posDefinite);
}

void addCompositeSCP(six::sidd::DerivedData& derivedData)
{
    if (!derivedData.errorStatistics)
        derivedData.errorStatistics.reset(new six::ErrorStatistics());
    auto& errorStatistics = derivedData.errorStatistics;

    errorStatistics->compositeSCP.reset(new six::CompositeSCP());
    errorStatistics->compositeSCP->xErr = 1.0;
    errorStatistics->compositeSCP->yErr = 1.0;
    errorStatistics->compositeSCP->xyErr = 0.2;
}

void addComponents(six::sidd::DerivedData& derivedData,
                   six::FrameType frameType,
                   bool includeOffDiagonals)
{
    if (!derivedData.errorStatistics)
        derivedData.errorStatistics.reset(new six::ErrorStatistics());
    auto& errorStatistics = derivedData.errorStatistics;

    errorStatistics->components.reset(new six::Components);
    auto& components = errorStatistics->components;
    components->posVelError.reset(new six::PosVelError);
    components->posVelError->frame = frameType;
    components->posVelError->p1 = 1.1;
    components->posVelError->p2 = 1.2;
    components->posVelError->p3 = 1.3;
    components->posVelError->v1 = 0.11;
    components->posVelError->v2 = 0.12;
    components->posVelError->v3 = 0.13;

    if (includeOffDiagonals)
    {
        components->posVelError->corrCoefs.reset(new six::CorrCoefs());
        components->posVelError->corrCoefs->p1p2 = 0.12;
        components->posVelError->corrCoefs->p1p3 = 0.23;
        components->posVelError->corrCoefs->p1v1 = 0.34;
        components->posVelError->corrCoefs->p1v2 = 0.45;
        components->posVelError->corrCoefs->p1v3 = 0.56;
        components->posVelError->corrCoefs->p2p3 = 0.67;
        components->posVelError->corrCoefs->p2v1 = 0.78;
        components->posVelError->corrCoefs->p2v2 = 0.89;
        components->posVelError->corrCoefs->p2v3 = 0.13;
        components->posVelError->corrCoefs->p3v1 = 0.24;
        components->posVelError->corrCoefs->p3v2 = 0.35;
        components->posVelError->corrCoefs->p3v3 = 0.46;
        components->posVelError->corrCoefs->v1v2 = 0.57;
        components->posVelError->corrCoefs->v1v3 = 0.68;
        components->posVelError->corrCoefs->v2v3 = 0.79;
    }

    components->radarSensor.reset(new six::RadarSensor);
    components->radarSensor->rangeBias = 0.1;
}

void addUnmodeled(six::sidd::DerivedData& derivedData, bool includeDecorr)
{
    if (!derivedData.errorStatistics)
        derivedData.errorStatistics.reset(new six::ErrorStatistics());
    auto& errorStatistics = derivedData.errorStatistics;

    errorStatistics->unmodeled = six::Unmodeled();
    auto& unmodeled = value(errorStatistics->unmodeled);
    unmodeled.Xrow = 1.7;
    unmodeled.Ycol = 1.8;
    unmodeled.XrowYcol = -0.86;

    if (includeDecorr)
    {
        unmodeled.unmodeledDecorr = six::Unmodeled::Decorr();
        auto& decorr = value(unmodeled.unmodeledDecorr);
        value(decorr.Xrow).corrCoefZero = 1.0;
        value(decorr.Xrow).decorrRate = 0.0006;
        value(decorr.Ycol).corrCoefZero = 1.0;
        value(decorr.Ycol).decorrRate = 0.0011;
    }
}

TEST_CASE(testErrorStatistics1)
{
    TestHarness& harness = TestHarness::getInstance();

    auto derivedData = harness.fakeDerivedData("3.0.0");

    auto model = harness.modelFromDerived(derivedData);

    checkCovarianceMatrix(testName, *model);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model->getParameterCovariance(i, i),
                       (i < 3) ? 10.0 : 0.1);

    derivedData->errorStatistics.reset(new six::ErrorStatistics());

    // TODO: this test should be augmented with the many possible permutations
    // of optional metadata available in the ErrorStatistics block
    //addCompositeSCP(*derivedData);
    addComponents(*derivedData, six::FrameType::RIC_ECF, false);
    addUnmodeled(*derivedData, true);

    model = harness.modelFromDerived(derivedData);
    checkCovarianceMatrix(testName, *model);

    /*for (int i = 0; i < 7; i++)
        std::cout << "parameter " << i << ": "
                  << model->getParameterCovariance(i, i) << std::endl;*/

    csm::ImageCoord ic(5000.5, 5000.5);
    std::vector<double> ue = model->getUnmodeledError(ic);
    /*for (size_t i = 0; i < ue.size(); i++)
        std::cout << "ue: " << ue[i] << std::endl;*/

    csm::ImageCoord ic2(5100.5, 5100.5);
    std::vector<double> cue = model->getUnmodeledCrossCovariance(ic, ic2);
    /*for (size_t i = 0; i < cue.size(); i++)
        std::cout << "cue: " << cue[i] << std::endl;*/
    for (size_t i = 0; i < cue.size(); i++)
        TEST_ASSERT_LESSER(cue[i], ue[i]);
}

TEST_CASE(testErrorStatistics2)
{
    TestHarness& harness = TestHarness::getInstance();

    auto derivedData = harness.fakeDerivedData("3.0.0");

    addComponents(*derivedData, six::FrameType::ECF, true);
    auto model_ecf = harness.modelFromDerived(derivedData);
    checkCovarianceMatrix(testName, *model_ecf);

    addComponents(*derivedData, six::FrameType::RIC_ECF, true);
    auto model_ric_ecf = harness.modelFromDerived(derivedData);
    checkCovarianceMatrix(testName, *model_ric_ecf);

    addComponents(*derivedData, six::FrameType::RIC_ECI, true);
    auto model_ric_eci = harness.modelFromDerived(derivedData);
    checkCovarianceMatrix(testName, *model_ric_eci);

    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
        {
            TEST_ASSERT_EQ(model_ecf->getParameterCovariance(i, j),
                           model_ric_ecf->getParameterCovariance(i, j));
            TEST_ASSERT_EQ(model_ecf->getParameterCovariance(i, j),
                           model_ric_eci->getParameterCovariance(i, j));
            TEST_ASSERT_EQ(model_ric_ecf->getParameterCovariance(i, j),
                           model_ric_eci->getParameterCovariance(i, j));
        }
}

TEST_CASE(testModelState)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sidd = harness.find("cropped_sidd.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sidd.string());

    TEST_ASSERT(plugin.canISDBeConvertedToModelState(isd, "SIDD_SENSOR_MODEL"));
    std::string state = plugin.convertISDToModelState(isd, "SIDD_SENSOR_MODEL");

    TEST_ASSERT_EQ(plugin.getModelNameFromModelState(state),
                   "SIDD_SENSOR_MODEL");
    TEST_ASSERT(
            plugin.canModelBeConstructedFromState("SIDD_SENSOR_MODEL", state));

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromState(state)));

    TEST_ASSERT_EQ(model->getModelState(), state);

    std::unique_ptr<csm::RasterGM> model2(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")));
    TEST_ASSERT_EQ(model2->getModelState(), state);

    model2->replaceModelState(state);
    TEST_ASSERT_EQ(model2->getModelState(), state);
}

TEST_CASE(testAdjParamsState)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sidd = harness.find("cropped_sidd.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sidd.string());

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")));

    TEST_ASSERT_EQ(model->getNumParameters(), 7);
    for (int i = 0; i < 7; i++)
        model->setParameterValue(i, 11 * i);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model->getParameterValue(i), 11 * i);

    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            model->setParameterCovariance(i, j, i * 2.3 + j * 0.11);
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            TEST_ASSERT_EQ(model->getParameterCovariance(i, j),
                           i * 2.3 + j * 0.11);

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
            TEST_ASSERT_EQ(model2->getParameterCovariance(i, j),
                           i * 2.3 + j * 0.11);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model2->getParameterType(i), csm::param::NONE);
}

// Test imageToGround projections using modified adjustable parameters
TEST_CASE(testAdjParams1)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sidd = harness.find("cropped_sidd.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sidd.string());

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")));

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
    const auto sidd = harness.find("cropped_sidd.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sidd.string());

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")));

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
    const auto sidd = harness.find("cropped_sidd.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sidd.string());

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SIDD_SENSOR_MODEL")));

    model->setImageIdentifier("test identifier");
    TEST_ASSERT_EQ(model->getImageIdentifier(), "test identifier");

    std::unique_ptr<csm::RasterGM> model2(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromState(model->getModelState())));
    TEST_ASSERT_EQ(model2->getImageIdentifier(), "test identifier");
}

TEST_MAIN(TEST_CHECK(testPluginParams);
          TEST_CHECK(testIID1FormatCheck);
          TEST_CHECK(testImageIndexMapping);
          TEST_CHECK(testFromFilenameISD);
          TEST_CHECK(testFromNitf21ISD);
          TEST_CHECK(testFromState);
          TEST_CHECK(testErrorStatistics1);
          TEST_CHECK(testErrorStatistics2);
          TEST_CHECK(testModelState);
          TEST_CHECK(testAdjParamsState);
          TEST_CHECK(testAdjParams1);
          TEST_CHECK(testAdjParams2);
          TEST_CHECK(testImageIdentifier);)
