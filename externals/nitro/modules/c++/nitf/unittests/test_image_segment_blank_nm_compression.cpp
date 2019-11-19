#include <cstdlib>
#include <ctime>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <import/nitf.h>
#include <import/types.h>
#include <nitf/ImageSegmentComputer.h>
#include <nitf/ImageSubheader.hpp>
#include <nitf/Record.hpp>
#include <nitf/Writer.hpp>
#include <nitf/Reader.hpp>
#include <math/Round.h>
#include <io/FileInputStream.h>
#include <io/TempFile.h>
#include "TestCase.h"

extern "C"{
   NITF_BOOL nitf_ImageIO_getMaskInfo(nitf_ImageIO* nitf,
                                      nitf_Uint32* imageDataOffset, nitf_Uint32* blockRecordLength,
                                      nitf_Uint32* padRecordLength, nitf_Uint32* padPixelValueLength,
                                      nitf_Uint8** padValue, nitf_Uint64** blockMask, nitf_Uint64** padMask);
                     
}
const nitf::Int64 BLOCK_LENGTH = 256;
const nitf::Int64 ILOC_MAX = 99999;
std::string generateILOC(const types::RowCol<nitf::Int64>& offset)
{
   std::ostringstream oss;

   oss.fill('0');
   oss << std::setw(5) << offset.row << std::setw(5) << offset.col;

   return oss.str();
}

nitf::ImageSubheader setImageSubHeader(
    nitf::Record&                     record,
    nitf::Int64                       imageIndex,
    const types::RowCol<nitf::Int64>& fullDims,
    const types::RowCol<nitf::Int64>& segmentDims,
    const types::RowCol<nitf::Int64>& segmentOffset,
    const types::RowCol<nitf::Int64>& globalSegmentOffset,
    bool fileSeparate)
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
   imgSubHdr.getNumRows().set((nitf::Int64)segmentDims.row);
   imgSubHdr.getNumCols().set((nitf::Int64)segmentDims.col);
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
   const std::string pixelValueType = "INT";
   const nitf::Int64 numBitsPerPixel = 8;
   const nitf::Int64 actualBitsPerPixel = 8;
   const std::string pixelJustification = "R";
   const std::string imageRepresentation = "MONO";
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
   nitf::Int64 blocksPerRow = (segmentDims.col + (BLOCK_LENGTH - 1)) / BLOCK_LENGTH;
   nitf::Int64 blocksPerCol = (segmentDims.row + (BLOCK_LENGTH - 1)) / BLOCK_LENGTH;

   imgSubHdr.getNumBlocksPerRow().set(blocksPerRow);
   imgSubHdr.getNumPixelsPerHorizBlock().set(BLOCK_LENGTH);
   imgSubHdr.getNumBlocksPerCol().set(blocksPerCol);
   imgSubHdr.getNumPixelsPerVertBlock().set(BLOCK_LENGTH);

   imgSubHdr.getImageDisplayLevel().set((nitf::Int64)imageIndex+1);
   imgSubHdr.getImageAttachmentLevel().set((nitf::Int64)imageIndex);

   imgSubHdr.getImageLocation().set(generateILOC(segmentOffset));

   return imgSubHdr;
}

nitf::Int64 getNumberBlocksPresent(const nitf_Uint64* mask,
                                   const nitf::Int64  numRows,
                                   const nitf::Int64  numCols)
{
   nitf::Int64 numBlocksPresent = 0;
   for (nitf::Int64 row = 0, idx = 0; row < numRows; ++row)
   {
      for (nitf::Int64 col = 0; col < numCols; ++col, ++idx)
      {
         if (mask[idx] != 0xFFFFFFFF)
         {
            ++numBlocksPresent;
         }
      }
   }

   return numBlocksPresent;
}

void createSingleBandBuffer(std::vector<nitf::Uint8>& buffer,
                            const nitf::ImageSegmentComputer& segmentComputer,
                            const types::RowCol<nitf::Int64>& fullDims,
                            const nitf::Int64 segmentIdxToMakeEmpty)
{
   const nitf::Int64 bytes = fullDims.area();
   const std::vector<nitf::ImageSegmentComputer::Segment> &segments = segmentComputer.getSegments();
   /* All segments should be the same size in this test so this is safe */
   const nitf::Int64 segmentSizeInBytes = segments[segmentIdxToMakeEmpty].numRows * fullDims.col;

   buffer.resize(bytes);
   memset(&buffer.front(), '\0', bytes);
   for (nitf::Uint32 segIdx = 0; segIdx < segments.size(); ++segIdx)
   {
      nitf::Uint8 *segStart = &buffer.front() + (segmentSizeInBytes * segIdx);
      /* Set only the center that way we are surrounded by empty blocks */
      if (segIdx != segmentIdxToMakeEmpty)
      {
         segStart[segmentSizeInBytes/2] = 0xff;
      }
   }
}

void createBuffers(std::vector<std::vector<nitf::Uint8> >& buffers,
                   const nitf::ImageSegmentComputer& imageSegmentComputer,
                   const types::RowCol<nitf::Int64>& fullDims)
{
   const nitf::Int64 nSegments = imageSegmentComputer.getSegments().size();
   /*
   * Create the memory images single band and then make the
   * segment blank for each one at different segments.
   */
   buffers.resize(nSegments);
   for (nitf::Int64 idx = 0; idx < nSegments; ++idx)
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
   const nitf::Int64 BLOCK_LENGTH_SCALED = BLOCK_LENGTH*4;
   const nitf::Int64 numberLines = BLOCK_LENGTH_SCALED * 3;
   const nitf::Int64 numberElements = BLOCK_LENGTH_SCALED * 2;
   const nitf::Int64 bytesPerSegment = BLOCK_LENGTH_SCALED * BLOCK_LENGTH_SCALED * 2;
   const nitf::Int64 elementSize     = 1;
   nitf::Int64 numberOfTests         = 0;
   const types::RowCol<nitf::Int64> fullDims(numberLines, numberElements);
   nitf::ImageSubheader img;
   nitf::Writer writer;
   std::vector<std::vector<nitf::Uint8> > buffers;
   nitf::ImageSegmentComputer imageSegmentComputer(numberLines,
                                                   numberElements,
                                                   elementSize,
                                                   ILOC_MAX,
                                                   bytesPerSegment,
                                                   BLOCK_LENGTH);

   const nitf::Int64 numSegments = imageSegmentComputer.getSegments().size();
   const std::vector<nitf::ImageSegmentComputer::Segment> &segments = imageSegmentComputer.getSegments();

   numberOfTests = numSegments;
   TEST_ASSERT_EQ(numSegments, 3);
   for (nitf::Int64 testSegmentIdx = 0; testSegmentIdx < numSegments; ++testSegmentIdx)
   {
      types::RowCol<nitf::Int64> dims(segments[testSegmentIdx].numRows, numberElements);
      TEST_ASSERT_EQ(dims.area(), bytesPerSegment);
   }
   createBuffers(buffers, imageSegmentComputer, fullDims);
   for (nitf::Int64 testIdx = 0; testIdx < numberOfTests; ++testIdx)
   {
      nitf::Record record(NITF_VER_21);
      io::TempFile tempNitf;
      nitf::ImageSubheader img;
      nitf::Writer writer;
      nitf::IOHandle output_io(tempNitf.pathname(),
                               NITF_ACCESS_WRITEONLY,
                               NITF_CREATE);
      std::vector<types::RowCol<nitf::Int64> > segmentDims(numSegments);
      std::vector<types::RowCol<nitf::Int64> > segmentOffsets(numSegments);
      for (nitf::Int64 ii = 0; ii < numSegments; ++ii)
      {
         types::RowCol<nitf::Int64> dims;
         types::RowCol<nitf::Int64> offset;
         nitf::Int64 numRows = segments[ii].numRows;
         dims = types::RowCol<nitf::Int64>(numRows, numberElements);
         offset = types::RowCol<nitf::Int64>(segments[ii].rowOffset, 0);
         segmentOffsets[ii] = types::RowCol<nitf::Int64>(segments[ii].firstRow, 0);

         img = setImageSubHeader(record, ii, fullDims, dims, offset, segmentOffsets[ii], false);
      }
      writer.prepare(output_io, record);

      for (nitf::Int64 ii = 0; ii < numSegments; ++ii)
      {
         nitf::Uint8 *buf = &buffers[testIdx].front() + (ii * bytesPerSegment);
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
         nitf_Uint32 imageDataOffset=0;        /* Offset to actual image data past masks */
         nitf_Uint32 blockRecordLength=0;      /* Block mask record length */
         nitf_Uint32 padRecordLength=0;        /* Pad mask record length */
         nitf_Uint32 padPixelValueLength=0;    /* Pad pixel value length in bytes */
         nitf_Uint8  *padValue = NULL;         /* Pad value */
         nitf_Uint64 *blockMask=NULL;          /* Block mask array */
         nitf_Uint64 *padMask=NULL;            /* Pad mask array */
         nitf::Int64 imgCtr = 0;
         nitf::IOHandle input_io(tempNitf.pathname(),
                                 NITF_ACCESS_READONLY,
                                 NITF_OPEN_EXISTING);
         nitf::Reader reader;
         nitf::Record record = reader.read(input_io);
         nitf::List images   = record.getImages();
         for (nitf::ListIterator imageIterator = images.begin();
              imageIterator != images.end();
              ++imageIterator)
         {
            nitf::ImageSegment seg(static_cast<nitf_ImageSegment *>((*imageIterator)));
            nitf::ImageSubheader subhdr     = seg.getSubheader();
            nitf::ImageReader imageReader   = reader.newImageReader(imgCtr);
            nitf::BlockingInfo blockingInfo = imageReader.getBlockingInfo();
            nitf_ImageReader *iReader       = static_cast<nitf_ImageReader *>(imageReader.getNativeOrThrow());
            TEST_ASSERT_TRUE(nitf_ImageIO_getMaskInfo(iReader->imageDeblocker,
                                                      &imageDataOffset, &blockRecordLength,
                                                      &padRecordLength, &padPixelValueLength,
                                                      &padValue, &blockMask, &padMask) != 0);

            const nitf::Int64 totalBlocks = blockingInfo.getNumBlocksPerRow()*blockingInfo.getNumBlocksPerCol();
            TEST_ASSERT_GREATER(blockRecordLength, 0);

            const nitf::Int64 nBlocksPresent = getNumberBlocksPresent(blockMask,
                                                                      blockingInfo.getNumBlocksPerRow(),
                                                                      blockingInfo.getNumBlocksPerCol());

            if (imgCtr == testIdx)
            {
               TEST_ASSERT_EQ(nBlocksPresent, 0);
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

int main(int argc, char* argv[])
{
   TEST_CHECK(testBlankSegmentsValid);

   return 0;
}
