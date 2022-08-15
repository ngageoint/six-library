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
#include <io/FileInputStream.h>
#include <xml/lite/MinidomParser.h>
#include <logging/NullLogger.h>

#include "TestCase.h"

const char* test_cphd_xml_optional_XML =
"<CPHD xmlns=\"urn:CPHD:1.0.0\">\n"
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
"            <CollectionStart>2013-04-10T08:52:09.000000Z</CollectionStart>\n"
"            <RcvCollectionStart>2014-04-10T08:52:09.000000Z</RcvCollectionStart>\n"
"            <TxTime1>1.300000000000000E00</TxTime1>\n"
"            <TxTime2>1.500000000000000E00</TxTime2>\n"
"        </Timeline>\n"
"        <FxBand>\n"
"            <FxMin>9.000000000000000E-01</FxMin>\n"
"            <FxMax>1.700000000000000E00</FxMax>\n"
"        </FxBand>\n"
"        <TOASwath>\n"
"            <TOAMin>3.400000000000000E00</TOAMin>\n"
"            <TOAMax>6.100000000000000E00</TOAMax>\n"
"        </TOASwath>\n"
"        <TropoParameters>\n"
"            <N0>6.520000000000000E01</N0>\n"
"            <RefHeight>IARP</RefHeight>\n"
"        </TropoParameters>\n"
"        <IonoParameters>\n"
"            <TECV>5.800000000000000E00</TECV>\n"
"            <F2Height>3.000000000000000E00</F2Height>\n"
"        </IonoParameters>\n"
"    </Global>\n"
"    <SceneCoordinates>\n"
"        <EarthModel>WGS_84</EarthModel>\n"
"        <IARP>\n"
"            <ECF>\n"
"                <X>1.200000000000000E00</X>\n"
"                <Y>2.300000000000000E00</Y>\n"
"                <Z>3.400000000000000E00</Z>\n"
"            </ECF>\n"
"            <LLH>\n"
"                <Lat>4.500000000000000E01</Lat>\n"
"                <Lon>-1.020000000000000E02</Lon>\n"
"                <HAE>3.400000000000000E00</HAE>\n"
"            </LLH>\n"
"        </IARP>\n"
"        <ReferenceSurface>\n"
"            <HAE>\n"
"                <uIAXLL>\n"
"                    <Lat>1.200000000000000E01</Lat>\n"
"                    <Lon>2.400000000000000E01</Lon>\n"
"                </uIAXLL>\n"
"                <uIAYLL>\n"
"                    <Lat>3.600000000000000E01</Lat>\n"
"                    <Lon>4.800000000000000E01</Lon>\n"
"                </uIAYLL>\n"
"            </HAE>\n"
"        </ReferenceSurface>\n"
"        <ImageArea>\n"
"            <X1Y1>\n"
"                <X>3.500000000000000E00</X>\n"
"                <Y>5.300000000000000E00</Y>\n"
"            </X1Y1>\n"
"            <X2Y2>\n"
"                <X>5.300000000000000E00</X>\n"
"                <Y>3.500000000000000E00</Y>\n"
"            </X2Y2>\n"
"            <Polygon size=\"3\">\n"
"                <Vertex index=\"1\">\n"
"                    <X>1.000000000000000E-01</X>\n"
"                    <Y>3.000000000000000E-01</Y>\n"
"                </Vertex>\n"
"                <Vertex index=\"2\">\n"
"                    <X>4.000000000000000E-01</X>\n"
"                    <Y>2.000000000000000E-01</Y>\n"
"                </Vertex>\n"
"                <Vertex index=\"3\">\n"
"                    <X>5.000000000000000E-01</X>\n"
"                    <Y>9.000000000000000E-01</Y>\n"
"                </Vertex>\n"
"            </Polygon>\n"
"        </ImageArea>\n"
"        <ImageAreaCornerPoints>\n"
"            <IACP index=\"1\">\n"
"                <Lat>1.000000000000000E01</Lat>\n"
"                <Lon>1.100000000000000E01</Lon>\n"
"            </IACP>\n"
"            <IACP index=\"2\">\n"
"                <Lat>2.000000000000000E01</Lat>\n"
"                <Lon>2.100000000000000E01</Lon>\n"
"            </IACP>\n"
"            <IACP index=\"3\">\n"
"                <Lat>3.000000000000000E01</Lat>\n"
"                <Lon>3.100000000000000E01</Lon>\n"
"            </IACP>\n"
"            <IACP index=\"4\">\n"
"                <Lat>4.000000000000000E01</Lat>\n"
"                <Lon>4.100000000000000E01</Lon>\n"
"            </IACP>\n"
"        </ImageAreaCornerPoints>\n"
"        <ImageGrid>\n"
"            <Identifier>Grid</Identifier>\n"
"            <IARPLocation>\n"
"                <Line>1.230000000000000E00</Line>\n"
"                <Sample>3.210000000000000E00</Sample>\n"
"            </IARPLocation>\n"
"            <IAXExtent>\n"
"                <LineSpacing>3.140000000000000E00</LineSpacing>\n"
"                <FirstLine>4</FirstLine>\n"
"                <NumLines>50</NumLines>\n"
"            </IAXExtent>\n"
"            <IAYExtent>\n"
"                <SampleSpacing>6.280000000000000E00</SampleSpacing>\n"
"                <FirstSample>8</FirstSample>\n"
"                <NumSamples>100</NumSamples>\n"
"            </IAYExtent>\n"
"            <SegmentList>\n"
"                <NumSegments>2</NumSegments>\n"
"                <Segment>\n"
"                    <Identifier>Segment1</Identifier>\n"
"                    <StartLine>0</StartLine>\n"
"                    <StartSample>1</StartSample>\n"
"                    <EndLine>2</EndLine>\n"
"                    <EndSample>3</EndSample>\n"
"                    <SegmentPolygon size=\"3\">\n"
"                        <SV index=\"1\">\n"
"                            <Line>4.000000000000000E-01</Line>\n"
"                            <Sample>6.000000000000000E-01</Sample>\n"
"                        </SV>\n"
"                        <SV index=\"2\">\n"
"                            <Line>8.000000000000000E-01</Line>\n"
"                            <Sample>1.200000000000000E00</Sample>\n"
"                        </SV>\n"
"                        <SV index=\"3\">\n"
"                            <Line>1.200000000000000E00</Line>\n"
"                            <Sample>1.800000000000000E00</Sample>\n"
"                        </SV>\n"
"                    </SegmentPolygon>\n"
"                </Segment>\n"
"                <Segment>\n"
"                    <Identifier>Segment2</Identifier>\n"
"                    <StartLine>1</StartLine>\n"
"                    <StartSample>2</StartSample>\n"
"                    <EndLine>3</EndLine>\n"
"                    <EndSample>4</EndSample>\n"
"                    <SegmentPolygon size=\"3\">\n"
"                        <SV index=\"1\">\n"
"                            <Line>4.000000000000000E-01</Line>\n"
"                            <Sample>6.000000000000000E-01</Sample>\n"
"                        </SV>\n"
"                        <SV index=\"2\">\n"
"                            <Line>8.000000000000000E-01</Line>\n"
"                            <Sample>1.200000000000000E00</Sample>\n"
"                        </SV>\n"
"                        <SV index=\"3\">\n"
"                            <Line>1.200000000000000E00</Line>\n"
"                            <Sample>1.800000000000000E00</Sample>\n"
"                        </SV>\n"
"                    </SegmentPolygon>\n"
"                </Segment>\n"
"            </SegmentList>\n"
"        </ImageGrid>\n"
"    </SceneCoordinates>\n"
"    <Data>\n"
"        <SignalArrayFormat>CI4</SignalArrayFormat>\n"
"        <NumBytesPVP>24</NumBytesPVP>\n"
"        <NumCPHDChannels>2</NumCPHDChannels>\n"
"        <SignalCompressionID>Compress</SignalCompressionID>\n"
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
"        <NumSupportArrays>3</NumSupportArrays>\n"
"        <SupportArray>\n"
"            <Identifier>1.0</Identifier>\n"
"            <NumRows>3</NumRows>\n"
"            <NumCols>4</NumCols>\n"
"            <BytesPerElement>8</BytesPerElement>\n"
"            <ArrayByteOffset>0</ArrayByteOffset>\n"
"        </SupportArray>\n"
"        <SupportArray>\n"
"            <Identifier>2.0</Identifier>\n"
"            <NumRows>3</NumRows>\n"
"            <NumCols>4</NumCols>\n"
"            <BytesPerElement>4</BytesPerElement>\n"
"            <ArrayByteOffset>96</ArrayByteOffset>\n"
"        </SupportArray>\n"
"        <SupportArray>\n"
"            <Identifier>AddedSupportArray</Identifier>\n"
"            <NumRows>3</NumRows>\n"
"            <NumCols>4</NumCols>\n"
"            <BytesPerElement>4</BytesPerElement>\n"
"            <ArrayByteOffset>144</ArrayByteOffset>\n"
"        </SupportArray>\n"
"    </Data>\n"
"    <Channel>\n"
"        <RefChId>ChId</RefChId>\n"
"        <FXFixedCPHD>true</FXFixedCPHD>\n"
"        <TOAFixedCPHD>false</TOAFixedCPHD>\n"
"        <SRPFixedCPHD>true</SRPFixedCPHD>\n"
"        <Parameters>\n"
"            <Identifier>CPI</Identifier>\n"
"            <RefVectorIndex>1</RefVectorIndex>\n"
"            <FXFixed>false</FXFixed>\n"
"            <TOAFixed>true</TOAFixed>\n"
"            <SRPFixed>true</SRPFixed>\n"
"            <SignalNormal>false</SignalNormal>\n"
"            <Polarization>\n"
"                <TxPol>X</TxPol>\n"
"                <RcvPol>RHC</RcvPol>\n"
"            </Polarization>\n"
"            <FxC>1.300000000000000E00</FxC>\n"
"            <FxBW>8.000000000000000E-01</FxBW>\n"
"            <FxBWNoise>5.000000000000000E-01</FxBWNoise>\n"
"            <TOASaved>2.700000000000000E00</TOASaved>\n"
"            <TOAExtended>\n"
"                <TOAExtSaved>1.000000000000000E00</TOAExtSaved>\n"
"                <LFMEclipse>\n"
"                    <FxEarlyLow>1.000000000000000E00</FxEarlyLow>\n"
"                    <FxEarlyHigh>2.000000000000000E00</FxEarlyHigh>\n"
"                    <FxLateLow>1.000000000000000E00</FxLateLow>\n"
"                    <FxLateHigh>2.000000000000000E00</FxLateHigh>\n"
"                </LFMEclipse>\n"
"            </TOAExtended>\n"
"            <DwellTimes>\n"
"                <CODId>CODPolynomial</CODId>\n"
"                <DwellId>DwellPolynomial</DwellId>\n"
"            </DwellTimes>\n"
"            <ImageArea>\n"
"                <X1Y1>\n"
"                    <X>3.500000000000000E00</X>\n"
"                    <Y>5.300000000000000E00</Y>\n"
"                </X1Y1>\n"
"                <X2Y2>\n"
"                    <X>5.300000000000000E00</X>\n"
"                    <Y>3.500000000000000E00</Y>\n"
"                </X2Y2>\n"
"                <Polygon size=\"3\">\n"
"                    <Vertex index=\"1\">\n"
"                        <X>1.000000000000000E-01</X>\n"
"                        <Y>3.000000000000000E-01</Y>\n"
"                    </Vertex>\n"
"                    <Vertex index=\"2\">\n"
"                        <X>4.000000000000000E-01</X>\n"
"                        <Y>2.000000000000000E-01</Y>\n"
"                    </Vertex>\n"
"                    <Vertex index=\"3\">\n"
"                        <X>5.000000000000000E-01</X>\n"
"                        <Y>9.000000000000000E-01</Y>\n"
"                    </Vertex>\n"
"                </Polygon>\n"
"            </ImageArea>\n"
"            <Antenna>\n"
"                <TxAPCId>TxAPCId</TxAPCId>\n"
"                <TxAPATId>TxAPATId</TxAPATId>\n"
"                <RcvAPCId>RcvAPCId</RcvAPCId>\n"
"                <RcvAPATId>RcvAPATId</RcvAPATId>\n"
"            </Antenna>\n"
"            <TxRcv>\n"
"                <TxWFId>TxWFId</TxWFId>\n"
"                <RcvId>RcvId</RcvId>\n"
"            </TxRcv>\n"
"            <TgtRefLevel>\n"
"                <PTRef>1.200000000000000E01</PTRef>\n"
"            </TgtRefLevel>\n"
"            <NoiseLevel>\n"
"                <PNRef>5.000000000000000E-01</PNRef>\n"
"                <BNRef>8.000000000000000E-01</BNRef>\n"
"                <FxNoiseProfile>\n"
"                    <Point>\n"
"                        <Fx>3.000000000000000E-01</Fx>\n"
"                        <PN>2.700000000000000E00</PN>\n"
"                    </Point>\n"
"                    <Point>\n"
"                        <Fx>5.000000000000000E-01</Fx>\n"
"                        <PN>2.700000000000000E00</PN>\n"
"                    </Point>\n"
"                </FxNoiseProfile>\n"
"            </NoiseLevel>\n"
"        </Parameters>\n"
"        <AddedParameters>\n"
"            <Parameter name=\"AddedParameter1\">Param</Parameter>\n"
"            <Parameter name=\"AddedParameter2\">Param</Parameter>\n"
"        </AddedParameters>\n"
"    </Channel>\n"
"    <PVP>\n"
"        <TxTime>\n"
"            <Offset>0</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </TxTime>\n"
"        <TxPos>\n"
"            <Offset>1</Offset>\n"
"            <Size>3</Size>\n"
"            <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </TxPos>\n"
"        <TxVel>\n"
"            <Offset>4</Offset>\n"
"            <Size>3</Size>\n"
"            <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </TxVel>\n"
"        <RcvTime>\n"
"            <Offset>7</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </RcvTime>\n"
"        <RcvPos>\n"
"            <Offset>8</Offset>\n"
"            <Size>3</Size>\n"
"            <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </RcvPos>\n"
"        <RcvVel>\n"
"            <Offset>11</Offset>\n"
"            <Size>3</Size>\n"
"            <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </RcvVel>\n"
"        <SRPPos>\n"
"            <Offset>14</Offset>\n"
"            <Size>3</Size>\n"
"            <Format>X=F8;Y=F8;Z=F8;</Format>\n"
"        </SRPPos>\n"
"        <aFDOP>\n"
"            <Offset>17</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </aFDOP>\n"
"        <aFRR1>\n"
"            <Offset>18</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </aFRR1>\n"
"        <aFRR2>\n"
"            <Offset>19</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </aFRR2>\n"
"        <FX1>\n"
"            <Offset>20</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </FX1>\n"
"        <FX2>\n"
"            <Offset>21</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </FX2>\n"
"        <TOA1>\n"
"            <Offset>22</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </TOA1>\n"
"        <TOA2>\n"
"            <Offset>23</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </TOA2>\n"
"        <TDTropoSRP>\n"
"            <Offset>24</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </TDTropoSRP>\n"
"        <SC0>\n"
"            <Offset>25</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </SC0>\n"
"        <SCSS>\n"
"            <Offset>26</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </SCSS>\n"
"        <AddedPVP>\n"
"            <Name>newParam1</Name>\n"
"            <Offset>27</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </AddedPVP>\n"
"        <AddedPVP>\n"
"            <Name>newParam2</Name>\n"
"            <Offset>28</Offset>\n"
"            <Size>1</Size>\n"
"            <Format>F8</Format>\n"
"        </AddedPVP>\n"
"    </PVP>\n"
"    <SupportArray>\n"
"        <IAZArray>\n"
"            <Identifier>1</Identifier>\n"
"            <ElementFormat>IAZ=F4;</ElementFormat>\n"
"            <X0>0.000000000000000E00</X0>\n"
"            <Y0>0.000000000000000E00</Y0>\n"
"            <XSS>5.000000000000000E00</XSS>\n"
"            <YSS>5.000000000000000E00</YSS>\n"
"        </IAZArray>\n"
"        <AntGainPhase>\n"
"            <Identifier>2</Identifier>\n"
"            <ElementFormat>Gain=F4;Phase=F4;</ElementFormat>\n"
"            <X0>0.000000000000000E00</X0>\n"
"            <Y0>0.000000000000000E00</Y0>\n"
"            <XSS>5.000000000000000E00</XSS>\n"
"            <YSS>5.000000000000000E00</YSS>\n"
"        </AntGainPhase>\n"
"        <AddedSupportArray>\n"
"            <Identifier>AddedSupportArray</Identifier>\n"
"            <ElementFormat>F4</ElementFormat>\n"
"            <X0>0.000000000000000E00</X0>\n"
"            <Y0>0.000000000000000E00</Y0>\n"
"            <XSS>5.000000000000000E00</XSS>\n"
"            <YSS>5.000000000000000E00</YSS>\n"
"            <XUnits>XUnits</XUnits>\n"
"            <YUnits>YUnits</YUnits>\n"
"            <ZUnits>ZUnits</ZUnits>\n"
"            <Parameter name=\"Parameter1\">Additional parameter</Parameter>\n"
"            <Parameter name=\"Parameter2\">Additional parameter</Parameter>\n"
"        </AddedSupportArray>\n"
"    </SupportArray>\n"
"    <Dwell>\n"
"        <NumCODTimes>1</NumCODTimes>\n"
"        <CODTime>\n"
"            <Identifier>codPolynomial1</Identifier>\n"
"            <CODTimePoly order1=\"1\" order2=\"1\">\n"
"                <Coef exponent1=\"0\" exponent2=\"0\">0.000000000000000E00</Coef>\n"
"                <Coef exponent1=\"0\" exponent2=\"1\">5.000000000000000E00</Coef>\n"
"                <Coef exponent1=\"1\" exponent2=\"0\">5.000000000000000E00</Coef>\n"
"                <Coef exponent1=\"1\" exponent2=\"1\">0.000000000000000E00</Coef>\n"
"            </CODTimePoly>\n"
"        </CODTime>\n"
"        <NumDwellTimes>1</NumDwellTimes>\n"
"        <DwellTime>\n"
"            <Identifier>dwellPolynomial1</Identifier>\n"
"            <DwellTimePoly order1=\"1\" order2=\"1\">\n"
"                <Coef exponent1=\"0\" exponent2=\"0\">0.000000000000000E00</Coef>\n"
"                <Coef exponent1=\"0\" exponent2=\"1\">2.000000000000000E00</Coef>\n"
"                <Coef exponent1=\"1\" exponent2=\"0\">3.000000000000000E00</Coef>\n"
"                <Coef exponent1=\"1\" exponent2=\"1\">0.000000000000000E00</Coef>\n"
"            </DwellTimePoly>\n"
"        </DwellTime>\n"
"    </Dwell>\n"
"    <ReferenceGeometry>\n"
"        <SRP>\n"
"            <ECF>\n"
"                <X>1.000000000000000E00</X>\n"
"                <Y>2.000000000000000E00</Y>\n"
"                <Z>3.500000000000000E00</Z>\n"
"            </ECF>\n"
"            <IAC>\n"
"                <X>1.500000000000000E00</X>\n"
"                <Y>2.500000000000000E00</Y>\n"
"                <Z>4.000000000000000E00</Z>\n"
"            </IAC>\n"
"        </SRP>\n"
"        <ReferenceTime>0.000000000000000E00</ReferenceTime>\n"
"        <SRPCODTime>2.300000000000000E01</SRPCODTime>\n"
"        <SRPDwellTime>2.500000000000000E01</SRPDwellTime>\n"
"        <Monostatic>\n"
"            <ARPPos>\n"
"                <X>1.000000000000000E01</X>\n"
"                <Y>1.000000000000000E01</Y>\n"
"                <Z>1.000000000000000E01</Z>\n"
"            </ARPPos>\n"
"            <ARPVel>\n"
"                <X>1.000000000000000E01</X>\n"
"                <Y>1.000000000000000E01</Y>\n"
"                <Z>1.000000000000000E01</Z>\n"
"            </ARPVel>\n"
"            <SideOfTrack>L</SideOfTrack>\n"
"            <SlantRange>2.000000000000000E01</SlantRange>\n"
"            <GroundRange>2.000000000000000E01</GroundRange>\n"
"            <DopplerConeAngle>3.000000000000000E01</DopplerConeAngle>\n"
"            <GrazeAngle>3.000000000000000E01</GrazeAngle>\n"
"            <IncidenceAngle>3.000000000000000E01</IncidenceAngle>\n"
"            <AzimuthAngle>3.000000000000000E01</AzimuthAngle>\n"
"            <TwistAngle>3.000000000000000E01</TwistAngle>\n"
"            <SlopeAngle>3.000000000000000E01</SlopeAngle>\n"
"            <LayoverAngle>3.000000000000000E01</LayoverAngle>\n"
"        </Monostatic>\n"
"    </ReferenceGeometry>\n"
"    <Antenna>\n"
"        <NumACFs>2</NumACFs>\n"
"        <NumAPCs>1</NumAPCs>\n"
"        <NumAntPats>1</NumAntPats>\n"
"        <AntCoordFrame>\n"
"            <Identifier>ACF1</Identifier>\n"
"            <XAxisPoly>\n"
"                <X order1=\"3\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"2\">3.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"3\">4.000000000000000E00</Coef>\n"
"                </X>\n"
"                <Y order1=\"3\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"2\">3.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"3\">4.000000000000000E00</Coef>\n"
"                </Y>\n"
"                <Z order1=\"3\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">0.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"2\">3.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"3\">0.000000000000000E00</Coef>\n"
"                </Z>\n"
"            </XAxisPoly>\n"
"            <YAxisPoly>\n"
"                <X order1=\"1\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                </X>\n"
"                <Y order1=\"1\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                </Y>\n"
"                <Z order1=\"1\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                </Z>\n"
"            </YAxisPoly>\n"
"        </AntCoordFrame>\n"
"        <AntCoordFrame>\n"
"            <Identifier>ACF2</Identifier>\n"
"            <XAxisPoly>\n"
"                <X order1=\"1\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                </X>\n"
"                <Y order1=\"1\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                </Y>\n"
"                <Z order1=\"1\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                </Z>\n"
"            </XAxisPoly>\n"
"            <YAxisPoly>\n"
"                <X order1=\"1\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                </X>\n"
"                <Y order1=\"1\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                </Y>\n"
"                <Z order1=\"1\">\n"
"                    <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"                </Z>\n"
"            </YAxisPoly>\n"
"        </AntCoordFrame>\n"
"        <AntPhaseCenter>\n"
"            <Identifier>APC</Identifier>\n"
"            <ACFId>ACF1</ACFId>\n"
"            <APCXYZ>\n"
"                <X>5.000000000000000E00</X>\n"
"                <Y>5.000000000000000E00</Y>\n"
"                <Z>5.000000000000000E00</Z>\n"
"            </APCXYZ>\n"
"        </AntPhaseCenter>\n"
"        <AntPattern>\n"
"            <Identifier>APAT</Identifier>\n"
"            <FreqZero>2.300000000000000E00</FreqZero>\n"
"            <GainZero>2.300000000000000E00</GainZero>\n"
"            <EBFreqShift>1</EBFreqShift>\n"
"            <MLFreqDilation>0</MLFreqDilation>\n"
"            <GainBSPoly order1=\"1\">\n"
"                <Coef exponent1=\"0\">1.000000000000000E00</Coef>\n"
"                <Coef exponent1=\"1\">2.000000000000000E00</Coef>\n"
"            </GainBSPoly>\n"
"            <EB>\n"
"                <DCXPoly order1=\"1\">\n"
"                    <Coef exponent1=\"0\">5.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">0.000000000000000E00</Coef>\n"
"                </DCXPoly>\n"
"                <DCYPoly order1=\"1\">\n"
"                    <Coef exponent1=\"0\">0.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\">5.000000000000000E00</Coef>\n"
"                </DCYPoly>\n"
"            </EB>\n"
"            <Array>\n"
"                <GainPoly order1=\"1\" order2=\"1\">\n"
"                    <Coef exponent1=\"0\" exponent2=\"0\">0.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"0\" exponent2=\"1\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\" exponent2=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\" exponent2=\"1\">5.000000000000000E00</Coef>\n"
"                </GainPoly>\n"
"                <PhasePoly order1=\"1\" order2=\"1\">\n"
"                    <Coef exponent1=\"0\" exponent2=\"0\">0.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"0\" exponent2=\"1\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\" exponent2=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\" exponent2=\"1\">5.000000000000000E00</Coef>\n"
"                </PhasePoly>\n"
"            </Array>\n"
"            <Element>\n"
"                <GainPoly order1=\"1\" order2=\"1\">\n"
"                    <Coef exponent1=\"0\" exponent2=\"0\">0.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"0\" exponent2=\"1\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\" exponent2=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\" exponent2=\"1\">5.000000000000000E00</Coef>\n"
"                </GainPoly>\n"
"                <PhasePoly order1=\"1\" order2=\"1\">\n"
"                    <Coef exponent1=\"0\" exponent2=\"0\">0.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"0\" exponent2=\"1\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\" exponent2=\"0\">1.000000000000000E00</Coef>\n"
"                    <Coef exponent1=\"1\" exponent2=\"1\">5.000000000000000E00</Coef>\n"
"                </PhasePoly>\n"
"            </Element>\n"
"            <GainPhaseArray>\n"
"                <Freq>2.300000000000000E00</Freq>\n"
"                <ArrayId>Parameter1</ArrayId>\n"
"                <ElementId>Parameter2</ElementId>\n"
"            </GainPhaseArray>\n"
"            <GainPhaseArray>\n"
"                <Freq>2.800000000000000E00</Freq>\n"
"                <ArrayId>Parameter1</ArrayId>\n"
"                <ElementId>Parameter2</ElementId>\n"
"            </GainPhaseArray>\n"
"        </AntPattern>\n"
"    </Antenna>\n"
"    <TxRcv>\n"
"        <NumTxWFs>1</NumTxWFs>\n"
"        <TxWFParameters>\n"
"            <Identifier>TxWFParam</Identifier>\n"
"            <PulseLength>3.000000000000000E00</PulseLength>\n"
"            <RFBandwidth>2.300000000000000E00</RFBandwidth>\n"
"            <FreqCenter>1.800000000000000E00</FreqCenter>\n"
"            <LFMRate>1.000000000000000E00</LFMRate>\n"
"            <Polarization>LHC</Polarization>\n"
"            <Power>5.000000000000000E00</Power>\n"
"        </TxWFParameters>\n"
"        <NumRcvs>2</NumRcvs>\n"
"        <RcvParameters>\n"
"            <Identifier>RcvParam1</Identifier>\n"
"            <WindowLength>3.000000000000000E00</WindowLength>\n"
"            <SampleRate>2.300000000000000E00</SampleRate>\n"
"            <IFFilterBW>2.300000000000000E00</IFFilterBW>\n"
"            <FreqCenter>1.800000000000000E00</FreqCenter>\n"
"            <LFMRate>1.000000000000000E00</LFMRate>\n"
"            <Polarization>LHC</Polarization>\n"
"            <PathGain>5.000000000000000E00</PathGain>\n"
"        </RcvParameters>\n"
"        <RcvParameters>\n"
"            <Identifier>RcvParam2</Identifier>\n"
"            <WindowLength>3.000000000000000E00</WindowLength>\n"
"            <SampleRate>2.300000000000000E00</SampleRate>\n"
"            <IFFilterBW>2.300000000000000E00</IFFilterBW>\n"
"            <FreqCenter>1.800000000000000E00</FreqCenter>\n"
"            <LFMRate>1.000000000000000E00</LFMRate>\n"
"            <Polarization>LHC</Polarization>\n"
"            <PathGain>5.000000000000000E00</PathGain>\n"
"        </RcvParameters>\n"
"    </TxRcv>\n"
"    <ErrorParameters>\n"
"        <Monostatic>\n"
"            <PosVelErr>\n"
"                <Frame>ECF</Frame>\n"
"                <P1>1.000000000000000E00</P1>\n"
"                <P2>1.000000000000000E00</P2>\n"
"                <P3>1.000000000000000E00</P3>\n"
"                <V1>1.000000000000000E00</V1>\n"
"                <V2>1.000000000000000E00</V2>\n"
"                <V3>1.000000000000000E00</V3>\n"
"                <CorrCoefs>\n"
"                    <P1P2>8.000000000000000E-01</P1P2>\n"
"                    <P1P3>8.000000000000000E-01</P1P3>\n"
"                    <P1V1>8.000000000000000E-01</P1V1>\n"
"                    <P1V2>8.000000000000000E-01</P1V2>\n"
"                    <P1V3>8.000000000000000E-01</P1V3>\n"
"                    <P2P3>8.000000000000000E-01</P2P3>\n"
"                    <P2V1>8.000000000000000E-01</P2V1>\n"
"                    <P2V2>8.000000000000000E-01</P2V2>\n"
"                    <P2V3>8.000000000000000E-01</P2V3>\n"
"                    <P3V1>8.000000000000000E-01</P3V1>\n"
"                    <P3V2>8.000000000000000E-01</P3V2>\n"
"                    <P3V3>8.000000000000000E-01</P3V3>\n"
"                    <V1V2>8.000000000000000E-01</V1V2>\n"
"                    <V1V3>8.000000000000000E-01</V1V3>\n"
"                    <V2V3>8.000000000000000E-01</V2V3>\n"
"                </CorrCoefs>\n"
"                <PositionDecorr>\n"
"                    <CorrCoefZero>5.000000000000000E-01</CorrCoefZero>\n"
"                    <DecorrRate>1.000000000000000E00</DecorrRate>\n"
"                </PositionDecorr>\n"
"            </PosVelErr>\n"
"            <RadarSensor>\n"
"                <RangeBias>5.000000000000000E-01</RangeBias>\n"
"                <ClockFreqSF>1.000000000000000E00</ClockFreqSF>\n"
"                <CollectionStartTime>1.000000000000000E00</CollectionStartTime>\n"
"                <RangeBiasDecorr>\n"
"                    <CorrCoefZero>5.000000000000000E-01</CorrCoefZero>\n"
"                    <DecorrRate>1.000000000000000E00</DecorrRate>\n"
"                </RangeBiasDecorr>\n"
"            </RadarSensor>\n"
"            <TropoError>\n"
"                <TropoRangeVertical>5.000000000000000E00</TropoRangeVertical>\n"
"                <TropoRangeSlant>5.000000000000000E00</TropoRangeSlant>\n"
"                <TropoRangeDecorr>\n"
"                    <CorrCoefZero>5.000000000000000E-01</CorrCoefZero>\n"
"                    <DecorrRate>1.000000000000000E00</DecorrRate>\n"
"                </TropoRangeDecorr>\n"
"            </TropoError>\n"
"            <IonoError>\n"
"                <IonoRangeVertical>5.000000000000000E00</IonoRangeVertical>\n"
"                <IonoRangeRateVertical>5.000000000000000E00</IonoRangeRateVertical>\n"
"                <IonoRgRgRateCC>5.000000000000000E-01</IonoRgRgRateCC>\n"
"                <IonoRangeVertDecorr>\n"
"                    <CorrCoefZero>5.000000000000000E-01</CorrCoefZero>\n"
"                    <DecorrRate>1.000000000000000E00</DecorrRate>\n"
"                </IonoRangeVertDecorr>\n"
"            </IonoError>\n"
"        </Monostatic>\n"
"    </ErrorParameters>\n"
"    <ProductInfo>\n"
"        <Profile>Profile</Profile>\n"
"        <CreationInfo>\n"
"            <Application>Application</Application>\n"
"            <DateTime>2014-04-10T08:52:09.000000Z</DateTime>\n"
"            <Site>Area51</Site>\n"
"            <Parameter name=\"Param1\">Value1</Parameter>\n"
"            <Parameter name=\"Param2\">Value2</Parameter>\n"
"        </CreationInfo>\n"
"        <Parameter name=\"Param1\">Value1</Parameter>\n"
"    </ProductInfo>\n"
"    <GeoInfo name=\"Airport\">\n"
"        <Desc name=\"Airport ID\">51</Desc>\n"
"        <GeoInfo name=\"Perimeter\">\n"
"            <Polygon size=\"4\">\n"
"                <Vertex index=\"1\">\n"
"                    <Lat>0.000000000000000E00</Lat>\n"
"                    <Lon>0.000000000000000E00</Lon>\n"
"                </Vertex>\n"
"                <Vertex index=\"2\">\n"
"                    <Lat>1.000000000000000E-01</Lat>\n"
"                    <Lon>0.000000000000000E00</Lon>\n"
"                </Vertex>\n"
"                <Vertex index=\"3\">\n"
"                    <Lat>1.000000000000000E-01</Lat>\n"
"                    <Lon>1.000000000000000E-01</Lon>\n"
"                </Vertex>\n"
"                <Vertex index=\"4\">\n"
"                    <Lat>0.000000000000000E00</Lat>\n"
"                    <Lon>0.000000000000000E00</Lon>\n"
"                </Vertex>\n"
"            </Polygon>\n"
"        </GeoInfo>\n"
"        <GeoInfo name=\"Runway\">\n"
"            <Desc name=\"ID\">04/22</Desc>\n"
"            <Line size=\"2\">\n"
"                <Endpoint index=\"1\">\n"
"                    <Lat>2.000000000000000E-02</Lat>\n"
"                    <Lon>3.000000000000000E-02</Lon>\n"
"                </Endpoint>\n"
"                <Endpoint index=\"2\">\n"
"                    <Lat>8.000000000000000E-02</Lat>\n"
"                    <Lon>8.000000000000000E-02</Lon>\n"
"                </Endpoint>\n"
"            </Line>\n"
"        </GeoInfo>\n"
"        <GeoInfo name=\"Runway\">\n"
"            <Desc name=\"ID\">09/27</Desc>\n"
"            <Line size=\"2\">\n"
"                <Endpoint index=\"1\">\n"
"                    <Lat>5.000000000000000E-02</Lat>\n"
"                    <Lon>2.000000000000000E-02</Lon>\n"
"                </Endpoint>\n"
"                <Endpoint index=\"2\">\n"
"                    <Lat>5.000000000000000E-02</Lat>\n"
"                    <Lon>8.000000000000000E-02</Lon>\n"
"                </Endpoint>\n"
"            </Line>\n"
"        </GeoInfo>\n"
"        <GeoInfo name=\"Control Tower\">\n"
"            <Desc name=\"ID\">Main</Desc>\n"
"            <Point>\n"
"                <Lat>6.000000000000000E-01</Lat>\n"
"                <Lon>4.000000000000000E-01</Lon>\n"
"            </Point>\n"
"        </GeoInfo>\n"
"    </GeoInfo>\n"
"    <GeoInfo name=\"Farm\">\n"
"        <Polygon size=\"5\">\n"
"            <Vertex index=\"1\">\n"
"                <Lat>1.000000000000000E00</Lat>\n"
"                <Lon>1.000000000000000E00</Lon>\n"
"            </Vertex>\n"
"            <Vertex index=\"2\">\n"
"                <Lat>1.100000000000000E00</Lat>\n"
"                <Lon>1.000000000000000E00</Lon>\n"
"            </Vertex>\n"
"            <Vertex index=\"3\">\n"
"                <Lat>1.100000000000000E00</Lat>\n"
"                <Lon>1.100000000000000E00</Lon>\n"
"            </Vertex>\n"
"            <Vertex index=\"4\">\n"
"                <Lat>1.200000000000000E00</Lat>\n"
"                <Lon>1.200000000000000E00</Lon>\n"
"            </Vertex>\n"
"            <Vertex index=\"5\">\n"
"                <Lat>1.000000000000000E00</Lat>\n"
"                <Lon>1.000000000000000E00</Lon>\n"
"            </Vertex>\n"
"        </Polygon>\n"
"    </GeoInfo>\n"
"    <MatchInfo>\n"
"        <NumMatchTypes>2</NumMatchTypes>\n"
"        <MatchType index=\"1\">\n"
"            <TypeID>STEREO</TypeID>\n"
"            <CurrentIndex>1</CurrentIndex>\n"
"            <NumMatchCollections>1</NumMatchCollections>\n"
"            <MatchCollection index=\"1\">\n"
"                <CoreName>CollectionName</CoreName>\n"
"                <MatchIndex>1</MatchIndex>\n"
"                <Parameter name=\"param1\">Match1</Parameter>\n"
"            </MatchCollection>\n"
"        </MatchType>\n"
"        <MatchType index=\"2\">\n"
"            <TypeID>COHERENT</TypeID>\n"
"            <CurrentIndex>1</CurrentIndex>\n"
"            <NumMatchCollections>1</NumMatchCollections>\n"
"            <MatchCollection index=\"2\">\n"
"                <CoreName>CollectionName</CoreName>\n"
"                <MatchIndex>1</MatchIndex>\n"
"                <Parameter name=\"param1\">Match1</Parameter>\n"
"            </MatchCollection>\n"
"        </MatchType>\n"
"    </MatchInfo>\n"
"</CPHD>\n";

TEST_CASE(testOptional)
{
    io::StringStream cphdStream;
    cphdStream.write(test_cphd_xml_optional_XML, strlen(test_cphd_xml_optional_XML));

    xml::lite::MinidomParser xmlParser;
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(cphdStream, cphdStream.available());
    const std::unique_ptr<cphd::Metadata> metadata =
            cphd::CPHDXMLControl().fromXML(xmlParser.getDocument());

    const cphd::SupportArray& supportArray = *(metadata->supportArray);
    TEST_ASSERT_EQ(supportArray.iazArray.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(supportArray.iazArray[0].getIdentifier(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(supportArray.iazArray[0].elementFormat, "IAZ=F4;");
    TEST_ASSERT_EQ(supportArray.iazArray[0].x0, 0.0);
    TEST_ASSERT_EQ(supportArray.iazArray[0].y0, 0.0);
    TEST_ASSERT_EQ(supportArray.iazArray[0].xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.iazArray[0].ySS, 5.0);

    TEST_ASSERT_EQ(supportArray.antGainPhase.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].getIdentifier(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].elementFormat, "Gain=F4;Phase=F4;");
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].x0, 0.0);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].y0, 0.0);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.antGainPhase[0].ySS, 5.0);

    TEST_ASSERT_EQ(supportArray.addedSupportArray.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(supportArray.addedSupportArray.count("AddedSupportArray"), static_cast<size_t>(1));
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.elementFormat, "F4");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.x0, 0.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.y0, 0.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.xSS, 5.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.ySS, 5.0);
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.xUnits, "XUnits");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.yUnits, "YUnits");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.zUnits, "ZUnits");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter[0].getName(), "Parameter1");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter[1].getName(), "Parameter2");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter[0].str(), "Additional parameter");
    TEST_ASSERT_EQ(supportArray.addedSupportArray.find("AddedSupportArray")->second.parameter[1].str(), "Additional parameter");

    const cphd::Antenna& antenna = *(metadata->antenna);
    TEST_ASSERT_EQ(antenna.antCoordFrame.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].identifier, "ACF1");
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].xAxisPoly.order(), static_cast<size_t>(3));
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

    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.order(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.coeffs()[0][0], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.coeffs()[1][0], 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.coeffs()[0][1], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[0].yAxisPoly.coeffs()[1][1], 2);

    TEST_ASSERT_EQ(antenna.antCoordFrame[1].identifier, "ACF2");
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.order(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.coeffs()[0][0], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.coeffs()[1][0], 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.coeffs()[0][1], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].xAxisPoly.coeffs()[1][1], 2);

    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.order(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.coeffs()[0][0], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.coeffs()[1][0], 2);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.coeffs()[0][1], 1);
    TEST_ASSERT_EQ(antenna.antCoordFrame[1].yAxisPoly.coeffs()[1][1], 2);

    TEST_ASSERT_EQ(antenna.antPhaseCenter.size(), static_cast<size_t>(1));
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

    TEST_ASSERT_EQ(antenna.antPattern.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].identifier, "APAT");
    TEST_ASSERT_EQ(antenna.antPattern[0].freqZero, 2.3);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainZero, 2.3);
    TEST_ASSERT_EQ(antenna.antPattern[0].ebFreqShift, 1);
    TEST_ASSERT_EQ(antenna.antPattern[0].mlFreqDilation, 0);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainBSPoly.order(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].gainBSPoly.coeffs()[0], 1.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainBSPoly.coeffs()[1], 2.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcxPoly.order(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcxPoly.coeffs()[0], 5.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcyPoly.order(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcyPoly.coeffs()[0], 0.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].eb.dcyPoly.coeffs()[1], 5.0);
    TEST_ASSERT_EQ(antenna.antPattern[0].array.gainPoly.orderX(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].array.gainPoly.orderY(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].array.phasePoly.orderX(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].array.phasePoly.orderY(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].element.gainPoly.orderX(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].element.gainPoly.orderY(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].element.phasePoly.orderX(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].element.phasePoly.orderY(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[0].freq, 2.3);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[0].arrayId, "Parameter1");
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[0].elementId, "Parameter2");
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[1].freq, 2.8);
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[1].arrayId, "Parameter1");
    TEST_ASSERT_EQ(antenna.antPattern[0].gainPhaseArray[1].elementId, "Parameter2");

    const cphd::TxRcv& txRcv = *(metadata->txRcv);
    TEST_ASSERT_EQ(txRcv.txWFParameters.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].identifier, "TxWFParam");
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].pulseLength, 3.0);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].rfBandwidth, 2.3);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].freqCenter, 1.8);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].lfmRate, 1.0);
    TEST_ASSERT_EQ(six::Enum::toString(txRcv.txWFParameters[0].polarization), "LHC");
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].polarization, cphd::PolarizationType::LHC);
    TEST_ASSERT_EQ(txRcv.txWFParameters[0].power, 5.0);

    TEST_ASSERT_EQ(txRcv.rcvParameters.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].identifier, "RcvParam1");
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].windowLength, 3.0);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].sampleRate, 2.3);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].ifFilterBW, 2.3);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].freqCenter, 1.8);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].lfmRate, 1.0);
    TEST_ASSERT_EQ(six::Enum::toString(txRcv.rcvParameters[0].polarization), "LHC");
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].polarization, cphd::PolarizationType::LHC);
    TEST_ASSERT_EQ(txRcv.rcvParameters[0].pathGain, 5.0);

    TEST_ASSERT_EQ(txRcv.rcvParameters[1].identifier, "RcvParam2");
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].windowLength, 3.0);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].sampleRate, 2.3);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].ifFilterBW, 2.3);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].freqCenter, 1.8);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].lfmRate, 1.0);
    TEST_ASSERT_EQ(six::Enum::toString(txRcv.rcvParameters[1].polarization), "LHC");
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].polarization, cphd::PolarizationType::LHC);
    TEST_ASSERT_EQ(txRcv.rcvParameters[1].pathGain, 5.0);

    const cphd::ErrorParameters& errorParams = *(metadata->errorParameters);
    TEST_ASSERT(errorParams.monostatic->posVelErr.frame == "ECF");
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.frame, six::FrameType(six::FrameType::ECF));
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.p1, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.p2, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.corrCoefs->p1p2, 0.8);
    TEST_ASSERT_EQ(errorParams.monostatic->posVelErr.corrCoefs->v2v3, 0.8);
    TEST_ASSERT_EQ(six::value(errorParams.monostatic->posVelErr.positionDecorr).corrCoefZero, 0.5);
    TEST_ASSERT_EQ(six::value(errorParams.monostatic->posVelErr.positionDecorr).decorrRate, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.rangeBias, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.clockFreqSF, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.collectionStartTime, 1.0);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.rangeBiasDecorr->corrCoefZero, 0.5);
    TEST_ASSERT_EQ(errorParams.monostatic->radarSensor.rangeBiasDecorr->decorrRate, 1.0);
    TEST_ASSERT_EQ(six::value(errorParams.monostatic->tropoError->tropoRangeVertical), 5.0);
    TEST_ASSERT_EQ(six::value(errorParams.monostatic->tropoError->tropoRangeSlant), 5.0);
    const auto& tropoRangeDecorr = value(errorParams.monostatic->tropoError->tropoRangeDecorr);
    TEST_ASSERT_EQ(tropoRangeDecorr.corrCoefZero, 0.5);
    TEST_ASSERT_EQ(tropoRangeDecorr.decorrRate, 1.0);
    TEST_ASSERT_EQ(six::value(errorParams.monostatic->ionoError->ionoRangeVertical), 5.0);
    TEST_ASSERT_EQ(six::value(errorParams.monostatic->ionoError->ionoRangeRateVertical), 5.0);
    TEST_ASSERT_EQ(six::value(errorParams.monostatic->ionoError->ionoRgRgRateCC), 0.5);
    const auto& ionoRangeVertDecorr = six::value(errorParams.monostatic->ionoError->ionoRangeVertDecorr);
    TEST_ASSERT_EQ(ionoRangeVertDecorr.corrCoefZero, 0.5);
    TEST_ASSERT_EQ(ionoRangeVertDecorr.decorrRate, 1.0);

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
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[0]->geometryLatLon.size(), static_cast<size_t>(4));
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
    TEST_ASSERT_EQ(geoInfo[0].geoInfos[1]->geometryLatLon.size(), static_cast<size_t>(2));
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
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon.size(), static_cast<size_t>(5));
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[0].getLat(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[0].getLon(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[1].getLat(), 1.1);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[1].getLon(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[4].getLat(), 1.0);
    TEST_ASSERT_EQ(geoInfo[1].geometryLatLon[4].getLon(), 1.0);

    const cphd::MatchInformation& matchInfo = *(metadata->matchInfo);
    TEST_ASSERT_EQ(matchInfo.types.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(matchInfo.types[0].typeID, "STEREO");
    TEST_ASSERT_EQ(matchInfo.types[0].currentIndex, 1);
    TEST_ASSERT_EQ(matchInfo.types[0].matchCollects.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(matchInfo.types[0].matchCollects[0].coreName, "CollectionName");
    TEST_ASSERT_EQ(matchInfo.types[0].matchCollects[0].matchIndex, 1);
    TEST_ASSERT_EQ(matchInfo.types[0].matchCollects[0].parameters[0].getName(), "param1");
    TEST_ASSERT_EQ(matchInfo.types[1].typeID, "COHERENT");
    TEST_ASSERT_EQ(matchInfo.types[1].currentIndex, 1);
    TEST_ASSERT_EQ(matchInfo.types[1].matchCollects.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(matchInfo.types[1].matchCollects[0].coreName, "CollectionName");
    TEST_ASSERT_EQ(matchInfo.types[1].matchCollects[0].matchIndex, 1);
    TEST_ASSERT_EQ(matchInfo.types[1].matchCollects[0].parameters[0].getName(), "param1");
}

TEST_MAIN(
        TEST_CHECK(testOptional);
)