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
import nitf.IOHandle;
import nitf.Record;
import nitf.SegmentSource;
import nitf.SegmentWriter;
import nitf.TRE;
import nitf.Writer;

public class TestDESCreate
{

    final static String DATA = "123456789ABCDEF0";

    public static void main(String[] args) throws Exception
    {

        Record rec = new Record();

        rec.getHeader().getFileHeader().setData("NITF");
        rec.getHeader().getFileVersion().setData("02.10");
        rec.getHeader().getClassification().setData("U");
        rec.getHeader().getFileTitle().setData("Test DES");

        // add a new DESegment
        final DESegment des = rec.newDESegment();

        // create and get a new subheaderFields TRE
        // you want to use the returned one since the TRE passed in gets cloned
        TRE newTRE = new TRE("TEST_DES");
        TRE tre = des.getSubheader().setSubheaderFields(newTRE);
        // tre.setField("FILCMT", "A comment");

        tre.setField("TEST_DES_COUNT", "16");
        tre.setField("TEST_DES_START", "065");
        tre.setField("TEST_DES_INCREMENT", "01");

        tre.print(System.out);

        des.getSubheader().getFilePartType().setData("DE");
        // set the typeId to the TRE tag
        des.getSubheader().getTypeID().setData("TEST_DES");
        des.getSubheader().getVersion().setData("01");
        des.getSubheader().getSecurityClass().setData("U");

        IOHandle handle = new IOHandle("test.ntf",
                IOHandle.NITF_ACCESS_WRITEONLY, IOHandle.NITF_CREATE);

        // setup a Writer
        Writer writer = new Writer();
        writer.prepare(rec, handle);

        final SegmentWriter deWriter = writer.getNewDEWriter(0);

        final SegmentSource source = SegmentSource.makeSegmentMemorySource(DATA
                .getBytes(), DATA.length(), 0, 0);

        deWriter.attachSource(source);

        writer.write();
    }

}
