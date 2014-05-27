/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include <fstream>
#include <stdexcept>
#include <memory>

#include <sys/Path.h>
#include <str/Convert.h>
#include <except/Exception.h>
#include <logging/Setup.h>
#include <six/XMLControlFactory.h>
#include <six/NITFReadControl.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>
#include <scene/ECEFToLLATransform.h>

namespace
{
types::RowCol<double> pixelToImagePoint(const six::sicd::ComplexData& data,
                                        const types::RowCol<double>& pixelLoc)
{
    const six::sicd::ImageData& imageData(*data.imageData);
    const types::RowCol<double> scpPixel(imageData.scpPixel);
    const types::RowCol<double> aoiOffset(imageData.firstRow,
                                          imageData.firstCol);

    const types::RowCol<double> offset(scpPixel - aoiOffset);

    const types::RowCol<double> sampleSpacing(
            data.grid->row->sampleSpacing,
            data.grid->col->sampleSpacing);

    const types::RowCol<double> imagePt(
            (pixelLoc.row - offset.row) * sampleSpacing.row,
            (pixelLoc.col - offset.col) * sampleSpacing.col);

    return imagePt;
}

std::ostream& operator<<(std::ostream& os, const scene::LatLonAlt& lla)
{
    os << "(" << lla.getLat() << ", " << lla.getLon() << ", "
       << lla.getAlt() << ")";
    return os;
}
}

template<size_t _M, size_t _N, typename _T> void printMatrix(const math::linear::MatrixMxN<_M,_N,_T>& mx, std::ostream& os)
{
	for (size_t i = 0; i < _M; ++i)
	{
		for (size_t j = 0; j < _N; ++j)
		{
			os << ", " << mx(i,j);
		}
	}
	os << std::endl;
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        const std::string progname(sys::Path::basename(argv[0]));
        if (argc != 2)
        {
            std::cerr << "Usage: " << progname
                      << " <SICD pathname>" << std::endl;
            return 1;
        }
        const std::string sicdPathname(argv[1]);
        //const types::RowCol<double> pixelLoc(str::toType<double>(argv[2]),
        //                                     str::toType<double>(argv[3]));
//        const bool haveHeight(argc == 5);
//        const double height(haveHeight ? str::toType<double>(argv[4]) : 0.0);

        // Read in the file
        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(six::DataType::COMPLEX,
                               new six::XMLControlCreatorT<
                                       six::sicd::ComplexXMLControl>());

        std::auto_ptr<logging::Logger> logger(logging::setupLogger(progname));
        six::NITFReadControl reader;
        reader.setLogger(logger.get());
        reader.setXMLControlRegistry(&xmlRegistry);
        reader.load(sicdPathname);

        // Make sure it's a SICD
        const six::Container* const container = reader.getContainer();
        if (container->getDataType() != six::DataType::COMPLEX)
        {
            std::cerr << sicdPathname << " is not a SICD\n";
            return 1;
        }
        const six::sicd::ComplexData* const data =
                reinterpret_cast<const six::sicd::ComplexData*>(
                        container->getData(0));

        // Build up the geometry info
        std::auto_ptr<const scene::SceneGeometry>
                geom(six::sicd::Utilities::getSceneGeometry(data));
        std::auto_ptr<const scene::ProjectionModel>
                projection(six::sicd::Utilities::getProjectionModel(
                        data, geom.get()));

        //const types::RowCol<double> imagePt =
        //        pixelToImagePoint(*data, pixelLoc);

        const scene::ECEFToLLATransform ecefToLLA;

        // Call imageToScene() where we don't specify a height
        const scene::Vector3 refPos(geom->getReferencePosition());
        scene::Vector3 groundPlaneNormal(refPos);
        groundPlaneNormal.normalize();

        double timeCOA(0.0);

        const types::RowCol<double> scpPixel(data->imageData->scpPixel);
        types::RowCol<double> scpPt = pixelToImagePoint(*data, scpPixel);
        const scene::Vector3 groundPt1 =
                        projection->imageToScene(scpPt, refPos, groundPlaneNormal,
                                                 &timeCOA);
        const scene::LatLonAlt latLon1(ecefToLLA.transform(groundPt1));
        double height = latLon1.getAlt();

        std::string function_output = sicdPathname;
        function_output.append(".function");
        std::string derivative_output = sicdPathname;
        derivative_output.append(".derivative");
        std::string sensorderivative_output = sicdPathname;
        sensorderivative_output.append(".sensorderivative");
        size_t numRows = data->imageData->numRows;
        size_t numCols = data->imageData->numCols;
        size_t deltaRow = (size_t) numRows/10;
        size_t deltaCol = (size_t) numCols/10;
        std::ofstream functionFile;
        std::ofstream derivativeFile;
        std::ofstream sensorderivativeFile;
        functionFile.open(function_output.data());
        if (!functionFile.is_open())
        {
        	std::cerr << "Could not open file: " << function_output << std::endl;
        }
        derivativeFile.open(derivative_output.data());
        if (!derivativeFile.is_open())
        {
        	std::cerr << "Could not open file: " << derivative_output << std::endl;
        }
        sensorderivativeFile.open(sensorderivative_output.data());
        if (!sensorderivativeFile.is_open())
        {
        	std::cerr << "Could not open file: " << sensorderivative_output << std::endl;
        }

        for (size_t i = 0; i < numRows; i = i+deltaRow)
        {
        	for (size_t j = 0; j < numCols; j = j+deltaCol)
        	{
        		types::RowCol<double> pixelLoc(i,j);
        		types::RowCol<double> imagePt = pixelToImagePoint(*data, pixelLoc);
        		scene::Vector3 groundPt2 = projection->imageToScene(imagePt,height);
        		functionFile << i << "," << j << "," <<
        		        		groundPt2[0] << "," <<
        		        		groundPt2[1] << "," <<
        		        		groundPt2[2] << std::endl;
        	}
        }
        for (size_t i = 0; i < numRows; i = i+deltaRow)
        {
        	for (size_t j = 0; j < numCols; j = j+deltaCol)
        	{
        		types::RowCol<double> pixelLoc(i,j);
        		types::RowCol<double> imagePt = pixelToImagePoint(*data, pixelLoc);
        		scene::Vector3 groundPt2 = projection->imageToScene(imagePt,height);
        		for (double deltapow = -10; deltapow < 1; deltapow = deltapow+0.1)
        		        {
        		        	double delta = pow(10,deltapow);
        		        	derivativeFile << delta;
        		        	printMatrix(projection->imageToScenePartials(imagePt, height, groundPt2, delta), derivativeFile);
        		        	sensorderivativeFile << delta;
        		        	printMatrix(projection->imageToSceneSensorPartials(imagePt, height, groundPt2, delta), sensorderivativeFile);
        		        }
        	}
        }

        functionFile.close();
        derivativeFile.close();

        //const scene::Vector3 groundPt2 = projection->imageToScene(imagePt, height);
         //std::cout << "Sensor Partials:" << std::endl;
        //std::cout << projection->imageToSceneSensorPartials(imagePt, height, groundPt2, (double) 0.0001);


        // Call imageToScene() where we do specify a height, using the height
        // from the previous call
//        const scene::Vector3 groundPt2 =
//                projection->imageToScene(imagePt, latLon1.getAlt());
//
//        const scene::LatLonAlt latLon2(ecefToLLA.transform(groundPt2));
//
//        // These two better be pretty close
//        std::cout << "Method 1:         " << latLon1
//                  << "\nMethod 2:         " << latLon2
//                  << std::endl;
//
//        if (haveHeight)
//        {
//            // Now call imageToScene() with the requested height
//            const scene::Vector3 groundPt3 =
//                    projection->imageToScene(imagePt, height);
//
//            const scene::LatLonAlt latLon3(ecefToLLA.transform(groundPt3));
//
//            std::cout << "Requested height: " << latLon3 << std::endl;
//        }

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
