/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <stdexcept>
#include <memory>

#include <std/filesystem>

#include <sys/Path.h>
#include <str/Convert.h>
#include <except/Exception.h>
#include <logging/Setup.h>
#include <six/XMLControlFactory.h>
#include <six/NITFReadControl.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/Utilities.h>
#include <scene/ECEFToLLATransform.h>

namespace fs = std::filesystem;

namespace
{
std::ostream& operator<<(std::ostream& os, const scene::LatLonAlt& lla)
{
    os << "(" << lla.getLat() << ", " << lla.getLon() << ", "
       << lla.getAlt() << ")";
    return os;
}
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        const auto progname(fs::path(argv[0]).filename());
        if (argc != 4 && argc != 5)
        {
            std::cerr << "Usage: " << progname
                      << " <SICD/SIDD pathname> <row pixel> <col pixel>"
                         " [height (m)]\n\n";
            return 1;
        }
        const std::string sixPathname(argv[1]);
        const types::RowCol<double> pixelLoc(str::toType<double>(argv[2]),
                                             str::toType<double>(argv[3]));
        const bool haveHeight(argc == 5);
        const double height(haveHeight ? str::toType<double>(argv[4]) : 0.0);

        // First, try to read in the file as a SICD, then try read as SIDD
        six::XMLControlRegistry* xmlRegistry = new six::XMLControlRegistry;
        xmlRegistry->addCreator(six::DataType::COMPLEX,
                               new six::XMLControlCreatorT<
                                       six::sicd::ComplexXMLControl>());
        xmlRegistry->addCreator(six::DataType::DERIVED,
                            new six::XMLControlCreatorT<six::sidd::DerivedXMLControl>());

        std::unique_ptr<logging::Logger> logger(logging::setupLogger(progname));
        six::NITFReadControl reader;
        reader.setLogger(logger.get());
        reader.setXMLControlRegistry(xmlRegistry);
        reader.load(sixPathname);
        // Check to see if it's a SICD
        auto container = reader.getContainer();
        std::unique_ptr<scene::ProjectionModel> projection;
        std::unique_ptr<scene::SceneGeometry> geom;
        scene::Vector3 groundPlaneNormal;
        types::RowCol<double> imagePt;
        if (container->getDataType() == six::DataType::COMPLEX)
        {
            const six::sicd::ComplexData* data =
                    reinterpret_cast<six::sicd::ComplexData*>(
                            container->getData(0));
            geom.reset(six::sicd::Utilities::getSceneGeometry(data));
            projection.reset(six::sicd::Utilities::getProjectionModel(
                    data, geom.get()));
            imagePt = data->pixelToImagePoint(pixelLoc);
            groundPlaneNormal = six::sicd::Utilities::getGroundPlaneNormal(*data);
        }
        else if (container->getDataType() == six::DataType::DERIVED)
        {
            const six::sidd::DerivedData* data =
                    reinterpret_cast<six::sidd::DerivedData*>(
                            container->getData(0));
            geom = six::sidd::Utilities::getSceneGeometry(data);
            projection = six::sidd::Utilities::getProjectionModel(data);
            imagePt = data->pixelToImagePoint(pixelLoc);
            groundPlaneNormal = geom->getReferencePosition().unit();
        }
        else
        {
            std::cerr << sixPathname << " is not a SICD or SIDD\n";
            return 1;
        }

        std::cout << "Converted imagePt : " << imagePt.row << ", "
            << imagePt.col << std::endl;

        const scene::ECEFToLLATransform ecefToLLA;
        const scene::Vector3 refPos(geom->getReferencePosition());
        double timeCOA(0.0);

        // Call imageToScene() where we don't specify a height
        const scene::Vector3 groundPt1 =
                projection->imageToScene(imagePt, refPos, groundPlaneNormal,
                                         &timeCOA);
        std::cout << "Ground Point : " << groundPt1 << std::endl;

        types::RowCol<double> imageReturnPoint =
                projection->sceneToImage(groundPt1, &timeCOA);

        std::cout << "Image Point : " << imageReturnPoint.row << ", "
                  << imageReturnPoint.col << std::endl;

        types::RowCol<double> imageSCP =
                projection->sceneToImage(geom->getReferencePosition(),
                                         &timeCOA);

        std::cout << "Image SCP Point : " << imageSCP.row << ", "
                  << imageSCP.col << std::endl;

        const scene::LatLonAlt latLon1(ecefToLLA.transform(groundPt1));

        // Call imageToScene() where we do specify a height, using the height
        // from the previous call
        const scene::Vector3 groundPt2 =
                projection->imageToScene(imagePt, latLon1.getAlt());

        const scene::LatLonAlt latLon2(ecefToLLA.transform(groundPt2));

        // These two better be pretty close
        std::cout << "Method 1:         " << latLon1
                  << "\nMethod 2:         " << latLon2
                  << std::endl;

        if (haveHeight)
        {
            // Now call imageToScene() with the requested height
            const scene::Vector3 groundPt3 =
                    projection->imageToScene(imagePt, height);

            const scene::LatLonAlt latLon3(ecefToLLA.transform(groundPt3));

            std::cout << "Requested height: " << latLon3 << std::endl;
        }

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}
