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

package nitf;

import junit.framework.TestCase;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * Test class for showing how to use an ImageSource
 */
public class ImageSourceTest extends TestCase
{
    private static final Log log = LogFactory.getLog(ImageSourceTest.class);

    public static final byte[] TEST_BUF = "ABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABC"
            .getBytes();

    public static final int TEST_BUF_NUM_BANDS = 3;

    public static final String[] TEST_BUF_BANDS = { "AAAAAAAAAAAAAAAA",
            "BBBBBBBBBBBBBBBB", "CCCCCCCCCCCCCCCC" };

    public void testImageSource()
    {
        try
        {
            ImageSource imageSource = new ImageSource();
            int size = TEST_BUF.length / TEST_BUF_NUM_BANDS;

            log.info("Source Data = " + new String(TEST_BUF));

            // add bandSources to the ImageSource
            for (int i = 0; i < TEST_BUF_NUM_BANDS; ++i)
            {
                MemorySource source = new MemorySource(TEST_BUF, size, i, 1,
                        TEST_BUF_NUM_BANDS - 1);
                assertTrue(imageSource.addBand(source));
            }

            // now, "read" the data from the memory source, and print it
            BandSource[] bandSources = imageSource.getBandSources();
            assertEquals(bandSources.length, TEST_BUF_NUM_BANDS);
            for (int i = 0; i < bandSources.length; i++)
            {
                byte[] buf = new byte[size];
                BandSource bandSource = bandSources[i];
                bandSource.read(buf, buf.length);
                log.info("Band " + (i + 1) + " Data = " + new String(buf));
                assertEquals(TEST_BUF_BANDS[i], new String(buf));
            }
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }
}