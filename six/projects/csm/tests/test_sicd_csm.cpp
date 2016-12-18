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

    void testFileISD()
    {
        testISD(csm::Isd(mSicdPathname));
    }

    void testNitfISD()
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

        testISD(nitfIsd);
    }

private:
    void testISD(const csm::Isd& isd)
    {
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

        csm::ImageCoord imagePt(scpPixel.row + 0.5, scpPixel.col + 0.5);
        const double height = scp.llh.getAlt();

        csm::EcefCoord coord = model->imageToGround(
                imagePt,
                height,
                0);
        std::cout << "CSM ECEF coord is " << coord.x << " " << coord.y << " " << coord.z << std::endl;

        scene::Vector3 csmCoord;
        csmCoord[0] = coord.x;
        csmCoord[1] = coord.y;
        csmCoord[2] = coord.z;
        scene::Vector3 diff = scp.ecf - csmCoord;
        std::cout << "Diff " << diff[0] << " " << diff[1] << " " << diff[2] << std::endl;

        // Round trip it
        csm::ImageCoord rtCoord = model->groundToImage(coord, 0);
        std::cout << "Got back " << rtCoord.line << " " << rtCoord.samp << std::endl;
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
        test.testFileISD();
        test.testNitfISD();

        return 0;
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
