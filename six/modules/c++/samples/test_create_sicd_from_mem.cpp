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
#include <import/io.h>
#include <logging/Setup.h>
#include <scene/Utilities.h>
#include "utils.h"

// For SICD implementation
#include <import/six/sicd.h>
#include <six/sicd/SICDWriteControl.h>

namespace
{
std::auto_ptr<six::Data>
createData(const types::RowCol<size_t>& dims)
{
    six::sicd::ComplexData* data(new six::sicd::ComplexData());
    std::auto_ptr<six::Data> scopedData(data);
    data->setPixelType(six::PixelType::RE32F_IM32F);
    data->setNumRows(dims.row);
    data->setNumCols(dims.col);
    data->setName("corename");
    data->setSource("sensorname");
    data->collectionInformation->classification.level = "UNCLASSIFIED";
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

    data->timeline->collectStart = six::DateTime();
    data->timeline->collectDuration = 1.0;
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

    return scopedData;
}

class Compare
{
public:
    Compare(const six::Data& lhsData,
            const std::vector<std::complex<float> >& lhsImage,
            const std::vector<std::string>& schemaPaths) :
        mLhsData(*reinterpret_cast<const six::sicd::ComplexData*>(lhsData.clone())),
        mLhsImage(lhsImage),
        mSchemaPaths(schemaPaths)
    {
    }

    bool operator()(const std::string& pathname) const
    {
        std::auto_ptr<six::sicd::ComplexData> rhsData;
        std::vector<std::complex<float> > rhsImage;
        six::sicd::Utilities::readSicd(pathname,
                                       mSchemaPaths,
                                       rhsData,
                                       rhsImage);

        if (mLhsImage != rhsImage)
        {
            for (size_t ii = 0; ii < mLhsImage.size(); ++ii)
            {
                if (mLhsImage[ii] != rhsImage[ii])
                {
                    std::cout << "Stops matching at " << ii << std::endl;
                    break;
                }
            }
        }

        return (mLhsData == *rhsData && mLhsImage == rhsImage);
    }

private:
    const six::sicd::ComplexData mLhsData;
    const std::vector<std::complex<float> > mLhsImage;
    const std::vector<std::string> mSchemaPaths;
};

template <typename T>
void subsetData(const T* orig,
                size_t origNumCols,
                const types::RowCol<size_t>& offset,
                const types::RowCol<size_t>& dims,
                std::vector<T>& output)
{
    output.resize(dims.area());
    const T* origPtr = orig + offset.row * origNumCols + offset.col;
    T* outputPtr = &output[0];
    for (size_t row = 0;
         row < dims.row;
         ++row, origPtr += origNumCols, outputPtr += dims.col)
    {
        ::memcpy(outputPtr, origPtr, dims.col * sizeof(T));
    }
}

void readFile(const std::string& pathname,
              std::vector<sys::byte>& contents)
{
    io::FileInputStream inStream(pathname);

    contents.resize(inStream.available());

    const size_t numRead = inStream.read(&contents[0], contents.size());
    if (numRead != contents.size())
    {
        throw except::Exception(Ctxt("Short read"));
    }
}

// Note that this will work because SIX is forcing the NITF date/time to match
// what's in the SICD XML and we're writing the same SICD XML in all our files
class CompareFiles
{
public:
    CompareFiles(const std::string& lhsPathname)
    {
        readFile(lhsPathname, mLHS);
    }

    bool operator()(const std::string& prefix,
                    const std::string& rhsPathname) const
    {
        readFile(rhsPathname, mRHS);

        if (mLHS == mRHS)
        {
            std::cout << prefix << " matches" << std::endl;
            return true;
        }
        else if (mLHS.size() != mRHS.size())
        {
            std::cerr << prefix << " DOES NOT MATCH: file sizes are "
                      << mLHS.size() << " vs. " << mRHS.size() << " bytes"
                      << std::endl;
        }
        else
        {
            size_t ii;
            for (ii = 0; ii < mLHS.size(); ++ii)
            {
                if (mLHS[ii] != mRHS[ii])
                {
                    break;
                }
            }

            std::cerr << prefix << " DOES NOT MATCH at byte " << ii
                      << std::endl;
        }

        return false;
    }

private:
    std::vector<sys::byte> mLHS;
    mutable std::vector<sys::byte> mRHS;
};

class EnsureFileCleanup
{
public:
    EnsureFileCleanup(const std::string& pathname) :
        mPathname(pathname)
    {
        removeIfExists();
    }

    ~EnsureFileCleanup()
    {
        try
        {
            removeIfExists();
        }
        catch (...)
        {
        }
    }

private:
    void removeIfExists()
    {
        sys::OS os;
        if (os.exists(mPathname))
        {
            os.remove(mPathname);
        }
    }

private:
    const std::string mPathname;
};

class Tester
{
public:
    Tester(const std::vector<std::string>& schemaPaths) :
        mNormalPathname("normal_write.nitf"),
        mNormalFileCleanup(mNormalPathname),
        mContainer(six::DataType::COMPLEX),
        mDims(123, 456),
        mImage(mDims.area()),
        mImagePtr(&mImage[0]),
        mTestPathname("streaming_write.nitf"),
        mSchemaPaths(schemaPaths),
        mRetCode(0)
    {
        for (size_t ii = 0; ii < mImage.size(); ++ii)
        {
            mImage[ii] = std::complex<float>(static_cast<float>(ii),
                                             static_cast<float>(ii * 10));
        }

        normalWrite();
    }

    int getRetCode() const
    {
        return mRetCode;
    }

    // Write the file out with a SICDWriteControl in one shot
    void testSingleWrite();

    // Writes with num cols == global num cols
    void testMultipleWritesOfFullRows();

    // Writes where some rows are written out with only some of the cols
    void testMultipleWritesOfPartialRows();

private:
    void normalWrite();

private:
    void compare(const std::string& prefix)
    {
        if (!(*mCompareFiles)(prefix, mTestPathname))
        {
            mRetCode = 1;
        }
    }

private:
    const std::string mNormalPathname;
    const EnsureFileCleanup mNormalFileCleanup;

    six::Container mContainer;
    const types::RowCol<size_t> mDims;
    std::vector<std::complex<float> > mImage;
    std::complex<float>* const mImagePtr;

    std::auto_ptr<const CompareFiles> mCompareFiles;
    const std::string mTestPathname;
    const std::vector<std::string> mSchemaPaths;

    int mRetCode;
};

void Tester::normalWrite()
{
    mContainer.addData(createData(mDims));

    six::NITFWriteControl writer;

    writer.initialize(&mContainer);

    six::BufferList buffers;
    buffers.push_back(reinterpret_cast<six::UByte*>(mImagePtr));
    writer.save(buffers, mNormalPathname, mSchemaPaths);

    mCompareFiles.reset(new CompareFiles(mNormalPathname));
}

void Tester::testSingleWrite()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::sicd::SICDWriteControl sicdWriter(mTestPathname, mSchemaPaths);

    sicdWriter.initialize(&mContainer);
    sicdWriter.save(mImagePtr, types::RowCol<size_t>(0, 0), mDims);
    sicdWriter.close();

    compare("Single write");
}

void Tester::testMultipleWritesOfFullRows()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::sicd::SICDWriteControl sicdWriter(mTestPathname,
                                           mSchemaPaths);
    sicdWriter.initialize(&mContainer);

    // Rows [40, 60)
    types::RowCol<size_t> offset(40, 0);
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(20, mDims.col));

    // Rows [5, 25)
    offset.row = 5;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(20, mDims.col));

    // Rows [0, 5)
    offset.row = 0;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(5, mDims.col));

    // Rows [100, 123)
    offset.row = 100;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(23, mDims.col));

    // Rows [25, 40)
    offset.row = 25;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(15, mDims.col));

    // Rows [60, 100)
    offset.row = 60;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(40, mDims.col));

    sicdWriter.close();

    compare("Multiple writes of full rows");
}

void Tester::testMultipleWritesOfPartialRows()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::sicd::SICDWriteControl sicdWriter(mTestPathname,
                                           mSchemaPaths);
    sicdWriter.initialize(&mContainer);

    // Rows [40, 60)
    // Cols [400, 456)
    types::RowCol<size_t> offset(40, 400);
    std::vector<std::complex<float> > subset;
    types::RowCol<size_t> subsetDims(20, 56);
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    // Rows [60, 123)
    offset.row = 60;
    offset.col = 0;
    subsetDims.row = 63;
    subsetDims.col = mDims.col;
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    // Rows [40, 60)
    // Cols [150, 400)
    offset.row = 40;
    offset.col = 150;
    subsetDims.row = 20;
    subsetDims.col = 250;
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    // Rows [0, 40)
    offset.row = 0;
    offset.col = 0;
    subsetDims.row = 40;
    subsetDims.col = mDims.col;
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    // Rows [40, 60)
    // Cols [0, 150)
    offset.row = 40;
    offset.col = 0;
    subsetDims.row = 20;
    subsetDims.col = 150;
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    sicdWriter.close();

    compare("Multiple writes of partial rows");
}
}

int main(int argc, char** argv)
{
    try
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription(
                              "This program creates a sample SICD NITF file of all zeros.");
        parser.addArgument("-r --rows", "Rows limit", cli::STORE, "maxRows",
                           "ROWS")->setDefault(-1);
        parser.addArgument("-s --size", "Max product size", cli::STORE,
                           "maxSize", "BYTES")->setDefault(-1);
        parser.addArgument("--schema", 
                           "Specify a schema or directory of schemas",
                           cli::STORE);

        std::auto_ptr<cli::Results> options(parser.parse(argc, argv));

        size_t maxRows(options->get<size_t>("maxRows"));
        size_t maxSize(options->get<size_t>("maxSize"));
        std::vector<std::string> schemaPaths;
        getSchemaPaths(*options, "--schema", "schema", schemaPaths);

        // create an XML registry
        six::XMLControlFactory::getInstance().addCreator(
                six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

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
        std::cout << "TODO: Fix overrides\n";
        /*
        if (maxRows > 0)
        {
            std::cout << "Overriding NITF max ILOC" << std::endl;
            writer.getOptions().setParameter(six::NITFWriteControl::OPT_MAX_ILOC_ROWS,
                                             maxRows);

        }
        if (maxSize > 0)
        {
            std::cout << "Overriding NITF product size" << std::endl;
            writer.getOptions().setParameter(six::NITFWriteControl::OPT_MAX_PRODUCT_SIZE,
                                             maxSize);
        }
        */

        Tester tester(schemaPaths);
        tester.testSingleWrite();
        tester.testMultipleWritesOfFullRows();
        tester.testMultipleWritesOfPartialRows();


        // TODO: Test 16-bit writes
        //       Test multi-seg


        return tester.getRetCode();
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
