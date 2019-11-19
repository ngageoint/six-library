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

#include <import/six.h>
#include <import/six/sidd.h>
#include <import/six/sicd.h>
#include <import/sio/lite.h>
#include <import/io.h>
#include <import/xml/lite.h>
#include "utils.h"

/*!
 *  This file takes in an SIO and turns it in to a SICD.
 *  It uses the WriteControl::save() function to stream
 *  it from file.  SIO's are native endian, NITF's are big endian
 *
 *  Since the data is a raw-stream, and sio.lite has FileReader defined
 *  already as a SeekableStream, we can stream the file directly in
 *  to the library.
 *
 *  The sio.lite module does not byte swap on read.  If it did, we would use
 *  the auto-swap facility that is default in the writer, which would assume
 *  that the bytes from the InputStream are in the same endian-ness as the
 *  host.  However, telling it which way to swap is actually optimal, so that
 *  an SIO written on a big-endian machine will not be byte swapped if the
 *  host machine is little-endian.
 *
 *  Here, we demonstrate checking if the endian-ness is different
 *  from the system and the system is BE (meaning data is LE).
 *
 *  This is a sample demonstration of using this file to segment
 *
 *  test_create_sidd sidd.xml martinirgb.sio 3 martinirgb.nitf
 *
 *  You can re-read the file into an SIO as follows:
 *
 *  test_dump_sidd_lines martinirgb.nitf -sio
 *
 *  If the file name ends in nitf or ntf, the file will be written as a
 *  NITF.  Otherwise, it will be written as a GeoTIFF.
 *
 */
const char* IMGMANIP_COLORMAP = "imgmanip_color_map";

/*!
 *  Figure out what kind of SIO data we have.  Could be MONO8I, RGB8LU or
 *  RGB24I.
 *
 *
 *  If its 8-bit indexed color, LUT is non-null
 */
six::LUT* getPixelInfo(sio::lite::FileHeader* fileHeader,
        six::PixelType& pixelType)
{

    pixelType = six::PixelType::MONO8I;

    int et = fileHeader->getElementType();
    int es = fileHeader->getElementSize();

    if (et != sio::lite::FileHeader::UNSIGNED && et
            != sio::lite::FileHeader::N_BYTE_UNSIGNED)
        throw except::Exception(Ctxt("Expected a product SIO image!"));

    if (es != 1 && es != 3)
        throw except::Exception(Ctxt("Expected a byte image!"));

    if (es == 2)
    {
        pixelType = six::PixelType::MONO16I;
    }
    else if (es == 3)
    {
        pixelType = six::PixelType::RGB24I;
    }

    if (!fileHeader->getNumUserDataFields())
        return NULL;

    // Otherwise
    six::LUT* lut = NULL;

    sio::lite::UserDataDictionary dict = fileHeader->getUserDataSection();
    for (sio::lite::UserDataDictionary::Iterator p = dict.begin(); p
            != dict.end(); ++p)
    {
        if (p->first == IMGMANIP_COLORMAP)
        {
            pixelType = six::PixelType::RGB8LU;
            // Switch the mode, and dont forget to slurp the colormap
            lut = new six::LUT((unsigned char*) &(p->second)[0], 256, 3);
            break;
        }

    }
    return lut;
}


six::WriteControl* getWriteControl(std::string outputName)
{

    sys::Path::StringPair p = sys::Path::splitExt(outputName);
    str::lower(p.second);

    six::WriteControl* writer = NULL;

    if (p.second == ".nitf" || p.second == ".ntf")
    {
        writer = new six::NITFWriteControl();
        std::cout << "Selecting NITF write control" << std::endl;
    }
#if !defined(SIX_TIFF_DISABLED)
    else
    {
        writer = new six::sidd::GeoTIFFWriteControl();
        std::cout << "Selecting GeoTIFF write control" << std::endl;
    }
#endif
    return writer;
}

/*!
 *  Read a SICD XML into a data structure.  If this was in a NITF
 *  you wouldnt bother with this step, since the ReadControl would
 *  do this for you.
 *
 */
six::sicd::ComplexData* getComplexData(std::string sicdXMLName)
{
    // Create a file input stream
    io::FileInputStream sicdXMLFile(sicdXMLName);

    // Create an XML parser
    xml::lite::MinidomParser xmlParser;

    // Parse the SICD XML input file
    xmlParser.parse(sicdXMLFile);

    // Get the SICD DOM
    xml::lite::Document *doc = xmlParser.getDocument();

    six::sicd::ComplexXMLControl xmlControl;
    return (six::sicd::ComplexData*) xmlControl.fromXML(
                doc, std::vector<std::string>());
}

/*!
 *  We want to create a SIDD NITF from something
 *  else.  For this simple example, I will use
 *  sio.lite to read in the image data.
 *
 *  SICD data is read in from the first argument.  To test multi-image
 *  SIDD, the <N times> argument uses the target <input-file> over and
 *  over as different images in the NITF.
 *
 *  The segmentation loophole can be exploitated by overriding the product
 *  size (essentially bluffing the 10GB limit, and overriding ILOC_R=99999,
 *  although you may not extend those limits -- they are NITF format maxes.
 */
int main(int argc, char** argv)
{
    if (argc != 5 && argc != 7)
    {
        die_printf(
                "Usage: %s <sicd-xml> <input-file> <N times> <output-file> (Max product size) (N rows limit)\n",
                argv[0]);
    }

    // The input SIO file
    std::string inputName(argv[2]);

    // How many images to write (from the one source)
    unsigned int repeatN = str::toType<unsigned int>(argv[3]);

    // Output file name
    std::string outputName(argv[4]);

    // Get a NITF or GeoTIFF writer
    six::WriteControl* writer = getWriteControl(outputName);

    // Is the SIO in big-endian?
    bool needsByteSwap = false;

    try
    {
        try
        {
            sys::OS().getEnv(six::SCHEMA_PATH);
        }
        catch(const except::Exception& )
        {
            throw except::Exception(Ctxt(
                    "Must specify SIDD schema path via " +
                    std::string(six::SCHEMA_PATH) + " environment variable"));
        }

        six::XMLControlFactory::getInstance().
            addCreator(
                six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>()
                );

        six::XMLControlFactory::getInstance().
            addCreator(
                six::DataType::DERIVED,
                new six::XMLControlCreatorT<six::sidd::DerivedXMLControl>()
                );

        // Get a Complex Data structure from an XML file
        six::Options options;

        // Set up the sicd
        io::FileInputStream fis(argv[1]);
        xml::lite::MinidomParser parser;
        parser.parse(fis);

        std::auto_ptr<logging::Logger> log (new logging::NullLogger());
        six::Data* complexData =
            six::XMLControlFactory::getInstance().newXMLControl(
                six::DataType::COMPLEX,
                log.get())->fromXML(parser.getDocument(),
                                    std::vector<std::string>());

        // Create a file container
        mem::SharedPtr<six::Container> container(new six::Container(
                six::DataType::DERIVED));

        // We have a source for each image
        std::vector<io::InputStream*> sources;

        // For each image
        for (unsigned int i = 0; i < repeatN; ++i)
        {
            // Make an sio.lite reader
            sio::lite::FileReader *sioReader = new sio::lite::FileReader(
                    new io::FileInputStream(inputName));

            // Get the header out
            sio::lite::FileHeader* fileHeader = sioReader->readHeader();

            /*
             * Yeah, this is getting set over and over, but that way its
             * easy to make this test case into a program with multiple images
             */
            needsByteSwap = sys::isBigEndianSystem()
                    && fileHeader->isDifferentByteOrdering();

            six::PixelType pixelType;

            // If we got past here, it must be one of our types
            six::LUT* lut = getPixelInfo(fileHeader, pixelType);

            // Make the object
            six::sidd::DerivedDataBuilder builder;
            six::sidd::DerivedData* data = builder.steal(); //steal it

            builder.addDisplay(pixelType);
            builder.addGeographicAndTarget(six::RegionType::GEOGRAPHIC_INFO);
            builder.addMeasurement(six::ProjectionType::PLANE);
            builder.addExploitationFeatures(1);

            data->setNumRows(fileHeader->getNumLines());
            data->setNumCols(fileHeader->getNumElements());

            data->productCreation->productName = "ProductName";
            data->productCreation->productClass = "Classy";
            data->productCreation->classification.classification = "U";

            six::sidd::ProcessorInformation& processorInformation =
                *data->productCreation->processorInformation;

            processorInformation.application = "ProcessorName";
            processorInformation.profile = "Profile";
            processorInformation.site = "Ypsilanti, MI";

            data->display->pixelType = pixelType;
            data->display->decimationMethod = six::DecimationMethod::BRIGHTEST_PIXEL;
            data->display->magnificationMethod = six::MagnificationMethod::NEAREST_NEIGHBOR;

            // Give'em our LUT
            if (lut)
            {
                if (pixelType == six::PixelType::RGB24I)
                {
                    data->display->remapInformation.reset(
                        new six::sidd::ColorDisplayRemap(lut));
                }
                else
                {
                    data->display->remapInformation.reset(
                        new six::sidd::MonochromeDisplayRemap("PEDF", lut));
                }
            }
            data->setImageCorners(makeUpCornersFromDMS());

            six::sidd::PlaneProjection* planeProjection =
                (six::sidd::PlaneProjection*) data->measurement->projection.get();

            planeProjection->timeCOAPoly = six::Poly2D(0, 0);
            planeProjection->timeCOAPoly[0][0] = 1;
            data->measurement->arpPoly = six::PolyXYZ(0);
            data->measurement->arpPoly[0] = 0.0;
            planeProjection->productPlane.rowUnitVector = 0.0;
            planeProjection->productPlane.colUnitVector = 0.0;

            six::sidd::Collection* parent =
                data->exploitationFeatures->collections[0].get();

            parent->information->resolution.rg = 0;
            parent->information->resolution.az = 0;
            parent->information->collectionDuration = 0;
            parent->information->collectionDateTime = six::DateTime();
            parent->information->radarMode = six::RadarModeType::SPOTLIGHT;
            parent->information->sensorName = "the sensor";
            data->exploitationFeatures->product.resolution.row = 0;
            data->exploitationFeatures->product.resolution.col = 0;

            data->annotations.push_back(mem::ScopedCopyablePtr<
                    six::sidd::Annotation>(new six::sidd::Annotation));
            six::sidd::Annotation *ann = (*data->annotations.rbegin()).get();
            std::cout << "Hey: " << ann->spatialReferenceSystem.get() << std::endl;
            ann->identifier = "1st Annotation";
            ann->objects.push_back(mem::ScopedCloneablePtr<
                    six::sidd::SFAGeometry>(new six::sidd::SFAPoint));

            sources.push_back(sioReader);
            container->addData(data);
        }
        container->addData(complexData);

        /*
         *  Under normal circumstances, the library uses the
         *  segmentation algorithm in the SICD spec, and numRowsLimit
         *  is set to Contants::ILOC_SZ.  If the user sets this, they
         *  want us to create an alternate numRowsLimit to force the
         *  library to segmeht on smaller boundaries.
         *
         *  This is handy especially for debugging, since it will force
         *  the algorithm to segment early.
         *
         */
        if (argc == 7)
        {
            std::cout << "Overriding NITF product size and max ILOC"
                    << std::endl;
            writer->getOptions().setParameter(
                    six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE, str::toType<
                            long>(argv[5]));

            writer->getOptions().setParameter(
                    six::NITFHeaderCreator::OPT_MAX_ILOC_ROWS,
                    str::toType<long>(argv[6]));

        }

        // Override auto-byte swap
        writer->getOptions().setParameter(six::WriteControl::OPT_BYTE_SWAP,
                six::Parameter((sys::Uint16_T) needsByteSwap));

        // Init the container
        writer->initialize(container);

        // Save the file
        writer->save(sources, outputName);

        // Delete the sources (sio read streams)
        for (unsigned int i = 0; i < sources.size(); ++i)
        {
            delete sources[i];
        }
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
    }

    delete writer;

    return 0;
}
