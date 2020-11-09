/*
 * =========================================================================
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 */

import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Iterator;
import java.util.List;

import javax.imageio.ImageIO;
import javax.imageio.ImageReadParam;
import javax.imageio.spi.IIORegistry;

import nitf.ImageSubheader;
import nitf.imageio.ImageIOUtils;
import nitf.imageio.NITFReader;
import nitf.imageio.NITFReaderSpi;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class ShowImage
{
    private static final Log log = LogFactory.getLog(ShowImage.class);

    static
    {
        IIORegistry registry = IIORegistry.getDefaultInstance();
        registry.registerServiceProvider(new NITFReaderSpi());
    }

    public static void main(String args[]) throws IOException
    {
        List<File> files = ImageIOUtils.getFiles(args, new String[] { "ntf",
                "nsf" });

        for (Iterator iter = files.iterator(); iter.hasNext();)
        {
            try
            {
                File file = (File) iter.next();
                log.debug("Reading: " + file.getAbsolutePath());
                NITFReader imageReader = (NITFReader) ImageIOUtils
                        .getImageReader("nitf", file);

                for (int i = 0; i < imageReader.getRecord().getImages().length; ++i)
                {
                    log.debug(file.getName() + "[" + i + "]");

                    ByteArrayOutputStream stream = new ByteArrayOutputStream();
                    ImageSubheader subheader = imageReader.getRecord()
                            .getImages()[i].getSubheader();
                    subheader.print(new PrintStream(stream));
                    log.debug(stream.toString());

                    try
                    {
                        int numBands = subheader.getBandCount();
                        String irep = subheader.getImageRepresentation()
                                .getStringData().trim();
                        int bitsPerPixel = subheader.getNumBitsPerPixel()
                                .getIntData();
                        int nBytes = (bitsPerPixel - 1) / 8 + 1;

                        if (irep.equals("RGB") && numBands == 3)
                        {
                            BufferedImage image = imageReader.read(i);
                            ImageIOUtils.showImage(image, file.getName() + "["
                                    + i + "]", true);
                        }
                        else
                        {
                            // read each band, separately
                            for (int j = 0; j < numBands; ++j)
                            {
                                if (nBytes == 1 || nBytes == 2 || nBytes == 4
                                        || nBytes == 8)
                                {
                                    ImageReadParam readParam = imageReader
                                            .getDefaultReadParam();
                                    readParam.setSourceBands(new int[] { j });
                                    BufferedImage image = imageReader.read(i,
                                            readParam);
                                    ImageIOUtils.showImage(image, file
                                            .getName()
                                            + "[" + i + "][" + j + "]", true);

                                    ImageIO.write(image, "jpg",
                                            new FileOutputStream("image" + i
                                                    + "_" + j + ".jpg"));

                                    // downsample
                                    // readParam.setSourceSubsampling(2, 2, 0,
                                    // 0);
                                    // BufferedImage smallerImage = imageReader
                                    // .read(i, readParam);
                                    //
                                    // ImageIOUtils.showImage(smallerImage,
                                    // "DOWNSAMPLED: " + file.getName());

                                }
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        System.out.println(ExceptionUtils.getStackTrace(e));
                        log.error(ExceptionUtils.getStackTrace(e));
                    }
                }
            }
            catch (Exception e)
            {
                log.debug(ExceptionUtils.getStackTrace(e));
            }
        }
    }
}
