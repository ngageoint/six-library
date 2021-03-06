/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

import java.io.File;
import java.io.IOException;

import nitf.FileSource;
import nitf.IOHandle;
import nitf.ImageReader;
import nitf.ImageSegment;
import nitf.ImageSource;
import nitf.ImageWriter;
import nitf.NITFException;
import nitf.Reader;
import nitf.Record;
import nitf.SubWindow;
import nitf.Writer;

/**
 * This test is a Java version of the C test_writer_3 test case. <p/> This test
 * reads in the given NITF file, then loops over the image segments, and writes
 * out each band to a separate file. <p/> Finally, it creates a BandSource for
 * each of the band files, and writes out a final NITF. None of the fields are
 * updated in the output NITF file, so if all works well, a diff of the input
 * NITF file and output NITF file should produce no results. <p/> *NOTE -
 * Currently, only writing headers and image segments works properly. If you try
 * testing with an image that has other segments, including TREs, you will
 * probably produce a file that diffs with the original. Future releases will
 * have a more complete writing capability.
 */
public class TestWriter3
{
    public static void main(String[] args) throws NITFException, IOException
    {

        if (args.length != 2)
        {
            System.out.println("Usage: java " + TestWriter3.class.getName()
                    + " <input-file> <output-file>");
            System.exit(1);
        }

        Record record = doRead(args[0]);
        record.getHeader().print(System.out);
        doWrite(record, args[0], args[1]);
    }

    /**
     * This constructs a name for the raw image data for each band
     * 
     * @param rootFile
     * @param imageNum
     * @param bandNum
     * @return
     */
    private static String makeBandName(String rootFile, int imageNum,
            int bandNum)
    {
        File file = new File(rootFile);
        String name = file.getName();
        String fileName = name + "__" + imageNum + "_band_" + bandNum;
        fileName = fileName.replaceAll("[.]", "_");
        fileName += ".man";
        return new File(file.getParentFile(), fileName).getPath();
    }

    /**
     * Reads the NITF file, and calls manuallyWriteImageBands for each image
     * 
     * @param inFile
     * @return
     * @throws nitf.NITFException
     */
    private static Record doRead(String inFile) throws nitf.NITFException
    {
        int numImages;

        Reader reader = new Reader();

        IOHandle io = new IOHandle(inFile, IOHandle.NITF_ACCESS_READONLY,
                IOHandle.NITF_OPEN_EXISTING);

        Record record = reader.read(io);
        numImages = record.getHeader().getNumImages().getIntData();

        System.out.println("record.getHeader() = " + record.getHeader());

        ImageSegment[] images = record.getImages();
        for (int i = 0; i < images.length; i++)
        {
            ImageSegment imageSegment = images[i];
            ImageReader deserializer = reader.getNewImageReader(i);
            System.out.println("Writing image " + i);
            /* Write the thing out */
            manuallyWriteImageBands(imageSegment, inFile, deserializer, i);
            System.out.println("Done");
        }

        return record;
    }

    /**
     * This function writes out each band to its own file
     * 
     * @param segment
     * @param imageName
     * @param deserializer
     * @param imageNumber
     * @throws NITFException
     */
    private static void manuallyWriteImageBands(ImageSegment segment,
            String imageName, ImageReader deserializer, int imageNumber)
            throws NITFException
    {
        int nBits = segment.getSubheader().getNumBitsPerPixel().getIntData();
        int nBands = segment.getSubheader().getNumImageBands().getIntData();
        int xBands = segment.getSubheader().getNumMultispectralImageBands()
                .getIntData();
        nBands += xBands;

        int nRows = segment.getSubheader().getNumRows().getIntData();
        int nColumns = segment.getSubheader().getNumCols().getIntData();
        int subimageSize = nRows * nColumns * ((nBits - 1) / 8 + 1);
        System.out.println("Window: " + subimageSize);

        System.out.println("NBANDS -> " + nBands);
        System.out.println("NROWS -> " + nRows);
        System.out.println("NCOLS -> " + nColumns);
        System.out.println("PVTYPE -> "
                + segment.getSubheader().getPixelValueType().getStringData());

        byte[][] buffer = new byte[nBands][subimageSize];
        int band = 0;

        SubWindow subimage = new SubWindow();
        subimage.setStartCol(0);
        subimage.setStartRow(0);
        subimage.setNumCols(nColumns);
        subimage.setNumRows(nRows);
        int[] bandList = new int[nBands];
        for (int i = 0; i < bandList.length; i++)
        {
            bandList[i] = i;
        }
        subimage.setBandList(bandList);
        subimage.setNumBands(nBands);

        System.out.println("About to read");

        deserializer.read(subimage, buffer);
        System.out.println("just read");
        for (int i = 0; i < nBands; i++)
        {
            String fileName = makeBandName(imageName, imageNumber, i);
            IOHandle toFile = new IOHandle(fileName,
                    IOHandle.NITF_ACCESS_WRITEONLY, IOHandle.NITF_CREATE);

            toFile.write(buffer[i], subimageSize);
            toFile.close();
        }
        System.out.println("done");
    }

    /**
     * This writes the final NITF file
     * 
     * @param record
     * @param inRootFile
     * @param outFile
     * @throws NITFException
     */
    private static void doWrite(Record record, String inRootFile, String outFile)
            throws NITFException
    {
        IOHandle output_io = new IOHandle(outFile,
                IOHandle.NITF_ACCESS_WRITEONLY, IOHandle.NITF_CREATE);

        Writer writer = new Writer();
        boolean b = writer.prepare(record, output_io);
        System.out.println("b = " + b);

        ImageSegment[] images = record.getImages();
        for (int i = 0; i < images.length; i++)
        {
            ImageSegment imageSegment = images[i];

            // just for giggles, lets add a LUT for this
            /*
             * BandInfo info = imageSegment.getSubheader().getBandInfo()[i]; if
             * (info.getLookupTable() == null) { byte[] data = new byte[256];
             * for (int j = 0; j < 256; ++j) data[i] = (byte)j; LookupTable lut =
             * new LookupTable(1, 256, data); info.setLookupTable(lut); }
             */

            int numImageBands = imageSegment.getSubheader().getNumImageBands()
                    .getIntData();

            ImageWriter iWriter = writer.getNewImageWriter(i);

            ImageSource iSource = setupBands(numImageBands, i, inRootFile);

            iWriter.attachSource(iSource);
        }

        boolean wroteOK = writer.write();
        System.out.println("wroteOK = " + wroteOK);

        output_io.close();
    }

    /**
     * Creates an imagesource from each of the bands
     * 
     * @param nbands
     * @param imageNum
     * @param inRootFile
     * @return
     * @throws NITFException
     */
    private static ImageSource setupBands(int nbands, int imageNum,
            String inRootFile) throws NITFException
    {
        ImageSource iSource = new ImageSource();
        for (int i = 0; i < nbands; i++)
        {
            String inFile = makeBandName(inRootFile, imageNum, i);
            IOHandle sourceHandle = new IOHandle(inFile,
                    IOHandle.NITF_ACCESS_READONLY, IOHandle.NITF_OPEN_EXISTING);

            FileSource bandSource = new FileSource(sourceHandle, 0, 1, 0);
            iSource.addBand(bandSource);
        }
        return iSource;
    }
}
