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
   NITF_BOOL nitf_ImageIO_getMaskInfo(nitf_ImageIO *nitf,
                                      nitf_Uint32 *imageDataOffset, nitf_Uint32 *blockRecordLength,
                                      nitf_Uint32 *padRecordLength, nitf_Uint32 *padPixelValueLength,
                                      nitf_Uint8 **padValue, nitf_Uint64 **blockMask, nitf_Uint64 **padMask);
                     
}
const int BLOCK_LENGTH = 1024;
const int64_t ILOC_MAX = 99999;
std::string generateILOC(const types::RowCol<size_t> &offset)
{
   std::ostringstream oss;

   oss.fill('0');
   oss << std::setw(5) << offset.row << std::setw(5) << offset.col;

   return oss.str();
}

nitf::ImageSubheader setImageSubHeader(
    nitf::Record &record,
    size_t imageIndex,
    const types::RowCol<size_t> &fullDims,
    const types::RowCol<size_t> &segmentDims,
    const types::RowCol<size_t> &segmentOffset,
    const types::RowCol<size_t> &globalSegmentOffset,
    bool fileSeparate)
{
   nitf::ImageSegment   imageSegment = record.newImageSegment();
   nitf::ImageSubheader imgSubHdr = imageSegment.getSubheader();
   imgSubHdr.getFilePartType().set("IM");
   std::string iidTag = "TEST";
   std::string imgSrc = "TEST";
   std::string imgDateTime;
   std::string IID = "TEST";
   std::string tgtId = "TEST";
   const char* compressionType = "NM";

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
   size_t numBands;
   std::string pixelValueType;
   size_t numBitsPerPixel;
   size_t actualBitsPerPixel;
   std::string pixelJustification;
   std::string imageRepresentation;
   std::string imageCategory;

   numBands = 1;
   pixelValueType = "INT";
   numBitsPerPixel = 8;
   actualBitsPerPixel = 8;
   pixelJustification = "R";
   imageRepresentation = "MONO";
   imageCategory = "SOS";
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

size_t getNumberBlocksPresent(const nitf_Uint64 *mask,
                              size_t numRows,
                              size_t numCols)
{
   size_t numBlocksPresent = 0;
   size_t row;
   size_t col;
   size_t idx;
   for (row = 0, idx = 0; row < numRows; ++row)
   {
      for (col = 0; col < numCols; ++col, ++idx)
      {
         if (mask[idx] != 0xFFFFFFFF)
         {
            ++numBlocksPresent;
         }
      }
   }

   return numBlocksPresent;
}
void createSingleBandBuffer(std::vector<uint8_t>& buffer,                                     
                  nitf::ImageSegmentComputer &segmentComputer,
                  const types::RowCol<size_t> &fullDims,
                  const int segmentIdxToMakeEmpty)
{
   size_t bytes = fullDims.row * fullDims.col;
   size_t segmentSizeInBytes = 0;
   const std::vector<nitf::ImageSegmentComputer::Segment> &segments = segmentComputer.getSegments();
   buffer.resize(bytes);
   segmentSizeInBytes = segments[0].numRows * fullDims.col;

   memset(&buffer.front(), '0xff', bytes);
   memset(&buffer.front() + (segmentSizeInBytes * segmentIdxToMakeEmpty),
          '\0', segmentSizeInBytes);
}

void createBuffers(std::vector<std::vector<uint8_t> > &buffers,
                   nitf::ImageSegmentComputer &imageSegmentComputer,
                   const types::RowCol<size_t> &fullDims)
{
   int64_t nSegments = imageSegmentComputer.getSegments().size();
   /*
   * Create the memory images single band and then make the
   * segment blank for each one at different segments.
   */
   buffers.resize(nSegments);
   for (int64_t idx = 0; idx < nSegments; ++idx)
   {
      createSingleBandBuffer(buffers[idx],
                             imageSegmentComputer,
                             fullDims,
                             idx);
   }
}

TEST_CASE(testBlankSegmentsValid)
{
   // This is rather lengthy but will allocate 3 segments 
   // then write them to a temp NITF file.
   // will read those files and then verify that the Masks are proper size
   // there should be 0 blocks written for blank segments
   //
   //
   // Create 3 segments for testing
   nitf_Error error; /* Pointer to the error object */
   int64_t numberLines     = BLOCK_LENGTH * 3;
   int64_t numberElements  = BLOCK_LENGTH * 2;
   int64_t bytesPerSegment = BLOCK_LENGTH * BLOCK_LENGTH * 2;
   int64_t elementSize     = 1;
   int32_t numberOfTests   = 0;
   uint32_t idx            = 0;
   const types::RowCol<size_t> fullDims(numberLines, numberElements);
   nitf::ImageSubheader img;
   nitf::Writer writer;
   std::vector<std::vector<uint8_t> > buffers;
   nitf::ImageSegmentComputer imageSegmentComputer(numberLines,
                                                   numberElements,
                                                   elementSize,
                                                   ILOC_MAX,
                                                   bytesPerSegment,
                                                   BLOCK_LENGTH);

   const int64_t numSegments = imageSegmentComputer.getSegments().size();
   const std::vector<nitf::ImageSegmentComputer::Segment> &segments = imageSegmentComputer.getSegments();

   numberOfTests = numSegments;
   TEST_ASSERT_EQ(numSegments, 3);
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
      std::vector<types::RowCol<size_t> > segmentDims(numSegments);
      std::vector<types::RowCol<size_t> > segmentOffsets(numSegments);
      for (size_t ii = 0; ii < numSegments; ++ii)
      {
         types::RowCol<size_t> dims;
         types::RowCol<size_t> offset;
         int64_t numRows = segments[ii].numRows;
         dims = types::RowCol<size_t>(numRows, numberElements);
         offset = types::RowCol<size_t>(segments[ii].rowOffset, 0);
         segmentOffsets[ii] = types::RowCol<size_t>(segments[ii].firstRow, 0);

         img = setImageSubHeader(record, ii, fullDims, dims, offset, segmentOffsets[ii], false);
      }
      writer.prepare(output_io, record);

      for (size_t ii = 0; ii < numSegments; ++ii)
      {
         uint8_t *buf = &buffers[testIdx].front() + (ii * bytesPerSegment);
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
         nitf_Reader *reader=NULL;         /* Reader object */
         nitf_Record *record = NULL;       /* Record used for input and output */
         nitf_IOHandle in;            /* Input I/O handle */
         nitf_ListIterator imgIter;   /* Image segment list iterator */
         nitf_ListIterator imgEnd;    /* Image segment list iterator */
         nitf_ImageSegment *seg = NULL; /* Image segment object */
         nitf_ImageSubheader *subhdr = NULL; /* Image subheader object */
         nitf_ImageReader *iReader = NULL;   /* Image reader */
         nitf_BlockingInfo *blkInfo = NULL;  /* Blocking information */

         /*  Image information */

         char imageMode[NITF_IMODE_SZ + 1]; /* Image (blocking) mode */

         /*  Mask structure and mask components */

         nitf_Uint32 imageDataOffset;     /* Offset to actual image data past masks */
         nitf_Uint32 blockRecordLength;   /* Block mask record length */
         nitf_Uint32 padRecordLength;     /* Pad mask record length */
         nitf_Uint32 padPixelValueLength; /* Pad pixel value length in bytes */
         nitf_Uint8 *padValue;            /* Pad value */
         nitf_Uint64 *blockMask;          /* Block mask array */
         nitf_Uint64 *padMask;            /* Pad mask array */
         size_t imgCtr = 0;

         in = nitf_IOHandle_create(tempNitf.pathname().c_str(),
                                   NITF_ACCESS_READONLY, NITF_OPEN_EXISTING, &error);
         TEST_ASSERT_TRUE( (NITF_INVALID_HANDLE(in)!=true) );
         reader = nitf_Reader_construct(&error);
         record = nitf_Reader_read(reader, in, &error);

         /* Loop through the image segments */
         imgIter = nitf_List_begin(record->images);
         imgEnd = nitf_List_end(record->images);
         while (nitf_ListIterator_notEqualTo(&imgIter, &imgEnd))
         {
            /*  Get information from the image subheader */
            seg = (nitf_ImageSegment *)nitf_ListIterator_get(&imgIter);
            TEST_ASSERT_TRUE((seg!=NULL));
            subhdr = seg->subheader;

            nitf_Field_get(subhdr->imageMode,
                           imageMode, NITF_CONV_STRING, NITF_IMODE_SZ + 1, &error);
            imageMode[NITF_IMODE_SZ] = 0;

            /*  Get an image reader which creates the nitf_ImageIO were the masks are */

            iReader = nitf_Reader_newImageReader(reader, imgCtr, NULL, &error);
            TEST_ASSERT_TRUE( (iReader!=NULL) );

            blkInfo = nitf_ImageReader_getBlockingInfo(iReader, &error);
            TEST_ASSERT_TRUE( (blkInfo!=NULL) );
            TEST_ASSERT_TRUE(nitf_ImageIO_getMaskInfo(iReader->imageDeblocker,
                                       &imageDataOffset, &blockRecordLength,
                                       &padRecordLength, &padPixelValueLength,
                                       &padValue, &blockMask, &padMask) != 0);
            size_t nBlocksPresent = 0;
            size_t totalBlocks = blkInfo->numBlocksPerRow * blkInfo->numBlocksPerCol;
            TEST_ASSERT_GREATER(blockRecordLength, 0);
            
            nBlocksPresent = getNumberBlocksPresent(blockMask,
                                                      blkInfo->numBlocksPerRow,
                                                      blkInfo->numBlocksPerCol);

            if (imgCtr == testIdx)
            {
               TEST_ASSERT_EQ(nBlocksPresent, 0);
            }
            else
            {
               TEST_ASSERT_EQ(nBlocksPresent, totalBlocks);
            }

            nitf_ImageReader_destruct(&iReader);
            nrt_ListIterator_increment(&imgIter);
            ++imgCtr;
         }
         /*    Clean-up */
         nitf_Reader_destruct(&reader);
         nitf_IOHandle_close(in);
         nitf_Record_destruct(&record);
      }
   }
}

int main(int argc, char *argv[])
{
   TEST_CHECK(testBlankSegmentsValid);
}
