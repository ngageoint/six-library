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
#include <except/Exception.h>
#include <scene/ECEFToLLATransform.h>
#include <six/Utilities.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/Utilities.h>
#include <sys/DLL.h>

#include <iostream>
#include <nitf/coda-oss.hpp>
#include <sstream>
#include <std/filesystem>

// CSM includes
#include <NitfIsd.h>
#include <Plugin.h>
#include <RasterGM.h>

#include "utilities.h"

namespace fs = std::filesystem;

namespace
{

class Test
{
public:
    Test(const std::string& siddPathname1,
         const std::string& siddPathname2,
         const std::string& confDir,
         const csm::Plugin& plugin) :
        mSiddPathname1(siddPathname1),
        mSiddPathname2(siddPathname2),
        mPlugin(plugin)
    {
        // Read in the SIDD XML
        mXmlRegistry.addCreator<six::sidd::DerivedXMLControl>();
        const auto schemaDir = (fs::path(confDir) / "schema" / "six").string();

        mReader1.setXMLControlRegistry(&mXmlRegistry);
        mReader1.load(mSiddPathname1, std::vector<std::string>(1, schemaDir));
        auto container1(mReader1.getContainer());

        mReader2.setXMLControlRegistry(&mXmlRegistry);
        mReader2.load(mSiddPathname2, std::vector<std::string>(1, schemaDir));
        auto container2(mReader2.getContainer());
    }

    bool testFileISD()
    {
        return testISD(csm::Isd(mSiddPathname1), csm::Isd(mSiddPathname2));
    }

    bool testNitfISD()
    {
        std::unique_ptr<csm::Nitf21Isd> nitfIsd1 =
                constructIsd(mSiddPathname1, mReader1, mXmlRegistry);
        std::unique_ptr<csm::Nitf21Isd> nitfIsd2 =
                constructIsd(mSiddPathname2, mReader2, mXmlRegistry);
        return testISD(*nitfIsd1, *nitfIsd2);
    }

private:
    scene::Vector3 imageToGround(const csm::RasterGM& model,
                                 const six::RowColDouble& scpPixel,
                                 double height,
                                 double offset)
    {
        csm::ImageCoord imagePt(scpPixel.row + offset, scpPixel.col + offset);
        csm::EcefCoord groundPt = model.imageToGround(imagePt, height, 0);
        scene::Vector3 returnedGroundPoint;

        returnedGroundPoint[0] = groundPt.x;
        returnedGroundPoint[1] = groundPt.y;
        returnedGroundPoint[2] = groundPt.z;
        return returnedGroundPoint;
    }

    six::RowColDouble groundToImage(const csm::RasterGM& model,
                                    const six::Vector3& scp,
                                    double offset)
    {
        csm::EcefCoord groundCoord(scp[0], scp[1], scp[2]);
        csm::ImageCoord imageCoord = model.groundToImage(groundCoord, 0);
        return six::RowColDouble(imageCoord.line - offset,
                                 imageCoord.samp - offset);
    }

    void displayWarnings(const std::string& function,
                         int modelNum,
                         csm::WarningList& warnings)
    {
        if (warnings.size())
        {
            std::cerr << function << " returned warnings for model " << modelNum
                      << ":" << std::endl;
            for (auto it = warnings.begin(); it != warnings.end(); it++)
            {
                std::cerr << "    id=" << it->getWarning() << " in function "
                          << it->getFunction() << ": " << it->getMessage()
                          << std::endl;
            }
            warnings.clear();
        }
    }

    bool testISD(const csm::Isd& isd1, const csm::Isd& isd2)
    {
        bool testPassed = true;

        std::unique_ptr<csm::RasterGM> model1;
        std::unique_ptr<csm::RasterGM> model2;

        csm::WarningList warnings;

        // Construct the model
        std::cout << std::endl
                  << "Constructing model for " << mSiddPathname1 << std::endl;
        for (size_t modelIdx = 0; modelIdx < mPlugin.getNumModels(); modelIdx++)
        {
            std::string modelName = mPlugin.getModelName(modelIdx);
            std::cout << "Model: " << modelName;
            if (mPlugin.canModelBeConstructedFromISD(isd1,
                                                     modelName,
                                                     &warnings))
            {
                std::cout << " possibly constructible: ";
                displayWarnings("canModelBeConstructedFromISD()", 1, warnings);
                try
                {
                    model1.reset(reinterpret_cast<csm::RasterGM*>(
                            mPlugin.constructModelFromISD(isd1,
                                                          modelName,
                                                          &warnings)));
                    std::cout << "success" << std::endl;
                }
                catch (const csm::Error& ex)
                {
                    std::cout << "failed" << std::endl;
                    std::cout << ex.what() << std::endl;
                }
                displayWarnings("constructModelFromISD()", 1, warnings);
            }
            else
            {
                std::cout << " not constructible" << std::endl;
                displayWarnings("canModelBeConstructedFromISD()", 1, warnings);
            }
        }

        std::cout << std::endl
                  << "Constructing model for " << mSiddPathname2 << std::endl;
        for (size_t modelIdx = 0; modelIdx < mPlugin.getNumModels(); modelIdx++)
        {
            std::string modelName = mPlugin.getModelName(modelIdx);
            std::cout << "Model: " << modelName;
            if (mPlugin.canModelBeConstructedFromISD(isd2,
                                                     modelName,
                                                     &warnings))
            {
                std::cout << " possibly constructible: ";
                displayWarnings("canModelBeConstructedFromISD()", 2, warnings);
                try
                {
                    model2.reset(reinterpret_cast<csm::RasterGM*>(
                            mPlugin.constructModelFromISD(isd2,
                                                          modelName,
                                                          &warnings)));
                    std::cout << "success" << std::endl;
                }
                catch (const csm::Error& ex)
                {
                    std::cout << "failed" << std::endl;
                    std::cout << ex.what() << std::endl;
                }
                displayWarnings("constructModelFromISD()", 2, warnings);
            }
            else
            {
                std::cout << " not constructible" << std::endl;
                displayWarnings("canModelBeConstructedFromISD()", 2, warnings);
            }
        }

        if (!model1 || !model2)
        {
            throw except::Exception(Ctxt("Failed to construct both models"));
        }

        std::cout << std::endl;

        if (model1->getCollectionIdentifier() !=
            model2->getCollectionIdentifier())
        {
            std::cerr << "getCollectionIdentifier() returned different values"
                      << std::endl;
            testPassed = false;
        }

        if (model1->getImageIdentifier() != model2->getImageIdentifier())
        {
            std::cerr << "getImageIdentifier() returned different values"
                      << std::endl;
            testPassed = false;
        }

        if (model1->getSensorIdentifier() != model2->getSensorIdentifier())
        {
            std::cerr << "getSensorIdentifier() returned different values"
                      << std::endl;
            testPassed = false;
        }

        if (model1->getSensorMode() != model2->getSensorMode())
        {
            std::cerr << "getSensorMode() returned different values"
                      << std::endl;
            testPassed = false;
        }

        if (model1->getSensorType() != model2->getSensorType())
        {
            std::cerr << "getSensorType() returned different values"
                      << std::endl;
            testPassed = false;
        }

        auto pedigree1 = model1->getPedigree();
        auto pedigree2 = model2->getPedigree();
        if (pedigree1 != pedigree2)
        {
            std::cerr << "getPedigree() returned different values" << std::endl;
            std::cerr << "    " << pedigree1 << std::endl;
            std::cerr << "    " << pedigree2 << std::endl;
            testPassed = false;
        }

        if (model1->getPlatformIdentifier() != model2->getPlatformIdentifier())
        {
            std::cerr << "getPlatformIdentifier() returned different values"
                      << std::endl;
            testPassed = false;
        }

        if (model1->getReferenceDateAndTime() !=
            model2->getReferenceDateAndTime())
        {
            std::cerr << "getReferenceDateAndTime() returned different values"
                      << std::endl;
            testPassed = false;
        }

        if (model1->getTrajectoryIdentifier() !=
            model2->getTrajectoryIdentifier())
        {
            std::cerr << "getTrajectoryIdentifier() returned different values"
                      << std::endl;
            testPassed = false;
        }

        csm::EcefCoord refPt1 = model1->getReferencePoint();
        csm::EcefCoord refPt2 = model2->getReferencePoint();
        if (refPt1.x != refPt2.x || refPt1.y != refPt2.y ||
            refPt1.z != refPt2.z)
        {
            std::cerr << "getReferencePoint() returned different values"
                      << std::endl;
            testPassed = false;
        }

        auto illumDir1 = model1->getIlluminationDirection(refPt1);
        auto illumDir2 = model2->getIlluminationDirection(refPt1);
        if (illumDir1.x != illumDir2.x || illumDir1.y != illumDir2.y ||
            illumDir1.z != illumDir2.z)
        {
            std::cerr << "getIlluminationDirection() returned different values:"
                      << std::endl;
            std::cerr << "    " << illumDir1.x << " " << illumDir1.y << " "
                      << illumDir1.z << std::endl;
            std::cerr << "    " << illumDir2.x << " " << illumDir2.y << " "
                      << illumDir2.z << std::endl;
            testPassed = false;
        }

        auto imageRange1 = model1->getValidImageRange();
        auto imageRange2 = model2->getValidImageRange();
        if (imageRange1.first.line != imageRange2.first.line ||
            imageRange1.first.samp != imageRange2.first.samp ||
            imageRange1.second.line != imageRange2.second.line ||
            imageRange1.second.samp != imageRange2.second.samp)
        {
            std::cerr << "getValidImageRange() returned different values:"
                      << std::endl;
            std::cerr << "    " << imageRange1.first.line << " "
                      << imageRange1.first.samp << " "
                      << imageRange1.second.line << " "
                      << imageRange1.second.samp << std::endl;
            std::cerr << "    " << imageRange2.first.line << " "
                      << imageRange2.first.samp << " "
                      << imageRange2.second.line << " "
                      << imageRange2.second.samp << std::endl;
            testPassed = false;
        }

        auto heightRange1 = model1->getValidHeightRange();
        auto heightRange2 = model2->getValidHeightRange();
        if (heightRange1.first != heightRange2.first ||
            heightRange1.second != heightRange2.second)
        {
            std::cerr << "getValidHeightRange() returned different values:"
                      << std::endl;
            std::cerr << "    " << heightRange1.first << " "
                      << heightRange1.second << std::endl;
            std::cerr << "    " << heightRange2.first << " "
                      << heightRange2.second << std::endl;
            testPassed = false;
        }

        auto imageSize1 = model1->getImageSize();
        auto imageSize2 = model2->getImageSize();
        if (imageSize1.line != imageSize2.line ||
            imageSize1.samp != imageSize2.samp)
        {
            std::cerr << "getImageSize() returned different values"
                      << std::endl;
            std::cerr << "    " << imageSize1.line << " " << imageSize1.samp
                      << std::endl;
            std::cerr << "    " << imageSize1.line << " " << imageSize1.samp
                      << std::endl;
            testPassed = false;
        }

        auto imageStart1 = model1->getImageStart();
        auto imageStart2 = model2->getImageStart();
        if (imageStart1.line != imageStart2.line ||
            imageStart1.samp != imageStart2.samp)
        {
            std::cerr << "getImageStart() returned different values"
                      << std::endl;
            testPassed = false;
        }

        try
        {
            double imageTime1 = model1->getImageTime(imageStart1);
            double imageTime2 = model2->getImageTime(imageStart1);
            if (imageTime1 != imageTime2)
            {
                std::cerr << "getImageTime() returned different values:"
                          << std::endl;
                std::cerr << "    " << imageTime1 << std::endl;
                std::cerr << "    " << imageTime2 << std::endl;
                testPassed = false;
            }
        }
        catch (const csm::Error& ex)
        {
            std::cerr << "getImageTime() threw exception:" << std::endl;
            std::cerr << "    " << ex.what() << std::endl;
            testPassed = false;
        }

        double numParams1 = model1->getNumParameters();
        double numParams2 = model2->getNumParameters();
        if (numParams1 != numParams2)
        {
            std::cerr << "getNumParameters() returned different values:"
                      << std::endl;
            std::cerr << "    " << numParams1 << std::endl;
            std::cerr << "    " << numParams2 << std::endl;
            testPassed = false;
        }

        try
        {
            auto sensorPosition1 = model1->getSensorPosition(0.0);
            auto sensorPosition2 = model2->getSensorPosition(0.0);
            if (sensorPosition1.x != sensorPosition2.x ||
                sensorPosition1.y != sensorPosition2.y ||
                sensorPosition1.z != sensorPosition2.z)
            {
                std::cerr << "getSensorPosition(double time) returned "
                             "different values"
                          << std::endl;
                std::cerr << "    " << sensorPosition1.x << " "
                          << sensorPosition1.y << " " << sensorPosition1.z
                          << std::endl;
                std::cerr << "    " << sensorPosition2.x << " "
                          << sensorPosition2.y << " " << sensorPosition2.z
                          << std::endl;
                testPassed = false;
            }
        }
        catch (const csm::Error& ex)
        {
            std::cerr << "getSensorPosition(double time) threw exception:"
                      << std::endl;
            std::cerr << "    " << ex.what() << std::endl;
            testPassed = false;
        }

        try
        {
            auto sensorVelocity1 = model1->getSensorVelocity(0.0);
            auto sensorVelocity2 = model2->getSensorVelocity(0.0);
            if (sensorVelocity1.x != sensorVelocity2.x ||
                sensorVelocity1.y != sensorVelocity2.y ||
                sensorVelocity1.z != sensorVelocity2.z)
            {
                std::cerr << "getSensorVelocity(double time) returned "
                             "different values"
                          << std::endl;
                std::cerr << "    " << sensorVelocity1.x << " "
                          << sensorVelocity1.y << " " << sensorVelocity1.z
                          << std::endl;
                std::cerr << "    " << sensorVelocity2.x << " "
                          << sensorVelocity2.y << " " << sensorVelocity2.z
                          << std::endl;
                testPassed = false;
            }
        }
        catch (const csm::Error& ex)
        {
            std::cerr << "getSensorVelocity(double time) threw exception:"
                      << std::endl;
            std::cerr << "    " << ex.what() << std::endl;
            testPassed = false;
        }

        double desiredPrecision = 0.1;
        double achievedPrecision1, achievedPrecision2;
        csm::ImageCoord imPt1 = model1->groundToImage(refPt1,
                                                      desiredPrecision,
                                                      &achievedPrecision1,
                                                      &warnings);
        displayWarnings("groundToImage()", 1, warnings);
        csm::ImageCoord imPt2 = model2->groundToImage(refPt1,
                                                      desiredPrecision,
                                                      &achievedPrecision2,
                                                      &warnings);
        displayWarnings("groundToImage()", 2, warnings);
        if (imPt1.line != imPt2.line || imPt1.samp != imPt2.samp ||
            achievedPrecision1 != achievedPrecision2)
        {
            std::cerr << "groundToImage() returned different values"
                      << std::endl;
            std::cerr << "    " << imPt1.line << " " << imPt1.samp << "  "
                      << achievedPrecision1 << std::endl;
            std::cerr << "    " << imPt2.line << " " << imPt2.samp << "  "
                      << achievedPrecision2 << std::endl;
            testPassed = false;
        }

        double height = 0.0;
        height = std::min(heightRange1.second,
                          std::max(heightRange1.first, height));
        height = std::min(heightRange2.second,
                          std::max(heightRange2.first, height));

        csm::EcefCoord gndPt1 = model1->imageToGround(imPt1,
                                                      height,
                                                      desiredPrecision,
                                                      &achievedPrecision1,
                                                      &warnings);
        displayWarnings("imageToGround()", 1, warnings);
        csm::EcefCoord gndPt2 = model2->imageToGround(imPt1,
                                                      height,
                                                      desiredPrecision,
                                                      &achievedPrecision2,
                                                      &warnings);
        displayWarnings("imageToGround()", 2, warnings);
        if (gndPt1.x != gndPt2.x || gndPt1.y != gndPt2.y ||
            gndPt1.z != gndPt2.z || achievedPrecision1 != achievedPrecision2)
        {
            std::cerr << "imageToGround() returned different values"
                      << std::endl;
            std::cerr << "    " << gndPt1.x << " " << gndPt1.y << " "
                      << gndPt1.z << "  " << achievedPrecision1 << std::endl;
            std::cerr << "    " << gndPt2.x << " " << gndPt2.y << " "
                      << gndPt2.z << "  " << achievedPrecision2 << std::endl;
            testPassed = false;
        }

        try
        {
            auto sensorPosition1 = model1->getSensorPosition(imPt1);
            auto sensorPosition2 = model2->getSensorPosition(imPt1);
            if (sensorPosition1.x != sensorPosition2.x ||
                sensorPosition1.y != sensorPosition2.y ||
                sensorPosition1.z != sensorPosition2.z)
            {
                std::cerr << "getSensorPosition(ImageCoord imagePt) returned "
                             "different values"
                          << std::endl;
                std::cerr << "    " << sensorPosition1.x << " "
                          << sensorPosition1.y << " " << sensorPosition1.z
                          << std::endl;
                std::cerr << "    " << sensorPosition2.x << " "
                          << sensorPosition2.y << " " << sensorPosition2.z
                          << std::endl;
                testPassed = false;
            }
        }
        catch (const csm::Error& ex)
        {
            std::cerr
                    << "getSensorPosition(ImageCoord imagePt) threw exception:"
                    << std::endl;
            std::cerr << "    " << ex.what() << std::endl;
            testPassed = false;
        }

        try
        {
            auto sensorVelocity1 = model1->getSensorVelocity(imPt1);
            auto sensorVelocity2 = model2->getSensorVelocity(imPt2);
            if (sensorVelocity1.x != sensorVelocity2.x ||
                sensorVelocity1.y != sensorVelocity2.y ||
                sensorVelocity1.z != sensorVelocity2.z)
            {
                std::cerr << "getSensorVelocity(ImageCoord imagePt) returned "
                             "different values"
                          << std::endl;
                std::cerr << "    " << sensorVelocity1.x << " "
                          << sensorVelocity1.y << " " << sensorVelocity1.z
                          << std::endl;
                std::cerr << "    " << sensorVelocity2.x << " "
                          << sensorVelocity2.y << " " << sensorVelocity2.z
                          << std::endl;
                testPassed = false;
            }
        }
        catch (const csm::Error& ex)
        {
            std::cerr
                    << "getSensorVelocity(ImageCoord imagePt) threw exception:"
                    << std::endl;
            std::cerr << "    " << ex.what() << std::endl;
            testPassed = false;
        }

        std::vector<double> gndPartials1 =
                model1->computeGroundPartials(refPt1);
        std::vector<double> gndPartials2 =
                model2->computeGroundPartials(refPt2);
        if (gndPartials1 != gndPartials2)
        {
            std::cerr << "computeGroundPartials() returned different values:"
                      << std::endl;
            std::cerr << "    " << gndPartials1[0] << " " << gndPartials1[1]
                      << "  " << gndPartials1[2] << " " << gndPartials1[3]
                      << "  " << gndPartials1[4] << " " << gndPartials1[5]
                      << "  " << std::endl;
            std::cerr << "    " << gndPartials2[0] << " " << gndPartials2[1]
                      << "  " << gndPartials2[2] << " " << gndPartials2[3]
                      << "  " << gndPartials2[4] << " " << gndPartials2[5]
                      << "  " << std::endl;
            testPassed = false;
        }

        std::vector<double> unmodeledError1 = model1->getUnmodeledError(imPt1);
        std::vector<double> unmodeledError2 = model2->getUnmodeledError(imPt2);
        if (unmodeledError1 != unmodeledError2)
        {
            std::cerr << "getUnmodeledError() returned different values:"
                      << std::endl;
            std::cerr << "    " << unmodeledError1[0] << " "
                      << unmodeledError1[1] << "  " << unmodeledError1[2] << " "
                      << unmodeledError1[3] << std::endl;
            std::cerr << "    " << unmodeledError2[0] << " "
                      << unmodeledError2[1] << "  " << unmodeledError2[2] << " "
                      << unmodeledError2[3] << std::endl;
            testPassed = false;
        }

        return testPassed;
    }

private:
    const std::string mSiddPathname1;
    const std::string mSiddPathname2;
    const csm::Plugin& mPlugin;

    six::XMLControlRegistry mXmlRegistry;
    six::NITFReadControl mReader1;
    six::NITFReadControl mReader2;
};

}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc != 3)
        {
            std::cerr << "Usage: " << fs::path(argv[0]).filename().string()
                      << " <measurable SIDD pathname> <polynomial fit gridded "
                         "display SIDD pathname>\n\n";
            return 1;
        }
        sys::OS os;

        // Go up two levels from current dir
        const std::string installPathname =
                sys::Path::splitPath(
                        sys::Path::splitPath(os.getCurrentExecutable()).first)
                        .first;

        const std::string dllPathname = findDllPathname(installPathname);
        const std::string confDir =
                sys::Path::joinPaths(installPathname, "conf");
        if (!os.exists(confDir))
        {
            throw except::Exception(Ctxt("Unable to find conf dir."));
        }

        const std::string measSiddPathname(argv[1]);
        const std::string pfgdSiddPathname(argv[2]);

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
            throw except::Exception(Ctxt("Unexpected plugin name '" +
                                         plugin.getPluginName() + "'"));
        }

        Test test(measSiddPathname, pfgdSiddPathname, confDir, plugin);

        std::cout << "*********************" << std::endl;
        std::cout << "Testing from File ISD" << std::endl;
        std::cout << "*********************" << std::endl;
        bool testPassed1 = test.testFileISD();

        std::cout << std::endl << std::endl;
        std::cout << "*********************" << std::endl;
        std::cout << "Testing from NITF ISD" << std::endl;
        std::cout << "*********************" << std::endl;
        bool testPassed2 = test.testNitfISD();

        return (testPassed1 && testPassed2) ? 0 : 1;
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
