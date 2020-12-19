/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>

#include <import/cli.h>
#include <import/six.h>
#include <import/io.h>

// For SICD implementation
#include <import/six/sicd.h>
#include "TestCase.h"

#define  MYTEST_ASSERT_EQ(X1, X2) \
{\
    std::string testNameOrig = testName; \
    testName += " " + str::toString(#X1); \
    TEST_ASSERT_EQ(X1, X2); \
    testName = testNameOrig; \
}

#define  MYTEST_ASSERT_ALMOST_EQ(X1, X2) \
{ \
    std::string testNameOrig = testName; \
    testName += " " + str::toString(#X1); \
    TEST_ASSERT_ALMOST_EQ(X1, X2); \
    testName = testNameOrig; \
}

#define  MYTEST_CASE_PARAMS(X) \
    void X(std::string testName, std::vector<std::string> params)

#define  MYTEST_CHECK_PARAMS(X) \
    X(#X,params); fprintf(stderr, "%-35s : PASSED\n", #X);

enum { FRMT_0_4_0 = 0, FRMT_0_4_1 = 1, FRMT_1_0_0 = 2};

/*!
*  We want to create a SICD NITF from something
*  else.  For this simple example, I will use
*  sio.lite to read in the image data.
*
*  SICD data is populated using the ComplexData data structure.
*
*  The segmentation loophole can be exploitated by overriding the product
*  size (essentially bluffing the 10GB limit, and overriding ILOC_R=99999,
*  although you may not extend those limits -- they are NITF format maxes.
*/

std::string  initCollectionInformationXML(unsigned int version)
{
    std::string xmlText("");
    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<CollectionInfo>")+
        std::string("<CollectorName>CollectorName</CollectorName>")+
        std::string("<IlluminatorName>IlluminatorName</IlluminatorName>")+
        std::string("<CoreName>CoreName</CoreName>")+
        std::string("<CollectType>MONOSTATIC</CollectType>")+
        std::string("<RadarMode>")+
        std::string("<ModeType>SPOTLIGHT</ModeType>")+
        std::string("<ModeID>radarModeID</ModeID>")+
        std::string("</RadarMode>")+
        std::string("<Classification>UNCLASS</Classification>")+
        std::string("<CountryCode>US</CountryCode>")+
        std::string("<CountryCode>FR</CountryCode>")+
        std::string("<CountryCode>UK</CountryCode>")+
        std::string("<CountryCode>IT</CountryCode>")+
        std::string("<CountryCode>AU</CountryCode>")+
        std::string("<Parameter name=\"CollectInfoParamName1\">CollectInfoParamVal1</Parameter>")+
        std::string("<Parameter name=\"CollectInfoParamName2\">CollectInfoParamVal2</Parameter>")+
        std::string("<Parameter name=\"CollectInfoParamName3\">CollectInfoParamVal3</Parameter>")+
        std::string("<Parameter name=\"CollectInfoParamName4\">CollectInfoParamVal4</Parameter>")+
        std::string("<Parameter name=\"CollectInfoParamName5\">CollectInfoParamVal5</Parameter>")+
        std::string("</CollectionInfo>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<CollectionInfo>")+
        std::string("<CollectorName class=\"xs:string\">CollectorName</CollectorName>")+
        std::string("<IlluminatorName class=\"xs:string\">IlluminatorName</IlluminatorName>")+
        std::string("<CoreName class=\"xs:string\">CoreName</CoreName>")+
        std::string("<CollectType class=\"xs:string\">MONOSTATIC</CollectType>")+
        std::string("<RadarMode>")+
        std::string("<ModeType class=\"xs:string\">SPOTLIGHT</ModeType>")+
        std::string("<ModeID class=\"xs:string\">radarModeID</ModeID>")+
        std::string("</RadarMode>")+
        std::string("<Classification class=\"xs:string\">UNCLASS</Classification>")+
        std::string("<CountryCode class=\"xs:string\">US</CountryCode>")+
        std::string("<CountryCode class=\"xs:string\">FR</CountryCode>")+
        std::string("<CountryCode class=\"xs:string\">UK</CountryCode>")+
        std::string("<CountryCode class=\"xs:string\">IT</CountryCode>")+
        std::string("<CountryCode class=\"xs:string\">AU</CountryCode>")+
        std::string("<Parameter class=\"xs:string\" name=\"CollectInfoParamName1\">CollectInfoParamVal1</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"CollectInfoParamName2\">CollectInfoParamVal2</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"CollectInfoParamName3\">CollectInfoParamVal3</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"CollectInfoParamName4\">CollectInfoParamVal4</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"CollectInfoParamName5\">CollectInfoParamVal5</Parameter>")+
        std::string("</CollectionInfo>");

    return xmlText;
}


std::string  initImageCreationXML(unsigned int version)
{
    std::string xmlText("");
    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<ImageCreation>")+
        std::string("<Application>Application</Application>")+
        std::string("<DateTime>2012-10-04T20:42:08.609000Z</DateTime>")+
        std::string("<Site>Site</Site>")+
        std::string("<Profile>Profile</Profile>")+
        std::string("</ImageCreation>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<ImageCreation>")+
        std::string("<Application class=\"xs:string\">Application</Application>")+
        std::string("<DateTime class=\"xs:dateTime\">2012-10-04T20:42:07.172000Z</DateTime>")+
        std::string("<Site class=\"xs:string\">Site</Site>")+
        std::string("<Profile class=\"xs:string\">Profile</Profile>")+
        std::string("</ImageCreation>");

    return xmlText;
}


std::string  initImageDataXML(unsigned int version)
{
    std::string xmlText("");
    char amplText[120];

    if (version == FRMT_1_0_0)
    {
        xmlText =
            std::string("<ImageData>")+
            std::string("<PixelType>RE32F_IM32F</PixelType>")+
            std::string("<AmpTable size=\"256\">");
        for (unsigned int i=0; i<256; ++i)
        {
            sprintf(amplText, "<Amplitude index=\"%d\">%s</Amplitude>",
                i, six::toString((double)i).c_str());
            xmlText += std::string(amplText);
        }
        xmlText +=
            std::string("</AmpTable>")+
            std::string("<NumRows>1000</NumRows>")+
            std::string("<NumCols>1000</NumCols>")+
            std::string("<FirstRow>0</FirstRow>")+
            std::string("<FirstCol>0</FirstCol>")+
            std::string("<FullImage>")+
            std::string("<NumRows>1000</NumRows>")+
            std::string("<NumCols>1000</NumCols>")+
            std::string("</FullImage>")+
            std::string("<SCPPixel>")+
            std::string("<Row>500</Row>")+
            std::string("<Col>500</Col>")+
            std::string("</SCPPixel>")+
            std::string("<ValidData size=\"3\">")+
            std::string("<Vertex index=\"1\">")+
            std::string("<Row>111</Row>")+
            std::string("<Col>111</Col>")+
            std::string("</Vertex>")+
            std::string("<Vertex index=\"2\">")+
            std::string("<Row>333</Row>")+
            std::string("<Col>333</Col>")+
            std::string("</Vertex>")+
            std::string("<Vertex index=\"3\">")+
            std::string("<Row>660</Row>")+
            std::string("<Col>660</Col>")+
            std::string("</Vertex>")+
            std::string("</ValidData>")+
            std::string("</ImageData>");
    }

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
    {
        xmlText =
            std::string("<ImageData>")+
            std::string("<PixelType class=\"xs:string\">RE32F_IM32F</PixelType>")+
            std::string("<AmpTable size=\"256\">");
        for (unsigned int i=0; i<256; ++i)
        {
            sprintf(amplText, "<Amplitude class=\"xs:double\" index=\"%d\">%s</Amplitude>",
                i, six::toString((double)i).c_str());
            xmlText += std::string(amplText);
        }
        xmlText +=
            std::string("</AmpTable>")+
            std::string("<NumRows class=\"xs:int\">1000</NumRows>")+
            std::string("<NumCols class=\"xs:int\">1000</NumCols>")+
            std::string("<FirstRow class=\"xs:int\">0</FirstRow>")+
            std::string("<FirstCol class=\"xs:int\">0</FirstCol>")+
            std::string("<FullImage>")+
            std::string("<NumRows class=\"xs:int\">1000</NumRows>")+
            std::string("<NumCols class=\"xs:int\">1000</NumCols>")+
            std::string("</FullImage>")+
            std::string("<SCPPixel>")+
            std::string("<Row class=\"xs:int\">500</Row>")+
            std::string("<Col class=\"xs:int\">500</Col>")+
            std::string("</SCPPixel>")+
            std::string("<ValidData size=\"3\">")+
            std::string("<Vertex index=\"1\">")+
            std::string("<Row class=\"xs:int\">111</Row>")+
            std::string("<Col class=\"xs:int\">111</Col>")+
            std::string("</Vertex>")+
            std::string("<Vertex index=\"2\">")+
            std::string("<Row class=\"xs:int\">333</Row>")+
            std::string("<Col class=\"xs:int\">333</Col>")+
            std::string("</Vertex>")+
            std::string("<Vertex index=\"3\">")+
            std::string("<Row class=\"xs:int\">666</Row>")+
            std::string("<Col class=\"xs:int\">666</Col>")+
            std::string("</Vertex>")+
            std::string("</ValidData>")+
            std::string("</ImageData>");
    }

    return xmlText;
}


std::string initGeoInfoXML(unsigned int version, size_t numInfos = 4, size_t numParams = 5)
{
    std::string xmlText;
    char geoText[256];

    // All GeoInfo blocks start the same
    // TODO: Switch over to use ostringstream rather than sprintf() throughout
    std::ostringstream ostr;
    ostr << "<GeoInfo name=\"geoinfo" << numInfos << "\">";
    xmlText = ostr.str();

    // In 1_0_0, Desc comes before GeoInfo children
    if (version == FRMT_1_0_0)
    {
        for (unsigned int i=0,n=1; i<numParams; ++i,++n)
        {
            sprintf(geoText, "<Desc name=\"GeoInfoParamName%u\">GeoInfoParamVal%u</Desc>", n,n);
            xmlText += std::string(geoText);
        }
    }

    // main loop
    if (version == FRMT_1_0_0 || version == FRMT_0_4_1 || version == FRMT_0_4_0)
    {
        for (unsigned int i = 0; i < numInfos; ++i)
        {
            xmlText += initGeoInfoXML(version, numInfos - 1, numParams);
        }
    }

    // Everything has been written to version one, so we can close the block
    if (version == FRMT_1_0_0)
    {
        xmlText += "</GeoInfo>";
    }

    // Still have to fill Desc for earlier version
    if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
    {
        for (unsigned int i=0,n=1; i<numParams; ++i,++n)
        {
            sprintf(geoText, "<Desc class=\"xs:string\" name=\"GeoInfoParamName%u\">GeoInfoParamVal%u</Desc>", n,n);
            xmlText += std::string(geoText);
        }
        xmlText += "</GeoInfo>";
    }
    return xmlText;
}


std::string  initGeoDataXML(unsigned int version, size_t numInfos = 4)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
    {
        xmlText =
            std::string("<GeoData>")+
            std::string("<EarthModel>WGS_84</EarthModel>")+
            std::string("<SCP>")+
            std::string("<ECF>")+
            std::string("<X>1.000000000000000E001</X>")+
            std::string("<Y>2.000000000000000E001</Y>")+
            std::string("<Z>3.000000000000000E001</Z>")+
            std::string("</ECF>")+
            std::string("<LLH>")+
            std::string("<Lat>1.000000000000000E001</Lat>")+
            std::string("<Lon>2.000000000000000E001</Lon>")+
            std::string("<HAE>3.000000000000000E001</HAE>")+
            std::string("</LLH>")+
            std::string("</SCP>")+
            std::string("<ImageCorners>")+
            std::string("<ICP index=\"1:FRFC\">")+
            std::string("<Lat>-2.000000000000000E001</Lat>")+
            std::string("<Lon>2.000000000000000E001</Lon>")+
            std::string("</ICP>")+
            std::string("<ICP index=\"2:FRLC\">")+
            std::string("<Lat>2.000000000000000E001</Lat>")+
            std::string("<Lon>2.000000000000000E001</Lon>")+
            std::string("</ICP>")+
            std::string("<ICP index=\"3:LRLC\">")+
            std::string("<Lat>2.000000000000000E001</Lat>")+
            std::string("<Lon>-2.000000000000000E001</Lon>")+
            std::string("</ICP>")+
            std::string("<ICP index=\"4:LRFC\">")+
            std::string("<Lat>-2.000000000000000E001</Lat>")+
            std::string("<Lon>-2.000000000000000E001</Lon>")+
            std::string("</ICP>")+
            std::string("</ImageCorners>");
    }

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
    {
        xmlText =
            std::string("<GeoData>")+
            std::string("<EarthModel class=\"xs:string\">WGS_84</EarthModel>")+
            std::string("<SCP>")+
            std::string("<ECF>")+
            std::string("<X class=\"xs:double\">1.000000000000000E001</X>")+
            std::string("<Y class=\"xs:double\">2.000000000000000E001</Y>")+
            std::string("<Z class=\"xs:double\">3.000000000000000E001</Z>")+
            std::string("</ECF>")+
            std::string("<LLH>")+
            std::string("<Lat class=\"xs:double\">1.000000000000000E001</Lat>")+
            std::string("<Lon class=\"xs:double\">2.000000000000000E001</Lon>")+
            std::string("<HAE class=\"xs:double\">3.000000000000000E001</HAE>")+
            std::string("</LLH>")+
            std::string("</SCP>")+
            std::string("<ImageCorners>")+
            std::string("<ICP index=\"1:FRFC\">")+
            std::string("<Lat class=\"xs:double\">-2.000000000000000E001</Lat>")+
            std::string("<Lon class=\"xs:double\">2.000000000000000E001</Lon>")+
            std::string("</ICP>")+
            std::string("<ICP index=\"2:FRLC\">")+
            std::string("<Lat class=\"xs:double\">2.000000000000000E001</Lat>")+
            std::string("<Lon class=\"xs:double\">2.000000000000000E001</Lon>")+
            std::string("</ICP>")+
            std::string("<ICP index=\"3:LRLC\">")+
            std::string("<Lat class=\"xs:double\">2.000000000000000E001</Lat>")+
            std::string("<Lon class=\"xs:double\">-2.000000000000000E001</Lon>")+
            std::string("</ICP>")+
            std::string("<ICP index=\"4:LRFC\">")+
            std::string("<Lat class=\"xs:double\">-2.000000000000000E001</Lat>")+
            std::string("<Lon class=\"xs:double\">-2.000000000000000E001</Lon>")+
            std::string("</ICP>")+
            std::string("</ImageCorners>");
    }

    if (version == FRMT_1_0_0 || version == FRMT_0_4_1 || version == FRMT_0_4_0)
    {
        for (unsigned int i=0; i<numInfos; ++i)
            xmlText += initGeoInfoXML(version, numInfos-1, 5);
        xmlText += "</GeoData>";
    }

    return xmlText;
}


std::string  initGridXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
    {
        xmlText =
            std::string("<Grid>")+
            std::string("<ImagePlane>GROUND</ImagePlane>")+
            std::string("<Type>XRGYCR</Type>")+
            std::string("<TimeCOAPoly order1=\"1\" order2=\"3\">")+
            std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.000000000000000E000</Coef>")+
            std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.000100000000000E000</Coef>")+
            std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.000200000000000E000</Coef>")+
            std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.000300000000000E000</Coef>")+
            std::string("<Coef exponent1=\"1\" exponent2=\"0\">1.010000000000000E000</Coef>")+
            std::string("<Coef exponent1=\"1\" exponent2=\"1\">1.010100000000000E000</Coef>")+
            std::string("<Coef exponent1=\"1\" exponent2=\"2\">1.010200000000000E000</Coef>")+
            std::string("<Coef exponent1=\"1\" exponent2=\"3\">1.010300000000000E000</Coef>")+
            std::string("</TimeCOAPoly>");
    }

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
    {
        xmlText =
            std::string("<Grid>")+
            std::string("<ImagePlane class=\"xs:string\">GROUND</ImagePlane>")+
            std::string("<Type class=\"xs:string\">XRGYCR</Type>")+
            std::string("<TimeCOAPoly order1=\"1\" order2=\"3\">")+
            std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.000000000000000E000</Coef>")+
            std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.000100000000000E000</Coef>")+
            std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.000200000000000E000</Coef>")+
            std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.000300000000000E000</Coef>")+
            std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">1.010000000000000E000</Coef>")+
            std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">1.010100000000000E000</Coef>")+
            std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">1.010200000000000E000</Coef>")+
            std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">1.010300000000000E000</Coef>")+
            std::string("</TimeCOAPoly>");
    }

    if (version == FRMT_1_0_0 || version == FRMT_0_4_1 || version == FRMT_0_4_0)
    {
        for (unsigned int k=0; k<2; ++k)
        {
            xmlText += (k==0) ? "<Row>" : "<Col>";

            if (version == FRMT_1_0_0)
                xmlText +=
                std::string("<UVectECF>")+
                std::string("<X>1.000000000000000E001</X>")+
                std::string("<Y>1.000000000000000E001</Y>")+
                std::string("<Z>1.000000000000000E001</Z>")+
                std::string("</UVectECF>")+
                std::string("<SS>1.000000000000000E001</SS>")+
                std::string("<ImpRespWid>1.100000000000000E002</ImpRespWid>")+
                std::string("<Sgn>+1</Sgn>")+
                std::string("<ImpRespBW>1.100000000000000E002</ImpRespBW>")+
                std::string("<KCtr>1.000000000000000E001</KCtr>")+
                std::string("<DeltaK1>1.100000000000000E001</DeltaK1>")+
                std::string("<DeltaK2>1.200000000000000E001</DeltaK2>")+
                std::string("<DeltaKCOAPoly order1=\"2\" order2=\"3\">")+
                std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.000000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.000100000000000E000</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.000200000000000E000</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.000300000000000E000</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"0\">1.010000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"1\">1.010100000000000E000</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"2\">1.010200000000000E000</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"3\">1.010300000000000E000</Coef>")+
                std::string("<Coef exponent1=\"2\" exponent2=\"0\">1.020000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"2\" exponent2=\"1\">1.020100000000000E000</Coef>")+
                std::string("<Coef exponent1=\"2\" exponent2=\"2\">1.020200000000000E000</Coef>")+
                std::string("<Coef exponent1=\"2\" exponent2=\"3\">1.020300000000000E000</Coef>")+
                std::string("</DeltaKCOAPoly>")+
                std::string("<WgtType>")+
                std::string("<WindowName>WindowName1</WindowName>")+
                std::string("<Parameter name=\"WgtParamName1\">WgtParamVal1</Parameter>")+
                std::string("<Parameter name=\"WgtParamName2\">WgtParamVal2</Parameter>")+
                std::string("<Parameter name=\"WgtParamName3\">WgtParamVal3</Parameter>")+
                std::string("<Parameter name=\"WgtParamName4\">WgtParamVal4</Parameter>")+
                std::string("<Parameter name=\"WgtParamName5\">WgtParamVal5</Parameter>")+
                std::string("</WgtType>")+
                std::string("<WgtFunct size=\"9\">")+
                std::string("<Wgt index=\"1\">0.000000000000000E000</Wgt>")+
                std::string("<Wgt index=\"2\">1.000000000000000E000</Wgt>")+
                std::string("<Wgt index=\"3\">2.000000000000000E000</Wgt>")+
                std::string("<Wgt index=\"4\">3.000000000000000E000</Wgt>")+
                std::string("<Wgt index=\"5\">4.000000000000000E000</Wgt>")+
                std::string("<Wgt index=\"6\">5.000000000000000E000</Wgt>")+
                std::string("<Wgt index=\"7\">6.000000000000000E000</Wgt>")+
                std::string("<Wgt index=\"8\">7.000000000000000E000</Wgt>")+
                std::string("<Wgt index=\"9\">8.000000000000000E000</Wgt>")+
                std::string("</WgtFunct>");

            else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
                xmlText +=
                std::string("<UVectECF>")+
                std::string("<X class=\"xs:double\">1.000000000000000E001</X>")+
                std::string("<Y class=\"xs:double\">1.000000000000000E001</Y>")+
                std::string("<Z class=\"xs:double\">1.000000000000000E001</Z>")+
                std::string("</UVectECF>")+
                std::string("<SS class=\"xs:double\">1.000000000000000E001</SS>")+
                std::string("<ImpRespWid class=\"xs:double\">1.100000000000000E002</ImpRespWid>")+
                std::string("<Sgn class=\"xs:int\">+1</Sgn>")+
                std::string("<ImpRespBW class=\"xs:double\">1.100000000000000E002</ImpRespBW>")+
                std::string("<KCtr class=\"xs:double\">1.000000000000000E001</KCtr>")+
                std::string("<DeltaK1 class=\"xs:double\">1.100000000000000E001</DeltaK1>")+
                std::string("<DeltaK2 class=\"xs:double\">1.200000000000000E001</DeltaK2>")+
                std::string("<DeltaKCOAPoly order1=\"2\" order2=\"3\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.000000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.000100000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.000200000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.000300000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">1.010000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">1.010100000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">1.010200000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">1.010300000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"0\">1.020000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"1\">1.020100000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"2\">1.020200000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"3\">1.020300000000000E000</Coef>")+
                std::string("</DeltaKCOAPoly>")+
                std::string("<WgtType class=\"xs:string\">WindowName1</WgtType>")+
                std::string("<WgtFunct size=\"9\">")+
                std::string("<Wgt class=\"xs:double\" index=\"1\">0.000000000000000E000</Wgt>")+
                std::string("<Wgt class=\"xs:double\" index=\"2\">1.000000000000000E000</Wgt>")+
                std::string("<Wgt class=\"xs:double\" index=\"3\">2.000000000000000E000</Wgt>")+
                std::string("<Wgt class=\"xs:double\" index=\"4\">3.000000000000000E000</Wgt>")+
                std::string("<Wgt class=\"xs:double\" index=\"5\">4.000000000000000E000</Wgt>")+
                std::string("<Wgt class=\"xs:double\" index=\"6\">5.000000000000000E000</Wgt>")+
                std::string("<Wgt class=\"xs:double\" index=\"7\">6.000000000000000E000</Wgt>")+
                std::string("<Wgt class=\"xs:double\" index=\"8\">7.000000000000000E000</Wgt>")+
                std::string("<Wgt class=\"xs:double\" index=\"9\">8.000000000000000E000</Wgt>")+
                std::string("</WgtFunct>");

            xmlText += (k==0) ? "</Row>" : "</Col>";
        }

        xmlText += "</Grid>";
    }

    return xmlText;
}


std::string  initTimelineXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<Timeline>")+
        std::string("<CollectStart>2012-10-04T20:42:08.609000Z</CollectStart>")+
        std::string("<CollectDuration>1.112223330000000E002</CollectDuration>")+
        std::string("<IPP size=\"3\">")+
        std::string("<Set index=\"1\">")+
        std::string("<TStart>1.111110000000000E002</TStart>")+
        std::string("<TEnd>2.222220000000000E002</TEnd>")+
        std::string("<IPPStart>11</IPPStart>")+
        std::string("<IPPEnd>22</IPPEnd>")+
        std::string("<IPPPoly order1=\"1\">")+
        std::string("<Coef exponent1=\"0\">1.000000000000000E-001</Coef>")+
        std::string("<Coef exponent1=\"1\">2.000000000000000E-001</Coef>")+
        std::string("</IPPPoly>")+
        std::string("</Set>")+
        std::string("<Set index=\"2\">")+
        std::string("<TStart>2.222220000000000E002</TStart>")+
        std::string("<TEnd>4.444440000000000E002</TEnd>")+
        std::string("<IPPStart>22</IPPStart>")+
        std::string("<IPPEnd>44</IPPEnd>")+
        std::string("<IPPPoly order1=\"2\">")+
        std::string("<Coef exponent1=\"0\">1.000000000000000E-001</Coef>")+
        std::string("<Coef exponent1=\"1\">2.000000000000000E-001</Coef>")+
        std::string("<Coef exponent1=\"2\">3.000000000000000E-001</Coef>")+
        std::string("</IPPPoly>")+
        std::string("</Set>")+
        std::string("<Set index=\"3\">")+
        std::string("<TStart>3.333330000000000E002</TStart>")+
        std::string("<TEnd>6.666660000000001E002</TEnd>")+
        std::string("<IPPStart>33</IPPStart>")+
        std::string("<IPPEnd>66</IPPEnd>")+
        std::string("<IPPPoly order1=\"3\">")+
        std::string("<Coef exponent1=\"0\">1.000000000000000E-001</Coef>")+
        std::string("<Coef exponent1=\"1\">2.000000000000000E-001</Coef>")+
        std::string("<Coef exponent1=\"2\">3.000000000000000E-001</Coef>")+
        std::string("<Coef exponent1=\"3\">4.000000000000000E-001</Coef>")+
        std::string("</IPPPoly>")+
        std::string("</Set>")+
        std::string("</IPP>")+
        std::string("</Timeline>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<Timeline>")+
        std::string("<CollectStart class=\"xs:dateTime\">2012-10-04T20:42:07.172000Z</CollectStart>")+
        std::string("<CollectDuration class=\"xs:double\">1.112223330000000E002</CollectDuration>")+
        std::string("<IPP size=\"3\">")+
        std::string("<Set index=\"1\">")+
        std::string("<TStart class=\"xs:double\">1.111110000000000E002</TStart>")+
        std::string("<TEnd class=\"xs:double\">2.222220000000000E002</TEnd>")+
        std::string("<IPPStart class=\"xs:int\">11</IPPStart>")+
        std::string("<IPPEnd class=\"xs:int\">22</IPPEnd>")+
        std::string("<IPPPoly order1=\"1\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.000000000000000E-001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">2.000000000000000E-001</Coef>")+
        std::string("</IPPPoly>")+
        std::string("</Set>")+
        std::string("<Set index=\"2\">")+
        std::string("<TStart class=\"xs:double\">2.222220000000000E002</TStart>")+
        std::string("<TEnd class=\"xs:double\">4.444440000000000E002</TEnd>")+
        std::string("<IPPStart class=\"xs:int\">22</IPPStart>")+
        std::string("<IPPEnd class=\"xs:int\">44</IPPEnd>")+
        std::string("<IPPPoly order1=\"2\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.000000000000000E-001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">2.000000000000000E-001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">3.000000000000000E-001</Coef>")+
        std::string("</IPPPoly>")+
        std::string("</Set>")+
        std::string("<Set index=\"3\">")+
        std::string("<TStart class=\"xs:double\">3.333330000000000E002</TStart>")+
        std::string("<TEnd class=\"xs:double\">6.666660000000001E002</TEnd>")+
        std::string("<IPPStart class=\"xs:int\">33</IPPStart>")+
        std::string("<IPPEnd class=\"xs:int\">66</IPPEnd>")+
        std::string("<IPPPoly order1=\"3\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.000000000000000E-001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">2.000000000000000E-001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">3.000000000000000E-001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\">4.000000000000000E-001</Coef>")+
        std::string("</IPPPoly>")+
        std::string("</Set>")+
        std::string("</IPP>")+
        std::string("</Timeline>");

    return xmlText;
}


std::string  initPositionXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<Position>")+
        std::string("<ARPPoly>")+
        std::string("<X order1=\"0\">")+
        std::string("<Coef exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"0\">")+
        std::string("<Coef exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"0\">")+
        std::string("<Coef exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</ARPPoly>")+
        std::string("<GRPPoly>")+
        std::string("<X order1=\"1\">")+
        std::string("<Coef exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"1\">")+
        std::string("<Coef exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"1\">")+
        std::string("<Coef exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</GRPPoly>")+
        std::string("<TxAPCPoly>")+
        std::string("<X order1=\"2\">")+
        std::string("<Coef exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">5.678000000000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"2\">")+
        std::string("<Coef exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">5.678000000000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"2\">")+
        std::string("<Coef exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">5.678000000000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</TxAPCPoly>")+
        std::string("<RcvAPC size=\"2\">")+
        std::string("<RcvAPCPoly index=\"1\">")+
        std::string("<X order1=\"1\">")+
        std::string("<Coef exponent1=\"0\">1.111111100000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">2.222222200000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"1\">")+
        std::string("<Coef exponent1=\"0\">1.111111100000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">2.222222200000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"1\">")+
        std::string("<Coef exponent1=\"0\">1.111111100000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">2.222222200000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</RcvAPCPoly>")+
        std::string("<RcvAPCPoly index=\"2\">")+
        std::string("<X order1=\"2\">")+
        std::string("<Coef exponent1=\"0\">2.222222200000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">3.333333300000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">4.444444400000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"2\">")+
        std::string("<Coef exponent1=\"0\">2.222222200000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">3.333333300000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">4.444444400000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"2\">")+
        std::string("<Coef exponent1=\"0\">2.222222200000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">3.333333300000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">4.444444400000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</RcvAPCPoly>")+
        std::string("</RcvAPC>")+
        std::string("</Position>");

    if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<Position>")+
        std::string("<ARPPoly>")+
        std::string("<X order1=\"0\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"0\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"0\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</ARPPoly>")+
        std::string("<GRPPoly>")+
        std::string("<X order1=\"1\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"1\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"1\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</GRPPoly>")+
        std::string("<TxAPCPoly>")+
        std::string("<X order1=\"2\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">5.678000000000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"2\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">5.678000000000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"2\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.234000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">3.456000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">5.678000000000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</TxAPCPoly>")+
        std::string("<RcvAPC size=\"2\">")+
        std::string("<RcvAPCPoly index=\"1\">")+
        std::string("<X order1=\"1\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111111100000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">2.222222200000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"1\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111111100000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">2.222222200000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"1\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111111100000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">2.222222200000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</RcvAPCPoly>")+
        std::string("<RcvAPCPoly index=\"2\">")+
        std::string("<X order1=\"2\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">2.222222200000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">3.333333300000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">4.444444400000000E001</Coef>")+
        std::string("</X>")+
        std::string("<Y order1=\"2\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">2.222222200000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">3.333333300000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">4.444444400000000E001</Coef>")+
        std::string("</Y>")+
        std::string("<Z order1=\"2\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">2.222222200000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">3.333333300000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">4.444444400000000E001</Coef>")+
        std::string("</Z>")+
        std::string("</RcvAPCPoly>")+
        std::string("</RcvAPC>")+
        std::string("</Position>");

    return xmlText;
}


std::string  initRadarCollectionXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<RadarCollection>")+
        std::string("<TxFrequency>")+
        std::string("<Min>-1.230000000000000E002</Min>")+
        std::string("<Max>1.230000000000000E002</Max>")+
        std::string("</TxFrequency>")+
        std::string("<RefFreqIndex>12345</RefFreqIndex>")+
        std::string("<Waveform size=\"2\">")+
        std::string("<WFParameters index=\"1\">")+
        std::string("<TxPulseLength>1.010000000000000E000</TxPulseLength>")+
        std::string("<TxRFBandwidth>1.020000000000000E000</TxRFBandwidth>")+
        std::string("<TxFreqStart>1.030000000000000E000</TxFreqStart>")+
        std::string("<TxFMRate>1.040000000000000E000</TxFMRate>")+
        std::string("<RcvDemodType>CHIRP</RcvDemodType>")+
        std::string("<RcvWindowLength>1.050000000000000E000</RcvWindowLength>")+
        std::string("<ADCSampleRate>1.060000000000000E000</ADCSampleRate>")+
        std::string("<RcvIFBandwidth>1.070000000000000E000</RcvIFBandwidth>")+
        std::string("<RcvFreqStart>1.080000000000000E000</RcvFreqStart>")+
        std::string("<RcvFMRate>1.090000000000000E000</RcvFMRate>")+
        std::string("</WFParameters>")+
        std::string("<WFParameters index=\"2\">")+
        std::string("<TxPulseLength>2.010000000000000E000</TxPulseLength>")+
        std::string("<TxRFBandwidth>2.020000000000000E000</TxRFBandwidth>")+
        std::string("<TxFreqStart>2.030000000000000E000</TxFreqStart>")+
        std::string("<TxFMRate>2.040000000000000E000</TxFMRate>")+
        std::string("<RcvDemodType>CHIRP</RcvDemodType>")+
        std::string("<RcvWindowLength>2.050000000000000E000</RcvWindowLength>")+
        std::string("<ADCSampleRate>2.060000000000000E000</ADCSampleRate>")+
        std::string("<RcvIFBandwidth>2.070000000000000E000</RcvIFBandwidth>")+
        std::string("<RcvFreqStart>2.080000000000000E000</RcvFreqStart>")+
        std::string("<RcvFMRate>2.090000000000000E000</RcvFMRate>")+
        std::string("</WFParameters>")+
        std::string("</Waveform>")+
        std::string("<TxPolarization>RHC</TxPolarization>")+
        std::string("<TxSequence size=\"2\">")+
        std::string("<TxStep index=\"1\">")+
        std::string("<WFIndex>1</WFIndex>")+
        std::string("<TxPolarization>RHC</TxPolarization>")+
        std::string("</TxStep>")+
        std::string("<TxStep index=\"2\">")+
        std::string("<WFIndex>2</WFIndex>")+
        std::string("<TxPolarization>RHC</TxPolarization>")+
        std::string("</TxStep>")+
        std::string("</TxSequence>")+
        std::string("<RcvChannels size=\"3\">")+
        std::string("<ChanParameters index=\"1\">")+
        std::string("<TxRcvPolarization>OTHER</TxRcvPolarization>")+
        std::string("<RcvAPCIndex>1</RcvAPCIndex>")+
        std::string("</ChanParameters>")+
        std::string("<ChanParameters index=\"2\">")+
        std::string("<TxRcvPolarization>V:V</TxRcvPolarization>")+
        std::string("<RcvAPCIndex>2</RcvAPCIndex>")+
        std::string("</ChanParameters>")+
        std::string("<ChanParameters index=\"3\">")+
        std::string("<TxRcvPolarization>V:H</TxRcvPolarization>")+
        std::string("<RcvAPCIndex>3</RcvAPCIndex>")+
        std::string("</ChanParameters>")+
        std::string("</RcvChannels>")+
        std::string("<Area>")+
        std::string("<Corner>")+
        std::string("<ACP index=\"1\">")+
        std::string("<Lat>-2.000000000000000E001</Lat>")+
        std::string("<Lon>2.000000000000000E001</Lon>")+
        std::string("<HAE>2.100000000000000E002</HAE>")+
        std::string("</ACP>")+
        std::string("<ACP index=\"2\">")+
        std::string("<Lat>2.000000000000000E001</Lat>")+
        std::string("<Lon>2.000000000000000E001</Lon>")+
        std::string("<HAE>2.200000000000000E002</HAE>")+
        std::string("</ACP>")+
        std::string("<ACP index=\"3\">")+
        std::string("<Lat>2.000000000000000E001</Lat>")+
        std::string("<Lon>-2.000000000000000E001</Lon>")+
        std::string("<HAE>2.400000000000000E002</HAE>")+
        std::string("</ACP>")+
        std::string("<ACP index=\"4\">")+
        std::string("<Lat>-2.000000000000000E001</Lat>")+
        std::string("<Lon>-2.000000000000000E001</Lon>")+
        std::string("<HAE>2.300000000000000E002</HAE>")+
        std::string("</ACP>")+
        std::string("</Corner>")+
        std::string("<Plane>")+
        std::string("<RefPt>")+
        std::string("<ECF>")+
        std::string("<X>1.100000000000000E000</X>")+
        std::string("<Y>2.200000000000000E000</Y>")+
        std::string("<Z>3.300000000000000E000</Z>")+
        std::string("</ECF>")+
        std::string("<Line>4.400000000000000E000</Line>")+
        std::string("<Sample>5.500000000000000E000</Sample>")+
        std::string("</RefPt>")+
        std::string("<XDir>")+
        std::string("<UVectECF>")+
        std::string("<X>1.230000000000000E001</X>")+
        std::string("<Y>4.560000000000000E001</Y>")+
        std::string("<Z>7.890000000000001E001</Z>")+
        std::string("</UVectECF>")+
        std::string("<LineSpacing>1.230000000000000E002</LineSpacing>")+
        std::string("<NumLines>9</NumLines>")+
        std::string("<FirstLine>11</FirstLine>")+
        std::string("</XDir>")+
        std::string("<YDir>")+
        std::string("<UVectECF>")+
        std::string("<X>1.230000000000000E001</X>")+
        std::string("<Y>4.560000000000000E001</Y>")+
        std::string("<Z>7.890000000000001E001</Z>")+
        std::string("</UVectECF>")+
        std::string("<SampleSpacing>1.230000000000000E002</SampleSpacing>")+
        std::string("<NumSamples>9</NumSamples>")+
        std::string("<FirstSample>11</FirstSample>")+
        std::string("</YDir>")+
        std::string("<SegmentList size=\"3\">")+
        std::string("<Segment index=\"1\">")+
        std::string("<StartLine>11</StartLine>")+
        std::string("<StartSample>22</StartSample>")+
        std::string("<EndLine>88</EndLine>")+
        std::string("<EndSample>99</EndSample>")+
        std::string("<Identifier>segmentxxx</Identifier>")+
        std::string("</Segment>")+
        std::string("<Segment index=\"2\">")+
        std::string("<StartLine>22</StartLine>")+
        std::string("<StartSample>44</StartSample>")+
        std::string("<EndLine>176</EndLine>")+
        std::string("<EndSample>198</EndSample>")+
        std::string("<Identifier>segmentxxx</Identifier>")+
        std::string("</Segment>")+
        std::string("<Segment index=\"3\">")+
        std::string("<StartLine>33</StartLine>")+
        std::string("<StartSample>66</StartSample>")+
        std::string("<EndLine>264</EndLine>")+
        std::string("<EndSample>297</EndSample>")+
        std::string("<Identifier>segmentxxx</Identifier>")+
        std::string("</Segment>")+
        std::string("</SegmentList>")+
        std::string("<Orientation>ARBITRARY</Orientation>")+
        std::string("</Plane>")+
        std::string("</Area>")+
        std::string("<Parameter name=\"RadarCollectionParamName1\">RadarCollectionParamVal1</Parameter>")+
        std::string("<Parameter name=\"RadarCollectionParamName2\">RadarCollectionParamVal2</Parameter>")+
        std::string("<Parameter name=\"RadarCollectionParamName3\">RadarCollectionParamVal3</Parameter>")+
        std::string("<Parameter name=\"RadarCollectionParamName4\">RadarCollectionParamVal4</Parameter>")+
        std::string("</RadarCollection>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<RadarCollection>")+
        std::string("<RefFreqIndex class=\"xs:int\">12345</RefFreqIndex>")+
        std::string("<TxFrequency>")+
        std::string("<Min class=\"xs:double\">-1.230000000000000E002</Min>")+
        std::string("<Max class=\"xs:double\">1.230000000000000E002</Max>")+
        std::string("</TxFrequency>")+
        std::string("<TxPolarization class=\"xs:string\">RHC</TxPolarization>")+
        std::string("<PolarizationHVAnglePoly order1=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.110000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">2.220000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">3.330000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\">4.440000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"4\">5.550000000000001E000</Coef>")+
        std::string("</PolarizationHVAnglePoly>")+
        std::string("<TxSequence size=\"2\">")+
        std::string("<TxStep index=\"1\">")+
        std::string("<WFIndex class=\"xs:int\">1</WFIndex>")+
        std::string("<TxPolarization class=\"xs:string\">RHC</TxPolarization>")+
        std::string("</TxStep>")+
        std::string("<TxStep index=\"2\">")+
        std::string("<WFIndex class=\"xs:int\">2</WFIndex>")+
        std::string("<TxPolarization class=\"xs:string\">RHC</TxPolarization>")+
        std::string("</TxStep>")+
        std::string("</TxSequence>")+
        std::string("<Waveform size=\"2\">")+
        std::string("<WFParameters index=\"1\">")+
        std::string("<TxPulseLength class=\"xs:double\">1.010000000000000E000</TxPulseLength>")+
        std::string("<TxRFBandwidth class=\"xs:double\">1.020000000000000E000</TxRFBandwidth>")+
        std::string("<TxFreqStart class=\"xs:double\">1.030000000000000E000</TxFreqStart>")+
        std::string("<TxFMRate class=\"xs:double\">1.040000000000000E000</TxFMRate>")+
        std::string("<RcvDemodType class=\"xs:string\">CHIRP</RcvDemodType>")+
        std::string("<RcvWindowLength class=\"xs:double\">1.050000000000000E000</RcvWindowLength>")+
        std::string("<ADCSampleRate class=\"xs:double\">1.060000000000000E000</ADCSampleRate>")+
        std::string("<RcvIFBandwidth class=\"xs:double\">1.070000000000000E000</RcvIFBandwidth>")+
        std::string("<RcvFreqStart class=\"xs:double\">1.080000000000000E000</RcvFreqStart>")+
        std::string("<RcvFMRate class=\"xs:double\">1.090000000000000E000</RcvFMRate>")+
        std::string("</WFParameters>")+
        std::string("<WFParameters index=\"2\">")+
        std::string("<TxPulseLength class=\"xs:double\">2.010000000000000E000</TxPulseLength>")+
        std::string("<TxRFBandwidth class=\"xs:double\">2.020000000000000E000</TxRFBandwidth>")+
        std::string("<TxFreqStart class=\"xs:double\">2.030000000000000E000</TxFreqStart>")+
        std::string("<TxFMRate class=\"xs:double\">2.040000000000000E000</TxFMRate>")+
        std::string("<RcvDemodType class=\"xs:string\">CHIRP</RcvDemodType>")+
        std::string("<RcvWindowLength class=\"xs:double\">2.050000000000000E000</RcvWindowLength>")+
        std::string("<ADCSampleRate class=\"xs:double\">2.060000000000000E000</ADCSampleRate>")+
        std::string("<RcvIFBandwidth class=\"xs:double\">2.070000000000000E000</RcvIFBandwidth>")+
        std::string("<RcvFreqStart class=\"xs:double\">2.080000000000000E000</RcvFreqStart>")+
        std::string("<RcvFMRate class=\"xs:double\">2.090000000000000E000</RcvFMRate>")+
        std::string("</WFParameters>")+
        std::string("</Waveform>")+
        std::string("<RcvChannels size=\"3\">")+
        std::string("<ChanParameters index=\"1\">")+
        std::string("<RcvAPCIndex class=\"xs:int\">1</RcvAPCIndex>")+
        std::string("<TxRcvPolarization class=\"xs:string\">OTHER</TxRcvPolarization>")+
        std::string("</ChanParameters>")+
        std::string("<ChanParameters index=\"2\">")+
        std::string("<RcvAPCIndex class=\"xs:int\">2</RcvAPCIndex>")+
        std::string("<TxRcvPolarization class=\"xs:string\">V:V</TxRcvPolarization>")+
        std::string("</ChanParameters>")+
        std::string("<ChanParameters index=\"3\">")+
        std::string("<RcvAPCIndex class=\"xs:int\">3</RcvAPCIndex>")+
        std::string("<TxRcvPolarization class=\"xs:string\">V:H</TxRcvPolarization>")+
        std::string("</ChanParameters>")+
        std::string("</RcvChannels>")+
        std::string("<Area>")+
        std::string("<Corner>")+
        std::string("<ACP index=\"1\">")+
        std::string("<Lat class=\"xs:double\">-2.000000000000000E001</Lat>")+
        std::string("<Lon class=\"xs:double\">2.000000000000000E001</Lon>")+
        std::string("<HAE class=\"xs:double\">2.100000000000000E002</HAE>")+
        std::string("</ACP>")+
        std::string("<ACP index=\"2\">")+
        std::string("<Lat class=\"xs:double\">2.000000000000000E001</Lat>")+
        std::string("<Lon class=\"xs:double\">2.000000000000000E001</Lon>")+
        std::string("<HAE class=\"xs:double\">2.200000000000000E002</HAE>")+
        std::string("</ACP>")+
        std::string("<ACP index=\"3\">")+
        std::string("<Lat class=\"xs:double\">2.000000000000000E001</Lat>")+
        std::string("<Lon class=\"xs:double\">-2.000000000000000E001</Lon>")+
        std::string("<HAE class=\"xs:double\">2.400000000000000E002</HAE>")+
        std::string("</ACP>")+
        std::string("<ACP index=\"4\">")+
        std::string("<Lat class=\"xs:double\">-2.000000000000000E001</Lat>")+
        std::string("<Lon class=\"xs:double\">-2.000000000000000E001</Lon>")+
        std::string("<HAE class=\"xs:double\">2.300000000000000E002</HAE>")+
        std::string("</ACP>")+
        std::string("</Corner>")+
        std::string("<Plane>")+
        std::string("<RefPt>")+
        std::string("<ECF>")+
        std::string("<X class=\"xs:double\">1.100000000000000E000</X>")+
        std::string("<Y class=\"xs:double\">2.200000000000000E000</Y>")+
        std::string("<Z class=\"xs:double\">3.300000000000000E000</Z>")+
        std::string("</ECF>")+
        std::string("<Line class=\"xs:double\">4.400000000000000E000</Line>")+
        std::string("<Sample class=\"xs:double\">5.500000000000000E000</Sample>")+
        std::string("</RefPt>")+
        std::string("<XDir>")+
        std::string("<UVectECF>")+
        std::string("<X class=\"xs:double\">1.230000000000000E001</X>")+
        std::string("<Y class=\"xs:double\">4.560000000000000E001</Y>")+
        std::string("<Z class=\"xs:double\">7.890000000000001E001</Z>")+
        std::string("</UVectECF>")+
        std::string("<LineSpacing class=\"xs:double\">1.230000000000000E002</LineSpacing>")+
        std::string("<NumLines class=\"xs:int\">9</NumLines>")+
        std::string("<FirstLine class=\"xs:int\">11</FirstLine>")+
        std::string("</XDir>")+
        std::string("<YDir>")+
        std::string("<UVectECF>")+
        std::string("<X class=\"xs:double\">1.230000000000000E001</X>")+
        std::string("<Y class=\"xs:double\">4.560000000000000E001</Y>")+
        std::string("<Z class=\"xs:double\">7.890000000000001E001</Z>")+
        std::string("</UVectECF>")+
        std::string("<SampleSpacing class=\"xs:double\">1.230000000000000E002</SampleSpacing>")+
        std::string("<NumSamples class=\"xs:int\">9</NumSamples>")+
        std::string("<FirstSample class=\"xs:int\">11</FirstSample>")+
        std::string("</YDir>")+
        std::string("<SegmentList size=\"3\">")+
        std::string("<Segment index=\"1\">")+
        std::string("<StartLine class=\"xs:int\">11</StartLine>")+
        std::string("<StartSample class=\"xs:int\">22</StartSample>")+
        std::string("<EndLine class=\"xs:int\">88</EndLine>")+
        std::string("<EndSample class=\"xs:int\">99</EndSample>")+
        std::string("<Identifier class=\"xs:string\">segmentxxx</Identifier>")+
        std::string("</Segment>")+
        std::string("<Segment index=\"2\">")+
        std::string("<StartLine class=\"xs:int\">22</StartLine>")+
        std::string("<StartSample class=\"xs:int\">44</StartSample>")+
        std::string("<EndLine class=\"xs:int\">176</EndLine>")+
        std::string("<EndSample class=\"xs:int\">198</EndSample>")+
        std::string("<Identifier class=\"xs:string\">segmentxxx</Identifier>")+
        std::string("</Segment>")+
        std::string("<Segment index=\"3\">")+
        std::string("<StartLine class=\"xs:int\">33</StartLine>")+
        std::string("<StartSample class=\"xs:int\">66</StartSample>")+
        std::string("<EndLine class=\"xs:int\">264</EndLine>")+
        std::string("<EndSample class=\"xs:int\">297</EndSample>")+
        std::string("<Identifier class=\"xs:string\">segmentxxx</Identifier>")+
        std::string("</Segment>")+
        std::string("</SegmentList>")+
        std::string("<Orientation class=\"xs:string\">ARBITRARY</Orientation>")+
        std::string("</Plane>")+
        std::string("</Area>")+
        std::string("<Parameter class=\"xs:string\" name=\"RadarCollectionParamName1\">RadarCollectionParamVal1</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"RadarCollectionParamName2\">RadarCollectionParamVal2</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"RadarCollectionParamName3\">RadarCollectionParamVal3</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"RadarCollectionParamName4\">RadarCollectionParamVal4</Parameter>")+
        std::string("</RadarCollection>");

    return xmlText;
}


std::string  initImageFormationXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<ImageFormation>")+
        std::string("<RcvChanProc>")+
        std::string("<NumChanProc>1</NumChanProc>")+
        std::string("<PRFScaleFactor>9.999000000000000E001</PRFScaleFactor>")+
        std::string("<ChanIndex>1</ChanIndex>")+
        std::string("<ChanIndex>2</ChanIndex>")+
        std::string("<ChanIndex>3</ChanIndex>")+
        std::string("</RcvChanProc>")+
        std::string("<TxRcvPolarizationProc>V:V</TxRcvPolarizationProc>")+
        std::string("<TStartProc>1.110000000000000E002</TStartProc>")+
        std::string("<TEndProc>9.990000000000000E002</TEndProc>")+
        std::string("<TxFrequencyProc>")+
        std::string("<MinProc>0.000000000000000E000</MinProc>")+
        std::string("<MaxProc>0.000000000000000E000</MaxProc>")+
        std::string("</TxFrequencyProc>")+
        std::string("<SegmentIdentifier>abcdef</SegmentIdentifier>")+
        std::string("<ImageFormAlgo>RMA</ImageFormAlgo>")+
        std::string("<STBeamComp>SV</STBeamComp>")+
        std::string("<ImageBeamComp>SV</ImageBeamComp>")+
        std::string("<AzAutofocus>SV</AzAutofocus>")+
        std::string("<RgAutofocus>SV</RgAutofocus>")+
        std::string("<Processing>")+
        std::string("<Type>ProcessingType0</Type>")+
        std::string("<Applied>true</Applied>")+
        std::string("<Parameter name=\"ImageFormationParamName1\">ImageFormationParamVal1</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName2\">ImageFormationParamVal2</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName3\">ImageFormationParamVal3</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName4\">ImageFormationParamVal4</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName5\">ImageFormationParamVal5</Parameter>")+
        std::string("</Processing>")+
        std::string("<Processing>")+
        std::string("<Type>ProcessingType1</Type>")+
        std::string("<Applied>true</Applied>")+
        std::string("<Parameter name=\"ImageFormationParamName1\">ImageFormationParamVal1</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName2\">ImageFormationParamVal2</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName3\">ImageFormationParamVal3</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName4\">ImageFormationParamVal4</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName5\">ImageFormationParamVal5</Parameter>")+
        std::string("</Processing>")+
        std::string("<Processing>")+
        std::string("<Type>ProcessingType2</Type>")+
        std::string("<Applied>true</Applied>")+
        std::string("<Parameter name=\"ImageFormationParamName1\">ImageFormationParamVal1</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName2\">ImageFormationParamVal2</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName3\">ImageFormationParamVal3</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName4\">ImageFormationParamVal4</Parameter>")+
        std::string("<Parameter name=\"ImageFormationParamName5\">ImageFormationParamVal5</Parameter>")+
        std::string("</Processing>")+
        std::string("<PolarizationCalibration>")+
        std::string("<DistortCorrectionApplied>true</DistortCorrectionApplied>")+
        std::string("<Distortion>")+
        std::string("<CalibrationDate>2012-10-04T20:42:08.609000Z</CalibrationDate>")+
        std::string("<A>1.000000000000000E000</A>")+
        std::string("<F1>")+
        std::string("<Real>1.000000000000000E000</Real>")+
        std::string("<Imag>2.000000000000000E000</Imag>")+
        std::string("</F1>")+
        std::string("<Q1>")+
        std::string("<Real>1.000000000000000E000</Real>")+
        std::string("<Imag>2.000000000000000E000</Imag>")+
        std::string("</Q1>")+
        std::string("<Q2>")+
        std::string("<Real>1.000000000000000E000</Real>")+
        std::string("<Imag>2.000000000000000E000</Imag>")+
        std::string("</Q2>")+
        std::string("<F2>")+
        std::string("<Real>3.000000000000000E000</Real>")+
        std::string("<Imag>4.000000000000000E000</Imag>")+
        std::string("</F2>")+
        std::string("<Q3>")+
        std::string("<Real>3.000000000000000E000</Real>")+
        std::string("<Imag>4.000000000000000E000</Imag>")+
        std::string("</Q3>")+
        std::string("<Q4>")+
        std::string("<Real>3.000000000000000E000</Real>")+
        std::string("<Imag>4.000000000000000E000</Imag>")+
        std::string("</Q4>")+
        std::string("<GainErrorA>5.000000000000000E000</GainErrorA>")+
        std::string("<GainErrorF1>5.000000000000000E000</GainErrorF1>")+
        std::string("<GainErrorF2>5.000000000000000E000</GainErrorF2>")+
        std::string("<PhaseErrorF1>6.000000000000000E000</PhaseErrorF1>")+
        std::string("<PhaseErrorF2>6.000000000000000E000</PhaseErrorF2>")+
        std::string("</Distortion>")+
        std::string("</PolarizationCalibration>")+
        std::string("</ImageFormation>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<ImageFormation>")+
        std::string("<SegmentIdentifier class=\"xs:string\">abcdef</SegmentIdentifier>")+
        std::string("<RcvChanProc>")+
        std::string("<NumChanProc class=\"xs:int\">1</NumChanProc>")+
        std::string("<PRFScaleFactor class=\"xs:double\">9.999000000000000E001</PRFScaleFactor>")+
        std::string("<ChanIndex class=\"xs:int\">1</ChanIndex>")+
        std::string("<ChanIndex class=\"xs:int\">2</ChanIndex>")+
        std::string("<ChanIndex class=\"xs:int\">3</ChanIndex>")+
        std::string("</RcvChanProc>")+
        std::string("<TxRcvPolarizationProc class=\"xs:string\">V:V</TxRcvPolarizationProc>")+
        std::string("<ImageFormAlgo class=\"xs:string\">RMA</ImageFormAlgo>")+
        std::string("<TStartProc class=\"xs:double\">1.110000000000000E002</TStartProc>")+
        std::string("<TEndProc class=\"xs:double\">9.990000000000000E002</TEndProc>")+
        std::string("<TxFrequencyProc>")+
        std::string("<MinProc class=\"xs:double\">0.000000000000000E000</MinProc>")+
        std::string("<MaxProc class=\"xs:double\">0.000000000000000E000</MaxProc>")+
        std::string("</TxFrequencyProc>")+
        std::string("<STBeamComp class=\"xs:string\">SV</STBeamComp>")+
        std::string("<ImageBeamComp class=\"xs:string\">SV</ImageBeamComp>")+
        std::string("<AzAutofocus class=\"xs:string\">SV</AzAutofocus>")+
        std::string("<RgAutofocus class=\"xs:string\">SV</RgAutofocus>")+
        std::string("<Processing>")+
        std::string("<Type class=\"xs:string\">ProcessingType0</Type>")+
        std::string("<Applied class=\"xs:boolean\">true</Applied>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName1\">ImageFormationParamVal1</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName2\">ImageFormationParamVal2</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName3\">ImageFormationParamVal3</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName4\">ImageFormationParamVal4</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName5\">ImageFormationParamVal5</Parameter>")+
        std::string("</Processing>")+
        std::string("<Processing>")+
        std::string("<Type class=\"xs:string\">ProcessingType1</Type>")+
        std::string("<Applied class=\"xs:boolean\">true</Applied>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName1\">ImageFormationParamVal1</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName2\">ImageFormationParamVal2</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName3\">ImageFormationParamVal3</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName4\">ImageFormationParamVal4</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName5\">ImageFormationParamVal5</Parameter>")+
        std::string("</Processing>")+
        std::string("<Processing>")+
        std::string("<Type class=\"xs:string\">ProcessingType2</Type>")+
        std::string("<Applied class=\"xs:boolean\">true</Applied>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName1\">ImageFormationParamVal1</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName2\">ImageFormationParamVal2</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName3\">ImageFormationParamVal3</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName4\">ImageFormationParamVal4</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ImageFormationParamName5\">ImageFormationParamVal5</Parameter>")+
        std::string("</Processing>")+
        std::string("<PolarizationCalibration>")+
        std::string("<HVAngleCompApplied class=\"xs:boolean\">true</HVAngleCompApplied>")+
        std::string("<DistortionCorrectionApplied class=\"xs:boolean\">true</DistortionCorrectionApplied>")+
        std::string("<Distortion>")+
        std::string("<CalibrationDate class=\"xs:dateTime\">2012-10-04T20:42:07.172000Z</CalibrationDate>")+
        std::string("<A class=\"xs:double\">1.000000000000000E000</A>")+
        std::string("<F1>")+
        std::string("<Real class=\"xs:double\">1.000000000000000E000</Real>")+
        std::string("<Imag class=\"xs:double\">2.000000000000000E000</Imag>")+
        std::string("</F1>")+
        std::string("<Q1>")+
        std::string("<Real class=\"xs:double\">1.000000000000000E000</Real>")+
        std::string("<Imag class=\"xs:double\">2.000000000000000E000</Imag>")+
        std::string("</Q1>")+
        std::string("<Q2>")+
        std::string("<Real class=\"xs:double\">1.000000000000000E000</Real>")+
        std::string("<Imag class=\"xs:double\">2.000000000000000E000</Imag>")+
        std::string("</Q2>")+
        std::string("<F2>")+
        std::string("<Real class=\"xs:double\">3.000000000000000E000</Real>")+
        std::string("<Imag class=\"xs:double\">4.000000000000000E000</Imag>")+
        std::string("</F2>")+
        std::string("<Q3>")+
        std::string("<Real class=\"xs:double\">3.000000000000000E000</Real>")+
        std::string("<Imag class=\"xs:double\">4.000000000000000E000</Imag>")+
        std::string("</Q3>")+
        std::string("<Q4>")+
        std::string("<Real class=\"xs:double\">3.000000000000000E000</Real>")+
        std::string("<Imag class=\"xs:double\">4.000000000000000E000</Imag>")+
        std::string("</Q4>")+
        std::string("<GainErrorA class=\"xs:double\">5.000000000000000E000</GainErrorA>")+
        std::string("<GainErrorF1 class=\"xs:double\">5.000000000000000E000</GainErrorF1>")+
        std::string("<GainErrorF2 class=\"xs:double\">5.000000000000000E000</GainErrorF2>")+
        std::string("<PhaseErrorF1 class=\"xs:double\">6.000000000000000E000</PhaseErrorF1>")+
        std::string("<PhaseErrorF2 class=\"xs:double\">6.000000000000000E000</PhaseErrorF2>")+
        std::string("</Distortion>")+
        std::string("</PolarizationCalibration>")+
        std::string("</ImageFormation>");

    return xmlText;
}


std::string  initSCPCOAXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<SCPCOA>")+
        std::string("<SCPTime>1.560574314284600E007</SCPTime>")+
        std::string("<ARPPos>")+
        std::string("<X>1.800000000000000E001</X>")+
        std::string("<Y>1.800000000000000E001</Y>")+
        std::string("<Z>1.800000000000000E001</Z>")+
        std::string("</ARPPos>")+
        std::string("<ARPVel>")+
        std::string("<X>1.900000000000000E001</X>")+
        std::string("<Y>1.900000000000000E001</Y>")+
        std::string("<Z>1.900000000000000E001</Z>")+
        std::string("</ARPVel>")+
        std::string("<ARPAcc>")+
        std::string("<X>2.000000000000000E001</X>")+
        std::string("<Y>2.000000000000000E001</Y>")+
        std::string("<Z>2.000000000000000E001</Z>")+
        std::string("</ARPAcc>")+
        std::string("<SideOfTrack>L</SideOfTrack>")+
        std::string("<SlantRange>1.100000000000000E001</SlantRange>")+
        std::string("<GroundRange>1.200000000000000E001</GroundRange>")+
        std::string("<DopplerConeAng>1.300000000000000E001</DopplerConeAng>")+
        std::string("<GrazeAng>1.400000000000000E001</GrazeAng>")+
        std::string("<IncidenceAng>1.500000000000000E001</IncidenceAng>")+
        std::string("<TwistAng>1.600000000000000E001</TwistAng>")+
        std::string("<SlopeAng>1.700000000000000E001</SlopeAng>")+
        std::string("<AzimAng>7.700000000000000E001</AzimAng>")+
        std::string("<LayoverAng>8.800000000000000E001</LayoverAng>")+
        std::string("</SCPCOA>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<SCPCOA>")+
        std::string("<SCPTime class=\"xs:double\">1.560574314284600E007</SCPTime>")+
        std::string("<ARPPos>")+
        std::string("<X class=\"xs:double\">1.800000000000000E001</X>")+
        std::string("<Y class=\"xs:double\">1.800000000000000E001</Y>")+
        std::string("<Z class=\"xs:double\">1.800000000000000E001</Z>")+
        std::string("</ARPPos>")+
        std::string("<ARPVel>")+
        std::string("<X class=\"xs:double\">1.900000000000000E001</X>")+
        std::string("<Y class=\"xs:double\">1.900000000000000E001</Y>")+
        std::string("<Z class=\"xs:double\">1.900000000000000E001</Z>")+
        std::string("</ARPVel>")+
        std::string("<ARPAcc>")+
        std::string("<X class=\"xs:double\">2.000000000000000E001</X>")+
        std::string("<Y class=\"xs:double\">2.000000000000000E001</Y>")+
        std::string("<Z class=\"xs:double\">2.000000000000000E001</Z>")+
        std::string("</ARPAcc>")+
        std::string("<SideOfTrack class=\"xs:string\">L</SideOfTrack>")+
        std::string("<SlantRange class=\"xs:double\">1.100000000000000E001</SlantRange>")+
        std::string("<GroundRange class=\"xs:double\">1.200000000000000E001</GroundRange>")+
        std::string("<DopplerConeAng class=\"xs:double\">1.300000000000000E001</DopplerConeAng>")+
        std::string("<GrazeAng class=\"xs:double\">1.400000000000000E001</GrazeAng>")+
        std::string("<IncidenceAng class=\"xs:double\">1.500000000000000E001</IncidenceAng>")+
        std::string("<TwistAng class=\"xs:double\">1.600000000000000E001</TwistAng>")+
        std::string("<SlopeAng class=\"xs:double\">1.700000000000000E001</SlopeAng>")+
        std::string("</SCPCOA>");

    return xmlText;
}


std::string  initRadiometricXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<Radiometric>")+
        std::string("<NoiseLevel>")+
        std::string("<NoiseLevelType>ABSOLUTE</NoiseLevelType>")+
        std::string("<NoisePoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.000000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.010000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.020000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.030000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"4\">1.040000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"0\">1.100000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"1\">1.110000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"2\">1.120000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"3\">1.130000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"4\">1.140000000000000E000</Coef>")+
        std::string("</NoisePoly>")+
        std::string("</NoiseLevel>")+
        std::string("<RCSSFPoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef exponent1=\"0\" exponent2=\"0\">2.000000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"1\">2.010000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"2\">2.020000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"3\">2.030000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"4\">2.040000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"0\">2.100000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"1\">2.110000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"2\">2.120000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"3\">2.130000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"4\">2.140000000000000E000</Coef>")+
        std::string("</RCSSFPoly>")+
        std::string("<SigmaZeroSFPoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef exponent1=\"0\" exponent2=\"0\">4.000000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"1\">4.010000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"2\">4.020000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"3\">4.030000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"4\">4.040000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"0\">4.100000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"1\">4.109999999999999E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"2\">4.119999999999999E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"3\">4.130000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"4\">4.140000000000000E000</Coef>")+
        std::string("</SigmaZeroSFPoly>")+
        std::string("<BetaZeroSFPoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef exponent1=\"0\" exponent2=\"0\">3.000000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"1\">3.010000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"2\">3.020000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"3\">3.030000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"4\">3.040000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"0\">3.100000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"1\">3.110000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"2\">3.120000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"3\">3.130000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"4\">3.140000000000000E000</Coef>")+
        std::string("</BetaZeroSFPoly>")+
        std::string("<GammaZeroSFPoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef exponent1=\"0\" exponent2=\"0\">5.000000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"1\">5.010000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"2\">5.020000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"3\">5.030000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"4\">5.040000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"0\">5.100000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"1\">5.109999999999999E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"2\">5.119999999999999E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"3\">5.130000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"4\">5.140000000000000E000</Coef>")+
        std::string("</GammaZeroSFPoly>")+
        std::string("</Radiometric>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<Radiometric>")+
        std::string("<NoisePoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.000000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.010000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.020000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.030000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">1.040000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">1.100000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">1.110000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">1.120000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">1.130000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">1.140000000000000E000</Coef>")+
        std::string("</NoisePoly>")+
        std::string("<RCSSFPoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">2.000000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">2.010000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">2.020000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">2.030000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">2.040000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">2.100000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">2.110000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">2.120000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">2.130000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">2.140000000000000E000</Coef>")+
        std::string("</RCSSFPoly>")+
        std::string("<BetaZeroSFPoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">3.000000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">3.010000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">3.020000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">3.030000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">3.040000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">3.100000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">3.110000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">3.120000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">3.130000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">3.140000000000000E000</Coef>")+
        std::string("</BetaZeroSFPoly>")+
        std::string("<SigmaZeroSFPoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">4.000000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">4.010000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">4.020000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">4.030000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">4.040000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">4.100000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">4.109999999999999E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">4.119999999999999E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">4.130000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">4.140000000000000E000</Coef>")+
        std::string("</SigmaZeroSFPoly>")+
        std::string("<SigmaZeroSFIncidenceMap class=\"xs:string\">APPLIED</SigmaZeroSFIncidenceMap>")+
        std::string("<GammaZeroSFPoly order1=\"1\" order2=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">5.000000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">5.010000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">5.020000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">5.030000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">5.040000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">5.100000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">5.109999999999999E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">5.119999999999999E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">5.130000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">5.140000000000000E000</Coef>")+
        std::string("</GammaZeroSFPoly>")+
        std::string("<GammaZeroSFIncidenceMap class=\"xs:string\">APPLIED</GammaZeroSFIncidenceMap>")+
        std::string("</Radiometric>");

    return xmlText;
}


std::string  initAntennaXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
    {
        xmlText = "<Antenna>";
        for (unsigned int k=0; k<3; ++k)
        {
            xmlText += (k==0) ? "<Tx>" : (k==1) ? "<Rcv>" : "<TwoWay>";

            xmlText +=
                std::string("<XAxisPoly>")+
                std::string("<X order1=\"0\">")+
                std::string("<Coef exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("</X>")+
                std::string("<Y order1=\"0\">")+
                std::string("<Coef exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("</Y>")+
                std::string("<Z order1=\"0\">")+
                std::string("<Coef exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("</Z>")+
                std::string("</XAxisPoly>")+
                std::string("<YAxisPoly>")+
                std::string("<X order1=\"1\">")+
                std::string("<Coef exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("<Coef exponent1=\"1\">1.111100000000000E002</Coef>")+
                std::string("</X>")+
                std::string("<Y order1=\"1\">")+
                std::string("<Coef exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("<Coef exponent1=\"1\">1.111100000000000E002</Coef>")+
                std::string("</Y>")+
                std::string("<Z order1=\"1\">")+
                std::string("<Coef exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("<Coef exponent1=\"1\">1.111100000000000E002</Coef>")+
                std::string("</Z>")+
                std::string("</YAxisPoly>")+
                std::string("<FreqZero>1.111111100000000E000</FreqZero>")+
                std::string("<EB>")+
                std::string("<DCXPoly order1=\"3\">")+
                std::string("<Coef exponent1=\"0\">1.000000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"1\">1.100000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"2\">1.200000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"3\">1.300000000000000E000</Coef>")+
                std::string("</DCXPoly>")+
                std::string("<DCYPoly order1=\"3\">")+
                std::string("<Coef exponent1=\"0\">1.000000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"1\">1.100000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"2\">1.200000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"3\">1.300000000000000E000</Coef>")+
                std::string("</DCYPoly>")+
                std::string("</EB>")+
                std::string("<Array>")+
                std::string("<GainPoly order1=\"1\" order2=\"4\">")+
                std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.100000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.101000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.102000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.103000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"4\">1.104000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"0\">1.110000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"1\">1.111000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"2\">1.112000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"3\">1.113000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"4\">1.114000000000000E001</Coef>")+
                std::string("</GainPoly>")+
                std::string("<PhasePoly order1=\"1\" order2=\"4\">")+
                std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.200000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.201000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.202000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.203000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"4\">1.204000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"0\">1.210000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"1\">1.211000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"2\">1.212000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"3\">1.213000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"4\">1.214000000000000E001</Coef>")+
                std::string("</PhasePoly>")+
                std::string("</Array>")+
                std::string("<Elem>")+
                std::string("<GainPoly order1=\"1\" order2=\"4\">")+
                std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.300000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.301000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.302000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.303000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"4\">1.304000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"0\">1.310000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"1\">1.311000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"2\">1.312000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"3\">1.313000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"4\">1.314000000000000E001</Coef>")+
                std::string("</GainPoly>")+
                std::string("<PhasePoly order1=\"1\" order2=\"4\">")+
                std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.400000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.401000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.402000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.403000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"0\" exponent2=\"4\">1.404000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"0\">1.410000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"1\">1.411000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"2\">1.412000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"3\">1.413000000000000E001</Coef>")+
                std::string("<Coef exponent1=\"1\" exponent2=\"4\">1.414000000000000E001</Coef>")+
                std::string("</PhasePoly>")+
                std::string("</Elem>")+
                std::string("<GainBSPoly order1=\"3\">")+
                std::string("<Coef exponent1=\"0\">1.000000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"1\">1.100000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"2\">1.200000000000000E000</Coef>")+
                std::string("<Coef exponent1=\"3\">1.300000000000000E000</Coef>")+
                std::string("</GainBSPoly>")+
                std::string("<EBFreqShift>true</EBFreqShift>")+
                std::string("<MLFreqDilation>false</MLFreqDilation>");

            xmlText += (k==0) ? "</Tx>" : (k==1) ? "</Rcv>" : "</TwoWay>";
        }
        xmlText += "</Antenna>";
    }

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
    {
        xmlText = "<Antenna>";
        for (unsigned int k=0; k<3; ++k)
        {
            xmlText += (k==0) ? "<Tx>" : (k==1) ? "<Rcv>" : "<TwoWay>";

            xmlText += std::string("<XAxisPoly>")+
                std::string("<X order1=\"0\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("</X>")+
                std::string("<Y order1=\"0\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("</Y>")+
                std::string("<Z order1=\"0\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("</Z>")+
                std::string("</XAxisPoly>")+
                std::string("<YAxisPoly>")+
                std::string("<X order1=\"1\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\">1.111100000000000E002</Coef>")+
                std::string("</X>")+
                std::string("<Y order1=\"1\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\">1.111100000000000E002</Coef>")+
                std::string("</Y>")+
                std::string("<Z order1=\"1\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111100000000000E002</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\">1.111100000000000E002</Coef>")+
                std::string("</Z>")+
                std::string("</YAxisPoly>")+
                std::string("<FreqZero class=\"xs:double\">1.111111100000000E000</FreqZero>")+
                std::string("<EB>")+
                std::string("<DCXPoly order1=\"3\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.000000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\">1.100000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"2\">1.200000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"3\">1.300000000000000E000</Coef>")+
                std::string("</DCXPoly>")+
                std::string("<DCYPoly order1=\"3\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.000000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\">1.100000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"2\">1.200000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"3\">1.300000000000000E000</Coef>")+
                std::string("</DCYPoly>")+
                std::string("</EB>")+
                std::string("<HPBW>")+
                std::string("<DCX class=\"xs:double\">1.011110000000000E002</DCX>")+
                std::string("<DCY class=\"xs:double\">1.022220000000000E002</DCY>")+
                std::string("</HPBW>")+
                std::string("<Array>")+
                std::string("<GainPoly order1=\"1\" order2=\"4\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.100000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.101000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.102000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.103000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">1.104000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">1.110000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">1.111000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">1.112000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">1.113000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">1.114000000000000E001</Coef>")+
                std::string("</GainPoly>")+
                std::string("<PhasePoly order1=\"1\" order2=\"4\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.200000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.201000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.202000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.203000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">1.204000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">1.210000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">1.211000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">1.212000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">1.213000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">1.214000000000000E001</Coef>")+
                std::string("</PhasePoly>")+
                std::string("</Array>")+
                std::string("<Elem>")+
                std::string("<GainPoly order1=\"1\" order2=\"4\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.300000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.301000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.302000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.303000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">1.304000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">1.310000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">1.311000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">1.312000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">1.313000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">1.314000000000000E001</Coef>")+
                std::string("</GainPoly>")+
                std::string("<PhasePoly order1=\"1\" order2=\"4\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.400000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.401000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.402000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.403000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">1.404000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">1.410000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">1.411000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">1.412000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">1.413000000000000E001</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">1.414000000000000E001</Coef>")+
                std::string("</PhasePoly>")+
                std::string("</Elem>")+
                std::string("<GainBSPoly order1=\"3\">")+
                std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.000000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"1\">1.100000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"2\">1.200000000000000E000</Coef>")+
                std::string("<Coef class=\"xs:double\" exponent1=\"3\">1.300000000000000E000</Coef>")+
                std::string("</GainBSPoly>")+
                std::string("<EBFreqShift class=\"xs:boolean\">true</EBFreqShift>")+
                std::string("<MLFreqDilation class=\"xs:boolean\">false</MLFreqDilation>");

            xmlText += (k==0) ? "</Tx>" : (k==1) ? "</Rcv>" : "</TwoWay>";
        }
        xmlText += "</Antenna>";
    }

    return xmlText;
}


std::string  initErrorStatisticsXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<ErrorStatistics>")+
        std::string("<CompositeSCP>")+
        std::string("<Rg>1.230000000000000E001</Rg>")+
        std::string("<Az>4.560000000000000E001</Az>")+
        std::string("<RgAz>7.890000000000001E001</RgAz>")+
        std::string("</CompositeSCP>")+
        std::string("<Components>")+
        std::string("<PosVelErr>")+
        std::string("<Frame>RIC_ECF</Frame>")+
        std::string("<P1>1.110000000000000E000</P1>")+
        std::string("<P2>2.220000000000000E000</P2>")+
        std::string("<P3>3.330000000000000E000</P3>")+
        std::string("<V1>1.111100000000000E001</V1>")+
        std::string("<V2>2.222200000000000E001</V2>")+
        std::string("<V3>3.333300000000000E001</V3>")+
        std::string("<CorrCoefs>")+
        std::string("<P1P2>1.020000000000000E002</P1P2>")+
        std::string("<P1P3>1.020000000000000E002</P1P3>")+
        std::string("<P1V1>1.110000000000000E002</P1V1>")+
        std::string("<P1V2>1.120000000000000E002</P1V2>")+
        std::string("<P1V3>1.130000000000000E002</P1V3>")+
        std::string("<P2P3>2.030000000000000E002</P2P3>")+
        std::string("<P2V1>2.110000000000000E002</P2V1>")+
        std::string("<P2V2>2.120000000000000E002</P2V2>")+
        std::string("<P2V3>2.130000000000000E002</P2V3>")+
        std::string("<P3V1>3.110000000000000E002</P3V1>")+
        std::string("<P3V2>3.120000000000000E002</P3V2>")+
        std::string("<P3V3>3.130000000000000E002</P3V3>")+
        std::string("<V1V2>1.112000000000000E003</V1V2>")+
        std::string("<V1V3>1.113000000000000E003</V1V3>")+
        std::string("<V2V3>1.213000000000000E003</V2V3>")+
        std::string("</CorrCoefs>")+
        std::string("<PositionDecorr>")+
        std::string("<CorrCoefZero>1.111000000000000E001</CorrCoefZero>")+
        std::string("<DecorrRate>1.111110000000000E003</DecorrRate>")+
        std::string("</PositionDecorr>")+
        std::string("</PosVelErr>")+
        std::string("<RadarSensor>")+
        std::string("<RangeBias>2.221110000000000E002</RangeBias>")+
        std::string("<ClockFreqSF>2.222220000000000E002</ClockFreqSF>")+
        std::string("<TransmitFreqSF>2.223330000000000E002</TransmitFreqSF>")+
        std::string("<RangeBiasDecorr>")+
        std::string("<CorrCoefZero>2.222000000000000E001</CorrCoefZero>")+
        std::string("<DecorrRate>2.222220000000000E003</DecorrRate>")+
        std::string("</RangeBiasDecorr>")+
        std::string("</RadarSensor>")+
        std::string("<TropoError>")+
        std::string("<TropoRangeVertical>3.331110000000000E002</TropoRangeVertical>")+
        std::string("<TropoRangeSlant>3.332220000000000E002</TropoRangeSlant>")+
        std::string("<TropoRangeDecorr>")+
        std::string("<CorrCoefZero>3.333000000000000E001</CorrCoefZero>")+
        std::string("<DecorrRate>3.333330000000000E003</DecorrRate>")+
        std::string("</TropoRangeDecorr>")+
        std::string("</TropoError>")+
        std::string("<IonoError>")+
        std::string("<IonoRangeVertical>4.441110000000000E002</IonoRangeVertical>")+
        std::string("<IonoRangeRateVertical>4.442220000000000E002</IonoRangeRateVertical>")+
        std::string("<IonoRgRgRateCC>4.443330000000000E002</IonoRgRgRateCC>")+
        std::string("<IonoRangeVertDecorr>")+
        std::string("<CorrCoefZero>4.444000000000000E001</CorrCoefZero>")+
        std::string("<DecorrRate>4.444440000000000E003</DecorrRate>")+
        std::string("</IonoRangeVertDecorr>")+
        std::string("</IonoError>")+
        std::string("</Components>")+
        std::string("<AdditionalParms>")+
        std::string("<Parameter name=\"ErrorStatisticsParamName1\">ErrorStatisticsParamVal1</Parameter>")+
        std::string("<Parameter name=\"ErrorStatisticsParamName2\">ErrorStatisticsParamVal2</Parameter>")+
        std::string("<Parameter name=\"ErrorStatisticsParamName3\">ErrorStatisticsParamVal3</Parameter>")+
        std::string("<Parameter name=\"ErrorStatisticsParamName4\">ErrorStatisticsParamVal4</Parameter>")+
        std::string("</AdditionalParms>")+
        std::string("</ErrorStatistics>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<ErrorStatistics>")+
        std::string("<CompositeSCP>")+
        std::string("<RgAzErr>")+
        std::string("<Rg class=\"xs:double\">1.230000000000000E001</Rg>")+
        std::string("<Az class=\"xs:double\">4.560000000000000E001</Az>")+
        std::string("<RgAz class=\"xs:double\">7.890000000000001E001</RgAz>")+
        std::string("</RgAzErr>")+
        std::string("</CompositeSCP>")+
        std::string("<Components>")+
        std::string("<PosVelErr>")+
        std::string("<Frame class=\"xs:string\">RIC_ECF</Frame>")+
        std::string("<P1 class=\"xs:double\">1.110000000000000E000</P1>")+
        std::string("<P2 class=\"xs:double\">2.220000000000000E000</P2>")+
        std::string("<P3 class=\"xs:double\">3.330000000000000E000</P3>")+
        std::string("<V1 class=\"xs:double\">1.111100000000000E001</V1>")+
        std::string("<V2 class=\"xs:double\">2.222200000000000E001</V2>")+
        std::string("<V3 class=\"xs:double\">3.333300000000000E001</V3>")+
        std::string("<CorrCoefs>")+
        std::string("<P1P2 class=\"xs:double\">1.020000000000000E002</P1P2>")+
        std::string("<P1P3 class=\"xs:double\">1.020000000000000E002</P1P3>")+
        std::string("<P1V1 class=\"xs:double\">1.110000000000000E002</P1V1>")+
        std::string("<P1V2 class=\"xs:double\">1.120000000000000E002</P1V2>")+
        std::string("<P1V3 class=\"xs:double\">1.130000000000000E002</P1V3>")+
        std::string("<P2P3 class=\"xs:double\">2.030000000000000E002</P2P3>")+
        std::string("<P2V1 class=\"xs:double\">2.110000000000000E002</P2V1>")+
        std::string("<P2V2 class=\"xs:double\">2.120000000000000E002</P2V2>")+
        std::string("<P2V3 class=\"xs:double\">2.130000000000000E002</P2V3>")+
        std::string("<P3V1 class=\"xs:double\">3.110000000000000E002</P3V1>")+
        std::string("<P3V2 class=\"xs:double\">3.120000000000000E002</P3V2>")+
        std::string("<P3V3 class=\"xs:double\">3.130000000000000E002</P3V3>")+
        std::string("<V1V2 class=\"xs:double\">1.112000000000000E003</V1V2>")+
        std::string("<V1V3 class=\"xs:double\">1.113000000000000E003</V1V3>")+
        std::string("<V2V3 class=\"xs:double\">1.213000000000000E003</V2V3>")+
        std::string("</CorrCoefs>")+
        std::string("<PositionDecorr>")+
        std::string("<CorrCoefZero class=\"xs:double\">1.111000000000000E001</CorrCoefZero>")+
        std::string("<DecorrRate class=\"xs:double\">1.111110000000000E003</DecorrRate>")+
        std::string("</PositionDecorr>")+
        std::string("</PosVelErr>")+
        std::string("<RadarSensor>")+
        std::string("<RangeBias class=\"xs:double\">2.221110000000000E002</RangeBias>")+
        std::string("<ClockFreqSF class=\"xs:double\">2.222220000000000E002</ClockFreqSF>")+
        std::string("<TransmitFreqSF class=\"xs:double\">2.223330000000000E002</TransmitFreqSF>")+
        std::string("<RangeBiasDecorr>")+
        std::string("<CorrCoefZero class=\"xs:double\">2.222000000000000E001</CorrCoefZero>")+
        std::string("<DecorrRate class=\"xs:double\">2.222220000000000E003</DecorrRate>")+
        std::string("</RangeBiasDecorr>")+
        std::string("</RadarSensor>")+
        std::string("<TropoError>")+
        std::string("<TropoRangeVertical class=\"xs:double\">3.331110000000000E002</TropoRangeVertical>")+
        std::string("<TropoRangeSlant class=\"xs:double\">3.332220000000000E002</TropoRangeSlant>")+
        std::string("<TropoRangeDecorr>")+
        std::string("<CorrCoefZero class=\"xs:double\">3.333000000000000E001</CorrCoefZero>")+
        std::string("<DecorrRate class=\"xs:double\">3.333330000000000E003</DecorrRate>")+
        std::string("</TropoRangeDecorr>")+
        std::string("</TropoError>")+
        std::string("<IonoError>")+
        std::string("<IonoRangeVertical class=\"xs:double\">4.441110000000000E002</IonoRangeVertical>")+
        std::string("<IonoRangeRateVertical class=\"xs:double\">4.442220000000000E002</IonoRangeRateVertical>")+
        std::string("<IonoRgRgRateCC class=\"xs:double\">4.443330000000000E002</IonoRgRgRateCC>")+
        std::string("<IonoRangeVertDecorr>")+
        std::string("<CorrCoefZero class=\"xs:double\">4.444000000000000E001</CorrCoefZero>")+
        std::string("<DecorrRate class=\"xs:double\">4.444440000000000E003</DecorrRate>")+
        std::string("</IonoRangeVertDecorr>")+
        std::string("</IonoError>")+
        std::string("</Components>")+
        std::string("<AdditionalParms>")+
        std::string("<Parameter class=\"xs:string\" name=\"ErrorStatisticsParamName1\">ErrorStatisticsParamVal1</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ErrorStatisticsParamName2\">ErrorStatisticsParamVal2</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ErrorStatisticsParamName3\">ErrorStatisticsParamVal3</Parameter>")+
        std::string("<Parameter class=\"xs:string\" name=\"ErrorStatisticsParamName4\">ErrorStatisticsParamVal4</Parameter>")+
        std::string("</AdditionalParms>")+
        std::string("</ErrorStatistics>");

    return xmlText;
}


std::string  initMatchInformationXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<MatchInfo>")+
        std::string("<NumMatchTypes>2</NumMatchTypes>")+
        std::string("<MatchType index=\"1\">")+
        std::string("<TypeID>COHERENT</TypeID>")+
        std::string("<CurrentIndex>1</CurrentIndex>")+
        std::string("<NumMatchCollections>2</NumMatchCollections>")+
        std::string("<MatchCollection index=\"1\">")+
        std::string("<CoreName>coreName</CoreName>")+
        std::string("<MatchIndex>1</MatchIndex>")+
        std::string("<Parameter name=\"MatchCollectParamName1\">MatchCollectParamVal1</Parameter>")+
        std::string("</MatchCollection>")+
        std::string("<MatchCollection index=\"2\">")+
        std::string("<CoreName>coreName</CoreName>")+
        std::string("<MatchIndex>2</MatchIndex>")+
        std::string("<Parameter name=\"MatchCollectParamName1\">MatchCollectParamVal1</Parameter>")+
        std::string("</MatchCollection>")+
        std::string("</MatchType>")+
        std::string("<MatchType index=\"2\">")+
        std::string("<TypeID>COHERENT</TypeID>")+
        std::string("<CurrentIndex>2</CurrentIndex>")+
        std::string("<NumMatchCollections>2</NumMatchCollections>")+
        std::string("<MatchCollection index=\"1\">")+
        std::string("<CoreName>coreName</CoreName>")+
        std::string("<MatchIndex>1</MatchIndex>")+
        std::string("<Parameter name=\"MatchCollectParamName1\">MatchCollectParamVal1</Parameter>")+
        std::string("</MatchCollection>")+
        std::string("<MatchCollection index=\"2\">")+
        std::string("<CoreName>coreName</CoreName>")+
        std::string("<MatchIndex>2</MatchIndex>")+
        std::string("<Parameter name=\"MatchCollectParamName1\">MatchCollectParamVal1</Parameter>")+
        std::string("</MatchCollection>")+
        std::string("</MatchType>")+
        std::string("</MatchInfo>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<MatchInfo>")+
        std::string("<Collect index=\"1\">")+
        std::string("<CollectorName class=\"xs:string\">collector name</CollectorName>")+
        std::string("<IlluminatorName class=\"xs:string\">illuminator name</IlluminatorName>")+
        std::string("<CoreName class=\"xs:string\">coreName</CoreName>")+
        std::string("<MatchType class=\"xs:string\">match type</MatchType>")+
        std::string("<MatchType class=\"xs:string\">match type</MatchType>")+
        std::string("<Parameter class=\"xs:string\" name=\"MatchCollectParamName1\">MatchCollectParamVal1</Parameter>")+
        std::string("</Collect>")+
        std::string("<Collect index=\"2\">")+
        std::string("<CollectorName class=\"xs:string\">collector name</CollectorName>")+
        std::string("<IlluminatorName class=\"xs:string\">illuminator name</IlluminatorName>")+
        std::string("<CoreName class=\"xs:string\">coreName</CoreName>")+
        std::string("<MatchType class=\"xs:string\">match type</MatchType>")+
        std::string("<MatchType class=\"xs:string\">match type</MatchType>")+
        std::string("<Parameter class=\"xs:string\" name=\"MatchCollectParamName1\">MatchCollectParamVal1</Parameter>")+
        std::string("</Collect>")+
        std::string("</MatchInfo>");

    return xmlText;
}


std::string  initPFAXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<PFA>")+
        std::string("<FPN>")+
        std::string("<X>1.000000000000000E001</X>")+
        std::string("<Y>2.000000000000000E001</Y>")+
        std::string("<Z>3.000000000000000E001</Z>")+
        std::string("</FPN>")+
        std::string("<IPN>")+
        std::string("<X>1.001000000000000E001</X>")+
        std::string("<Y>2.002000000000000E001</Y>")+
        std::string("<Z>3.003000000000000E001</Z>")+
        std::string("</IPN>")+
        std::string("<PolarAngRefTime>9.999000000000000E001</PolarAngRefTime>")+
        std::string("<PolarAngPoly order1=\"4\">")+
        std::string("<Coef exponent1=\"0\">2.222000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">4.444000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">6.666000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"3\">8.888000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"4\">1.111000000000000E002</Coef>")+
        std::string("</PolarAngPoly>")+
        std::string("<SpatialFreqSFPoly order1=\"3\">")+
        std::string("<Coef exponent1=\"0\">1.111000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">2.222000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">3.333000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"3\">4.444000000000000E001</Coef>")+
        std::string("</SpatialFreqSFPoly>")+
        std::string("<Krg1>1.000000000000000E001</Krg1>")+
        std::string("<Krg2>2.000000000000000E001</Krg2>")+
        std::string("<Kaz1>1.100000000000000E002</Kaz1>")+
        std::string("<Kaz2>2.200000000000000E002</Kaz2>")+
        std::string("<STDeskew>")+
        std::string("<Applied>true</Applied>")+
        std::string("<STDSPhasePoly order1=\"2\" order2=\"3\">")+
        std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.111111100000000E003</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.111222200000000E003</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.111333300000000E003</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.111444400000000E003</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"0\">2.222111100000000E003</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"1\">2.222222200000000E003</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"2\">2.222333300000000E003</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"3\">2.222444400000000E003</Coef>")+
        std::string("<Coef exponent1=\"2\" exponent2=\"0\">3.333111100000000E003</Coef>")+
        std::string("<Coef exponent1=\"2\" exponent2=\"1\">3.333222200000000E003</Coef>")+
        std::string("<Coef exponent1=\"2\" exponent2=\"2\">3.333333300000000E003</Coef>")+
        std::string("<Coef exponent1=\"2\" exponent2=\"3\">3.333444400000000E003</Coef>")+
        std::string("</STDSPhasePoly>")+
        std::string("</STDeskew>")+
        std::string("</PFA>");

    else if (version == FRMT_0_4_1 || version == FRMT_0_4_0)
        xmlText =
        std::string("<PFA>")+
        std::string("<FPN>")+
        std::string("<X class=\"xs:double\">1.000000000000000E001</X>")+
        std::string("<Y class=\"xs:double\">2.000000000000000E001</Y>")+
        std::string("<Z class=\"xs:double\">3.000000000000000E001</Z>")+
        std::string("</FPN>")+
        std::string("<IPN>")+
        std::string("<X class=\"xs:double\">1.001000000000000E001</X>")+
        std::string("<Y class=\"xs:double\">2.002000000000000E001</Y>")+
        std::string("<Z class=\"xs:double\">3.003000000000000E001</Z>")+
        std::string("</IPN>")+
        std::string("<PolarAngRefTime class=\"xs:double\">9.999000000000000E001</PolarAngRefTime>")+
        std::string("<PolarAngPoly order1=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">2.222000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">4.444000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">6.666000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\">8.888000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"4\">1.111000000000000E002</Coef>")+
        std::string("</PolarAngPoly>")+
        std::string("<SpatialFreqSFPoly order1=\"3\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">2.222000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">3.333000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\">4.444000000000000E001</Coef>")+
        std::string("</SpatialFreqSFPoly>")+
        std::string("<Krg1 class=\"xs:double\">1.000000000000000E001</Krg1>")+
        std::string("<Krg2 class=\"xs:double\">2.000000000000000E001</Krg2>")+
        std::string("<Kaz1 class=\"xs:double\">1.100000000000000E002</Kaz1>")+
        std::string("<Kaz2 class=\"xs:double\">2.200000000000000E002</Kaz2>")+
        std::string("<STDeskew>")+
        std::string("<Applied class=\"xs:boolean\">true</Applied>")+
        std::string("<STDSPhasePoly order1=\"2\" order2=\"3\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.111111100000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.111222200000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.111333300000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.111444400000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">2.222111100000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">2.222222200000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">2.222333300000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">2.222444400000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"0\">3.333111100000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"1\">3.333222200000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"2\">3.333333300000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"3\">3.333444400000000E003</Coef>")+
        std::string("</STDSPhasePoly>")+
        std::string("</STDeskew>")+
        std::string("</PFA>");

    return xmlText;
}


std::string  initRMA_INCAXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<RMA>")+
        std::string("<RMAlgoType>OMEGA_K</RMAlgoType>")+
        std::string("<ImageType>INCA</ImageType>")+
        std::string("<INCA>")+
        std::string("<TimeCAPoly order1=\"4\">")+
        std::string("<Coef exponent1=\"0\">0.000000000000000E000</Coef>")+
        std::string("<Coef exponent1=\"1\">1.111000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">2.222000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"3\">3.333000000000000E001</Coef>")+
        std::string("<Coef exponent1=\"4\">4.444000000000000E001</Coef>")+
        std::string("</TimeCAPoly>")+
        std::string("<R_CA_SCP>1.230000000000000E000</R_CA_SCP>")+
        std::string("<FreqZero>2.340000000000000E000</FreqZero>")+
        std::string("<DRateSFPoly order1=\"2\" order2=\"4\">")+
        std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.111110000000000E002</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.112220000000000E002</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.113330000000000E002</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.114440000000000E002</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"4\">1.115550000000000E002</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"0\">2.221110000000000E002</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"1\">2.222220000000000E002</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"2\">2.223330000000000E002</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"3\">2.224440000000000E002</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"4\">2.225550000000000E002</Coef>")+
        std::string("<Coef exponent1=\"2\" exponent2=\"0\">3.331110000000000E002</Coef>")+
        std::string("<Coef exponent1=\"2\" exponent2=\"1\">3.332220000000000E002</Coef>")+
        std::string("<Coef exponent1=\"2\" exponent2=\"2\">3.333330000000000E002</Coef>")+
        std::string("<Coef exponent1=\"2\" exponent2=\"3\">3.334440000000000E002</Coef>")+
        std::string("<Coef exponent1=\"2\" exponent2=\"4\">3.335550000000000E002</Coef>")+
        std::string("</DRateSFPoly>")+
        std::string("<DopCentroidPoly order1=\"1\" order2=\"3\">")+
        std::string("<Coef exponent1=\"0\" exponent2=\"0\">1.111111100000000E003</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"1\">1.111222200000000E003</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"2\">1.111333300000000E003</Coef>")+
        std::string("<Coef exponent1=\"0\" exponent2=\"3\">1.111444400000000E003</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"0\">2.222111100000000E003</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"1\">2.222222200000000E003</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"2\">2.222333300000000E003</Coef>")+
        std::string("<Coef exponent1=\"1\" exponent2=\"3\">2.222444400000000E003</Coef>")+
        std::string("</DopCentroidPoly>")+
        std::string("<DopCentroidCOA>false</DopCentroidCOA>")+
        std::string("</INCA>")+
        std::string("</RMA>");

    else if (version == FRMT_0_4_1)
        xmlText =
        std::string("<RMA>")+
        std::string("<RMAlgoType class=\"xs:string\">OMEGA_K</RMAlgoType>")+
        std::string("<ImageType class=\"xs:string\">INCA</ImageType>")+
        std::string("<INCA>")+
        std::string("<TimeCAPoly order1=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">0.000000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">1.111000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">2.222000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\">3.333000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"4\">4.444000000000000E001</Coef>")+
        std::string("</TimeCAPoly>")+
        std::string("<R_CA_SCP class=\"xs:double\">1.230000000000000E000</R_CA_SCP>")+
        std::string("<FreqZero class=\"xs:double\">2.340000000000000E000</FreqZero>")+
        std::string("<DRateSFPoly order1=\"2\" order2=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.111110000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.112220000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.113330000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.114440000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"4\">1.115550000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">2.221110000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">2.222220000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">2.223330000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">2.224440000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"4\">2.225550000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"0\">3.331110000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"1\">3.332220000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"2\">3.333330000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"3\">3.334440000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"4\">3.335550000000000E002</Coef>")+
        std::string("</DRateSFPoly>")+
        std::string("<DopCentroidPoly order1=\"1\" order2=\"3\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.111111100000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.111222200000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.111333300000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.111444400000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">2.222111100000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">2.222222200000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">2.222333300000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">2.222444400000000E003</Coef>")+
        std::string("</DopCentroidPoly>")+
        std::string("<DopCentroidCOA class=\"xs:boolean\">false</DopCentroidCOA>")+
        std::string("</INCA>")+
        std::string("</RMA>");

    else if (version == FRMT_0_4_0)
        xmlText =
        std::string("<RMA>")+
        std::string("<RMAlgoType class=\"xs:string\">OMEGA_K</RMAlgoType>")+
        std::string("<ImageType class=\"xs:string\">INCA</ImageType>")+
        std::string("<INCA>")+
        std::string("<TimeCAPoly order1=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">0.000000000000000E000</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">1.111000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">2.222000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\">3.333000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"4\">4.444000000000000E001</Coef>")+
        std::string("</TimeCAPoly>")+
        std::string("<R_CA_SCP class=\"xs:double\">1.230000000000000E000</R_CA_SCP>")+
        std::string("<FreqZero class=\"xs:double\">2.340000000000000E000</FreqZero>")+
        std::string("<DRateSFPoly order1=\"4\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.111110000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">1.112220000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">1.113330000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\">1.114440000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"4\">1.115550000000000E002</Coef>")+
        std::string("</DRateSFPoly>")+
        std::string("<DopCentroidPoly order1=\"1\" order2=\"3\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.111111100000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.111222200000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.111333300000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"3\">1.111444400000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">2.222111100000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">2.222222200000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">2.222333300000000E003</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"3\">2.222444400000000E003</Coef>")+
        std::string("</DopCentroidPoly>")+
        std::string("<DopCentroidCOA class=\"xs:boolean\">false</DopCentroidCOA>")+
        std::string("</INCA>")+
        std::string("</RMA>");

    return xmlText;
}


std::string  initRMA_RMATXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<RMA>")+
        std::string("<RMAlgoType>OMEGA_K</RMAlgoType>")+
        std::string("<ImageType>RMAT</ImageType>")+
        std::string("<RMAT>")+
        std::string("<PosRef>")+
        std::string("<X>1.111000000000000E000</X>")+
        std::string("<Y>2.222000000000000E000</Y>")+
        std::string("<Z>3.333000000000000E000</Z>")+
        std::string("</PosRef>")+
        std::string("<VelRef>")+
        std::string("<X>1.111100000000000E001</X>")+
        std::string("<Y>2.222200000000000E001</Y>")+
        std::string("<Z>3.333300000000000E001</Z>")+
        std::string("</VelRef>")+
        std::string("<DopConeAngRef>1.234560000000000E002</DopConeAngRef>")+
        std::string("</RMAT>")+
        std::string("</RMA>");

    else if (version == FRMT_0_4_1)
        xmlText =
        std::string("<RMA>")+
        std::string("<RMAlgoType class=\"xs:string\">OMEGA_K</RMAlgoType>")+
        std::string("<ImageType class=\"xs:string\">RMAT</ImageType>")+
        std::string("<RMAT>")+
        std::string("<RefTime class=\"xs:double\">1.234000000000000E000</RefTime>")+
        std::string("<PosRef>")+
        std::string("<X class=\"xs:double\">1.111000000000000E000</X>")+
        std::string("<Y class=\"xs:double\">2.222000000000000E000</Y>")+
        std::string("<Z class=\"xs:double\">3.333000000000000E000</Z>")+
        std::string("</PosRef>")+
        std::string("<UnitVelRef>")+
        std::string("<X class=\"xs:double\">1.111100000000000E001</X>")+
        std::string("<Y class=\"xs:double\">2.222200000000000E001</Y>")+
        std::string("<Z class=\"xs:double\">3.333300000000000E001</Z>")+
        std::string("</UnitVelRef>")+
        std::string("<DistRLPoly order1=\"3\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\">1.001000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\">2.002000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\">3.003000000000000E001</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\">4.004000000000000E001</Coef>")+
        std::string("</DistRLPoly>")+
        std::string("<CosDCACOAPoly order1=\"3\" order2=\"2\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.000010000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.000020000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.000030000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">2.000010000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">2.000020000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">2.000030000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"0\">3.000010000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"1\">3.000020000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"2\">3.000030000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\" exponent2=\"0\">4.000010000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\" exponent2=\"1\">4.000020000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\" exponent2=\"2\">4.000030000000000E002</Coef>")+
        std::string("</CosDCACOAPoly>")+
        std::string("<Kx1 class=\"xs:double\">1.100000000000000E000</Kx1>")+
        std::string("<Kx2 class=\"xs:double\">1.200000000000000E000</Kx2>")+
        std::string("<Ky1 class=\"xs:double\">2.100000000000000E000</Ky1>")+
        std::string("<Ky2 class=\"xs:double\">2.200000000000000E000</Ky2>")+
        std::string("</RMAT>")+
        std::string("</RMA>");

    else if (version == FRMT_0_4_0)
        xmlText =
        std::string("<RMA>")+
        std::string("<RMAlgoType class=\"xs:string\">OMEGA_K</RMAlgoType>")+
        std::string("<ImageType class=\"xs:string\">RMAT</ImageType>")+
        std::string("<RMAT>")+
        std::string("<RMRefTime class=\"xs:double\">1.234000000000000E000</RMRefTime>")+
        std::string("<RMPosRef>")+
        std::string("<X class=\"xs:double\">1.111000000000000E000</X>")+
        std::string("<Y class=\"xs:double\">2.222000000000000E000</Y>")+
        std::string("<Z class=\"xs:double\">3.333000000000000E000</Z>")+
        std::string("</RMPosRef>")+
        std::string("<RMVelRef>")+
        std::string("<X class=\"xs:double\">1.111100000000000E001</X>")+
        std::string("<Y class=\"xs:double\">2.222200000000000E001</Y>")+
        std::string("<Z class=\"xs:double\">3.333300000000000E001</Z>")+
        std::string("</RMVelRef>")+
        std::string("<CosDCACOAPoly order1=\"3\" order2=\"2\">")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"0\">1.000010000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"1\">1.000020000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"0\" exponent2=\"2\">1.000030000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"0\">2.000010000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"1\">2.000020000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"1\" exponent2=\"2\">2.000030000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"0\">3.000010000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"1\">3.000020000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"2\" exponent2=\"2\">3.000030000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\" exponent2=\"0\">4.000010000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\" exponent2=\"1\">4.000020000000000E002</Coef>")+
        std::string("<Coef class=\"xs:double\" exponent1=\"3\" exponent2=\"2\">4.000030000000000E002</Coef>")+
        std::string("</CosDCACOAPoly>")+
        std::string("<Kx1 class=\"xs:double\">1.100000000000000E000</Kx1>")+
        std::string("<Kx2 class=\"xs:double\">1.200000000000000E000</Kx2>")+
        std::string("<Ky1 class=\"xs:double\">2.100000000000000E000</Ky1>")+
        std::string("<Ky2 class=\"xs:double\">2.200000000000000E000</Ky2>")+
        std::string("</RMAT>")+
        std::string("</RMA>");

    return xmlText;
}


std::string  initRMA_RMCRXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<RMA>")+
        std::string("<RMAlgoType>OMEGA_K</RMAlgoType>")+
        std::string("<ImageType>RMCR</ImageType>")+
        std::string("<RMCR>")+
        std::string("<PosRef>")+
        std::string("<X>1.111000000000000E000</X>")+
        std::string("<Y>2.222000000000000E000</Y>")+
        std::string("<Z>3.333000000000000E000</Z>")+
        std::string("</PosRef>")+
        std::string("<VelRef>")+
        std::string("<X>1.111100000000000E001</X>")+
        std::string("<Y>2.222200000000000E001</Y>")+
        std::string("<Z>3.333300000000000E001</Z>")+
        std::string("</VelRef>")+
        std::string("<DopConeAngRef>1.234560000000000E002</DopConeAngRef>")+
        std::string("</RMCR>")+
        std::string("</RMA>");

    return xmlText;
}


std::string  initRgAzCompXML(unsigned int version)
{
    std::string xmlText("");

    if (version == FRMT_1_0_0)
        xmlText =
        std::string("<RgAzComp>")+
        std::string("<AzSF>1.300000000000000E001</AzSF>")+
        std::string("<KazPoly order1=\"3\">")+
        std::string("<Coef exponent1=\"0\">1.111110000000000E001</Coef>")+
        std::string("<Coef exponent1=\"1\">2.222220000000000E001</Coef>")+
        std::string("<Coef exponent1=\"2\">3.333330000000000E001</Coef>")+
        std::string("<Coef exponent1=\"3\">4.444440000000000E001</Coef>")+
        std::string("</KazPoly>")+
        std::string("</RgAzComp>");

    return xmlText;
}


std::string  initComplexDataXML(unsigned int version, std::string tag1 = "PFA", std::string tag2 = "")
{
    std::string xmlText("");
    if (version == FRMT_0_4_0)
        xmlText = "<SICD xmlns=\"urn:SICD:0.4.0\">";
    else if (version == FRMT_0_4_1)
        xmlText = "<SICD xmlns=\"urn:SICD:0.4.1\">";
    else // if (version == FRMT_1_0_0)
        xmlText = "<SICD xmlns=\"urn:SICD:1.0.0\">";  // default version

    xmlText += initCollectionInformationXML(version);
    xmlText += initImageCreationXML(version);
    xmlText += initImageDataXML(version);
    xmlText += initGeoDataXML(version);
    xmlText += initGridXML(version);
    xmlText += initTimelineXML(version);
    xmlText += initPositionXML(version);
    xmlText += initRadarCollectionXML(version);
    xmlText += initImageFormationXML(version);
    xmlText += initSCPCOAXML(version);
    xmlText += initRadiometricXML(version);
    xmlText += initAntennaXML(version);
    xmlText += initErrorStatisticsXML(version);
    xmlText += initMatchInformationXML(version);

    if (tag1 == "PFA")
        xmlText += initPFAXML(version);
    else if (tag1 == "RgAzComp")
        xmlText += initRgAzCompXML(version);
    else if (tag1 == "RMA")
    {
        if (tag2 == "INCA")
            xmlText += initRMA_INCAXML(version);
        else if (tag2 == "RMAT")
            xmlText += initRMA_RMATXML(version);
        else if (tag2 == "RMCR")
            xmlText += initRMA_RMCRXML(version);
    }

    xmlText += "</SICD>";
    return xmlText;
}

void diffXMLs(std::string label1, std::string xml1,
              std::string label2, std::string xml2,
              unsigned int dispCnt=-1, unsigned int size=60)
{
    if (dispCnt != 0)
    {
        for (unsigned int i=0,n=0; i<=xml1.size()/size; ++i)
        {
            if (xml1.substr(i*size,size) != xml2.substr(i*size,size))
            {
                std::cout << label1 << " " << i*size << ": " << xml1.substr(i*size,size) << std::endl
                    << label2 << " " << i*size << ": " << xml2.substr(i*size,size) << std::endl
                    << std::endl;
                if (dispCnt>0 && (++n)>=dispCnt) break;
            }
        }
    }
}


bool cmpRoundTripXMLs(std::string xmlText, std::string xmlPath = "",
                      bool verbose = false, unsigned int debugLineCnt = 10)
{
    try
    {
        // parse the programetically generated SICD XML string
        xml::lite::MinidomParser parser;
        io::StringStream oss;
        oss.write(xmlText.c_str(), xmlText.size());
        parser.parse(oss);

        // apply parsed XML to document, print its XML string and compare to original XML string
        const xml::lite::Document* doc = parser.getDocument();
        oss.reset();
        doc->getRootElement()->print(oss);
        std::string preRTxml(oss.stream().str());

        std::unique_ptr<logging::Logger> log(new logging::NullLogger());

        // translate XML into Complex Data structure
        const std::unique_ptr<six::XMLControl> xmlControl(
            six::XMLControlFactory::getInstance().newXMLControl(six::DataType::COMPLEX, log.get()));
        six::sicd::ComplexData* data = (
            six::sicd::ComplexData*)xmlControl->fromXML(doc, std::vector<std::string>());

        // translate data structure to XML string
        const std::unique_ptr<xml::lite::Document> rtDoc(
            xmlControl->toXML((six::sicd::ComplexData*)data, std::vector<std::string>()));
        oss.reset();
        rtDoc->getRootElement()->print(oss);
        std::string postRTxml(oss.stream().str());

        // read document back into Complex Data structure to compare against
        six::sicd::ComplexData* rtData = (
            six::sicd::ComplexData*)xmlControl->fromXML(rtDoc.get(), std::vector<std::string>());

        // if output path provided, write out XML results to files
        if (!xmlPath.empty())
        {
            std::string  baseName = xmlPath + std::string("\\") + data->getVersion();
            if (data->pfa.get() != nullptr)
                baseName += "_PFA";
            else if (data->rma.get() != nullptr)
            {
                if (data->rma->inca.get() != nullptr)
                    baseName += "_RMA_INCA";
                else if (data->rma->rmat.get() != nullptr)
                    baseName += "_RMA_RMAT";
                else if (data->rma->rmcr.get() != nullptr)
                    baseName += "_RMA_RMCR";
            }
            else if (data->rgAzComp.get() != nullptr)
                baseName += "_RgAzComp";

            if (verbose) std::cout << std::endl;

            // pretty-print the input XML string to file
            std::string outName = baseName + std::string(".xml");
            if (verbose) std::cout << "Original  XML file: " << outName << std::endl;
            io::FileOutputStream prOs(outName);
            doc->getRootElement()->prettyPrint(prOs);

            // pretty-print the round-tripped XML string to file
            outName = baseName + std::string(".RT.xml");
            if (verbose) std::cout << "RoundTrip XML file: " << outName << std::endl;
            io::FileOutputStream rtOs(outName);
            rtDoc->getRootElement()->prettyPrint(rtOs);
        }

        // for unit test, compare the two ComplexData objects produced
        if (*data != *rtData)
        {
            if (verbose) diffXMLs("Input    ", xmlText,  "Parsed   ", postRTxml,  debugLineCnt);
            return false;
        }
    }
    catch (except::Exception& /*ex*/)
    {
        return false;
    }

    return true;
}


MYTEST_CASE_PARAMS(RoundTrip_XML_0_4_0_PFA)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_0_4_0, "PFA"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_0_4_0_RMA_INCA)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_0_4_0, "RMA", "INCA"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_0_4_0_RMA_RMAT)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_0_4_0, "RMA", "RMAT"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_0_4_1_PFA)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_0_4_1, "PFA"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_0_4_1_RMA_INCA)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_0_4_1, "RMA", "INCA"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_0_4_1_RMA_RMAT)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_0_4_1, "RMA", "RMAT"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_1_0_0_PFA)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_1_0_0, "PFA"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_1_0_0_RMA_INCA)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_1_0_0, "RMA", "INCA"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_1_0_0_RMA_RMAT)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_1_0_0, "RMA", "RMAT"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_1_0_0_RMA_RMCR)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_1_0_0, "RMA", "RMCR"), params[0], params[1]=="true"));
}
MYTEST_CASE_PARAMS(RoundTrip_XML_1_0_0_RgAzComp)
{
    TEST_ASSERT_GREATER_EQ(params.size(), 2);
    TEST_ASSERT(cmpRoundTripXMLs(initComplexDataXML(FRMT_1_0_0, "RgAzComp"), params[0], params[1]=="true"));
}


int main(int argc, char** argv)
{
    try
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription("Unit-tests for SCID NITF Round Trip code, \
                              with option to write output XML files.");
        parser.addArgument("xmlPath",  "output XML path",  cli::STORE,
            "xmlPath", "xmlPath")->setDefault("");
        parser.addArgument("-d",  "enable verbose output",  cli::STORE_TRUE,
            "verbose", "verbose")->setDefault("");
        cli::Results *options = parser.parse(argc, (const char**) argv);
        std::vector<std::string> params;
        params.push_back(options->get<std::string> ("xmlPath"));
        params.push_back(options->get<std::string> ("verbose"));

        six::XMLControlFactory::getInstance().addCreator(six::DataType::COMPLEX,
            new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        // Validate that the resultant XML data from SICD Complex Data
        //   matches before and after round-trip for each format and algorithm

        MYTEST_CHECK_PARAMS( RoundTrip_XML_1_0_0_PFA      );
        MYTEST_CHECK_PARAMS( RoundTrip_XML_1_0_0_RMA_INCA );
        MYTEST_CHECK_PARAMS( RoundTrip_XML_1_0_0_RMA_RMAT );
        MYTEST_CHECK_PARAMS( RoundTrip_XML_1_0_0_RMA_RMCR );
        MYTEST_CHECK_PARAMS( RoundTrip_XML_1_0_0_RgAzComp );

        MYTEST_CHECK_PARAMS( RoundTrip_XML_0_4_1_PFA      );
        MYTEST_CHECK_PARAMS( RoundTrip_XML_0_4_1_RMA_INCA );
        MYTEST_CHECK_PARAMS( RoundTrip_XML_0_4_1_RMA_RMAT );

        MYTEST_CHECK_PARAMS( RoundTrip_XML_0_4_0_PFA      );
        MYTEST_CHECK_PARAMS( RoundTrip_XML_0_4_0_RMA_INCA );
        MYTEST_CHECK_PARAMS( RoundTrip_XML_0_4_0_RMA_RMAT );
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
}

