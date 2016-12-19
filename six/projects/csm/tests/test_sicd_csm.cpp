/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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
#include <iostream>
#include <sstream>

#include <sys/DLL.h>
#include <sys/Conf.h>
#include <except/Exception.h>
#include <six/Utilities.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>

// CSM includes
#include <RasterGM.h>
#include <Plugin.h>
#include <NitfIsd.h>

// TODO
// - Test each SICD version (0.4.0, 0.4.1, 0.5.0, 1.0.0, 1.0.1, 1.1.0)
// - Test SICD with extra non-SICD XML_DATA_CONTENT
// - Test 0.5 pixel on either side of the SCP, test that what we have below is
//   the closest, and test that it's close to the original within some small
//   tolerance (and fail if these aren't all true)
// - If practical, auto-find config dir and CSM DLL dir (scan DLL dir to find
//   plugin filename)
// - Build equivalent test for SIDD

namespace
{
std::string toString(nitf::FileSecurity security)
{
    const std::string str = security.getClassificationSystem().toString() +
            security.getCodewords().toString() +
            security.getControlAndHandling().toString() +
            security.getReleasingInstructions().toString() +
            security.getDeclassificationType().toString() +
            security.getDeclassificationDate().toString() +
            security.getDeclassificationExemption().toString() +
            security.getDowngrade().toString() +
            security.getDowngradeDateTime().toString() +
            security.getClassificationText().toString() +
            security.getClassificationAuthorityType().toString() +
            security.getClassificationAuthority().toString() +
            security.getClassificationReason().toString() +
            security.getSecuritySourceDate().toString() +
            security.getSecurityControlNumber().toString();

    return str;
}

std::string toString(nitf::DESubheader subheader)
{
    const nitf::Uint32 subheaderFieldsLen(subheader.getSubheaderFieldsLength());

    std::ostringstream ostr;
    ostr << subheader.getFilePartType().toString()
         << subheader.getTypeID().toString()
         << subheader.getVersion().toString()
         << subheader.getSecurityClass().toString()
         << toString(subheader.getSecurityGroup())
         << std::setw(4) << std::setfill('0') << subheaderFieldsLen;

    // This tacks on XML_DATA_CONTENT when applicable
    if (subheaderFieldsLen > 0)
    {
        nitf::TRE tre = subheader.getSubheaderFields();
        for (nitf::TRE::Iterator iter = tre.begin(); iter != tre.end(); ++iter)
        {
            nitf::Pair pair = *iter;
            ostr << tre.getField(pair.getKey()).toString();
        }
    }

    return ostr.str();
}

class Test
{
public:
    Test(const std::string& sicdPathname,
         const std::string& confDir,
         const csm::Plugin& plugin) :
        mSicdPathname(sicdPathname),
        mPlugin(plugin)
    {
        // Read in the SICD XML
        mXmlRegistry.addCreator(six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        mReader.setXMLControlRegistry(&mXmlRegistry);

        const std::string schemaDir =
                sys::Path(confDir).join("schema").join("six");
        mReader.load(mSicdPathname, std::vector<std::string>(1, schemaDir));
        mComplexData = six::sicd::Utilities::getComplexData(mReader);
    }

    bool testFileISD()
    {
        return testISD(csm::Isd(mSicdPathname));
    }

    bool testNitfISD()
    {
        csm::Nitf21Isd nitfIsd(mSicdPathname);

        csm::Des des;

        // NITRO parsed the subheader into a nice structure - need to grab all
        // the fields and jam them back into a string like CSM wants
        nitf::DESegment segment = static_cast<nitf::DESegment>(
                mReader.getRecord().getDataExtensions().getFirst().getData());
        des.setSubHeader(toString(segment.getSubheader()));

        // The DES's data is just the SICD XML string
        des.setData(six::toXMLString(mComplexData.get(), &mXmlRegistry));

        nitfIsd.addFileDes(des);

        return testISD(nitfIsd);
    }

private:
    scene::Vector3 imageToGround(std::auto_ptr<csm::RasterGM> model,
            const six::RowColInt& scpPixel, double height, double offset)
    {
        csm::ImageCoord imagePt(scpPixel.row + offset, scpPixel.col + offset);
        csm::EcefCoord groundPt = model->imageToGround(
                imagePt,
                height,
                0);
        scene::Vector3 returnedGroundPoint;

        returnedGroundPoint[0] = groundPt.x;
        returnedGroundPoint[1] = groundPt.y;
        returnedGroundPoint[2] = groundPt.z;
        return returnedGroundPoint;
    }

    six::RowColDouble groundToImage(std::auto_ptr<csm::RasterGM> model,
            const six::SCP& scp, double offset)
    {
        csm::EcefCoord coord(scp.ecf[0], scp.ecf[1], scp.ecf[2]);
        csm::ImageCoord rtCoord = model->groundToImage(coord, 0);
        return six::RowColDouble(rtCoord.line - offset, rtCoord.samp - offset);
    }

    bool testISD(const csm::Isd& isd)
    {
        bool testPassed = true;

        // Construct the model
        // We take ownership of the memory for this
        if (!mPlugin.canModelBeConstructedFromISD(isd, MODEL_NAME))
        {
            throw except::Exception(Ctxt("Can't construct ISD"));
        }

        std::auto_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
                mPlugin.constructModelFromISD(isd, MODEL_NAME)));

        const six::RowColInt scpPixel = mComplexData->imageData->scpPixel;
        const six::SCP scp = mComplexData->geoData->scp;
        const double height = scp.llh.getAlt();

        // The offsets past the first should result in a number farther off
        // than the others
        std::vector<double> offsets(3);
        std::vector<double> imageDifferences(offsets.size());
        std::vector<double> groundDifferences(offsets.size());
        offsets[0] = .5;
        offsets[1] = 0;
        offsets[2] = -.5;
        offsets[3] = 1;

        for (size_t ii = 0; ii < offsets.size(); ++ii)
        {
            six::RowColDouble convertedImagePoint =
                    groundToImage(model, scp, offsets[ii]);

            six::RowColDouble doubleImagePoint(
                    static_cast<double>(scpPixel.row),
                    static_cast<double>(scpPixel.col));

            six::RowColDouble imagePointDifference =
                    convertedImagePoint - doubleImagePoint;
            imagePointDifference.row = std::abs(imagePointDifference.row);
            imagePointDifference.col = std::abs(imagePointDifference.col);
            imageDifferences[ii] = std::max(imagePointDifference.row,
                    imagePointDifference.col);

            scene::Vector3 convertedGroundPoint =
                    imageToGround(model, scpPixel, height, offsets[ii]);
            scene::Vector3 groundPointDifference =
                    convertedGroundPoint - scp.ecf;
            for (size_t jj = 0; jj < 3; ++jj)
            {
                groundPointDifference[jj] = std::abs(groundPointDifference[jj]);
            }
            groundDifferences[ii] = std::max(groundPointDifference[0],
                    std::max(groundPointDifference[1],
                            groundPointDifference[2]));
        }

        double leastGroundDifference = *std::min_element(
                groundDifferences.begin(), groundDifferences.end());
        double leastImageDifference = *std::min_element(
                imageDifferences.begin(), imageDifferences.end());

        if (leastGroundDifference != groundDifferences[0] ||
            leastImageDifference != imageDifferences[0])
        {
            std::cerr << "There was an offset better than +.5\n";
            testPassed = false;
        }

        if (leastGroundDifference > .1)
        {
            std::cerr << "Converted ground point > .1 away from SCP\n";
            testPassed = false;
        }

        if (leastImageDifference > .1)
        {
            std::cerr << "Converted image point > .1 away from scpPixel\n";
        }
        return testPassed;
    }

private:
    static const char MODEL_NAME[];;

    const std::string mSicdPathname;
    const csm::Plugin& mPlugin;

    six::XMLControlRegistry mXmlRegistry;
    six::NITFReadControl mReader;
    std::auto_ptr<six::sicd::ComplexData> mComplexData;
};

const char Test::MODEL_NAME[] = "SICD_SENSOR_MODEL";
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc != 4)
        {
            std::cerr << "Usage: " << sys::Path::basename(argv[0])
                      << " <SIX CSM DLL pathname> <SICD pathname>"
                      << " <SIX conf directory>\n\n";
            return 1;
        }
        const std::string dllPathname(argv[1]);
        const std::string sicdPathname(argv[2]);
        const std::string confDir(argv[3]);

        // Load the SIX CSM DLL
        // Quite frankly I don't know by what magic csm::Plugin::getList() finds
        // it - some global is getting set automatically when the DLL is just
        // opened that it's finding
        sys::DLL dll(dllPathname);
        csm::Plugin::setDataDirectory(confDir);

        // Make sure we found it
        csm::PluginList pluginList = csm::Plugin::getList();

        if (pluginList.size() != 1)
        {
            throw except::Exception(Ctxt("Expected 1 plugin but found " +
                    str::toString(pluginList.size())));
        }

        const csm::Plugin& plugin = **pluginList.begin();

        if (plugin.getPluginName() != "SIX")
        {
            throw except::Exception(Ctxt(
                    "Unexpected plugin name '" + plugin.getPluginName() + "'"));
        }

        Test test(sicdPathname, confDir, plugin);
        const bool testPassed = test.testFileISD() && test.testNitfISD();

        return testPassed ? 0 : 1;
    }

    // TODO: At least on Windows we don't ever seem to actually get exceptions
    //       that make it out of the plugin
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }

    return 1;
}
