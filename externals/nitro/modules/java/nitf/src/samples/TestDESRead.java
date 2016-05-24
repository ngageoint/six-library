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

import nitf.DESegment;
import nitf.DESubheader;
import nitf.IOHandle;
import nitf.NITFException;
import nitf.Reader;
import nitf.Record;
import nitf.SegmentReader;
import nitf.TRE;

/**
 * Tests Reading the DES data using the new DES UserSegment plugin(s) This is a
 * Java version of the C test case test_DES_read.c
 */
public class TestDESRead
{

    public static void main(String[] args) throws NITFException
    {
        if (args.length != 1)
        {
            System.out.println("Usage: java TestDESRead <nitf-file>");
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

        final DESegment[] dataExtensions = record.getDataExtensions();
        for (int i = 0; i < dataExtensions.length; i++)
        {
            DESegment dataExtension = dataExtensions[i];
            final DESubheader subheader = dataExtension.getSubheader();
            final TRE subheaderFields = subheader.getSubheaderFields();
            subheaderFields.print(System.out);

            final SegmentReader deReader = reader.getNewDEReader(i);
            byte[] buf = new byte[(int) deReader.getSize()];

            final boolean status = deReader.read(buf, (int) deReader.getSize());
            System.out.println("status = " + status);
            System.out.println("buf = " + new String(buf));
        }
        handle.close();
    }

}
