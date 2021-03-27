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

#include <std/filesystem>

#include <nitf/coda-oss.hpp>
#include <sys/DLL.h>
#include <except/Exception.h>
#include <six/Utilities.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/Utilities.h>
#include <scene/ECEFToLLATransform.h>

// CSM includes
#include <RasterGM.h>
#include <Plugin.h>
#include <NitfIsd.h>

#include "utilities.h"

namespace fs = std::filesystem;

namespace
{

class Test
{
public:
    Test(const std::string& siddPathname,
         const std::string& confDir,
         const csm::Plugin& plugin) :
        mSiddPathname(siddPathname),
        mPlugin(plugin)
    {
        // Read in the SIDD XML
        mXmlRegistry.addCreator(six::DataType::DERIVED,
                new six::XMLControlCreatorT<six::sidd::DerivedXMLControl>());

        mReader.setXMLControlRegistry(&mXmlRegistry);

        const auto schemaDir =  (fs::path(confDir) / "schema" / "six").string();
        mReader.load(mSiddPathname, std::vector<std::string>(1, schemaDir));
        auto container(mReader.getContainer());
        mDerivedData.reset(reinterpret_cast<six::sidd::DerivedData*>(
                container->getData(0)->clone()));
    }

    bool testFileISD()
    {
        return testISD(csm::Isd(mSiddPathname));
    }

    bool testNitfISD()
    {
        std::unique_ptr<csm::Nitf21Isd> nitfIsd = constructIsd(mSiddPathname,
                mReader, mDerivedData.get(), mXmlRegistry);
        return testISD(*nitfIsd);
    }

private:
    scene::Vector3 imageToGround(const csm::RasterGM& model,
            const six::RowColDouble& scpPixel, double height, double offset)
    {
        csm::ImageCoord imagePt(scpPixel.row + offset, scpPixel.col + offset);
        csm::EcefCoord groundPt = model.imageToGround(
                imagePt,
                height,
                0);
        scene::Vector3 returnedGroundPoint;

        returnedGroundPoint[0] = groundPt.x;
        returnedGroundPoint[1] = groundPt.y;
        returnedGroundPoint[2] = groundPt.z;
        return returnedGroundPoint;
    }

    six::RowColDouble groundToImage(const csm::RasterGM& model,
            const six::Vector3& scp, double offset)
    {
        csm::EcefCoord groundCoord(scp[0], scp[1], scp[2]);
        csm::ImageCoord imageCoord = model.groundToImage(groundCoord, 0);
        return six::RowColDouble(imageCoord.line - offset,
                imageCoord.samp - offset);
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

        std::unique_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
                mPlugin.constructModelFromISD(isd, MODEL_NAME)));

        if (!mDerivedData->measurement->projection->isMeasurable())
        {
            throw except::Exception(Ctxt("Test requires SIDD with "
                    "Measureable Projection"));
        }
        six::RowColDouble scpPixel = mDerivedData->measurement->projection->
                referencePoint.rowCol;
        six::Vector3 scp = mDerivedData->measurement->projection->
                referencePoint.ecef;

        // Get height from SCP for conversion
        scene::ECEFToLLATransform transformer;
        six::LatLonAlt lla = transformer.transform(scp);
        const double height = lla.getAlt();

        // The offsets past the first should result in a number farther off
        // than the others
        std::vector<double> offsets(4);
        std::vector<double> imageDifferences(offsets.size());
        std::vector<double> groundDifferences(offsets.size());
        offsets[0] = 0;
        offsets[1] = .5;
        offsets[2] = -.5;
        offsets[3] = 1;
        const double pixelTolerance = 0;
        const double groundTolerance = .001;

        for (size_t ii = 0; ii < offsets.size(); ++ii)
        {
            // Find difference between converted and given scpPixel
            six::RowColDouble convertedImagePoint =
                    groundToImage(*model, scp, offsets[ii]);
            six::RowColDouble imagePointDifference = absoluteDifference(
                    convertedImagePoint, scpPixel);

            // Find difference between converted and given SCP
            scene::Vector3 convertedGroundPoint =
                    imageToGround(*model, scpPixel, height, offsets[ii]);
            scene::Vector3 groundPointDifference = absoluteDifference(
                    convertedGroundPoint, scp);

            // Just need to check that the greatest difference is
            // under our tolerance
            imageDifferences[ii] = std::max(imagePointDifference.row,
                    imagePointDifference.col);

            groundDifferences[ii] = std::max(groundPointDifference[0],
                    std::max(groundPointDifference[1],
                            groundPointDifference[2]));
        }

        double leastGroundDifference = *std::min_element(
                groundDifferences.begin(), groundDifferences.end());
        double leastImageDifference = *std::min_element(
                imageDifferences.begin(), imageDifferences.end());

        if (leastImageDifference != imageDifferences[0])
        {
            std::cerr << "There was an offset better than " <<
                    offsets[0] << "\n";
            testPassed = false;
        }

        if (leastGroundDifference != groundDifferences[0])
        {
            std::cerr << "There was an offset better than " <<
                    offsets[0] << "\n";
            testPassed = false;
        }

        if (leastGroundDifference > groundTolerance)
        {
            std::cerr << "Converted ground point > " << groundTolerance <<
                    " away from SCP\n";
            testPassed = false;
        }

        if (leastImageDifference > pixelTolerance)
        {
            std::cerr << "Converted image point > " << pixelTolerance <<
                    " away from scpPixel\n";
            testPassed = false;
        }
        return testPassed;
    }

private:
    static const char MODEL_NAME[];

    const std::string mSiddPathname;
    const csm::Plugin& mPlugin;

    six::XMLControlRegistry mXmlRegistry;
    six::NITFReadControl mReader;
    std::unique_ptr<six::sidd::DerivedData> mDerivedData;
};

const char Test::MODEL_NAME[] = "SIDD_SENSOR_MODEL";
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc != 2)
        {
            std::cerr << "Usage: " << fs::path(argv[0]).filename().string()
                      << " <SIDD pathname>\n\n";
            return 1;
        }
        sys::OS os;

        // Go up two levels from current dir
        const std::string installPathname =
                sys::Path::splitPath(sys::Path::splitPath(
                        os.getCurrentExecutable()).first).first;

        const std::string dllPathname = findDllPathname(installPathname);
        const std::string confDir =
                sys::Path::joinPaths(installPathname, "conf");
        if (!os.exists(confDir))
        {
            throw except::Exception(Ctxt("Unable to find conf dir."));
        }

        const std::string siddPathname(argv[1]);

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

        Test test(siddPathname, confDir, plugin);
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
