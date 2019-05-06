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

#include <import/cli.h>
#include <import/six.h>
#include <import/sio/lite.h>
#include <import/io.h>
#include <logging/Setup.h>
#include <scene/Utilities.h>
#include "utils.h"

// For SICD implementation
#include <import/six/sicd.h>

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
 *  test_create_sicd WRA_.6844L0001.vv04.sio WRA_sicd.nitf 10000000 10000
 *
 *  You can re-read the file into an SIO as follows:
 *
 *  test_dump_sicd_lines WRA_sicd.nitf -sio
 */

/*!
 *  We want to create a SICD NITF from something
 *  else.  For this simple example, I will use
 *  sio.lite to read in the image data.
 *
 *  SICD data is populated using the ComplexData data structure.
 *
 *  The segmentation loophole can be exploited by overriding the product
 *  size (essentially bluffing the 10GB limit, and overriding ILOC_R=99999,
 *  although you may not extend those limits -- they are NITF format maxes.
 */
int main(int argc, char** argv)
{
    try
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription(
                              "This program creates a sample SICD NITF file from a complex SIO input.");
        parser.addArgument("-r --rows", "Rows limit", cli::STORE, "maxRows",
                           "ROWS")->setDefault(-1);
        parser.addArgument("-s --size", "Max product size", cli::STORE,
                           "maxSize", "BYTES")->setDefault(-1);
        parser.addArgument("--class", "Classification Level", cli::STORE,
                           "classLevel", "LEVEL")->setDefault("UNCLASSIFIED");
        parser.addArgument("--schema",
                           "Specify a schema or directory of schemas",
                           cli::STORE);
        parser.addArgument("--version",
                           "Override the SICD version that's created",
                           cli::STORE);
        parser.addArgument("sio", "SIO input file", cli::STORE, "sio", "SIO",
                           1, 1);
        parser.addArgument("output", "Output filename", cli::STORE, "output",
                           "OUTPUT", 1, 1);

        std::auto_ptr<cli::Results> options(parser.parse(argc, argv));

        std::string inputName(options->get<std::string> ("sio"));
        std::string outputName(options->get<std::string> ("output"));
        long maxRows(options->get<long> ("maxRows"));
        long maxSize(options->get<long> ("maxSize"));
        std::string classLevel(options->get<std::string> ("classLevel"));
        std::vector<std::string> schemaPaths;
        getSchemaPaths(*options, "--schema", "schema", schemaPaths);

        std::auto_ptr<logging::Logger> logger(
                logging::setupLogger(sys::Path::basename(argv[0])));

        // create an XML registry
        // The reason to do this is to avoid adding XMLControlCreators to the
        // XMLControlFactory singleton - this way has more fine-grained control
        //        XMLControlRegistry xmlRegistry;
        //        xmlRegistry.addCreator(DataType::COMPLEX, new XMLControlCreatorT<
        //                six::sicd::ComplexXMLControl> ());

        six::XMLControlFactory::getInstance().addCreator(
                six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        // Open a file with inputName
        io::FileInputStream inputFile(inputName);

        // Assume its an SIO, and wrap the stream
        sio::lite::FileReader sioReader(&inputFile);

        // Parse the SIO header
        const sio::lite::FileHeader* const fileHeader =
                sioReader.readHeader();

        // Make sure its a complex file
        if (fileHeader->getElementType()
                != sio::lite::FileHeader::COMPLEX_FLOAT)
            throw except::Exception(Ctxt("Expected a complex float SIO image!"));

        // And that its std::complex<float>-able
        if (fileHeader->getElementSize() != 8)
            throw except::Exception(
                                    Ctxt(
                                         "Expected a 4-real 4-imag complex image!"));

        // Create the Data
        // TODO: Use a ComplexDataBuilder?
        six::sicd::ComplexData* data(new six::sicd::ComplexData());
        std::auto_ptr<six::Data> scopedData(data);

        if (options->hasValue("version"))
        {
            data->setVersion(options->get<std::string>("version"));
        }

        data->setPixelType(six::PixelType::RE32F_IM32F);
        data->setNumRows(fileHeader->getNumLines());
        data->setNumCols(fileHeader->getNumElements());
        data->setName("corename");
        data->setSource("sensorname");
        data->collectionInformation->classification.level = classLevel;
        data->setCreationTime(six::DateTime());
        data->setImageCorners(makeUpCornersFromDMS());
        data->collectionInformation->radarMode = six::RadarModeType::SPOTLIGHT;
        data->scpcoa->sideOfTrack = six::SideOfTrackType::LEFT;
        data->geoData->scp.llh = six::LatLonAlt(42.2708, -83.7264);
        data->geoData->scp.ecf =
                scene::Utilities::latLonToECEF(data->geoData->scp.llh);
        data->grid->timeCOAPoly = six::Poly2D(0, 0);
        data->grid->timeCOAPoly[0][0] = 15605743.142846;
        data->position->arpPoly = six::PolyXYZ(0);
        data->position->arpPoly[0] = 0.0;

        data->radarCollection->txFrequencyMin = 0.0;
        data->radarCollection->txFrequencyMax = 0.0;
        data->radarCollection->txPolarization = six::PolarizationType::OTHER;
        mem::ScopedCloneablePtr<six::sicd::ChannelParameters>
                rcvChannel(new six::sicd::ChannelParameters());
        rcvChannel->txRcvPolarization = six::DualPolarizationType::OTHER;
        data->radarCollection->rcvChannels.push_back(rcvChannel);

        data->grid->row->sign = six::FFTSign::POS;
        data->grid->row->unitVector = 0.0;
        data->grid->row->sampleSpacing = 0;
        data->grid->row->impulseResponseWidth = 0;
        data->grid->row->impulseResponseBandwidth = 0;
        data->grid->row->kCenter = 0;
        data->grid->row->deltaK1 = 0;
        data->grid->row->deltaK2 = 0;
        data->grid->col->sign = six::FFTSign::POS;
        data->grid->col->unitVector = 0.0;
        data->grid->col->sampleSpacing = 0;
        data->grid->col->impulseResponseWidth = 0;
        data->grid->col->impulseResponseBandwidth = 0;
        data->grid->col->kCenter = 0;
        data->grid->col->deltaK1 = 0;
        data->grid->col->deltaK2 = 0;

        data->imageFormation->rcvChannelProcessed->numChannelsProcessed = 1;
        data->imageFormation->rcvChannelProcessed->channelIndex.push_back(0);

        data->pfa.reset(new six::sicd::PFA());
        data->pfa->spatialFrequencyScaleFactorPoly = six::Poly1D(0);
        data->pfa->spatialFrequencyScaleFactorPoly[0] = 42;
        data->pfa->polarAnglePoly = six::Poly1D(0);
        data->pfa->polarAnglePoly[0] = 42;

        data->timeline->collectDuration = 0;
        data->imageFormation->txRcvPolarizationProc =
                six::DualPolarizationType::OTHER;
        data->imageFormation->tStartProc = 0;
        data->imageFormation->tEndProc = 0;

        data->scpcoa->scpTime = 15605743.142846;
        data->scpcoa->slantRange = 0.0;
        data->scpcoa->groundRange = 0.0;
        data->scpcoa->dopplerConeAngle = 0.0;
        data->scpcoa->grazeAngle = 0.0;
        data->scpcoa->incidenceAngle = 0.0;
        data->scpcoa->twistAngle = 0.0;
        data->scpcoa->slopeAngle = 0.0;
        data->scpcoa->azimAngle = 0.0;
        data->scpcoa->layoverAngle = 0.0;
        data->scpcoa->arpPos = 0.0;
        data->scpcoa->arpVel = 0.0;
        data->scpcoa->arpAcc = 0.0;

        data->pfa->focusPlaneNormal = 0.0;
        data->pfa->imagePlaneNormal = 0.0;
        data->pfa->polarAngleRefTime = 0.0;
        data->pfa->krg1 = 0;
        data->pfa->krg2 = 0;
        data->pfa->kaz1 = 0;
        data->pfa->kaz2 = 0;

        data->imageFormation->txFrequencyProcMin = 0;
        data->imageFormation->txFrequencyProcMax = 0;

        mem::SharedPtr<six::Container> container(new six::Container(
                six::DataType::COMPLEX));
        container->addData(scopedData);

        six::Options writerOptions;
        /*
         *  Under normal circumstances, the library uses the
         *  segmentation algorithm in the SICD spec, and numRowsLimit
         *  is set to Contants::ILOC_SZ.  If the user sets this, they
         *  want us to create an alternate numRowsLimit to force the
         *  library to segment on smaller boundaries.
         *
         *  This is handy especially for debugging, since it will force
         *  the algorithm to segment early.
         *
         */
        if (maxRows > 0)
        {
            std::cout << "Overriding NITF max ILOC" << std::endl;
            writerOptions.setParameter(six::NITFHeaderCreator::OPT_MAX_ILOC_ROWS,
                                             maxRows);

        }
        if (maxSize > 0)
        {
            std::cout << "Overriding NITF product size" << std::endl;
            writerOptions.setParameter(six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE,
                                             maxSize);
        }

        // NITF data is always big endian, so check if we need to swap
        const bool needsByteSwap =
            (sys::isBigEndianSystem() && fileHeader->isDifferentByteOrdering())
         || (!sys::isBigEndianSystem() && !fileHeader->isDifferentByteOrdering());

        writerOptions.setParameter(
                six::WriteControl::OPT_BYTE_SWAP,
                six::Parameter((sys::Uint16_T) needsByteSwap));

        six::NITFWriteControl writer(writerOptions, container);
        writer.setLogger(logger.get());
        std::vector<io::InputStream*> sources;
        sources.push_back(&sioReader);

        writer.save(sources, outputName, schemaPaths);

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Caught except::Exception: " << ex.getMessage()
                  << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
        return 1;
    }
}
