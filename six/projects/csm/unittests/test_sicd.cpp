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
#include <six/sicd/ComplexXMLControl.h>
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
        return sys::test::findGITModuleFile("croppedNitfs", "SICD", filename);
    }

    std::string fakeSicdXmlData(std::string sicdVersion)
    {
        // Fake sicd metadata that still should work through projection

        std::string sicdOpen =
                R"(<SICD xmlns="urn:SICD:)" + sicdVersion + R"(">)";
        std::string collectionInfo =
                R"(<CollectionInfo><CollectorName>FAKE</CollectorName><CoreName>FAKE_FAKE</CoreName><CollectType>MONOSTATIC</CollectType><RadarMode><ModeType>SPOTLIGHT</ModeType></RadarMode><Classification>UNCLASSIFIED</Classification></CollectionInfo>)";
        std::string imageData =
                R"(<ImageData><PixelType>RE32F_IM32F</PixelType><NumRows>10000</NumRows><NumCols>10000</NumCols><FirstRow>0</FirstRow><FirstCol>0</FirstCol><FullImage><NumRows>10000</NumRows><NumCols>10000</NumCols></FullImage><SCPPixel><Row>5000</Row><Col>5000</Col></SCPPixel></ImageData>)";
        std::string geoData =
                R"(<GeoData><EarthModel>WGS_84</EarthModel><SCP><ECF><X>6378137</X><Y>0</Y><Z>0</Z></ECF><LLH><Lat>0</Lat><Lon>0</Lon><HAE>0</HAE></LLH></SCP><ImageCorners><ICP index="1:FRFC"><Lat>0.01</Lat><Lon>-0.01</Lon></ICP><ICP index="2:FRLC"><Lat>0.01</Lat><Lon>0.01</Lon></ICP><ICP index="3:LRLC"><Lat>-0.01</Lat><Lon>0.01</Lon></ICP><ICP index="4:LRFC"><Lat>-0.01</Lat><Lon>-0.01</Lon></ICP></ImageCorners></GeoData>)";
        std::string grid =
                R"(<Grid><ImagePlane>SLANT</ImagePlane><Type>RGZERO</Type><TimeCOAPoly order1="0" order2="0"><Coef exponent1="0" exponent2="0">2.5</Coef></TimeCOAPoly><Row><UVectECF><X>-0.70710678</X><Y>0.70710678</Y><Z>0</Z></UVectECF><SS>0.8</SS><ImpRespWid>1.0</ImpRespWid><Sgn>-1</Sgn><ImpRespBW>1</ImpRespBW><KCtr>63.3</KCtr><DeltaK1>-0.5</DeltaK1><DeltaK2>0.5</DeltaK2></Row><Col><UVectECF><X>0</X><Y>0</Y><Z>1</Z></UVectECF><SS>0.8</SS><ImpRespWid>1</ImpRespWid><Sgn>-1</Sgn><ImpRespBW>1</ImpRespBW><KCtr>0</KCtr><DeltaK1>-0.5</DeltaK1><DeltaK2>0.5</DeltaK2></Col></Grid>)";
        std::string timeline =
                R"(<Timeline><CollectStart>2000-01-01T00:00:00Z</CollectStart><CollectDuration>5</CollectDuration></Timeline>)";
        std::string position =
                R"(<Position><ARPPoly><X order1="1"><Coef exponent1="0">6383137</Coef><Coef exponent1="1">0</Coef></X><Y order1="1"><Coef exponent1="0">-5000</Coef><Coef exponent1="1">0</Coef></Y><Z order1="1"><Coef exponent1="0">-2500</Coef><Coef exponent1="1">1000</Coef></Z></ARPPoly></Position>)";
        std::string radarCollection =
                R"(<RadarCollection><TxFrequency><Min>9E9</Min><Max>10E9</Max></TxFrequency><TxPolarization>H</TxPolarization><RcvChannels size="1"><ChanParameters index="1"><TxRcvPolarization>H:H</TxRcvPolarization></ChanParameters></RcvChannels></RadarCollection>)";
        std::string imageFormation =
                R"(<ImageFormation><RcvChanProc><NumChanProc>1</NumChanProc><ChanIndex>1</ChanIndex></RcvChanProc><TxRcvPolarizationProc>H:H</TxRcvPolarizationProc><TStartProc>0</TStartProc><TEndProc>5</TEndProc><TxFrequencyProc><MinProc>9E9</MinProc><MaxProc>10E9</MaxProc></TxFrequencyProc><ImageFormAlgo>OTHER</ImageFormAlgo><STBeamComp>SV</STBeamComp><ImageBeamComp>NO</ImageBeamComp><AzAutofocus>NO</AzAutofocus><RgAutofocus>NO</RgAutofocus></ImageFormation>)";
        std::string scpCoa =
                R"(<SCPCOA><SCPTime>2.5</SCPTime><ARPPos><X>6383137</X><Y>-5000</Y><Z>0</Z></ARPPos><ARPVel><X>0</X><Y>0</Y><Z>1000</Z></ARPVel><ARPAcc><X>0</X><Y>0</Y><Z>0</Z></ARPAcc><SideOfTrack>R</SideOfTrack><SlantRange>7071</SlantRange><GroundRange>5000</GroundRange><DopplerConeAng>90</DopplerConeAng><GrazeAng>45</GrazeAng><IncidenceAng>45</IncidenceAng><TwistAng>0</TwistAng><SlopeAng>45</SlopeAng><AzimAng>90</AzimAng><LayoverAng>90</LayoverAng></SCPCOA>)";
        std::string rma =
                R"(<RMA><RMAlgoType>OMEGA_K</RMAlgoType><ImageType>INCA</ImageType><INCA><TimeCAPoly order1="1"><Coef exponent1="0">2.5</Coef><Coef exponent1="1">0.001</Coef></TimeCAPoly><R_CA_SCP>7071</R_CA_SCP><FreqZero>9.5E9</FreqZero><DRateSFPoly order1="0" order2="0"><Coef exponent1="0" exponent2="0">1</Coef></DRateSFPoly></INCA></RMA>)";
        std::string sicdClose = R"(</SICD>)";

        return sicdOpen + collectionInfo + imageData + geoData + grid +
                timeline + position + radarCollection + imageFormation +
                scpCoa + rma + sicdClose;
    }

    std::unique_ptr<six::sicd::ComplexData> fakeComplexData(
            std::string sicdVersion)
    {
        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator<six::sicd::ComplexXMLControl>();

        std::string xml = fakeSicdXmlData(sicdVersion);

        logging::NullLogger logger;
        auto data = six::parseDataFromString(xmlRegistry,
                                             xml,
                                             six::DataType::COMPLEX,
                                             {schemaPath()},
                                             logger);

        return std::unique_ptr<six::sicd::ComplexData>(
                reinterpret_cast<six::sicd::ComplexData*>(data.release()));
    }

    std::unique_ptr<csm::RasterGM> modelFromComplex(
            std::unique_ptr<six::sicd::ComplexData>& complexData)
    {
        // It only needs to pass inspection as a SICD.  The version numbers
        // specified here are not used within the plugins.
        std::string desHeader =
                R"(DEXML_DATA_CONTENT         01UUS                                                                                      )"
                R"(                                                                              077399999XML     2000-01-01T00:00:00Z   )"
                R"(                                     SICD Volume 1 Design & Implementation Description Document  1.2       2016-04-12T)"
                R"(00:00:00Zurn:SICD:1.2.0                                                                                               )"
                R"(           +00.00000000+000.00000000+00.00000000+000.00000000+00.00000000+000.00000000+00.00000000+000.00000000+00.000)"
                R"(00000+000.00000000                                                                                                    )"
                R"(                                                                                                                      )"
                R"(                                                                                                                      )"
                R"(                             )";

        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator<six::sicd::ComplexXMLControl>();

        csm::Des des;
        des.setSubHeader(desHeader);
        des.setData(str::to_native(
                six::toXMLString(complexData.get(), &xmlRegistry)));

        csm::Nitf21Isd isd;
        isd.addFileDes(des);

        std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
                mPlugin->constructModelFromISD(isd, "SICD_SENSOR_MODEL")));
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

        mSchemaPath = sicdRootSchemaDir.string();

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
    TEST_ASSERT_EQ(plugin.getNumModels(), 2);

    TEST_ASSERT_EQ(plugin.getModelName(0), "SICD_SENSOR_MODEL");
    TEST_ASSERT_EQ(plugin.getModelFamily(0), "GeometricRaster");

    TEST_ASSERT_EQ(plugin.getModelName(1), "SIDD_SENSOR_MODEL");
    TEST_ASSERT_EQ(plugin.getModelFamily(1), "GeometricRaster");
}

void testCommon(std::string& testName, csm::RasterGM& model)
{
    // TODO: add checks for getValidImageRange() if we can identify what the
    // return should look like
    /*std::pair<csm::ImageCoord, csm::ImageCoord> vir =
            model->getValidImageRange();*/

    csm::ImageCoord ul = model.getImageStart();
    csm::ImageVector size = model.getImageSize();
    TEST_ASSERT_EQ(ul.line, 100);
    TEST_ASSERT_EQ(ul.samp, 100);
    TEST_ASSERT_EQ(size.line, 5);
    TEST_ASSERT_EQ(size.line, 5);

    // TODO: add more testing using other csm model methods
    /*csm::EcefCoord refpt = model->getReferencePoint();
    csm::ImageCoord ic = model->groundToImage(refpt);*/
}

TEST_CASE(testFromFilenameISD)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd(sicd.string());

    TEST_ASSERT(plugin.canModelBeConstructedFromISD(isd, "SICD_SENSOR_MODEL"));

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SICD_SENSOR_MODEL")));

    testCommon(testName, *model);
}

TEST_CASE(testFromNitf21ISD)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    six::XMLControlRegistry xmlRegistry;
    six::NITFReadControl reader;
    std::unique_ptr<six::sicd::ComplexData> complexData;

    // Read in the SICD XML
    xmlRegistry.addCreator<six::sicd::ComplexXMLControl>();

    reader.setXMLControlRegistry(&xmlRegistry);

    reader.load(sicd.string(),
                std::vector<std::string>(1, harness.schemaPath()));
    complexData = six::sicd::Utilities::getComplexData(reader);

    auto isd =
            constructIsd(sicd.string(), reader, complexData.get(), xmlRegistry);

    TEST_ASSERT(plugin.canModelBeConstructedFromISD(*isd, "SICD_SENSOR_MODEL"));

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(*isd, "SICD_SENSOR_MODEL")));

    testCommon(testName, *model);
}

TEST_CASE(testFromState)
{
    TestHarness& harness = TestHarness::getInstance();
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sicd.string());

    TEST_ASSERT(plugin.canISDBeConvertedToModelState(isd, "SICD_SENSOR_MODEL"));
    std::string state = plugin.convertISDToModelState(isd, "SICD_SENSOR_MODEL");

    TEST_ASSERT_EQ(plugin.getModelNameFromModelState(state),
                   "SICD_SENSOR_MODEL");
    TEST_ASSERT(
            plugin.canModelBeConstructedFromState("SICD_SENSOR_MODEL", state));

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

void addCompositeSCP(six::sicd::ComplexData& complexData)
{
    if (!complexData.errorStatistics)
        complexData.errorStatistics.reset(new six::ErrorStatistics());
    auto& errorStatistics = complexData.errorStatistics;

    errorStatistics->compositeSCP.reset(new six::CompositeSCP());
    errorStatistics->compositeSCP->xErr = 1.0;
    errorStatistics->compositeSCP->yErr = 1.0;
    errorStatistics->compositeSCP->xyErr = 0.2;
}

void addComponents(six::sicd::ComplexData& complexData,
                   six::FrameType frameType,
                   bool includeOffDiagonals)
{
    if (!complexData.errorStatistics)
        complexData.errorStatistics.reset(new six::ErrorStatistics());
    auto& errorStatistics = complexData.errorStatistics;

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

void addUnmodeled(six::sicd::ComplexData& complexData, bool includeDecorr)
{
    if (!complexData.errorStatistics)
        complexData.errorStatistics.reset(new six::ErrorStatistics());
    auto& errorStatistics = complexData.errorStatistics;

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

    auto complexData = harness.fakeComplexData("1.3.0");

    auto model = harness.modelFromComplex(complexData);

    checkCovarianceMatrix(testName, *model);
    for (int i = 0; i < 7; i++)
        TEST_ASSERT_EQ(model->getParameterCovariance(i, i),
                       (i < 3) ? 10.0 : 0.1);

    complexData->errorStatistics.reset(new six::ErrorStatistics());

    // TODO: this test should be augmented with the many possible permutations
    // of optional metadata available in the ErrorStatistics block
    //addCompositeSCP(*complexData);
    addComponents(*complexData, six::FrameType::RIC_ECF, false);
    addUnmodeled(*complexData, true);

    model = harness.modelFromComplex(complexData);
    checkCovarianceMatrix(testName, *model);

    for (int i = 0; i < 7; i++)
        std::cout << "parameter " << i << ": "
                  << model->getParameterCovariance(i, i) << std::endl;

    csm::ImageCoord ic(5000.5, 5000.5);
    std::vector<double> ue = model->getUnmodeledError(ic);
    for (int i = 0; i < ue.size(); i++)
        std::cout << "ue: " << ue[i] << std::endl;

    csm::ImageCoord ic2(5100.5, 5100.5);
    std::vector<double> cue = model->getUnmodeledCrossCovariance(ic, ic2);
    for (int i = 0; i < cue.size(); i++)
        std::cout << "cue: " << cue[i] << std::endl;
    for (int i = 0; i < cue.size(); i++)
        TEST_ASSERT_LESSER(cue[i], ue[i]);
}

TEST_CASE(testErrorStatistics2)
{
    TestHarness& harness = TestHarness::getInstance();

    auto complexData = harness.fakeComplexData("1.3.0");

    addComponents(*complexData, six::FrameType::ECF, true);
    auto model_ecf = harness.modelFromComplex(complexData);
    checkCovarianceMatrix(testName, *model_ecf);

    addComponents(*complexData, six::FrameType::RIC_ECF, true);
    auto model_ric_ecf = harness.modelFromComplex(complexData);
    checkCovarianceMatrix(testName, *model_ric_ecf);

    addComponents(*complexData, six::FrameType::RIC_ECI, true);
    auto model_ric_eci = harness.modelFromComplex(complexData);
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
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sicd.string());

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

    csm::Isd isd = csm::Isd(sicd.string());

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SICD_SENSOR_MODEL")));

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
    const auto sicd = harness.find("cropped_sicd_120.nitf");
    const csm::Plugin& plugin = harness.plugin();

    csm::Isd isd = csm::Isd(sicd.string());

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

    csm::Isd isd = csm::Isd(sicd.string());

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

    csm::Isd isd = csm::Isd(sicd.string());

    std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromISD(isd, "SICD_SENSOR_MODEL")));

    model->setImageIdentifier("test identifier");
    TEST_ASSERT_EQ(model->getImageIdentifier(), "test identifier");

    std::unique_ptr<csm::RasterGM> model2(reinterpret_cast<csm::RasterGM*>(
            plugin.constructModelFromState(model->getModelState())));
    TEST_ASSERT_EQ(model2->getImageIdentifier(), "test identifier");
}

TEST_MAIN(TEST_CHECK(testPluginParams); TEST_CHECK(testFromFilenameISD);
          TEST_CHECK(testFromNitf21ISD);
          TEST_CHECK(testFromState);
          TEST_CHECK(testErrorStatistics1);
          TEST_CHECK(testErrorStatistics2);
          TEST_CHECK(testModelState);
          TEST_CHECK(testAdjParamsState);
          TEST_CHECK(testAdjParams1);
          TEST_CHECK(testAdjParams2);
          TEST_CHECK(testImageIdentifier);)
