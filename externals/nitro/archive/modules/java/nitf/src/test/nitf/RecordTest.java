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

package nitf;

import junit.framework.TestCase;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class RecordTest extends TestCase
{
    private static final Log log = LogFactory.getLog(RecordTest.class);

    public void testCreate()
    {
        try
        {
            Record record = new Record();
            assertTrue(record.getHeader().getFileHeader().setData("NITF"));
            assertEquals(record.getHeader().getFileHeader().toString(), "NITF");
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }

    public void testImageComments()
    {
        try
        {
            Record record = new Record();
            ImageSegment imageSegment = record.newImageSegment();
            ImageSubheader subheader = imageSegment.getSubheader();
            assertTrue(subheader.insertImageComment(null, 0) >= 0);
            assertTrue(subheader.insertImageComment("2nd comment", 1) >= 0);

            Field[] comments = subheader.getImageComments();
            assertEquals(2, comments.length);
            log.info(comments[0]);
            log.info(comments[1]);
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }

}
