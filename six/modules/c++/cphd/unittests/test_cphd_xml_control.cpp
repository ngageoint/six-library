#include <iostream>
#include <memory>

#include <cphd/CPHDXMLControl.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/SceneCoordinates.h>
#include <io/MockSeekableInputStream.h>
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
"                <LineSpacing>6.28</LineSpacing>\n"
"                <FirstLine>8</FirstLine>\n"
"                <NumLines>100</NumLines>\n"
"            </IAYExtent>\n"
"            <SegmentList>\n"
"                <NumSegments>2</NumSegments>\n"
"                <Segment>\n"
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
"        <NumBytesPVP>23</NumBytesPVP>\n"
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
"        </Parameters>\n"
"    </Channel>\n"
"</CPHD>";
}

TEST_CASE(testReadXML)
{
    io::MockSeekableInputStream cphdStream(XML);
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
    TEST_ASSERT_EQ(scene.imageGrid->yExtent.lineSpacing, 6.28);
    TEST_ASSERT_EQ(scene.imageGrid->yExtent.firstLine, 8);
    TEST_ASSERT_EQ(scene.imageGrid->yExtent.numLines, 100);
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
    TEST_ASSERT_EQ(data.numBytesPVP, 23);
    TEST_ASSERT_EQ(data.channels.size(), 2);
    TEST_ASSERT_EQ(data.channels[0].identifier, "Channel");
    TEST_ASSERT_EQ(data.channels[0].numVectors, 2);
    TEST_ASSERT_EQ(data.channels[0].numSamples, 3);
    TEST_ASSERT_EQ(data.channels[0].signalArrayByteOffset, 0);
    TEST_ASSERT_EQ(data.channels[0].pvpArrayByteOffset, 1);
    TEST_ASSERT_EQ(data.channels[0].compressedSignalSize, 3);
    TEST_ASSERT_EQ(data.signalCompressionID, "Compress");
    TEST_ASSERT_EQ(data.supportArrays.size(), 1);
    TEST_ASSERT_EQ(data.supportArrays[0].identifier, "ID");
    TEST_ASSERT_EQ(data.supportArrays[0].numRows, 25);
    TEST_ASSERT_EQ(data.supportArrays[0].numCols, 70);
    TEST_ASSERT_EQ(data.supportArrays[0].bytesPerElement, 4);
    TEST_ASSERT_EQ(data.supportArrays[0].arrayByteOffset, 3);

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
/*    TEST_ASSERT_EQ(channel.parameters[0].polarization.txPol,
                   PolarizationType::X);
*/
}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        TEST_CHECK(testReadXML);
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
