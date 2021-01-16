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

import java.io.IOException;

import nitf.GraphicSegment;
import nitf.IOHandle;
import nitf.IOInterface;
import nitf.ImageSegment;
import nitf.MemoryIO;
import nitf.NITFException;
import nitf.Reader;
import nitf.Record;
import nitf.StreamIOWriteHandler;
import nitf.TextSegment;
import nitf.Writer;

public class TestWriter4
{
    public static void main(String[] args) throws NITFException, IOException
    {

        if (args.length < 1)
        {
            System.out.println("Usage: java " + TestWriter4.class.getName()
                    + " <input-file> [<output-file>]");
            System.exit(1);
        }

        Reader reader = new Reader();
        IOInterface input = new IOHandle(args[0],
                IOHandle.NITF_ACCESS_READONLY, IOHandle.NITF_OPEN_EXISTING);
        Record record = reader.read(input);

        record.print(System.out);

        IOInterface output = null;
        if (args.length >= 2)
        {
            output = new IOHandle(args[1], IOHandle.NITF_ACCESS_WRITEONLY,
                    IOHandle.NITF_CREATE);
        }
        else
        {
            // else, bypass and write to memory
            output = new MemoryIO((int) input.getSize());
        }

        Writer writer = new Writer();
        writer.prepare(record, output);

        ImageSegment[] images = record.getImages();
        for (int i = 0; i < images.length; i++)
        {
            ImageSegment segment = images[i];
            long offset = segment.getImageOffset();
            StreamIOWriteHandler handler = new StreamIOWriteHandler(input,
                    offset, segment.getImageEnd() - offset);
            writer.setImageWriteHandler(i, handler);
        }

        GraphicSegment[] graphics = record.getGraphics();
        for (int i = 0; i < graphics.length; i++)
        {
            GraphicSegment segment = graphics[i];
            long offset = segment.getOffset();
            StreamIOWriteHandler handler = new StreamIOWriteHandler(input,
                    offset, segment.getEnd() - offset);
            writer.setGraphicWriteHandler(i, handler);
        }

        TextSegment[] texts = record.getTexts();
        for (int i = 0; i < texts.length; i++)
        {
            TextSegment segment = texts[i];
            long offset = segment.getOffset();
            StreamIOWriteHandler handler = new StreamIOWriteHandler(input,
                    offset, segment.getEnd() - offset);
            writer.setTextWriteHandler(i, handler);
        }

        writer.write();

        input.close();
        output.close();
    }
}
