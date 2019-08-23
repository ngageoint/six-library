#include <iostream>
#include <fstream>
#include <memory>

#include <cphd/CPHDXMLControl.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/SceneCoordinates.h>
#include <io/MockSeekableInputStream.h>
#include <xml/lite/MinidomParser.h>
#include "TestCase.h"

#  define TEST_CASE_ARGUMENTS(X) void X(std::string testName, int argc, char** argv)
#  define TEST_CHECK_ARGUMENTS(X) try{ X(std::string(#X),argc,argv); std::cerr << #X << ": PASSED" << std::endl; } catch(except::Throwable& ex) { die_printf("%s: FAILED: Exception thrown: %s\n", std::string(#X).c_str(), ex.toString().c_str()); }

std::string getPathname(int argc, char** argv)
{
    // Parse the command line
    if (argc != 2)
    {
        throw except::Exception(Ctxt(
                    "Invalid number of cmd line args"));
    }
    else
    {
        return argv[1];
    }
}

void parseXMLFile(xml::lite::MinidomParser& xmlParser, std::string pathname)
{
    // Parse XML file
    io::FileInputStream ifs(pathname);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(ifs, ifs.available());
}


TEST_CASE_ARGUMENTS(testOptional)
{
    // Get pathname from cmd line
    std::string pathname = "/data1/u/vyadav/six-library/six/modules/c++/cphd/unittests/sample_xml/cphd02.xml";
    // std::string schema = "/data1/u/vyadav/six-library/six/modules/c++/cphd/conf/schema/CPHD_schema_V1.0.0_2017_10_20.xsd";
    std::string schema = "";

    xml::lite::MinidomParser xmlParser;
    parseXMLFile(xmlParser, pathname);


    cphd::CPHDXMLControl xmlControl;
    std::vector<std::string> schemaPaths;
    if (schema.length())
    {
        schemaPaths.push_back(schema);
        xmlControl.setSchemaPaths(schemaPaths);
    }

    // Select specific nodes to be populated
    std::vector<std::string> nodeNames;
    nodeNames.push_back("SupportArray");
    nodeNames.push_back("Antenna");
    nodeNames.push_back("TxRcv");
    nodeNames.push_back("ErrorParameters");
    nodeNames.push_back("ProductInfo");
    nodeNames.push_back("GeoInfo");
    nodeNames.push_back("MatchInfo");


    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata =
            xmlControl.fromXML(xmlParser.getDocument(), nodeNames);

    const cphd::SupportArray& supportArray = metadata->supportArray;
    TEST_ASSERT_EQ(supportArray.iazArray.size(), 2);
    TEST_ASSERT_EQ(supportArray.iazArray[0].getIdentifier(), 1);
    TEST_ASSERT_EQ(supportArray.iazArray[0].elementFormat, "IAZ=F4;");
    TEST_ASSERT_EQ(supportArray.iazArray[0].x0, 0.0);
    TEST_ASSERT_EQ(supportArray.iazArray[0].y0, 0.0);
    TEST_ASSERT_EQ(supportArray.iazArray[0].xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.iazArray[0].ySS, 5.0);
    TEST_ASSERT_EQ(supportArray.iazArray[1].getIdentifier(), 2);
    TEST_ASSERT_EQ(supportArray.iazArray[1].elementFormat, "IAZ=F4;");
    TEST_ASSERT_EQ(supportArray.iazArray[1].x0, 0.0);
    TEST_ASSERT_EQ(supportArray.iazArray[1].y0, 0.0);
    TEST_ASSERT_EQ(supportArray.iazArray[1].xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.iazArray[1].ySS, 5.0);

    TEST_ASSERT_EQ(supportArray.antGainPhase.size(), 1);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].getIdentifier(), 1);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].elementFormat, "Gain=F4;Phase=F4;");
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].x0, 0.0);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].y0, 0.0);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].ySS, 5.0);

    TEST_ASSERT_EQ(supportArray.addedSupportArray.size(), 1);
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].identifier, "AddedSupportArray");
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].elementFormat, "F4");
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].x0, 0.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].y0, 0.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].ySS, 5.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].xUnits, "XUnits");
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].yUnits, "YUnits");
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].zUnits, "ZUnits");
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].parameter.size(), 2);
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].parameter[0].getName(), "Parameter1");
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].parameter[1].getName(), "Parameter2");
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].parameter[0].str(), "Additional parameter");
    TEST_ASSERT_EQ(supportArray.addedSupportArray[0].parameter[1].str(), "Additional parameter");

    const cphd::Antenna& antenna = metadata->antenna;
    TEST_ASSERT_EQ(antenna.numACFs, 2);
    TEST_ASSERT_EQ(antenna.numAPCs, 1);
    TEST_ASSERT_EQ(antenna.numAntPats, 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame.size(), 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].identifier, "ACF1");
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.order(), 3);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[0][0], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[1][0], 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[2][0], 3);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[3][0], 0);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[0][1], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[1][1], 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[2][1], 3);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[3][1], 4);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[0][2], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[1][2], 0);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[2][2], 3);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[3][2], 0);

    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.order(), 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.coeffs()[0][0], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.coeffs()[1][0], 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.coeffs()[0][1], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.coeffs()[1][1], 2);

    TEST_ASSERT_EQ(antenna.antCoordFrame[1].identifier, "ACF2");
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.order(), 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.coeffs()[0][0], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.coeffs()[1][0], 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.coeffs()[0][1], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.coeffs()[1][1], 2);

    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.order(), 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.coeffs()[0][0], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.coeffs()[1][0], 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.coeffs()[0][1], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.coeffs()[1][1], 2);

    TEST_ASSERT_EQ(antenna.antPhaseCenter.size(), 1);
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].identifier, "APC");
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].acfId, "ACF1");
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].apcXYZ[0], 5.0);
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].apcXYZ[1], 5.0);
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].apcXYZ[2], 5.0);

    TEST_ASSERT_EQ(antenna.antPhaseCenter.size(), 1);
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].identifier, "APC");
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].acfId, "ACF1");
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].apcXYZ[0], 5.0);
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].apcXYZ[1], 5.0);
    TEST_ASSERT_EQ(antenna.antPhaseCenter[0].apcXYZ[2], 5.0);

    TEST_ASSERT_EQ(antenna.antPattern.size(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].identifier, "APAT");
    TEST_ASSERT_EQ(antenna.antPattern[0].freqZero, 2.3);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainZero, 2.3);
    TEST_ASSERT_EQ(antenna.antPattern[0].ebFreqShift, 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].mlFreqDilation, 0);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainBSPoly.order(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainBSPoly.coeffs()[0], 1.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainBSPoly.coeffs()[1], 2.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcXPoly.order(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcXPoly.coeffs()[0], 5.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcYPoly.order(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcYPoly.coeffs()[0], 0.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcYPoly.coeffs()[1], 5.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].array.gainPoly.orderX(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].array.gainPoly.orderY(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].array.phasePoly.orderX(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].array.phasePoly.orderY(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].element.gainPoly.orderX(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].element.gainPoly.orderY(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].element.phasePoly.orderX(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].element.phasePoly.orderY(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[0].freq, 2.3);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[0].arrayId, "Parameter1");
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[0].elementId, "Parameter2");
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[1].freq, 2.8);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[1].arrayId, "Parameter1");
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[1].elementId, "Parameter2");

    const cphd::TxRcv& txRcv = metadata->txRcv;
    TEST_ASSERT_EQ(txRcv.numTxWFs, 1);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].identifier, "TxWFParam");
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].pulseLength, 3.0);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].rfBandwidth, 2.3);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].freqCenter, 1.8);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].lfmRate, 1.0);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].polarization.toString(), "LHC");
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].power, 5.0);

    TEST_ASSERT_EQ(txRcv.numRcvs, 2);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].identifier, "RcvParam1");
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].windowLength, 3.0);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].sampleRate, 2.3);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].ifFilterBW, 2.3);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].freqCenter, 1.8);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].lfmRate, 1.0);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].polarization.toString(), "LHC");
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].pathGain, 5.0);

    TEST_ASSERT_EQ(txRcv.rcvParameters[1].identifier, "RcvParam2");
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].windowLength, 3.0);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].sampleRate, 2.3);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].ifFilterBW, 2.3);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].freqCenter, 1.8);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].lfmRate, 1.0);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].polarization.toString(), "LHC");
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].pathGain, 5.0);

    const cphd::ErrorParameters& errorParams = metadata->errorParameters;
    TEST_ASSERT_EQ(errorParams.monostatic.posVelErr.frame.toString(), "ECF");
    TEST_ASSERT_EQ(errorParams.monostatic.posVelErr.p1, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic.posVelErr.p2, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic.posVelErr.corrCoefs.p1p2, 0.8);
    TEST_ASSERT_EQ(errorParams.monostatic.posVelErr.corrCoefs.v2v3, 0.8);
    TEST_ASSERT_EQ(errorParams.monostatic.posVelErr.positionDecorr.corrCoefZero, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic.posVelErr.positionDecorr.decorrRate, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic.radarSensor.rangeBias, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic.radarSensor.clockFreqSF, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic.radarSensor.collectionStartTime, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic.radarSensor.rangeBiasDecorr.corrCoefZero, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic.radarSensor.rangeBiasDecorr.decorrRate, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic.tropoError.tropoRangeVertical, 5.0);
    TEST_ASSERT_EQ(errorParams.monostatic.tropoError.tropoRangeSlant, 5.0);
    TEST_ASSERT_EQ(errorParams.monostatic.tropoError.tropoRangeDecorr.corrCoefZero, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic.tropoError.tropoRangeDecorr.decorrRate, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic.ionoError.ionoRangeVertical, 5.0);
    TEST_ASSERT_EQ(errorParams.monostatic.ionoError.ionoRangeRateVertical, 5.0);
    TEST_ASSERT_EQ(errorParams.monostatic.ionoError.ionoRgRgRateCC, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic.ionoError.ionoRangeVertDecorr.corrCoefZero, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic.ionoError.ionoRangeVertDecorr.decorrRate, 1.0);

    const cphd::ProductInfo& productInfo = metadata->productInfo;
    TEST_ASSERT_EQ(productInfo.profile, "Profile");
    TEST_ASSERT_EQ(productInfo.creationInfo[0].application, "Application");
    TEST_ASSERT_EQ(productInfo.creationInfo[0].dateTime.getYear(), 2014);
    TEST_ASSERT_EQ(productInfo.creationInfo[0].site, "Area51");
    TEST_ASSERT_EQ(productInfo.creationInfo[0].parameter[0].getName(), "Param1");
    TEST_ASSERT_EQ(productInfo.creationInfo[0].parameter[1].getName(), "Param2");
    TEST_ASSERT_EQ(productInfo.parameter[0].getName(), "Param1");

    std::vector<cphd::GeoInfo> geoInfo = metadata->geoInfo;
    TEST_ASSERT_EQ(geoInfo[0].getName(), "Airport");
    TEST_ASSERT_EQ(geoInfo[0].desc[0].getName(), "Airport ID");
    TEST_ASSERT_EQ(geoInfo[0].desc[0].str(), "51");
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].getName(), "Perimeter");
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].numVertices, 4);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[0].index, 1);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[0].getLat(), 0.0);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[0].getLon(), 0.0);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[1].index, 2);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[1].getLat(), 0.1);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[1].getLon(), 0.0);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[2].index, 3);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[2].getLat(), 0.1);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[2].getLon(), 0.1);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[3].index, 4);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[3].getLat(), 0.0);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[0].polygon[0].vertex[3].getLon(), 0.0);

    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].getName(), "Runway");
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].desc[0].getName(), "ID");
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].desc[0].str(), "04/22");
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].line[0].numEndpoints, 2);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].line[0].endpoint[0].index, 1);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].line[0].endpoint[0].getLat(), 0.02);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].line[0].endpoint[0].getLon(), 0.03);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].line[0].endpoint[1].index, 2);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].line[0].endpoint[1].getLat(), 0.08);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[1].line[0].endpoint[1].getLon(), 0.08);

    TEST_ASSERT_EQ(geoInfo[0].geoInfo[3].getName(), "Control Tower");
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[3].desc[0].getName(), "ID");
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[3].desc[0].str(), "Main");
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[3].point[0].getLat(), 0.6);
    TEST_ASSERT_EQ(geoInfo[0].geoInfo[3].point[0].getLon(), 0.4);

    TEST_ASSERT_EQ(geoInfo[1].getName(), "Farm");
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].numVertices, 5);
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].vertex[0].index, 1);
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].vertex[0].getLat(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].vertex[0].getLon(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].vertex[1].index, 2);
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].vertex[1].getLat(), 1.1);
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].vertex[1].getLon(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].vertex[4].index, 5);
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].vertex[4].getLat(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].polygon[0].vertex[4].getLon(), 1.0);

    const cphd::MatchInfo& matchInfo = metadata->matchInfo;
    TEST_ASSERT_EQ(matchInfo.numMatchTypes, 2);
    TEST_ASSERT_EQ(matchInfo.matchType[0].index, 1);
    TEST_ASSERT_EQ(matchInfo.matchType[0].typeID, "STEREO");
    TEST_ASSERT_EQ(matchInfo.matchType[0].currentIndex, 1);
    TEST_ASSERT_EQ(matchInfo.matchType[0].numMatchCollections, 1);
    TEST_ASSERT_EQ(matchInfo.matchType[0].matchCollection[0].index, 1);
    TEST_ASSERT_EQ(matchInfo.matchType[0].matchCollection[0].coreName, "CollectionName");
    TEST_ASSERT_EQ(matchInfo.matchType[0].matchCollection[0].matchIndex, 1);
    TEST_ASSERT_EQ(matchInfo.matchType[0].matchCollection[0].parameter[0].getName(), "param1");
    TEST_ASSERT_EQ(matchInfo.matchType[1].index, 2);
    TEST_ASSERT_EQ(matchInfo.matchType[1].typeID, "COHERENT");
    TEST_ASSERT_EQ(matchInfo.matchType[1].currentIndex, 1);
    TEST_ASSERT_EQ(matchInfo.matchType[1].numMatchCollections, 1);
    TEST_ASSERT_EQ(matchInfo.matchType[1].matchCollection[0].index, 2);
    TEST_ASSERT_EQ(matchInfo.matchType[1].matchCollection[0].coreName, "CollectionName");
    TEST_ASSERT_EQ(matchInfo.matchType[1].matchCollection[0].matchIndex, 1);
    TEST_ASSERT_EQ(matchInfo.matchType[1].matchCollection[0].parameter[0].getName(), "param1");



}

int main(int argc, char** argv)
{
    try
    {
        TEST_CHECK_ARGUMENTS(testOptional);
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
