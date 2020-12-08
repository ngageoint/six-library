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

#include <sys/Conf.h>
#include <sys/Path.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <scene/ProjectionModel.h>
#include <scene/Utilities.h>
#include <six/NITFReadControl.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/Utilities.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sidd/DerivedXMLControl.h>

#include <sys/Filesystem.h>
namespace fs = std::filesystem;

namespace
{
void usage(const std::string& progname, std::ostream& ostr)
{
    ostr << progname << " -h\n"
         << progname
         << " [-ecef_to_image x y z] [-latlon_to_image lat lon alt]"
         << " [-image_to_ground row col] <SICD pathname>\n\n"
         << "Exactly one of -ecef_to_image, -latlon_to_image, or "
         << "-image_to_ground must be specified\n\n";
}

class Converter
{
public:
    Converter(const std::string& pathname);

    void groundToImage(const scene::Vector3& groundPt) const;

    void groundToImage(const scene::LatLonAlt& latLon) const
    {
        groundToImage(scene::Utilities::latLonToECEF(latLon));
    }

    void imageToGround(double row, double col) const;

private:
    std::unique_ptr<const scene::SceneGeometry> mGeometry;
    std::unique_ptr<const scene::ProjectionModel> mProjModel;
    scene::Vector3 mGroundPlaneNormal;

    types::RowCol<double> mSampleSpacing;
    six::RowColInt mSCP;
    types::RowCol<double> mAOIOffset;
};

Converter::Converter(const std::string& pathname)
{
    // Read in the SICD
    six::NITFReadControl reader;

    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(
        six::DataType::COMPLEX,
        new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());
    xmlRegistry.addCreator(
        six::DataType::DERIVED,
        new six::XMLControlCreatorT<six::sidd::DerivedXMLControl>());

    reader.setXMLControlRegistry(&xmlRegistry);
    reader.load(pathname);

    // Verify it's a SICD
    std::shared_ptr<const six::Container> container(reader.getContainer());
    if (container->getDataType() != six::DataType::COMPLEX)
    {
        throw except::InvalidFormatException(Ctxt("Expected a SICD NITF"));
    }
    const six::sicd::ComplexData* sicdData =
        reinterpret_cast<const six::sicd::ComplexData*>(
            container->getData(0));

    // Set up projection model
    mGeometry.reset(six::sicd::Utilities::getSceneGeometry(sicdData));
    mProjModel.reset(six::sicd::Utilities::getProjectionModel(
        sicdData, mGeometry.get()));

    // Save off some stuff we'll need later
    mSampleSpacing.row = sicdData->grid->row->sampleSpacing;
    mSampleSpacing.col = sicdData->grid->col->sampleSpacing;
    mSCP = sicdData->imageData->scpPixel;
    mAOIOffset.row = static_cast<double>(sicdData->imageData->firstRow);
    mAOIOffset.col = static_cast<double>(sicdData->imageData->firstCol);
    mGroundPlaneNormal = six::sicd::Utilities::getGroundPlaneNormal(*sicdData);
}

void Converter::groundToImage(const scene::Vector3& groundPt) const
{
    // Convert to image coordinates
    double timeCOA(0.0);
    const types::RowCol<double> imagePt(mProjModel->sceneToImage(groundPt,
                                                                 &timeCOA));

    // Convert to pixel coordinates
    const types::RowCol<double> pixelPt(
        imagePt.row / mSampleSpacing.row + mSCP.row - mAOIOffset.row,
        imagePt.col / mSampleSpacing.col + mSCP.col - mAOIOffset.col);

    std::cout << "Image point: row = "
              << imagePt.row << ", col = " << imagePt.col
              << "\nTime COA: " << timeCOA
              << "\nPixel point: row = "
              << pixelPt.row << ", col = " << pixelPt.col << std::endl;
}

void Converter::imageToGround(double row, double col) const
{
    // Convert to image coordinates
    const types::RowCol<double> offset(mSCP - mAOIOffset);

    const types::RowCol<double>
        imagePt((row - offset.row) * mSampleSpacing.row,
                (col - offset.col) * mSampleSpacing.col);

    // Convert to scene coordinates
    double timeCOA(0.0);
    const scene::Vector3 groundPt =
        mProjModel->imageToScene(imagePt,
                                 mGeometry->getReferencePosition(),
                                 mGroundPlaneNormal,
                                 &timeCOA);

    const scene::LatLonAlt latLon(scene::Utilities::ecefToLatLon(groundPt));

    std::cout << "Time COA: " << timeCOA
              << "\nECEF: x = " << groundPt[0] << ", y = " << groundPt[1]
              << ", z = " << groundPt[2]
              << "\nLat = " << latLon.getLat() << ", lon = "
              << latLon.getLon() << ", alt = " << latLon.getAlt()
              << std::endl;
}
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        const std::string progname(fs::path(argv[0]).filename());
        if (argc < 2)
        {
            usage(progname, std::cerr);
            return 1;
        }

        const std::string mode(argv[1]);
        if (mode == "-h")
        {
            usage(progname, std::cout);
            return 0;
        }

        const int expectedNumArgs =
            (mode == "-ecef_to_image" || mode == "-latlon_to_image") ?
                6 : 5;
        if (argc != expectedNumArgs)
        {
            usage(progname, std::cerr);
            return 1;
        }

        const std::string pathname(argv[expectedNumArgs - 1]);
        const Converter converter(pathname);

        // Run the appropriate conversion
        if (mode == "-ecef_to_image")
        {
            scene::Vector3 groundPt;
            for (size_t ii = 0; ii < 3; ++ii)
            {
                groundPt[ii] = str::toType<double>(argv[2 + ii]);
            }
            converter.groundToImage(groundPt);
        }
        else if (mode == "-latlon_to_image")
        {
            const scene::LatLonAlt latLon(str::toType<double>(argv[2]),
                                          str::toType<double>(argv[3]),
                                          str::toType<double>(argv[4]));
            converter.groundToImage(latLon);
        }
        else if (mode == "-image_to_ground")
        {
            const double row(str::toType<double>(argv[2]));
            const double col(str::toType<double>(argv[3]));
            converter.imageToGround(row, col);
        }
        else
        {
            usage(progname, std::cerr);
            return 1;
        }
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Error: " << ex.toString() << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error\n";
        return 1;
    }

    return 0;
}
