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

import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferByte;
import java.awt.image.SampleModel;
import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.List;

import javax.imageio.ImageIO;

import nitf.BandInfo;
import nitf.FileHeader;
import nitf.FileSource;
import nitf.IOHandle;
import nitf.ImageSegment;
import nitf.ImageSource;
import nitf.ImageSubheader;
import nitf.ImageWriter;
import nitf.MemorySource;
import nitf.NITFException;
import nitf.Record;
import nitf.Version;
import nitf.Writer;

/**
 * This test writes a test image out to a file
 */
public class TestWriter2
{

    public static final String FILENAME = "test_file.src";

    public static final String OUTPUT_NAME = "test_file.ntf";

    private static String _outputName = OUTPUT_NAME;

    private static int _width;

    private static int _height;

    private static int _bands;

    public static void main(String[] args) throws nitf.NITFException,
            IOException
    {

        final List list = Arrays.asList(args);
        String inputFile = null;
        for (int i = 0; i < list.size(); i++)
        {
            String arg = ((String) list.get(i)).trim();
            if (arg.equals("-o") && i < list.size() - 1)
            {
                _outputName = (String) list.get(++i);
            }
            else if (arg.equals("--help") || arg.equals("--?"))
            {
                System.out.println("usage: java " + TestWriter2.class.getName()
                        + " [-b bands] [-o name] [--help | --?] image");
                System.out.println("    Where:");
                System.out
                        .println("-------------------------------------------------------");
                System.out
                        .println("name       ............    sets the output file name");
                System.out.println("--help and --? print this help message");
                System.exit(0);
            }
            else
                inputFile = arg;
        }

        /* write the test image out */
        // writeTestImage();
        /* create bandsources from the image */
        // final ImageSource imageSource = makeImageSource();
        final ImageSource imageSource = makeImageSource(inputFile);

        writeNITFImage(imageSource);

        // delete the test_file.src file
        new File(FILENAME).delete();
    }

    private static void writeNITFImage(ImageSource imageSource)
            throws NITFException
    {

        Record record = new Record(Version.NITF_21);

        // fill the header
        fillHeader(record);

        // fill the image
        fillImageSubheader(record);

        IOHandle outHandle = new IOHandle(_outputName,
                IOHandle.NITF_ACCESS_WRITEONLY, IOHandle.NITF_CREATE);
        Writer writer = new Writer();

        writer.prepare(record, outHandle);
        final ImageWriter imageWriter = writer.getNewImageWriter(0);
        imageWriter.attachSource(imageSource);

        final boolean status = writer.write();
        System.out.println("Write Image Status = " + status);

        outHandle.close();
    }

    private static void fillImageSubheader(Record record) throws NITFException
    {
        final ImageSegment imageSegment = record.newImageSegment();
        final ImageSubheader subheader = imageSegment.getSubheader();

        subheader.getImageCoordinateSystem().setData(" ");

        subheader.getFilePartType().setData("IM");
        // imageID
        subheader.getImageId().setData(
                String.valueOf(System.currentTimeMillis()));

        // title
        subheader.getImageTitle().setData("Test Image");

        // target ID
        // subheader.getTargetId().setData("");

        // source
        subheader.getImageSource().setData("Artificial");

        // pixelType
        subheader.getPixelValueType().setData("INT");

        // pixelJust
        subheader.getPixelJustification().setData("R");

        // compression
        subheader.getImageCompression().setData("NC");

        // displayLevel
        subheader.getImageDisplayLevel().setData("1");

        // attachmentLevel
        subheader.getImageAttachmentLevel().setData("0");

        // imageCategory
        subheader.getImageCategory().setData("VIS");

        // imageRep
        switch (_bands)
        {
        case 1:
            subheader.getImageRepresentation().setData("MONO");
            break;
        case 3:
            subheader.getImageRepresentation().setData("RGB");
            break;
        default:
            subheader.getImageRepresentation().setData("MULTI");
            break;
        }

        // imageMode
        subheader.getImageMode().setData("B");

        // magnification
        subheader.getImageMagnification().setData("1.0");

        // imageLocation
        subheader.getImageLocation().setData("0000000000");

        // syncCode
        subheader.getImageSyncCode().setData("0");

        // integer fields
        subheader.getNumCols().setData(String.valueOf(_width));
        subheader.getNumRows().setData(String.valueOf(_height));
        subheader.getNumBitsPerPixel().setData(String.valueOf(8));
        subheader.getActualBitsPerPixel().setData(String.valueOf(8));
        subheader.getNumBlocksPerCol().setData(String.valueOf(1));
        subheader.getNumBlocksPerRow().setData(String.valueOf(1));
        subheader.getNumPixelsPerHorizBlock().setData(String.valueOf(_width));
        subheader.getNumPixelsPerVertBlock().setData(String.valueOf(_height));

        subheader.insertImageComment("This is a test image", 0);
        subheader.insertImageComment(
                "It is comprised of artificial pixel data", 1);

        subheader.removeImageComment(0);

        final String irep = "RGB";

        /* create new bands */
        subheader.createBands(_bands);
        final BandInfo[] bandInfos = subheader.getBandInfo();
        for (int i = 0; i < bandInfos.length; i++)
        {
            BandInfo bandInfo = bandInfos[i];
            bandInfo.getNumLUTs().setData("0");
            if (_bands == 3)
            {
                bandInfo.getRepresentation().setData("" + irep.charAt(i));
            }
            // bandInfo.getSubcategory().setData("0");
            bandInfo.getImageFilterCondition().setData("N");
        }

        // hard-coded values for now
        subheader.getImageSecurityClass().setData("U");
        subheader.getEncrypted().setData("0");
    }

    private static void fillHeader(Record record) throws NITFException
    {
        final FileHeader header = record.getHeader();
        header.getFileHeader().setData("NITF");
        header.getFileVersion().setData("02.10");

        header.getSystemType().setData("    ");
        header.getBackgroundColor().setData("   ");

        // set the date
        header.getFileDateTime().setData(
                new SimpleDateFormat("ddHHmmss'Z'MMMyy").format(new Date())
                        .toUpperCase());

        header.getOriginStationID().setData("Test Image - Ypsilanti");

        header.getOriginatorName().setData("GD-AIS");

        header.getOriginatorPhone().setData("734-480-xxxx");

        header.getComplianceLevel().setData("03");

        // these are all defaults hard-coded to the values given
        header.getClassification().setData("U");
        header.getEncrypted().setData("0");
        header.getMessageCopyNum().setData("0");
        header.getMessageNumCopies().setData("0");
    }

    private static ImageSource makeImageSource(String input)
            throws NITFException, IOException
    {
        if (input == null)
            throw new IOException("no input image provided");

        File file = new File(input);
        if (!file.exists())
            throw new IOException("input image does not exist");

        BufferedImage bufImage = ImageIO.read(file);
        _bands = bufImage.getData().getNumBands();
        _height = bufImage.getHeight();
        _width = bufImage.getWidth();
        int type = bufImage.getType();

        ImageSource imageSource = new ImageSource();

        DataBuffer dataBuffer = bufImage.getData().getDataBuffer();

        System.out.println(type);
        if (type == BufferedImage.TYPE_BYTE_GRAY)
        {
            DataBufferByte dbb = (DataBufferByte) dataBuffer;
            MemorySource source = new MemorySource(dbb.getData(0), _height
                    * _width, 0, 1, 0);
            imageSource.addBand(source);
        }
        else if (type == BufferedImage.TYPE_3BYTE_BGR)
        {
            SampleModel sampleModel = bufImage.getSampleModel();
            for (int i = 0; i < _bands; ++i)
            {
                int[] data = sampleModel.getSamples(0, 0, _width, _height, i,
                        (int[]) null, dataBuffer);
                byte[] buf = new byte[data.length];
                for (int j = 0; j < data.length; ++j)
                {
                    buf[j] = (byte) data[j];
                }
                MemorySource source = new MemorySource(buf, _height * _width,
                        0, 1, 0);
                imageSource.addBand(source);
            }
        }
        else
            throw new NITFException("Unsupported image type");

        return imageSource;
    }

}
