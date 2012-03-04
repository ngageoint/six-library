/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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


#include <cstdio>

#include <import/nitf.hpp>

/*
    Test program for reading a DE segment

  This program creates a NITF file with a file header and a single DE Segment.

  The record is cloned from an input NITF with a single DE segment. Some
  fields and the data are modified and the result written out as a new NITF

  The input NITF file should have no DES segments

  This program uses the test_DES_example DE segment

  The calling sequence is:

    test_des_write inputFile outputFile
*/

static const char data[] = "123456789ABCDEF0";

/*
*   Ugly function to create the DES user header (a TRE)
*
*  This is here because the C++ library does not seem to have
*  a way to do this
*/

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 3)
        {
            fprintf(stderr, "Usage %s inputFile outputFile\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        /*    Get the input record */

        nitf::IOHandle in(argv[1]); /* Input I/O handle */
        nitf::Reader reader;
        nitf::Record record = reader.read(in);

        // Create the DE segment

        nitf::DESegment des = record.newDataExtensionSegment();

        // Set-up DE header


        des.getSubheader().getFilePartType().set("DE");
        des.getSubheader().getTypeID().set("TEST_DES");
        des.getSubheader().getVersion().set("01");
        des.getSubheader().getSecurityClass().set("U");

        /*       nitf::FileSecurity security =
                             record.getHeader().getSecurityGroup();
               des.getSubheader().setSecurityGroup(security); */
        nitf::FileSecurity security =
            record.getHeader().getSecurityGroup();
        des.getSubheader().setSecurityGroup(security.clone());

        // Set-up user header

        nitf::TRE *usrHdr = new nitf::TRE("TEST DES", "TEST DES");
        usrHdr->getField("TEST_DES_COUNT").set("16");
        usrHdr->getField("TEST_DES_START").set("065");
        usrHdr->getField("TEST_DES_INCREMENT").set("01");

        des.getSubheader().setSubheaderFields(*usrHdr);

        nitf::Writer writer;
        nitf::IOHandle output_io(argv[2], NITF_ACCESS_WRITEONLY, NITF_CREATE);
        writer.prepare(output_io, record);

        nitf::SegmentWriter sWriter = writer.newDEWriter(0);
        nitf::SegmentMemorySource sSource(data, strlen(data), 0, 0, false);
        sWriter.attachSource(sSource);
        writer.write();

        exit(EXIT_SUCCESS);
    }
    catch (except::Throwable & t)
    {
        std::cout << t.getTrace() << std::endl;
    }
}
