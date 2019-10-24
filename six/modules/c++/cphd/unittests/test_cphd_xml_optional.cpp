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
#include <fstream>
#include <memory>
#include <logging/NullLogger.h>


#include <cphd/CPHDXMLControl.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/SceneCoordinates.h>
#include <io/FileInputStream.h>
#include <xml/lite/MinidomParser.h>
#include "TestCase.h"

void parseXMLFile(xml::lite::MinidomParser& xmlParser, std::string pathname)
{
    // Parse XML file
    io::FileInputStream ifs(pathname);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(ifs, ifs.available());
}

TEST_CASE(testOptional)
{
    // Get pathname from cmd line
    std::string pathname = "/data1/u/vyadav/six-library/six/modules/c++/cphd/unittests/sample_xml/cphd02.xml";
    std::string schema = "/data1/u/vyadav/six-library/six/modules/c++/cphd/conf/schema/CPHD_schema_V1.0.0_2017_10_20.xsd";

    xml::lite::MinidomParser xmlParser;
    parseXMLFile(xmlParser, pathname);


    std::vector<std::string> schemaPaths;
    if (schema.length())
    {
        schemaPaths.push_back(schema);
    }

    //CPHDXMLControl object with no logger
    cphd::CPHDXMLControl xmlControl(new logging::NullLogger(), true, schemaPaths);

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
            xmlControl.fromXML(xmlParser.getDocument());

    const cphd::SupportArray& supportArray = *(metadata->supportArray);
    TEST_ASSERT_EQ(supportArray.iazArray.size(), 1);
    TEST_ASSERT_EQ(supportArray.iazArray[0].getIdentifier(), 1);
    TEST_ASSERT_EQ(supportArray.iazArray[0].elementFormat, "IAZ=F4;");
    TEST_ASSERT_EQ(supportArray.iazArray[0].x0, 0.0);
    TEST_ASSERT_EQ(supportArray.iazArray[0].y0, 0.0);
    TEST_ASSERT_EQ(supportArray.iazArray[0].xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.iazArray[0].ySS, 5.0);

    TEST_ASSERT_EQ(supportArray.antGainPhase.size(), 1);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].getIdentifier(), 2);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].elementFormat, "Gain=F4;Phase=F4;");
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].x0, 0.0);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].y0, 0.0);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].ySS, 5.0);

    TEST_ASSERT_EQ(supportArray.addedSupportArray.size(), 1);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.count("AddedSupportArray"), 1);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.elementFormat, "F4");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.x0, 0.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.y0, 0.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.ySS, 5.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.xUnits, "XUnits");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.yUnits, "YUnits");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.zUnits, "ZUnits");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter.size(), 2);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter[0].getName(), "Parameter1");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter[1].getName(), "Parameter2");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter[0].str(), "Additional parameter");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter[1].str(), "Additional parameter");

    const cphd::Antenna& antenna = *(metadata->antenna);
    TEST_ASSERT_EQ(antenna.antCoordFrame.size(), 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].identifier, "ACF1");
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.order(), 3);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[0][0], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[1][0], 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[2][0], 3);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.coeffs()[3][0], 4);
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
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcxPoly.order(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcxPoly.coeffs()[0], 5.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcyPoly.order(), 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcyPoly.coeffs()[0], 0.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcyPoly.coeffs()[1], 5.0);
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

    const cphd::TxRcv& txRcv = *(metadata->txRcv);
    TEST_ASSERT_EQ(txRcv.txWFParameters.size(), 1);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].identifier, "TxWFParam");
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].pulseLength, 3.0);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].rfBandwidth, 2.3);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].freqCenter, 1.8);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].lfmRate, 1.0);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].polarization.toString(), "LHC");
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].power, 5.0);

    TEST_ASSERT_EQ(txRcv.rcvParameters.size(), 2);
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

    const cphd::ErrorParameters& errorParams = *(metadata->errorParameters);
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.frame.toString(), "ECF");
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.p1, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.p2, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.corrCoefs->p1p2, 0.8);
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.corrCoefs->v2v3, 0.8);
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.positionDecorr.corrCoefZero, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.positionDecorr.decorrRate, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.rangeBias, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.clockFreqSF, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.collectionStartTime, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.rangeBiasDecorr->corrCoefZero, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.rangeBiasDecorr->decorrRate, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->tropoError->tropoRangeVertical, 5.0);
    TEST_ASSERT_EQ(errorParams.monostatic->tropoError->tropoRangeSlant, 5.0);
    TEST_ASSERT_EQ(errorParams.monostatic->tropoError->tropoRangeDecorr.corrCoefZero, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic->tropoError->tropoRangeDecorr.decorrRate, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->ionoError->ionoRangeVertical, 5.0);
    TEST_ASSERT_EQ(errorParams.monostatic->ionoError->ionoRangeRateVertical, 5.0);
    TEST_ASSERT_EQ(errorParams.monostatic->ionoError->ionoRgRgRateCC, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic->ionoError->ionoRangeVertDecorr.corrCoefZero, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic->ionoError->ionoRangeVertDecorr.decorrRate, 1.0);

    const cphd::ProductInfo& productInfo = *(metadata->productInfo);
    TEST_ASSERT_EQ(productInfo.profile, "Profile");
    TEST_ASSERT_EQ(productInfo.creationInfo[0].application, "Application");
    TEST_ASSERT_EQ(productInfo.creationInfo[0].dateTime.getYear(), 2014);
    TEST_ASSERT_EQ(productInfo.creationInfo[0].site, "Area51");
    TEST_ASSERT_EQ(productInfo.creationInfo[0].parameter[0].getName(), "Param1");
    TEST_ASSERT_EQ(productInfo.creationInfo[0].parameter[1].getName(), "Param2");
    TEST_ASSERT_EQ(productInfo.parameter[0].getName(), "Param1");

    std::vector<cphd::GeoInfo> geoInfo = metadata->geoInfo;
    TEST_ASSERT_EQ(geoInfo[0].name, "Airport");
    TEST_ASSERT_EQ(geoInfo[0].desc[0].getName(), "Airport ID");
    TEST_ASSERT_EQ(geoInfo[0].desc[0].str(), "51");
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->name, "Perimeter");
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon.size(), 4);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon[0].getLat(), 0.0);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon[0].getLon(), 0.0);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon[1].getLat(), 0.1);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon[1].getLon(), 0.0);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon[2].getLat(), 0.1);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon[2].getLon(), 0.1);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon[3].getLat(), 0.0);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon[3].getLon(), 0.0);

    TEST_ASSERT_EQ(geoInfo[0].geoInfos[1]->name, "Runway");
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[1]->desc[0].getName(), "ID");
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[1]->desc[0].str(), "04/22");
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[1]->geometryLatLon.size(), 2);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[1]->geometryLatLon[0].getLat(), 0.02);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[1]->geometryLatLon[0].getLon(), 0.03);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[1]->geometryLatLon[1].getLat(), 0.08);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[1]->geometryLatLon[1].getLon(), 0.08);

    TEST_ASSERT_EQ(geoInfo[0].geoInfos[3]->name, "Control Tower");
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[3]->desc[0].getName(), "ID");
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[3]->desc[0].str(), "Main");
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[3]->geometryLatLon[0].getLat(), 0.6);
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[3]->geometryLatLon[0].getLon(), 0.4);

    TEST_ASSERT_EQ(geoInfo[1].name, "Farm");
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon.size(), 5);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[0].getLat(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[0].getLon(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[1].getLat(), 1.1);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[1].getLon(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[4].getLat(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[4].getLon(), 1.0);

    const cphd::MatchInformation& matchInfo = *(metadata->matchInfo);
    TEST_ASSERT_EQ(matchInfo.types.size(), 2);
    TEST_ASSERT_EQ(matchInfo.types[0]->typeID, "STEREO");
    TEST_ASSERT_EQ(matchInfo.types[0]->currentIndex, 1);
    TEST_ASSERT_EQ(matchInfo.types[0]->matchCollects.size(), 1);
    TEST_ASSERT_EQ(matchInfo.types[0]->matchCollects[0].coreName, "CollectionName");
    TEST_ASSERT_EQ(matchInfo.types[0]->matchCollects[0].matchIndex, 1);
    TEST_ASSERT_EQ(matchInfo.types[0]->matchCollects[0].parameters[0].getName(), "param1");
    TEST_ASSERT_EQ(matchInfo.types[1]->typeID, "COHERENT");
    TEST_ASSERT_EQ(matchInfo.types[1]->currentIndex, 1);
    TEST_ASSERT_EQ(matchInfo.types[1]->matchCollects.size(), 1);
    TEST_ASSERT_EQ(matchInfo.types[1]->matchCollects[0].coreName, "CollectionName");
    TEST_ASSERT_EQ(matchInfo.types[1]->matchCollects[0].matchIndex, 1);
    TEST_ASSERT_EQ(matchInfo.types[1]->matchCollects[0].parameters[0].getName(), "param1");
}

int main()
{
    try
    {
        TEST_CHECK(testOptional);
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
