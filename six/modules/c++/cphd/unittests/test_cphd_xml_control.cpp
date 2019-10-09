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

#include <cphd/CPHDXMLControl.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/SceneCoordinates.h>
#include <io/StringStream.h>
#include <xml/lite/MinidomParser.h>
#include "TestCase.h"

namespace
{
static const char XML[] =
"<CPHD>\n"
"    <CollectionID>\n"
"        <CollectorName>Collector</CollectorName>\n"
"        <CoreName>Core</CoreName>\n"
"        <CollectType>MONOSTATIC</CollectType>\n"
"        <RadarMode>\n"
"            <ModeType>STRIPMAP</ModeType>\n"
"            <ModeID>Mode</ModeID>\n"
"        </RadarMode>\n"
"        <Classification>U</Classification>\n"
"        <ReleaseInfo>Release</ReleaseInfo>\n"
"        <CountryCode>US,GB,AZ</CountryCode>\n"
"        <Parameter name=\"param1\">val</Parameter>\n"
"    </CollectionID>\n"
"    <Global>\n"
"        <DomainType>FX</DomainType>\n"
"        <SGN>+1</SGN>\n"
"        <Timeline>\n"
"            <CollectionStart>2013-04-10T08:52:09.0Z</CollectionStart>\n"
"            <RcvCollectionStart>2014-04-10T08:52:09.0Z</RcvCollectionStart>\n"
"            <TxTime1>1.3</TxTime1>\n"
"            <TxTime2>1.5</TxTime2>\n"
"        </Timeline>\n"
"        <FxBand>\n"
"            <FxMin>0.9</FxMin>\n"
"            <FxMax>1.7</FxMax>\n"
"        </FxBand>\n"
"        <TOASwath>\n"
"            <TOAMin>3.4</TOAMin>\n"
"            <TOAMax>6.1</TOAMax>\n"
"        </TOASwath>\n"
"        <TropoParameters>\n"
"            <N0>65.2</N0>\n"
"            <RefHeight>IARP</RefHeight>\n"
"        </TropoParameters>\n"
"        <IonoParameters>\n"
"            <TECV>5.8</TECV>\n"
"            <F2Height>3</F2Height>\n"
"        </IonoParameters>\n"
"    </Global>\n"
"    <SceneCoordinates>\n"
"        <EarthModel>WGS_84</EarthModel>\n"
"        <IARP>\n"
"            <ECF>\n"
"                <X>1.2</X>\n"
"                <Y>2.3</Y>\n"
"                <Z>3.4</Z>\n"
"            </ECF>\n"
"            <LLH>\n"
"                <Lat>45</Lat>\n"
"                <Lon>-102</Lon>\n"
"                <HAE>3.4</HAE>\n"
"            </LLH>\n"
"        </IARP>\n"
"        <ReferenceSurface>\n"
"            <HAE>\n"
"                <uIAXLL>\n"
"                    <Lat>12</Lat>\n"
"                    <Lon>24</Lon>\n"
"                </uIAXLL>\n"
"                <uIAYLL>\n"
"                    <Lat>36</Lat>\n"
"                    <Lon>48</Lon>\n"
"                </uIAYLL>\n"
"            </HAE>\n"
"        </ReferenceSurface>\n"
"        <ImageArea>\n"
"            <X1Y1>\n"
"                <X>3.5</X>\n"
"                <Y>5.3</Y>\n"
"            </X1Y1>\n"
"            <X2Y2>\n"
"                <X>5.3</X>\n"
"                <Y>3.5</Y>\n"
"            </X2Y2>\n"
"            <Polygon size=\"3\">\n"
"                <Vertex index=\"1\">\n"
"                    <X>0.1</X>\n"
"                    <Y>0.3</Y>\n"
"                </Vertex>\n"
"                <Vertex index=\"2\">\n"
"                    <X>0.4</X>\n"
"                    <Y>0.2</Y>\n"
"                </Vertex>\n"
"                <Vertex index=\"3\">\n"
"                    <X>0.5</X>\n"
"                    <Y>0.9</Y>\n"
"                </Vertex>\n"
"            </Polygon>\n"
"        </ImageArea>\n"
"        <ImageAreaCornerPoints>\n"
"            <IACP index=\"1\">\n"
"                <Lat>10</Lat>\n"
"                <Lon>11</Lon>\n"
"            </IACP>\n"
"            <IACP index=\"2\">\n"
"                <Lat>20</Lat>\n"
"                <Lon>21</Lon>\n"
"            </IACP>\n"
"            <IACP index=\"3\">\n"
"                <Lat>30</Lat>\n"
"                <Lon>31</Lon>\n"
"            </IACP>\n"
"            <IACP index=\"4\">\n"
"                <Lat>40</Lat>\n"
"                <Lon>41</Lon>\n"
"            </IACP>\n"
"        </ImageAreaCornerPoints>\n"
"        <ImageGrid>\n"
"            <Identifier>Grid</Identifier>\n"
"            <IARPLocation>\n"
"                <Line>1.23</Line>\n"
"                <Sample>3.21</Sample>\n"
"            </IARPLocation>\n"
"            <IAXExtent>\n"
"                <LineSpacing>3.14</LineSpacing>\n"
"                <FirstLine>4</FirstLine>\n"
"                <NumLines>50</NumLines>\n"
"            </IAXExtent>\n"
"            <IAYExtent>\n"
"                <SampleSpacing>6.28</SampleSpacing>\n"
"                <FirstSample>8</FirstSample>\n"
"                <NumSamples>100</NumSamples>\n"
"            </IAYExtent>\n"
"            <SegmentList>\n"
"                <NumSegments>2</NumSegments>\n"
"                <Segment>\n"
"                   <Identifier>Segment</Identifier>\n"
"                   <StartLine>0</StartLine>\n"
"                   <StartSample>1</StartSample>\n"
"                   <EndLine>2</EndLine>\n"
"                   <EndSample>3</EndSample>\n"
"                   <SegmentPolygon size=\"3\">\n"
"                       <SV index=\"1\">\n"
"                           <Line>0.4</Line>\n"
"                           <Sample>0.6</Sample>\n"
"                       </SV>\n"
"                       <SV index=\"2\">\n"
"                           <Line>0.8</Line>\n"
"                           <Sample>1.2</Sample>\n"
"                       </SV>\n"
"                       <SV index=\"3\">\n"
"                           <Line>1.2</Line>\n"
"                           <Sample>1.8</Sample>\n"
"                       </SV>\n"
"                   </SegmentPolygon>\n"
"                </Segment>\n"
"                <Segment>\n"
"                   <Identifier>Segment</Identifier>\n"
"                   <StartLine>1</StartLine>\n"
"                   <StartSample>2</StartSample>\n"
"                   <EndLine>3</EndLine>\n"
"                   <EndSample>4</EndSample>\n"
"                   <SegmentPolygon size=\"3\">\n"
"                       <SV index=\"1\">\n"
"                           <Line>0.4</Line>\n"
"                           <Sample>0.6</Sample>\n"
"                       </SV>\n"
"                       <SV index=\"2\">\n"
"                           <Line>0.8</Line>\n"
"                           <Sample>1.2</Sample>\n"
"                       </SV>\n"
"                       <SV index=\"3\">\n"
"                           <Line>1.2</Line>\n"
"                           <Sample>1.8</Sample>\n"
"                       </SV>\n"
"                   </SegmentPolygon>\n"
"                </Segment>\n"
"            </SegmentList>\n"
"        </ImageGrid>\n"
"    </SceneCoordinates>\n"
"    <Data>\n"
"        <SignalArrayFormat>CI4</SignalArrayFormat>\n"
"        <NumBytesPVP>24</NumBytesPVP>\n"
"        <NumCPHDChannels>2</NumCPHDChannels>\n"
"        <Channel>\n"
"            <Identifier>Channel</Identifier>\n"
"            <NumVectors>2</NumVectors>\n"
"            <NumSamples>3</NumSamples>\n"
"            <SignalArrayByteOffset>0</SignalArrayByteOffset>\n"
"            <PVPArrayByteOffset>1</PVPArrayByteOffset>\n"
"            <CompressedSignalSize>3</CompressedSignalSize>\n"
"        </Channel>\n"
"        <Channel>\n"
"            <Identifier>Channel</Identifier>\n"
"            <NumVectors>2</NumVectors>\n"
"            <NumSamples>3</NumSamples>\n"
"            <SignalArrayByteOffset>0</SignalArrayByteOffset>\n"
"            <PVPArrayByteOffset>1</PVPArrayByteOffset>\n"
"            <CompressedSignalSize>3</CompressedSignalSize>\n"
"        </Channel>\n"
"        <SignalCompressionID>Compress</SignalCompressionID>\n"
"        <NumSupportingArrays>1</NumSupportingArrays>\n"
"        <SupportArray>\n"
"            <Identifier>ID</Identifier>\n"
"            <NumRows>25</NumRows>\n"
"            <NumCols>70</NumCols>\n"
"            <BytesPerElement>4</BytesPerElement>\n"
"            <ArrayByteOffset>3</ArrayByteOffset>\n"
"        </SupportArray>\n"
"    </Data>\n"
"    <Channel>\n"
"        <RefChId>ChId</RefChId>\n"
"        <FXFixedCPHD>true</FXFixedCPHD>\n"
"        <TOAFixedCPHD>false</TOAFixedCPHD>\n"
"        <SRPFixedCPHD>true</SRPFixedCPHD>\n"
"        <Parameters>\n"
"            <Identifier>CPI</Identifier>\n"
"            <RefVectorIndex>0</RefVectorIndex>\n"
"            <FXFixed>false</FXFixed>\n"
"            <TOAFixed>true</TOAFixed>\n"
"            <SRPFixed>true</SRPFixed>\n"
"            <SignalNormal>false</SignalNormal>\n"
"            <Polarization>\n"
"                <TxPol>X</TxPol>\n"
"                <RcvPol>RHC</RcvPol>\n"
"            </Polarization>\n"
"            <FxC>1.3</FxC>\n"
"            <FxBW>0.8</FxBW>\n"
"            <FxBWNoise>0.5</FxBWNoise>\n"
"            <TOASaved>2.7</TOASaved>\n"
"            <TOAExtended>\n"
"                <TOAExtSaved>1.0</TOAExtSaved>\n"
"                <LFMEclipse>\n"
"                    <FxEarlyLow>1.0</FxEarlyLow>\n"
"                    <FxEarlyHigh>2.0</FxEarlyHigh>\n"
"                    <FxLateLow>1.0</FxLateLow>\n"
"                    <FxLateHigh>2.0</FxLateHigh>\n"
"                </LFMEclipse>\n"
"            </TOAExtended>\n"
"            <DwellTimes>\n"
"            <CODId>CODPolynomial</CODId>\n"
"            <DwellId>DwellPolynomial</DwellId>\n"
"            </DwellTimes>\n"
"            <ImageArea>\n"
"                <X1Y1>\n"
"                    <X>3.5</X>\n"
"                    <Y>5.3</Y>\n"
"                </X1Y1>\n"
"                <X2Y2>\n"
"                    <X>5.3</X>\n"
"                    <Y>3.5</Y>\n"
"                </X2Y2>\n"
"                <Polygon size=\"3\">\n"
"                    <Vertex index=\"1\">\n"
"                        <X>0.1</X>\n"
"                        <Y>0.3</Y>\n"
"                    </Vertex>\n"
"                    <Vertex index=\"2\">\n"
"                        <X>0.4</X>\n"
"                        <Y>0.2</Y>\n"
"                    </Vertex>\n"
"                    <Vertex index=\"3\">\n"
"                        <X>0.5</X>\n"
"                        <Y>0.9</Y>\n"
"                    </Vertex>\n"
"                </Polygon>\n"
"            </ImageArea>\n"
"        </Parameters>\n"
"    </Channel>\n"
"    <PVP>\n"
"        <txTime>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </txTime>\n"
"        <txPos>\n"
"        <Offset>0</Offset>\n"
"        <Size>3</Size>\n"
"        <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </txPos>\n"
"        <TxVel>\n"
"        <Offset>0</Offset>\n"
"        <Size>3</Size>\n"
"        <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </TxVel>\n"
"        <RcvTime>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </RcvTime>\n"
"        <RcvPos>\n"
"        <Offset>0</Offset>\n"
"        <Size>3</Size>\n"
"        <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </RcvPos>\n"
"        <rcvVel>\n"
"        <Offset>0</Offset>\n"
"        <Size>3</Size>\n"
"        <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </rcvVel>\n"
"        <SRPPos>\n"
"        <Offset>0</Offset>\n"
"        <Size>3</Size>\n"
"        <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </SRPPos>\n"
"        <aFDOP>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>\"F8\"</Format>\n"
"        </aFDOP>\n"
"        <aFRR1>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </aFRR1>\n"
"        <aFRR2>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </aFRR2>\n"
"        <FX1>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </FX1>\n"
"        <FX2>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </FX2>\n"
"        <TOA1>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </TOA1>\n"
"        <TOA2>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </TOA2>\n"
"        <TDTropoSRP>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </TDTropoSRP>\n"
"        <SC0>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </SC0>\n"
"        <SCSS>\n"
"        <Offset>0</Offset>\n"
"        <Size>1</Size>\n"
"        <Format>F8</Format>\n"
"        </SCSS>\n"
"        <addedPVP>\n"
"            <Name>newParam1</Name>\n"
"            <Offset>0</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </addedPVP>\n"
"        <addedPVP>\n"
"            <Name>newParam2</Name>\n"
"            <Offset>0</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </addedPVP>\n"
"    </PVP>\n"
"    <Dwell>\n"
"        <NumCODTimes>1</NumCODTimes>\n"
"        <NumDwellTimes>1</NumDwellTimes>\n"
"        <CODTime>\n"
"            <Identifier>codPolynomial1</Identifier>\n"
"            <CODTimePoly order1=\"1\" order2=\"1\">\n"
"                <Coef exponent1=\"1\" exponent2=\"0\">5</Coef>\n"
"                <Coef exponent1=\"0\" exponent2=\"1\">5</Coef>\n"
"            </CODTimePoly>\n"
"        </CODTime>\n"
"        <DwellTime>\n"
"            <Identifier>dwellPolynomial1</Identifier>\n"
"            <DwellTimePoly order1=\"1\" order2=\"1\">\n"
"                <Coef exponent1=\"1\" exponent2=\"0\">3</Coef>\n"
"                <Coef exponent1=\"0\" exponent2=\"1\">2</Coef>\n"
"            </DwellTimePoly>\n"
"        </DwellTime>\n"
"    </Dwell>\n"
"    <ReferenceGeometry>\n"
"        <SRP>\n"
"            <ECF>\n"
"                <X>1.0</X>\n"
"                <Y>2.0</Y>\n"
"                <Z>3.5</Z>\n"
"            </ECF>\n"
"            <IAC>\n"
"                <X>1.5</X>\n"
"                <Y>2.5</Y>\n"
"                <Z>4.0</Z>\n"
"            </IAC>\n"
"        </SRP>\n"
"        <ReferenceTime>0.0</ReferenceTime>\n"
"        <SRPCODTime>23.0</SRPCODTime>\n"
"        <SRPDwellTime>25.0</SRPDwellTime>\n"
"        <Bistatic>\n"
"            <AzimuthAngle>45.0</AzimuthAngle>\n"
"            <AzimuthAngleRate>20.0</AzimuthAngleRate>\n"
"            <BistaticAngle>45.0</BistaticAngle>\n"
"            <BistaticAngleRate>20.0</BistaticAngleRate>\n"
"            <GrazeAngle>30.0</GrazeAngle>\n"
"            <TwistAngle>30.0</TwistAngle>\n"
"            <SlopeAngle>30.0</SlopeAngle>\n"
"            <LayoverAngle>30.0</LayoverAngle>\n"
"            <TxPlatform>\n"
"                <Time>10.0</Time>\n"
"                <Pos>\n"
"                    <X>1.5</X>\n"
"                    <Y>2.5</Y>\n"
"                    <Z>4.0</Z>\n"
"                </Pos>\n"
"                <Vel>\n"
"                    <X>1.5</X>\n"
"                    <Y>2.5</Y>\n"
"                    <Z>4.0</Z>\n"
"                </Vel>\n"
"                <SideOfTrack>L</SideOfTrack>\n"
"                <SlantRange>20.0</SlantRange>\n"
"                <GroundRange>20.0</GroundRange>\n"
"                <DopplerConeAngle>30.0</DopplerConeAngle>\n"
"                <GrazeAngle>30.0</GrazeAngle>\n"
"                <IncidenceAngle>30.0</IncidenceAngle>\n"
"                <AzimuthAngle>30.0</AzimuthAngle>\n"
"            </TxPlatform>\n"
"            <RcvPlatform>\n"
"                <Time>10.0</Time>\n"
"                <Pos>\n"
"                    <X>1.5</X>\n"
"                    <Y>2.5</Y>\n"
"                    <Z>4.0</Z>\n"
"                </Pos>\n"
"                <Vel>\n"
"                    <X>1.5</X>\n"
"                    <Y>2.5</Y>\n"
"                    <Z>4.0</Z>\n"
"                </Vel>\n"
"                <SideOfTrack>L</SideOfTrack>\n"
"                <SlantRange>20.0</SlantRange>\n"
"                <GroundRange>20.0</GroundRange>\n"
"                <DopplerConeAngle>60.0</DopplerConeAngle>\n"
"                <GrazeAngle>60.0</GrazeAngle>\n"
"                <IncidenceAngle>60.0</IncidenceAngle>\n"
"                <AzimuthAngle>60.0</AzimuthAngle>\n"
"            </RcvPlatform>\n"
"        </Bistatic>\n"
"    </ReferenceGeometry>\n"
"</CPHD>";
}

TEST_CASE(testReadXML)
{
    io::StringStream cphdStream;
    cphdStream.write(XML, strlen(XML));

    xml::lite::MinidomParser xmlParser;
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(cphdStream, cphdStream.available());
    const std::auto_ptr<cphd::Metadata> metadata =
            cphd::CPHDXMLControl().fromXML(xmlParser.getDocument());

    // CollectionID
    TEST_ASSERT_EQ(metadata->collectionID.collectorName, "Collector");
    TEST_ASSERT_EQ(metadata->collectionID.coreName, "Core");
    TEST_ASSERT_EQ(metadata->collectionID.collectType,
                   six::CollectType::MONOSTATIC);
    TEST_ASSERT_EQ(metadata->collectionID.radarMode,
                   six::RadarModeType::STRIPMAP);
    TEST_ASSERT_EQ(metadata->collectionID.radarModeID, "Mode");
    TEST_ASSERT_EQ(metadata->collectionID.getClassificationLevel(), "U");    
    TEST_ASSERT_EQ(metadata->collectionID.releaseInfo, "Release");
    TEST_ASSERT_EQ(metadata->collectionID.countryCodes[0], "US");
    TEST_ASSERT_EQ(metadata->collectionID.countryCodes[1], "GB");
    TEST_ASSERT_EQ(metadata->collectionID.countryCodes[2], "AZ");

    TEST_ASSERT_EQ(metadata->collectionID.parameters[0].getName(), "param1");
    TEST_ASSERT_EQ(metadata->collectionID.parameters[0].str(), "val");

    // Global
    const cphd::Global& global = metadata->global;
    TEST_ASSERT_EQ(global.domainType, cphd::DomainType::FX);
    TEST_ASSERT_EQ(global.sgn, cphd::PhaseSGN::PLUS_1);
    const cphd::Timeline& timeline = global.timeline;
    TEST_ASSERT_EQ(timeline.collectionStart.getYear(), 2013);
    TEST_ASSERT_EQ(timeline.rcvCollectionStart.getYear(), 2014);
    TEST_ASSERT_EQ(timeline.txTime1, 1.3);
    TEST_ASSERT_EQ(timeline.txTime2, 1.5);
    TEST_ASSERT_EQ(global.fxBand.fxMin, 0.9);
    TEST_ASSERT_EQ(global.fxBand.fxMax, 1.7);
    TEST_ASSERT_EQ(global.toaSwath.toaMin, 3.4);
    TEST_ASSERT_EQ(global.toaSwath.toaMax, 6.1);
    TEST_ASSERT_TRUE(global.tropoParameters.get());
    TEST_ASSERT_EQ(global.tropoParameters->n0, 65.2);
    TEST_ASSERT_EQ(global.tropoParameters->refHeight, cphd::RefHeight::IARP);
    TEST_ASSERT_TRUE(global.ionoParameters.get());
    TEST_ASSERT_EQ(global.ionoParameters->tecv, 5.8);
    TEST_ASSERT_EQ(global.ionoParameters->f2Height, 3);

    // Scene Coordinates
    const cphd::SceneCoordinates& scene = metadata->sceneCoordinates;
    TEST_ASSERT_EQ(scene.earthModel, cphd::EarthModelType::WGS_84);
    TEST_ASSERT_EQ(scene.iarp.ecf[0], 1.2);
    TEST_ASSERT_EQ(scene.iarp.ecf[1], 2.3);
    TEST_ASSERT_EQ(scene.iarp.ecf[2], 3.4);

    TEST_ASSERT_EQ(scene.iarp.llh.getLat(), 45);
    TEST_ASSERT_EQ(scene.iarp.llh.getLon(), -102);
    TEST_ASSERT_EQ(scene.iarp.llh.getAlt(), 3.4);

    TEST_ASSERT_TRUE(scene.referenceSurface.hae.get());
    TEST_ASSERT_EQ(scene.referenceSurface.hae->uIax.getLat(), 12);
    TEST_ASSERT_EQ(scene.referenceSurface.hae->uIax.getLon(), 24);
    TEST_ASSERT_EQ(scene.referenceSurface.hae->uIay.getLat(), 36);
    TEST_ASSERT_EQ(scene.referenceSurface.hae->uIay.getLon(), 48);

    TEST_ASSERT_EQ(scene.imageArea.x1y1[0], 3.5)
    TEST_ASSERT_EQ(scene.imageArea.x1y1[1], 5.3)
    TEST_ASSERT_EQ(scene.imageArea.x2y2[0], 5.3)
    TEST_ASSERT_EQ(scene.imageArea.x2y2[1], 3.5)
    TEST_ASSERT_EQ(scene.imageArea.polygon.size(), 3);
    TEST_ASSERT_EQ(scene.imageArea.polygon[0][0], .1);
    TEST_ASSERT_EQ(scene.imageArea.polygon[0][1], .3);
    TEST_ASSERT_EQ(scene.imageArea.polygon[1][0], .4);
    TEST_ASSERT_EQ(scene.imageArea.polygon[1][1], .2);
    TEST_ASSERT_EQ(scene.imageArea.polygon[2][0], .5);
    TEST_ASSERT_EQ(scene.imageArea.polygon[2][1], .9);

    TEST_ASSERT_EQ(scene.imageAreaCorners.upperLeft.getLat(), 10);
    TEST_ASSERT_EQ(scene.imageAreaCorners.upperLeft.getLon(), 11);

    TEST_ASSERT_EQ(scene.imageAreaCorners.upperRight.getLat(), 20);
    TEST_ASSERT_EQ(scene.imageAreaCorners.upperRight.getLon(), 21);

    TEST_ASSERT_EQ(scene.imageAreaCorners.lowerRight.getLat(), 30);
    TEST_ASSERT_EQ(scene.imageAreaCorners.lowerRight.getLon(), 31);

    TEST_ASSERT_EQ(scene.imageAreaCorners.lowerLeft.getLat(), 40);
    TEST_ASSERT_EQ(scene.imageAreaCorners.lowerLeft.getLon(), 41);

    TEST_ASSERT_TRUE(scene.imageGrid.get());
    TEST_ASSERT_EQ(scene.imageGrid->identifier, "Grid");
    TEST_ASSERT_EQ(scene.imageGrid->iarpLocation.line, 1.23);
    TEST_ASSERT_EQ(scene.imageGrid->iarpLocation.sample, 3.21);
    TEST_ASSERT_EQ(scene.imageGrid->xExtent.lineSpacing, 3.14);
    TEST_ASSERT_EQ(scene.imageGrid->xExtent.firstLine, 4);
    TEST_ASSERT_EQ(scene.imageGrid->xExtent.numLines, 50);
    TEST_ASSERT_EQ(scene.imageGrid->yExtent.sampleSpacing, 6.28);
    TEST_ASSERT_EQ(scene.imageGrid->yExtent.firstSample, 8);
    TEST_ASSERT_EQ(scene.imageGrid->yExtent.numSamples, 100);
    TEST_ASSERT_EQ(scene.imageGrid->segments.size(), 2);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].startLine, 0);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].startSample, 1);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].endLine, 2);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].endSample, 3);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon.size(), 3);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[0].line, 0.4);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[0].sample, 0.6);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[1].line, 0.8);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[1].sample, 1.2);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[2].line, 1.2);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[2].sample, 1.8);

    // Data
    const cphd::Data& data = metadata->data;
    TEST_ASSERT_EQ(data.signalArrayFormat, cphd::SignalArrayFormat::CI4);
    TEST_ASSERT_EQ(data.numBytesPVP, 24);
    TEST_ASSERT_EQ(data.channels.size(), 2);
    TEST_ASSERT_EQ(data.channels[0].identifier, "Channel");
    TEST_ASSERT_EQ(data.channels[0].numVectors, 2);
    TEST_ASSERT_EQ(data.channels[0].numSamples, 3);
    TEST_ASSERT_EQ(data.channels[0].signalArrayByteOffset, 0);
    TEST_ASSERT_EQ(data.channels[0].pvpArrayByteOffset, 1);
    TEST_ASSERT_EQ(data.channels[0].compressedSignalSize, 3);
    TEST_ASSERT_EQ(data.signalCompressionID, "Compress");
    TEST_ASSERT_EQ(data.supportArrayMap.size(), 1);
    const std::string identifier = "ID";
    TEST_ASSERT_EQ(data.sa_IDMap.count(identifier), 1);
    const size_t offset = data.sa_IDMap.find(identifier)->second;
    TEST_ASSERT_EQ(data.supportArrayMap.count(offset), 1);
    // const cphd::Data::SupportArray& support = data.supportArrays[identifier];
    TEST_ASSERT_EQ(data.supportArrayMap.find(offset)->second.numRows, 25);
    TEST_ASSERT_EQ(data.supportArrayMap.find(offset)->second.numCols, 70);
    TEST_ASSERT_EQ(data.supportArrayMap.find(offset)->second.bytesPerElement, 4);
    TEST_ASSERT_EQ(data.supportArrayMap.find(offset)->second.arrayByteOffset, 3);

    // Channel
    const cphd::Channel& channel = metadata->channel;
    TEST_ASSERT_EQ(channel.refChId, "ChId");
    TEST_ASSERT_EQ(channel.fxFixedCphd, six::BooleanType::IS_TRUE);
    TEST_ASSERT_EQ(channel.toaFixedCphd, six::BooleanType::IS_FALSE);
    TEST_ASSERT_EQ(channel.srpFixedCphd, six::BooleanType::IS_TRUE);
    TEST_ASSERT_EQ(channel.parameters.size(), 1);
    TEST_ASSERT_EQ(channel.parameters[0].identifier, "CPI");
    TEST_ASSERT_EQ(channel.parameters[0].refVectorIndex, 0);
    TEST_ASSERT_EQ(channel.parameters[0].fxFixed,
                   six::BooleanType::IS_FALSE);
    TEST_ASSERT_EQ(channel.parameters[0].toaFixed,
                   six::BooleanType::IS_TRUE);
    TEST_ASSERT_EQ(channel.parameters[0].srpFixed,
                   six::BooleanType::IS_TRUE);
    TEST_ASSERT_EQ(channel.parameters[0].signalNormal,
                   six::BooleanType::IS_FALSE);

    TEST_ASSERT_EQ(channel.parameters[0].polarization.txPol,
                   cphd::PolarizationType::X);
    TEST_ASSERT_EQ(channel.parameters[0].polarization.rcvPol,
                   cphd::PolarizationType::RHC);

    TEST_ASSERT_EQ(channel.parameters[0].fxC, 1.3);
    TEST_ASSERT_EQ(channel.parameters[0].fxBW, 0.8);
    TEST_ASSERT_EQ(channel.parameters[0].toaSaved, 2.7);
    TEST_ASSERT_EQ(channel.parameters[0].toaExtended->toaExtSaved, 1.0);
    TEST_ASSERT_EQ(channel.parameters[0].toaExtended->lfmEclipse->fxEarlyLow, 1.0);
    TEST_ASSERT_EQ(channel.parameters[0].toaExtended->lfmEclipse->fxEarlyHigh, 2.0);
    TEST_ASSERT_EQ(channel.parameters[0].toaExtended->lfmEclipse->fxLateLow, 1.0);
    TEST_ASSERT_EQ(channel.parameters[0].toaExtended->lfmEclipse->fxLateHigh, 2.0);

    TEST_ASSERT_EQ(channel.parameters[0].dwellTimes.codId, "CODPolynomial");
    TEST_ASSERT_EQ(channel.parameters[0].dwellTimes.dwellId, "DwellPolynomial");

    TEST_ASSERT_EQ(channel.parameters[0].imageArea.x1y1[0], 3.5);
    TEST_ASSERT_EQ(channel.parameters[0].imageArea.x1y1[1], 5.3);
    TEST_ASSERT_EQ(channel.parameters[0].imageArea.x2y2[0], 5.3);
    TEST_ASSERT_EQ(channel.parameters[0].imageArea.x2y2[1], 3.5);

    TEST_ASSERT_EQ(channel.parameters[0].imageArea.polygon[0][0], 0.1);
    TEST_ASSERT_EQ(channel.parameters[0].imageArea.polygon[0][1], 0.3);
    TEST_ASSERT_EQ(channel.parameters[0].imageArea.polygon[1][0], 0.4);
    TEST_ASSERT_EQ(channel.parameters[0].imageArea.polygon[1][1], 0.2);
    TEST_ASSERT_EQ(channel.parameters[0].imageArea.polygon[2][0], 0.5);
    TEST_ASSERT_EQ(channel.parameters[0].imageArea.polygon[2][1], 0.9);

    TEST_ASSERT_EQ(channel.parameters[0].antenna->txAPCId, "TxAPCId");
    TEST_ASSERT_EQ(channel.parameters[0].antenna->txAPATId, "TxAPATId");
    TEST_ASSERT_EQ(channel.parameters[0].antenna->rcvAPCId, "RcvAPCId");
    TEST_ASSERT_EQ(channel.parameters[0].antenna->rcvAPATId, "RcvAPATId");

    TEST_ASSERT_EQ(channel.parameters[0].txRcv->txWFId[0], "TxWFId");
    TEST_ASSERT_EQ(channel.parameters[0].txRcv->rcvId[0], "rcvId");

    TEST_ASSERT_EQ(channel.parameters[0].tgtRefLevel->ptRef, 12.0);

    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->pnRef, 0.5);
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->bnRef, 0.8);

    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point.size(), 2);
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point[0].fx, 0.3);
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point[0].pn, 2.7);
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point[1].fx, 0.5);
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point[1].pn, 2.7);

    TEST_ASSERT_EQ(channel.addedParameters[0].str(), "AddedParameter1");
    TEST_ASSERT_EQ(channel.addedParameters[1].str(), "AddedParameter2");


    //PVP
    const cphd::Pvp& pvp = metadata->pvp;
    TEST_ASSERT_EQ(pvp.txTime.getOffset(), 0);
    TEST_ASSERT_EQ(pvp.txTime.getSize(), 1);
    TEST_ASSERT_EQ(pvp.txTime.getFormat(), "F8");
    TEST_ASSERT_EQ(pvp.txPos.getOffset(), 0);
    TEST_ASSERT_EQ(pvp.txPos.getSize(), 3);
    TEST_ASSERT_EQ(pvp.txPos.getFormat(), "X=F8;Y=F8;Z=F8;");
    TEST_ASSERT_EQ(pvp.rcvVel.getOffset(), 0);
    TEST_ASSERT_EQ(pvp.rcvVel.getSize(), 3);
    TEST_ASSERT_EQ(pvp.rcvVel.getFormat(), "X=F8;Y=F8;Z=F8;");
    TEST_ASSERT_EQ(pvp.addedPVP.size(), 2);
    TEST_ASSERT_EQ(pvp.addedPVP[0].getName(), "newParam1");
    TEST_ASSERT_EQ(pvp.addedPVP[1].getName(), "newParam2");

    //Dwell
    const cphd::Dwell& dwell = metadata->dwell;
    TEST_ASSERT_EQ(dwell.cod.size(), 1);
    TEST_ASSERT_EQ(dwell.dtime.size(), 1);
    TEST_ASSERT_EQ(dwell.cod[0].identifier, "codPolynomial1");
    TEST_ASSERT_EQ(dwell.dtime[0].identifier, "dwellPolynomial1");
    TEST_ASSERT_EQ(dwell.cod[0].codTimePoly.orderX(), 1);
    TEST_ASSERT_EQ(dwell.cod[0].codTimePoly.orderY(), 1);
    TEST_ASSERT_EQ(dwell.dtime[0].dwellTimePoly.orderX(), 1);
    TEST_ASSERT_EQ(dwell.dtime[0].dwellTimePoly.orderY(), 1);

    // ReferenceGeometry
    const cphd::ReferenceGeometry& ref = metadata->referenceGeometry;
    TEST_ASSERT_EQ(ref.srp.ecf[0], 1.0);
    TEST_ASSERT_EQ(ref.srp.ecf[1], 2.0);
    TEST_ASSERT_EQ(ref.srp.ecf[2], 3.5);
    TEST_ASSERT_EQ(ref.srp.iac[0], 1.5);
    TEST_ASSERT_EQ(ref.srp.iac[1], 2.5);
    TEST_ASSERT_EQ(ref.srp.iac[2], 4.0);
    TEST_ASSERT_EQ(ref.referenceTime, 0.0);
    TEST_ASSERT_EQ(ref.srpCODTime, 23.0);
    TEST_ASSERT_EQ(ref.srpDwellTime, 25.0);
    TEST_ASSERT_EQ(ref.bistatic->azimuthAngle, 45.0);
    TEST_ASSERT_EQ(ref.bistatic->azimuthAngleRate, 20);
    TEST_ASSERT_EQ(ref.bistatic->bistaticAngle, 45.0);
    TEST_ASSERT_EQ(ref.bistatic->bistaticAngleRate, 20);
    TEST_ASSERT_EQ(ref.bistatic->grazeAngle, 30.0);
    TEST_ASSERT_EQ(ref.bistatic->twistAngle, 30.0);
    TEST_ASSERT_EQ(ref.bistatic->slopeAngle, 30.0);
    TEST_ASSERT_EQ(ref.bistatic->layoverAngle, 30.0);

    TEST_ASSERT_EQ(ref.bistatic->txPlatform.time, 10);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.pos[0], 1.5);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.pos[1], 2.5);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.pos[2], 4.0);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.vel[0], 1.5);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.vel[1], 2.5);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.vel[2], 4.0);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.sideOfTrack.toString(), "LEFT");
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.slantRange, 20);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.groundRange, 20);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.dopplerConeAngle, 30.0);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.grazeAngle, 30.0);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.incidenceAngle, 30.0);
    TEST_ASSERT_EQ(ref.bistatic->txPlatform.azimuthAngle, 30.0);

    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.time, 10);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.pos[0], 1.5);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.pos[1], 2.5);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.pos[2], 4.0);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.vel[0], 1.5);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.vel[1], 2.5);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.vel[2], 4.0);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.sideOfTrack.toString(), "LEFT");
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.slantRange, 20);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.groundRange, 20);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.dopplerConeAngle, 60.0);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.grazeAngle, 60.0);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.incidenceAngle, 60.0);
    TEST_ASSERT_EQ(ref.bistatic->rcvPlatform.azimuthAngle, 60.0);
}


int main(int /*argc*/, char** /*argv*/)
{

    try
    {
        TEST_CHECK(testReadXML);
        // TEST_CHECK(testValidation);
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
