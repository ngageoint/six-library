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
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

import nitf.DownSampler;
import nitf.IOHandle;
import nitf.ImageReader;
import nitf.ImageSegment;
import nitf.NITFException;
import nitf.PixelSkipDownSampler;
import nitf.Reader;
import nitf.Record;
import nitf.SubWindow;

/**
 * This test reads an image and writes out the raw image data You must have
 * write privileges in the directory this is run in, or you can pass in as an
 * optional parameter an output directory. Also, you can pass in a row and
 * column skip factor, which can be used to downsample the image (decimation).
 */
public class TestImageLoading
{

    /**
     * Performs the write call, writing one row at a time
     * 
     * @param segment
     *            the image sement to write
     * @param outputDir
     *            the output directory
     * @param baseName
     *            the base name of the input image
     * @param deserializer
     *            the imagereader object
     * @param imageNumber
     *            the image number in the file
     */
    private static void writeImage(ImageSegment segment, File outputDir,
            String baseName, ImageReader deserializer, int imageNumber,
            DownSampler downSampler)
    {

        int nBits = segment.getSubheader().getNumBitsPerPixel().getIntData();
        int nBands = segment.getSubheader().getNumImageBands().getIntData();
        int xBands = segment.getSubheader().getNumMultispectralImageBands()
                .getIntData();
        // get the number of bands
        nBands += xBands;

        int nRows = segment.getSubheader().getNumRows().getIntData();
        int nColumns = segment.getSubheader().getNumCols().getIntData();

        int rowSkip = 1;
        int colSkip = 1;

        if (downSampler != null)
        {
            rowSkip = downSampler.getRowSkip();
            colSkip = downSampler.getColSkip();
            nRows /= rowSkip;
            nColumns /= colSkip;
        }

        int subimageSize = nColumns * ((nBits - 1) / 8 + 1);

        byte[][] buffer = new byte[nBands][subimageSize];

        try
        {
            IOHandle[] handles = new IOHandle[nBands];

            // first, create an output handle for each band
            for (int i = 0; i < nBands; ++i)
            {
                File outFile = new File(outputDir, baseName + "_" + nRows + "x"
                        + nColumns + "_" + imageNumber + "_BAND" + i + ".out");

                System.out.println("Creating output file: "
                        + outFile.getAbsolutePath());

                // create an output write-only handle
                IOHandle handle = new IOHandle(outFile.getPath(),
                        IOHandle.NITF_ACCESS_WRITEONLY, IOHandle.NITF_CREATE);

                handles[i] = handle;
            }

            // set the band list
            int[] bands = new int[nBands];
            for (int i = 0; i < bands.length; i++)
                bands[i] = i;

            // set up the subwindow
            SubWindow subimage = new SubWindow();
            subimage.setStartCol(0);
            subimage.setNumCols(nColumns);
            subimage.setNumRows(1);
            subimage.setBandList(bands);
            subimage.setNumBands(nBands);

            // read one row at a time, and write it to the file
            // Note- this is not very efficient at all
            for (int i = 0; i < nRows; ++i)
            {
                subimage.setStartRow(i * rowSkip);

                if (downSampler != null)
                    subimage.setDownSampler(downSampler);

                // read the image data to the buffer
                boolean status = deserializer.read(subimage, buffer);

                // iterate over the bands, and write the data to its own file
                for (int j = 0; j < nBands; ++j)
                {
                    // write the data to the output handle
                    handles[j].write(buffer[j], subimageSize);
                }
            }

            // now close each handle
            for (int i = 0; i < nBands; ++i)
            {
                handles[i].close();
                System.out.println("Done writing to file: "
                        + handles[i].getFileName());
            }
        }
        catch (NITFException nitx)
        {
            nitx.printStackTrace();
            return;
        }
    }

    public static void main(String[] args) throws NITFException
    {
        // must supply a filename
        if (args.length < 1)
        {
            System.err
                    .println("usage: java TestImageLoading fileName [-sr skipRows] [-sc skipCols] [outputDir]");
            System.exit(0);
        }

        List argList = new Vector();
        argList.addAll(Arrays.asList(args));
        argList.remove(0);

        int skipRows = 1;
        int skipCols = 1;
        // by default, set the output to the current dir
        String outDirname = System.getProperty("user.dir");

        for (Iterator iter = argList.iterator(); iter.hasNext();)
        {
            String arg = (String) iter.next();
            if (arg.equals("-sr") && iter.hasNext())
            {
                skipRows = Integer.parseInt((String) iter.next());
            }
            else if (arg.equals("-sc") && iter.hasNext())
            {
                skipCols = Integer.parseInt((String) iter.next());
            }
            else
                outDirname = arg;
        }

        // normalize, if bad data
        skipRows = skipRows <= 0 ? 1 : skipRows;
        skipCols = skipCols <= 0 ? 1 : skipCols;

        // try to get the outputDir, if one is passed in
        File outputDir = new File(outDirname);
        outputDir.mkdirs();

        // must be able to write to it, or exit
        if (!outputDir.canWrite())
        {
            System.err.println("Unable to write to output directory ["
                    + outputDir + "]");
            System.exit(1);
        }

        // create a new nitf.Reader object
        Reader reader = new Reader();

        // create an IOHandle to open the file read-only
        IOHandle handle = new IOHandle(args[0], IOHandle.NITF_ACCESS_READONLY,
                IOHandle.NITF_OPEN_EXISTING);

        // read the file, and get back the nitf.Record object
        Record record = reader.read(handle);

        // make a downsampler
        DownSampler downSampler = new PixelSkipDownSampler(skipRows, skipCols);

        // get the image segments
        ImageSegment[] segments = record.getImages();
        for (int i = 0; i < segments.length; ++i)
        {
            ImageSegment segment = segments[i];

            // get a new ImageReader
            ImageReader deserializer = reader.getNewImageReader(i);

            // write the image
            writeImage(segment, outputDir, new File(args[0]).getName(),
                    deserializer, 0, downSampler);
        }

        // close the handle
        handle.close();
    }

}
