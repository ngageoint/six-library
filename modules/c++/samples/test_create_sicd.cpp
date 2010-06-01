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

#ifdef USE_SIO_LITE

#include <import/six.h>
#include <import/sio/lite.h>
#include <import/io.h>
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
 *  The segmentation loophole can be exploitated by overriding the product
 *  size (essentially bluffing the 10GB limit, and overriding ILOC_R=99999,
 *  although you may not extend those limits -- they are NITF format maxes.
 */
int main(int argc, char** argv)
{

    if (argc != 3 && argc != 5)
    {
        die_printf(
                   "Usage: %s <sio-image-file> <sicd-output-file> (Max product size) (N rows limit)\n",
                   argv[0]);
    }

    std::string inputName(argv[1]);
    std::string outputName(argv[2]);

    try
    {

        //--------------------------------------------------------------------
        // Registers a Creator that produces ComplexXMLControls on demand.
        // This function must be called prior to complex object/file creation.
        // If its called from multiple threads, must be synchronized
        // TODO: Replace with lazy plugin?
        //--------------------------------------------------------------------
        six::XMLControlFactory::getInstance(). addCreator(
                                                          six::DataClass::COMPLEX,
                                                          new six::XMLControlCreatorT<
                                                                  six::sicd::ComplexXMLControl>());

        // Open a file with inputName
        io::FileInputStream inputFile(inputName);

        // Assume its an SIO, and wrap the stream
        sio::lite::FileReader *sioReader =
                new sio::lite::FileReader(&inputFile);

        // Parse the SIO header
        sio::lite::FileHeader* fileHeader = sioReader->readHeader();

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
        six::sicd::ComplexData* data = new six::sicd::ComplexData();
        data->setPixelType(six::PixelType::RE32F_IM32F);
        data->setNumRows(fileHeader->getNumLines());
        data->setNumCols(fileHeader->getNumElements());
        data->setName("corename");
        data->setCreationTime(six::DateTime());
        data->setImageCorners(makeUpCornersFromDMS());
        data->collectionInformation->radarMode = six::RadarModeType::SPOTLIGHT;
        data->scpcoa->sideOfTrack = six::SideOfTrackType::LEFT;
        data->grid->timeCOAPoly = six::Poly2D(0, 0);
        data->grid->timeCOAPoly[0][0] = 15605743.142846;
        data->position->arpPoly = six::PolyXYZ(0);
        data->position->arpPoly[0] = 0.0;

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
        data->pfa->spatialFrequencyScaleFactorPoly = six::Poly1D(0);
        data->pfa->spatialFrequencyScaleFactorPoly[0] = 42;
        data->pfa->polarAnglePoly = six::Poly1D(0);
        data->pfa->polarAnglePoly[0] = 42;

        data->timeline->collectDuration = 0;
        data->imageFormation->tStartProc = 0;
        data->imageFormation->tEndProc = 0;

        data->scpcoa->scpTime = 15605743.142846;
        data->scpcoa->slantRange = 0;
        data->scpcoa->groundRange = 0;
        data->scpcoa->dopplerConeAngle = 0;
        data->scpcoa->grazeAngle = 0;
        data->scpcoa->incidenceAngle = 0;
        data->scpcoa->twistAngle = 0;
        data->scpcoa->slopeAngle = 0;
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

        six::Container* container = new six::Container(six::DataType::COMPLEX);
        container->addData(data);
        six::WriteControl* writer = new six::NITFWriteControl();

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
        if (argc == 5)
        {
            std::cout << "Overriding NITF product size and max ILOC"
                    << std::endl;
            writer->getOptions().setParameter(
                                              six::NITFWriteControl::OPT_MAX_PRODUCT_SIZE,
                                              str::toType<long>(argv[3]));

            writer->getOptions().setParameter(
                                              six::NITFWriteControl::OPT_MAX_ILOC_ROWS,
                                              str::toType<long>(argv[4]));

        }

        // Means its little endian stream
        bool needsByteSwap = sys::isBigEndianSystem()
                && fileHeader->isDifferentByteOrdering();

        writer->getOptions().setParameter(
                                          six::WriteControl::OPT_BYTE_SWAP,
                                          six::Parameter(
                                                         (sys::Uint16_T) needsByteSwap));

        writer->initialize(container);
        std::vector<io::InputStream*> sources;
        sources.push_back(sioReader);

        writer->save(sources, outputName);
        delete container;
        delete sioReader;
        delete writer;
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.getMessage() << std::endl;
    }
    return 0;
}
#else
int main(int argc, char** argv)
{
    std::cout << "sio.lite module not compiled in, test could not be run"
    << std::endl;
    return 0;
}
#endif
