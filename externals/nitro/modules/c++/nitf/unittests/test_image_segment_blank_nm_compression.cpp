#include <cstdlib>
#include <ctime>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <nitf/coda-oss.hpp>
#include <nitf/ImageSegmentComputer.h>
#include <nitf/ImageSubheader.hpp>
#include <nitf/Record.hpp>
#include <nitf/Writer.hpp>
#include <nitf/Reader.hpp>
#include <math/Round.h>
#include <io/FileInputStream.h>
#include <io/TempFile.h>

#include "TestCase.h"

static constexpr int64_t BLOCK_LENGTH = 256;
static constexpr int64_t ILOC_MAX = 99999;
static std::string generateILOC(const types::RowCol<int64_t>& offset)
{
   std::ostringstream oss;

   oss.fill('0');
   oss << std::setw(5) << offset.row << std::setw(5) << offset.col;

   return oss.str();
}

static nitf::ImageSubheader setImageSubHeader(
    nitf::Record&                     record,
    size_t                       imageIndex,
    const types::RowCol<int64_t>& segmentDims,
    const types::RowCol<int64_t>& segmentOffset)
{
   nitf::ImageSegment   imageSegment = record.newImageSegment();
   nitf::ImageSubheader imgSubHdr = imageSegment.getSubheader();
   const std::string iidTag = "TEST";
   const std::string imgSrc = "TEST";
   const std::string imgDateTime;
   const std::string IID = "TEST";
   const std::string tgtId = "TEST";
   const char* compressionType = "NM";

   imgSubHdr.getFilePartType().set("IM");
   imgSubHdr.getEncrypted().set(0);
   imgSubHdr.getNumRows().set((int64_t)segmentDims.row);
   imgSubHdr.getNumCols().set((int64_t)segmentDims.col);
   imgSubHdr.getNumImageComments().set(0);
   imgSubHdr.getImageCompression().set(compressionType);
   imgSubHdr.getCompressionRate().set("    ");
   imgSubHdr.insertImageComment("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0);
   std::vector<nitf::BandInfo> bands;
   nitf::BandInfo bandI;
   bandI.getRepresentation().set("M ");
   bandI.getSubcategory().set("      ");
   bandI.getImageFilterCondition().set("N");
   bandI.getImageFilterCode().set("   ");
   bandI.getNumLUTs().set(0);
   bands.push_back(bandI);
   const auto pixelValueType = nitf::PixelValueType::Integer;
   const int64_t numBitsPerPixel = 8;
   const int64_t actualBitsPerPixel = 8;
   const std::string pixelJustification = "R";
   const auto imageRepresentation = nitf::ImageRepresentation::MONO;
   const std::string imageCategory = "SOS";

   imgSubHdr.setPixelInformation(
      pixelValueType,
      numBitsPerPixel,
      actualBitsPerPixel,
      pixelJustification,
      imageRepresentation,
      imageCategory,
      bands);
   imgSubHdr.getImageSyncCode().set("0");
   imgSubHdr.getImageMode().set("B");
   int64_t blocksPerRow = (segmentDims.col + (BLOCK_LENGTH - 1)) / BLOCK_LENGTH;
   int64_t blocksPerCol = (segmentDims.row + (BLOCK_LENGTH - 1)) / BLOCK_LENGTH;

   imgSubHdr.getNumBlocksPerRow().set(blocksPerRow);
   imgSubHdr.getNumPixelsPerHorizBlock().set(BLOCK_LENGTH);
   imgSubHdr.getNumBlocksPerCol().set(blocksPerCol);
   imgSubHdr.getNumPixelsPerVertBlock().set(BLOCK_LENGTH);

   imgSubHdr.getImageDisplayLevel().set((int64_t)imageIndex+1);
   imgSubHdr.getImageAttachmentLevel().set((int64_t)imageIndex);

   imgSubHdr.getImageLocation().set(generateILOC(segmentOffset));

   return imgSubHdr;
}

int64_t getNumberBlocksPresent(const uint64_t* mask,
                                   const int64_t  numRows,
                                   const int64_t  numCols)
{
   int64_t numBlocksPresent = 0;
   for (int64_t row = 0, idx = 0; row < numRows; ++row)
   {
      for (int64_t col = 0; col < numCols; ++col, ++idx)
      {
         if (mask[idx] != 0xFFFFFFFF)
         {
            ++numBlocksPresent;
         }
      }
   }

   return numBlocksPresent;
}

void createSingleBandBuffer(std::vector<std::byte>& buffer,
                            const nitf::ImageSegmentComputer& segmentComputer,
                            const types::RowCol<int64_t>& fullDims,
                            const size_t segmentIdxToMakeEmpty)
{
   const auto bytes = static_cast<size_t>(fullDims.area());
   const std::vector<nitf::ImageSegmentComputer::Segment> &segments = segmentComputer.getSegments();
   /* All segments should be the same size in this test so this is safe */
   const auto segmentSizeInBytes = segments[segmentIdxToMakeEmpty].numRows * fullDims.col;

   buffer.resize(bytes);
   memset(&buffer.front(), '\0', bytes);
   for (uint32_t segIdx = 0; segIdx < segments.size(); ++segIdx)
   {
      auto segStart = &buffer.front() + (segmentSizeInBytes * segIdx);
      /* Set only the center that way we are surrounded by empty blocks */
      if (segIdx != segmentIdxToMakeEmpty)
      {
         segStart[segmentSizeInBytes/2] = static_cast<std::byte>(0xff);
      }
   }
}

void createBuffers(std::vector<std::vector<std::byte> >& buffers,
                   const nitf::ImageSegmentComputer& imageSegmentComputer,
                   const types::RowCol<int64_t>& fullDims)
{
   const auto nSegments = imageSegmentComputer.getSegments().size();
   /*
   * Create the memory images single band and then make the
   * segment blank for each one at different segments.
   */
   buffers.resize(nSegments);
   for (size_t idx = 0; idx < nSegments; ++idx)
   {
      createSingleBandBuffer(buffers[idx],
                             imageSegmentComputer,
                             fullDims,
                             idx);
   }
}

TEST_CASE(testBlankSegmentsValid)
{
   /*
    * This is rather lengthy but will allocate 3 segments 
    * then write them to a temp NITF file.
    * will read those files and then verify that the Masks are proper size
    * there should be 0 blocks written for blank segments
    *
    *
    * Create 3 segments for testing
    */
   const int64_t BLOCK_LENGTH_SCALED = BLOCK_LENGTH*4;
   const int64_t numberLines = BLOCK_LENGTH_SCALED * 3;
   const int64_t numberElements = BLOCK_LENGTH_SCALED * 2;
   const int64_t bytesPerSegment = BLOCK_LENGTH_SCALED * BLOCK_LENGTH_SCALED * 2;
   const int64_t elementSize     = 1;
   const types::RowCol<int64_t> fullDims(numberLines, numberElements);
   std::vector<std::vector<std::byte> > buffers;
   nitf::ImageSegmentComputer imageSegmentComputer(numberLines,
                                                   numberElements,
                                                   elementSize,
                                                   ILOC_MAX,
                                                   bytesPerSegment,
                                                   BLOCK_LENGTH);

   const auto numSegments = imageSegmentComputer.getSegments().size();
   const std::vector<nitf::ImageSegmentComputer::Segment> &segments = imageSegmentComputer.getSegments();

   auto numberOfTests = numSegments;
   TEST_ASSERT_EQ(numSegments, static_cast<size_t>(3));
   for (size_t testSegmentIdx = 0; testSegmentIdx < numSegments; ++testSegmentIdx)
   {
      const types::RowCol<size_t> dims(segments[testSegmentIdx].numRows, numberElements);
      TEST_ASSERT_EQ(dims.area(), static_cast<size_t>(bytesPerSegment));
   }
   createBuffers(buffers, imageSegmentComputer, fullDims);
   for (size_t testIdx = 0; testIdx < numberOfTests; ++testIdx)
   {
      nitf::Record record(NITF_VER_21);
      io::TempFile tempNitf;
      nitf::ImageSubheader img;
      nitf::Writer writer;
      nitf::IOHandle output_io(tempNitf.pathname(),
                               NITF_ACCESS_WRITEONLY,
                               NITF_CREATE);
      std::vector<types::RowCol<int64_t> > segmentDims(numSegments);
      std::vector<types::RowCol<int64_t> > segmentOffsets(numSegments);
      for (size_t ii = 0; ii < numSegments; ++ii)
      {
         const auto numRows = segments[ii].numRows;
         const types::RowCol<size_t> dims(numRows, numberElements);
         const types::RowCol<size_t> offset(segments[ii].rowOffset, 0);
         segmentOffsets[ii] = types::RowCol<size_t>(segments[ii].firstRow, 0);

         img = setImageSubHeader(record, ii, dims, offset);
      }
      writer.prepare(output_io, record);

      for (int ii = 0; ii < static_cast<int>(numSegments); ++ii)
      {
         auto buf = &buffers[testIdx].front() + (ii * bytesPerSegment);
         nitf::ImageWriter imageWriter = writer.newImageWriter(ii);
         imageWriter.setWriteCaching(1);
         nitf::ImageSource iSource;
         nitf::MemorySource memSource(buf,
                                      bytesPerSegment,
                                      0, 1, 0);
         iSource.addBand(memSource);
         imageWriter.attachSource(iSource);
      }
      writer.write();
      output_io.close();

      {
         uint32_t imageDataOffset=0;        /* Offset to actual image data past masks */
         uint32_t blockRecordLength=0;      /* Block mask record length */
         uint32_t padRecordLength=0;        /* Pad mask record length */
         uint32_t padPixelValueLength=0;    /* Pad pixel value length in bytes */
         uint8_t  *padValue = nullptr;         /* Pad value */
         uint64_t *blockMask= nullptr;          /* Block mask array */
         uint64_t *padMask= nullptr;            /* Pad mask array */
         int imgCtr = 0;
         nitf::IOHandle input_io(tempNitf.pathname(),
                                 NITF_ACCESS_READONLY,
                                 NITF_OPEN_EXISTING);
         nitf::Reader reader;
         record = reader.read(input_io);
         nitf::List images   = record.getImages();
         for (nitf::ListIterator imageIterator = images.begin();
              imageIterator != images.end();
              ++imageIterator)
         {
            nitf::ImageSegment seg(static_cast<nitf_ImageSegment *>((*imageIterator)));
            nitf::ImageSubheader subhdr     = seg.getSubheader();
            nitf::ImageReader imageReader   = reader.newImageReader(imgCtr);
            nitf::BlockingInfo blockingInfo = imageReader.getBlockingInfo();
            TEST_ASSERT_TRUE(imageReader.getMaskInfo(
                                                      imageDataOffset, blockRecordLength,
                                                      padRecordLength, padPixelValueLength,
                                                      padValue, blockMask, padMask) != 0);
            TEST_ASSERT_GREATER(blockRecordLength, 0u);

            const int64_t totalBlocks = blockingInfo.getNumBlocksPerRow()*blockingInfo.getNumBlocksPerCol();
            TEST_ASSERT_GREATER(totalBlocks, 0);

            const int64_t nBlocksPresent = getNumberBlocksPresent(blockMask,
                                                                      blockingInfo.getNumBlocksPerRow(),
                                                                      blockingInfo.getNumBlocksPerCol());

            if (imgCtr == static_cast<int>(testIdx))
            {
               TEST_ASSERT_EQ(nBlocksPresent, static_cast<int64_t>(0));
            }
            else
            {
               TEST_ASSERT_GREATER(nBlocksPresent, 0);
            }
            ++imgCtr;
         }
      }
   }
}

TEST_MAIN(
TEST_CHECK(testBlankSegmentsValid);
)
