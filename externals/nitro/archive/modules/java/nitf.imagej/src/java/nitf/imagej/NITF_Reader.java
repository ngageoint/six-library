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

package nitf.imagej;

import ij.IJ;
import ij.ImageJ;
import ij.ImagePlus;
import ij.ImageStack;
import ij.Prefs;
import ij.plugin.frame.PlugInFrame;
import ij.text.TextPanel;
import ij.text.TextWindow;
import ij.util.Java2;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferByte;
import java.awt.image.DataBufferUShort;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.PrintStream;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

import javax.imageio.ImageReadParam;
import javax.imageio.spi.IIORegistry;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileFilter;

import nitf.ImageSubheader;
import nitf.Record;
import nitf.imageio.ImageIOUtils;
import nitf.imageio.NITFReader;
import nitf.imageio.NITFReaderSpi;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class NITF_Reader extends PlugInFrame
{
    private Log log = LogFactory.getLog(NITF_Reader.class);

    private JFileChooser fileChooser;

    private FileFilter fileFilter;

    protected final static String NITF_READER_DIR = "NITF_Reader.lastdir";

    static
    {
        IIORegistry registry = IIORegistry.getDefaultInstance();
        registry.registerServiceProvider(new NITFReaderSpi());
    }

    public NITF_Reader()
    {
        super("NITF Reader");
        Java2.setSystemLookAndFeel();
        fileChooser = new JFileChooser();
        fileChooser.setDialogTitle("Open NITF Image(s)...");
        String dir;
        if ((dir = Prefs.get(NITF_READER_DIR, null)) != null)
            fileChooser.setCurrentDirectory(new File(dir));
        fileChooser.setMultiSelectionEnabled(true);
        fileChooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
        fileChooser.setAcceptAllFileFilterUsed(true);
        fileFilter = new FileFilter()
        {
            public boolean accept(File f)
            {
                String name = f.getName().toLowerCase().trim();
                return f.isDirectory()
                        || ((name.endsWith("ntf") || name.endsWith("nitf") || name
                                .endsWith("nsf")) && f.canRead());
            }

            public String getDescription()
            {
                return "NITF Images (*.nitf,*.ntf, *.nsif)";
            }
        };
        fileChooser.setFileFilter(fileFilter);
    }

    private List<File> chooseFiles()
    {
        List<File> filesToOpen = new Vector<File>();
        if (JFileChooser.APPROVE_OPTION == fileChooser.showOpenDialog(IJ
                .getInstance()))
        {
            File[] selectedFiles = fileChooser.getSelectedFiles();

            for (int i = 0; i < selectedFiles.length; i++)
            {
                File file = selectedFiles[i];
                if (file.isDirectory())
                {
                    File[] listFiles = file.listFiles();
                    for (int j = 0; j < listFiles.length; j++)
                    {
                        File subFile = listFiles[j];
                        if (subFile.isFile() && fileFilter.accept(subFile))
                        {
                            filesToOpen.add(subFile);
                        }
                    }
                }
                else
                    filesToOpen.add(file);
            }

            // save the file location
            if (selectedFiles.length == 1 && selectedFiles[0].isDirectory())
                Prefs.set(NITF_READER_DIR, selectedFiles[0].getAbsolutePath());
            else if (selectedFiles.length > 1)
                Prefs.set(NITF_READER_DIR, selectedFiles[0].getParent());
        }
        return filesToOpen;
    }

    public void run(String arg)
    {
        List<File> filesToOpen = chooseFiles();
        for (Iterator iter = filesToOpen.iterator(); iter.hasNext();)
        {
            File file = (File) iter.next();
            openNITF(file);
        }
    }

    /**
     * Opens the requested NITF, displaying a Text Window with the metadata
     * contents, and an Image window for each image in the file
     * 
     * @param dir
     * @param name
     */
    public void openNITF(File file)
    {
        try
        {
            log.debug("Reading: " + file.getAbsolutePath());
            NITFReader imageReader = (NITFReader) ImageIOUtils.getImageReader(
                    "nitf", file);

            Record record = imageReader.getRecord();
            // print the record contents to a byte stream
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            record.print(new PrintStream(out));

            TextWindow resultsWindow = new TextWindow(file.getName()
                    + " [metadata]", "", IJ.getInstance().getWidth(), 400);
            TextPanel textPanel = resultsWindow.getTextPanel();
            textPanel.append(out.toString());
            textPanel.setFont(new Font("Courier New", Font.PLAIN, 16));
            resultsWindow.setVisible(true);

            Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

            for (int i = 0; i < record.getImages().length; ++i)
            {
                ImageSubheader subheader = record.getImages()[i].getSubheader();
                try
                {
                    int numCols = subheader.getNumCols().getIntData();
                    int numRows = subheader.getNumRows().getIntData();
                    int numBands = subheader.getBandCount();
                    String irep = subheader.getImageRepresentation()
                            .getStringData().trim();
                    int bitsPerPixel = subheader.getNumBitsPerPixel()
                            .getIntData();

                    ImageReadParam readParam = imageReader
                            .getDefaultReadParam();

                    String downsampleString = "";

                    // if the image is larger than 1.5 times the screen
                    // width/height,
                    // then just downsample right away
                    if (numCols > (screenSize.width * 1.5)
                            || numRows > (screenSize.height * 1.5))
                    {
                        int xTimes = (int) (numCols / (double) screenSize.width);
                        int yTimes = (int) (numRows / (double) screenSize.height);

                        downsampleString = " (downsampled from " + numCols
                                + "x" + numRows;

                        // pick the min, in case it turns out to be a little
                        // strip
                        int pixelSkip = Math.min(xTimes, yTimes);
                        numCols = (int) Math.ceil(numCols / pixelSkip);
                        numRows = (int) Math.ceil(numCols / pixelSkip);

                        downsampleString += "   to   " + numCols + "x"
                                + numRows + ")";
                        readParam.setSourceSubsampling(pixelSkip, pixelSkip, 0,
                                0);
                    }

                    if (irep.equals("RGB") && numBands == 3)
                    {
                        BufferedImage image = imageReader.read(i, readParam);
                        ImagePlus imagePlus = new ImagePlus(file.getName()
                                + " [" + i + "] - RGB" + downsampleString,
                                image);
                        imagePlus.show();
                    }
                    else
                    {
                        ImageStack imageStack = new ImageStack(numCols, numRows);
                        // read each band, separately
                        for (int j = 0; j < numBands; ++j)
                        {
                            if (bitsPerPixel == 16 || bitsPerPixel == 8
                                    || bitsPerPixel == 32 || bitsPerPixel == 64)
                            {
                                readParam.setSourceBands(new int[] { j });
                                BufferedImage image = imageReader.read(i,
                                        readParam);
                                imageStack.addSlice(file.getName() + " [" + i
                                        + "] Band " + j + downsampleString,
                                        getPixelsFromBufferedImage(image));
                            }
                        }
                        if (imageStack.getSize() > 0)
                        {
                            ImagePlus imagePlus = new ImagePlus(file.getName()
                                    + " [" + i + "]" + downsampleString,
                                    imageStack);
                            imagePlus.show();
                        }
                    }
                }
                catch (Exception e)
                {
                    log.debug(ExceptionUtils.getStackTrace(e));
                }
            }
        }
        catch (Exception e)
        {
            log.debug(ExceptionUtils.getStackTrace(e));
            IJ.error(e.getMessage());
        }
    }

    private static Object getPixelsFromBufferedImage(BufferedImage bufImage)
    {
        DataBuffer dataBuffer = bufImage.getData().getDataBuffer();
        if (dataBuffer.getDataType() == DataBuffer.TYPE_BYTE)
            return ((DataBufferByte) dataBuffer).getData();
        else if (dataBuffer.getDataType() == DataBuffer.TYPE_USHORT)
            return ((DataBufferUShort) dataBuffer).getData();
        return null;
    }

    public static void main(String[] args)
    {
        ImageJ ij = new ImageJ();
        // ij.setVisible(true);

        NITF_Reader reader = new NITF_Reader();
        reader.run("");
    }
}
