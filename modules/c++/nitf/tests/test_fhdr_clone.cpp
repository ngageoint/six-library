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

#include <import/nitf.hpp>

#define SHOW(X) std::cout << #X << "=" << X << std::endl
#define SHOWI(X) std::cout << #X << "=" << X << std::endl
#define PRINT_HDR(X) std::cout << #X; printHdr(X)

void printHdr(nitf::FileHeader header)
{
    nitf::Uint32 i;
    SHOW( header.getFileHeader().toString() );
    SHOW( header.getFileVersion().toString() );
    SHOW( header.getComplianceLevel().toString() );
    SHOW( header.getSystemType().toString() );
    SHOW( header.getOriginStationID().toString() );
    SHOW( header.getFileDateTime().toString() );
    SHOW( header.getFileTitle().toString() );
    SHOW( header.getClassification().toString() );
    SHOW( header.getMessageCopyNum().toString() );
    SHOW( header.getMessageNumCopies().toString() );
    SHOW( header.getEncrypted().toString() );
    SHOW( header.getBackgroundColor().toString() );
    SHOW( header.getOriginatorName().toString() );
    SHOW( header.getOriginatorPhone().toString() );
    SHOWI( (nitf::Uint32)header.getFileLength() );
    SHOWI( (nitf::Uint32)header.getHeaderLength() );

    printf("The number of IMAGES contained in this file [%d]\n",
           (int)header.getNumImages());

    for (i = 0; i < (nitf::Uint32)header.getNumImages(); i++)
    {
        printf("\tThe length of IMAGE subheader [%d]: %d bytes\n",
               i, (nitf::Uint32)header.getImageInfo(i).getLengthSubheader());
        printf("\tThe length of the IMAGE data: %d bytes\n",
               (nitf::Uint32)header.getImageInfo(i).getLengthData());
    }

    printf("The number of GRAPHICS contained in this file [%d]\n",
           (nitf::Uint32)header.getNumGraphics());

    for (i = 0; i < (nitf::Uint32)header.getNumGraphics(); i++)
    {
        printf("\tThe length of GRAPHICS subheader [%d]: %d bytes\n",
               i, (nitf::Uint32)header.getGraphicInfo(i).getLengthSubheader());
        printf("\tThe length of the GRAPHICS data: %d bytes\n\n",
               (nitf::Uint32)header.getGraphicInfo(i).getLengthData());
    }
    printf("The number of LABELS contained in this file [%d]\n",
           (nitf::Uint32)header.getNumLabels());

    assert((nitf::Uint32)header.getNumLabels() == 0);

    printf("The number of TEXTS contained in this file [%d]\n",
           (nitf::Uint32)header.getNumTexts());

    for (i = 0; i < (nitf::Uint32)header.getNumTexts(); i++)
    {
        printf("\tThe length of TEXT subheader [%d]: %d bytes\n",
               i, (int)header.getTextInfo(i).getLengthSubheader());
        printf("\tThe length of the TEXT data: %d bytes\n\n",
               (int)header.getTextInfo(i).getLengthData());
    }
    printf("The number of DATA EXTENSIONS contained in this file [%d]\n",
           (nitf::Uint32)header.getNumDataExtensions());

    for (i = 0; i < (nitf::Uint32)header.getNumDataExtensions(); i++)
    {
        printf("\tThe length of DATA EXTENSION subheader [%d]: %d bytes\n",
               i, (nitf::Uint32)header.getDataExtensionInfo(i).getLengthSubheader());
        printf("\tThe length of the DATA EXTENSION data: %d bytes\n\n",
               (nitf::Uint32)header.getDataExtensionInfo(i).getLengthData());

    }
    printf("The number of RESERVED EXTENSIONS contained in this file [%d]\n",
           (nitf::Uint32)header.getNumReservedExtensions());

    for (i = 0; i < (nitf::Uint32)header.getNumReservedExtensions(); i++)
    {
        printf("\tThe length of RESERVED EXTENSION subheader [%d]: %d bytes\n",
               i, (nitf::Uint32)header.getReservedExtensionInfo(i).getLengthSubheader());
        printf("\tThe length of the RESERVED EXTENSION data: %d bytes\n\n",
               (nitf::Uint32)header.getReservedExtensionInfo(i).getLengthData());

    }

    printf("The user-defined header length [%d]\n", (nitf::Uint32)header.getUserDefinedHeaderLength());

    printf("The extended header length [%d]\n", (nitf::Uint32)header.getExtendedHeaderLength());

}


int main(int argc, char** argv)
{
    try
    {
        if (argc != 2)
        {
            throw nitf::NITFException(Ctxt(FmtX("Usage: %s <nitf-file>\n", argv[0])));
        }

        nitf::Reader reader;
        nitf::IOHandle io(argv[1]);

        /*  Ironically, the one function you would think  */
        /*  needs a boolean return value returns void     */
        nitf::Record header = reader.read(io);
        nitf::Record twin = header.clone();

        PRINT_HDR( header.getHeader() );
        PRINT_HDR( twin.getHeader() );

        return 0;
    }
    catch (except::Throwable& t)
    {
        std::cout << t.getTrace() << std::endl;
    }
}
