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
}

TEST_CASE(testReadXML)
{
    io::StringStream cphdStream;
    cphdStream.write(XML, strlen(XML));

    six::xml_lite::MinidomParser xmlParser;
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(cphdStream, cphdStream.available());
    const std::unique_ptr<cphd::Metadata> metadata =
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
    TEST_ASSERT_TRUE(global.tropoParameters.get() != nullptr);
    TEST_ASSERT_EQ(global.tropoParameters->n0, 65.2);
    TEST_ASSERT_EQ(global.tropoParameters->refHeight, cphd::RefHeight::IARP);
    TEST_ASSERT_TRUE(global.ionoParameters.get() != nullptr);
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

    TEST_ASSERT_TRUE(scene.referenceSurface.hae.get() != nullptr);
    TEST_ASSERT_EQ(scene.referenceSurface.hae->uIax.getLat(), 12);
    TEST_ASSERT_EQ(scene.referenceSurface.hae->uIax.getLon(), 24);
    TEST_ASSERT_EQ(scene.referenceSurface.hae->uIay.getLat(), 36);
    TEST_ASSERT_EQ(scene.referenceSurface.hae->uIay.getLon(), 48);

    TEST_ASSERT_EQ(scene.imageArea.x1y1[0], 3.5);
    TEST_ASSERT_EQ(scene.imageArea.x1y1[1], 5.3);
    TEST_ASSERT_EQ(scene.imageArea.x2y2[0], 5.3);
    TEST_ASSERT_EQ(scene.imageArea.x2y2[1], 3.5);
    TEST_ASSERT_EQ(scene.imageArea.polygon.size(), static_cast<size_t>(3));
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

    TEST_ASSERT_TRUE(scene.imageGrid.get() != nullptr);
    TEST_ASSERT_EQ(scene.imageGrid->identifier, "Grid");
    TEST_ASSERT_EQ(scene.imageGrid->iarpLocation.line, 1.23);
    TEST_ASSERT_EQ(scene.imageGrid->iarpLocation.sample, 3.21);
    TEST_ASSERT_EQ(scene.imageGrid->xExtent.lineSpacing, 3.14);
    TEST_ASSERT_EQ(scene.imageGrid->xExtent.firstLine, 4);
    TEST_ASSERT_EQ(scene.imageGrid->xExtent.numLines, static_cast<size_t>(50));
    TEST_ASSERT_EQ(scene.imageGrid->yExtent.sampleSpacing, 6.28);
    TEST_ASSERT_EQ(scene.imageGrid->yExtent.firstSample, 8);
    TEST_ASSERT_EQ(scene.imageGrid->yExtent.numSamples, static_cast<size_t>(100));
    TEST_ASSERT_EQ(scene.imageGrid->segments.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].startLine, 0);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].startSample, 1);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].endLine, 2);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].endSample, 3);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[0].line, 0.4);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[0].sample, 0.6);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[1].line, 0.8);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[1].sample, 1.2);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[2].line, 1.2);
    TEST_ASSERT_EQ(scene.imageGrid->segments[0].polygon[2].sample, 1.8);

    // Data
    const cphd::Data& data = metadata->data;
    TEST_ASSERT_EQ(data.signalArrayFormat, cphd::SignalArrayFormat::CI4);
    TEST_ASSERT_EQ(data.numBytesPVP, static_cast<size_t>(24));
    TEST_ASSERT_EQ(data.channels.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(data.channels[0].identifier, "Channel");
    TEST_ASSERT_EQ(data.channels[0].numVectors, static_cast<size_t>(2));
    TEST_ASSERT_EQ(data.channels[0].numSamples, static_cast<size_t>(3));
    TEST_ASSERT_EQ(data.channels[0].signalArrayByteOffset, static_cast<size_t>(0));
    TEST_ASSERT_EQ(data.channels[0].pvpArrayByteOffset, static_cast<size_t>(1));
    TEST_ASSERT_EQ(data.channels[0].compressedSignalSize, static_cast<size_t>(3));
    TEST_ASSERT_EQ(data.signalCompressionID, "Compress");
    TEST_ASSERT_EQ(data.supportArrayMap.size(), static_cast<size_t>(3));
    const std::string identifier = "1.0";
    TEST_ASSERT_EQ(data.supportArrayMap.count(identifier), static_cast<size_t>(1));
    TEST_ASSERT_EQ(data.supportArrayMap.find(identifier)->second.numRows, static_cast<size_t>(3));
    TEST_ASSERT_EQ(data.supportArrayMap.find(identifier)->second.numCols, static_cast<size_t>(4));
    TEST_ASSERT_EQ(data.supportArrayMap.find(identifier)->second.bytesPerElement, static_cast<size_t>(8));
    TEST_ASSERT_EQ(data.supportArrayMap.find(identifier)->second.arrayByteOffset, static_cast<size_t>(0));

    // Channel
    const cphd::Channel& channel = metadata->channel;
    TEST_ASSERT_EQ(channel.refChId, "ChId");
    TEST_ASSERT_EQ(channel.fxFixedCphd, six::BooleanType::IS_TRUE);
    TEST_ASSERT_EQ(channel.toaFixedCphd, six::BooleanType::IS_FALSE);
    TEST_ASSERT_EQ(channel.srpFixedCphd, six::BooleanType::IS_TRUE);
    TEST_ASSERT_EQ(channel.parameters.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(channel.parameters[0].identifier, "CPI");
    TEST_ASSERT_EQ(channel.parameters[0].refVectorIndex, static_cast<size_t>(1));
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
    TEST_ASSERT_EQ(channel.parameters[0].txRcv->rcvId[0], "RcvId");

    TEST_ASSERT_EQ(channel.parameters[0].tgtRefLevel->ptRef, 12.0);

    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->pnRef, 0.5);
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->bnRef, 0.8);

    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point[0].fx, 0.3);
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point[0].pn, 2.7);
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point[1].fx, 0.5);
    TEST_ASSERT_EQ(channel.parameters[0].noiseLevel->fxNoiseProfile->point[1].pn, 2.7);

    TEST_ASSERT_EQ(channel.addedParameters[0].getName(), "AddedParameter1");
    TEST_ASSERT_EQ(channel.addedParameters[0].str(), "Param");
    TEST_ASSERT_EQ(channel.addedParameters[1].getName(), "AddedParameter2");
    TEST_ASSERT_EQ(channel.addedParameters[0].str(), "Param");


    //PVP
    const cphd::Pvp& pvp = metadata->pvp;
    TEST_ASSERT_EQ(pvp.txTime.getOffset(), static_cast<size_t>(0));
    TEST_ASSERT_EQ(pvp.txTime.getSize(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(pvp.txTime.getFormat(), "F8");
    TEST_ASSERT_EQ(pvp.txPos.getOffset(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(pvp.txPos.getSize(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(pvp.txPos.getFormat(), "X=F8;Y=F8;Z=F8;");
    TEST_ASSERT_EQ(pvp.rcvVel.getOffset(), static_cast<size_t>(11));
    TEST_ASSERT_EQ(pvp.rcvVel.getSize(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(pvp.rcvVel.getFormat(), "X=F8;Y=F8;Z=F8;");
    TEST_ASSERT_EQ(pvp.addedPVP.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(pvp.addedPVP.find("newParam1")->second.getName(), "newParam1");
    TEST_ASSERT_EQ(pvp.addedPVP.find("newParam1")->second.getOffset(), static_cast<size_t>(27));
    TEST_ASSERT_EQ(pvp.addedPVP.find("newParam2")->second.getName(), "newParam2");
    TEST_ASSERT_EQ(pvp.addedPVP.find("newParam2")->second.getOffset(), static_cast<size_t>(28));

    //Dwell
    const cphd::Dwell& dwell = metadata->dwell;
    TEST_ASSERT_EQ(dwell.cod.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(dwell.dtime.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(dwell.cod[0].identifier, "codPolynomial1");
    TEST_ASSERT_EQ(dwell.dtime[0].identifier, "dwellPolynomial1");
    TEST_ASSERT_EQ(dwell.cod[0].codTimePoly.orderX(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(dwell.cod[0].codTimePoly.orderY(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(dwell.dtime[0].dwellTimePoly.orderX(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(dwell.dtime[0].dwellTimePoly.orderY(), static_cast<size_t>(1));

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
    TEST_ASSERT_EQ(ref.monostatic->arpPos[0], 10);
    TEST_ASSERT_EQ(ref.monostatic->arpPos[1], 10);
    TEST_ASSERT_EQ(ref.monostatic->arpPos[2], 10);
    TEST_ASSERT_EQ(ref.monostatic->arpVel[0], 10);
    TEST_ASSERT_EQ(ref.monostatic->arpVel[1], 10);
    TEST_ASSERT_EQ(ref.monostatic->arpVel[2], 10);
    TEST_ASSERT(ref.monostatic->sideOfTrack == "LEFT");
    TEST_ASSERT_EQ(ref.monostatic->sideOfTrack, six::SideOfTrackType::LEFT);
    TEST_ASSERT_EQ(ref.monostatic->azimuthAngle, 30.0);
    TEST_ASSERT_EQ(ref.monostatic->grazeAngle, 30.0);
    TEST_ASSERT_EQ(ref.monostatic->twistAngle, 30.0);
    TEST_ASSERT_EQ(ref.monostatic->slopeAngle, 30.0);
    TEST_ASSERT_EQ(ref.monostatic->layoverAngle, 30.0);
    TEST_ASSERT_EQ(ref.monostatic->dopplerConeAngle, 30.0);
}

TEST_MAIN((void)argv; (void)argc;
        TEST_CHECK(testReadXML);
        // TEST_CHECK(testValidation);
)