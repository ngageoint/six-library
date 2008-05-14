/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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


import nitf.BandSource;
import nitf.ImageSource;
import nitf.MemorySource;

/**
 * Test class for showing how to use an ImageSource
 */
public class TestImageSource
{

    public static final byte[] MEMBUF = "ABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABC"
            .getBytes();

    public static final int BANDS = 3;

    public static void main(String[] args) throws nitf.NITFException
    {
        /* create bandsources from the buffer */
        ImageSource imageSource = new ImageSource();
        int size = MEMBUF.length / BANDS;

        System.out.println("Source Data = " + new String(MEMBUF));

        // add bandSources to the ImageSource
        for (int i = 0; i < BANDS; ++i)
        {
            MemorySource source = new MemorySource(MEMBUF, size, i, 1, BANDS - 1);
            imageSource.addBand(source);
        }

        // now, "read" the data from the memory source, and print it
        BandSource[] bandSources = imageSource.getBandSources();
        for (int i = 0; i < bandSources.length; i++)
        {
            byte[] buf = new byte[size];
            BandSource bandSource = bandSources[i];
            bandSource.read(buf, buf.length);
            System.out
                    .println("Band " + (i + 1) + " Data = " + new String(buf));
        }
    }
}

