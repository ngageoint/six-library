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

import java.io.File;
import java.io.FilenameFilter;
import java.io.PrintStream;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

import nitf.DESegment;
import nitf.GraphicSegment;
import nitf.IOHandle;
import nitf.ImageSegment;
import nitf.LabelSegment;
import nitf.NITFException;
import nitf.RESegment;
import nitf.Reader;
import nitf.Record;
import nitf.SegmentReader;
import nitf.TextSegment;

import org.apache.commons.io.FilenameUtils;

/**
 * This test outputs the data from the NITF record to System.out
 */
public class TestDumpRecord
{

    private static void dumpRecord(Record record, Reader reader, PrintStream out)
                                                                                 throws NITFException
    {
        out.println("----- Header -----");
        record.getHeader().print(out);

        final ImageSegment[] images = record.getImages();
        for (int i = 0; i < images.length; i++)
        {
            out.println("----- Image[" + i + "] -----");
            ImageSegment image = images[i];
            image.getSubheader().print(out);
        }

        final GraphicSegment[] graphics = record.getGraphics();
        for (int i = 0; i < graphics.length; i++)
        {
            out.println("----- Graphic[" + i + "] -----");
            GraphicSegment graphic = graphics[i];
            graphic.getSubheader().print(out);
        }

        final LabelSegment[] labels = record.getLabels();
        for (int i = 0; i < labels.length; i++)
        {
            out.println("----- Label[" + i + "] -----");
            LabelSegment label = labels[i];
            label.getSubheader().print(out);
        }

        final TextSegment[] texts = record.getTexts();
        for (int i = 0; i < texts.length; i++)
        {
            out.println("----- Text[" + i + "] -----");
            TextSegment text = texts[i];
            text.getSubheader().print(out);
            SegmentReader textReader = reader.getNewTextReader(i);
            byte[] data = new byte[(int) textReader.getSize()];
            textReader.read(data);
            out.println("TEXT[" + i + "] = '" + new String(data) + "'");
        }

        final DESegment[] dataExtensions = record.getDataExtensions();
        for (int i = 0; i < dataExtensions.length; i++)
        {
            out.println("----- DES[" + i + "] -----");
            DESegment dataExtension = dataExtensions[i];
            dataExtension.getSubheader().print(out);
            SegmentReader segReader = reader.getNewDEReader(i);
            byte[] data = new byte[(int) segReader.getSize()];
            segReader.read(data);
            out.println("DES[" + i + "] = '" + new String(data) + "'");
        }

        final RESegment[] reservedExtensions = record.getReservedExtensions();
        for (int i = 0; i < reservedExtensions.length; i++)
        {
            out.println("----- RES[" + i + "] -----");
            RESegment reservedExtension = reservedExtensions[i];
            reservedExtension.getSubheader().print(out);
        }
    }

    public static void main(String[] args) throws NITFException
    {
        List<String> argList = Arrays.asList(args);
        List<File> files = new LinkedList<File>();
        for (String arg : argList)
        {
            File f = new File(arg);
            if (f.isDirectory())
            {
                File[] dirFiles = f.listFiles(new FilenameFilter()
                {
                    public boolean accept(File dir, String name)
                    {
                        String ext = FilenameUtils.getExtension(name)
                                                  .toLowerCase();
                        return ext.matches("nitf|nsf|ntf");
                    }
                });
                files.addAll(Arrays.asList(dirFiles));
            }
            else
                files.add(f);
        }

        Reader reader = new Reader();
        for (File file : files)
        {
            PrintStream out = System.out;

            out.println("=== " + file.getAbsolutePath() + " ===");
            IOHandle handle = new IOHandle(file.getAbsolutePath());
            Record record = reader.read(handle);
            dumpRecord(record, reader, out);
            handle.close();

            record.destruct(); // tells the memory manager to decrement the ref
            // count
        }
    }
}
