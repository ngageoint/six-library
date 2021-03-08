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


import nitf.*;

/**
 * This test outputs the data from the NITF record to System.out
 */
public class TestReader2
{

    public static void main(String[] args) throws NITFException
    {
        if (args.length != 1)
        {
            System.out.println("Usage: java TestReader2 <nitf-file>");
            System.exit(0);
        }
        System.out.println("Creating reader...");
        Reader reader = new Reader();
        System.out.println("Setting up handle...");
        IOHandle handle = new IOHandle(args[0], IOHandle.NITF_ACCESS_READONLY,
            IOHandle.NITF_OPEN_EXISTING);
        System.out.println("Parsing NITF file...");
        Record record = reader.read(handle);
        
        record.print(System.out);
        
        TextSegment[] texts = record.getTexts();
        for (int i = 0; i < texts.length; i++)
        {
            TextSegment segment = texts[i];
            SegmentReader textReader = reader.getNewTextReader(i);
            byte[] data = new byte[(int)textReader.getSize()];
            textReader.read(data);
            System.out.println("DATA = '" + new String(data) + "'");
        }
        

        handle.close();
    }

}

