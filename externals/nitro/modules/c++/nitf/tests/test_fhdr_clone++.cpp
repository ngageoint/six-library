/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#define SHOW(X) std::cout << #X << "=" << X.toString() << std::endl
#define SHOWI(X) std::cout << #X << "=" << static_cast<uint32_t>(X) << std::endl
#define PRINT_HDR(X) std::cout << #X; printHdr(X)

void printHdr(nitf::FileHeader header)
{
    uint32_t i;
    SHOW( header.getFileHeader() );
    SHOW( header.getFileVersion() );
    SHOW( header.getComplianceLevel() );
    SHOW( header.getSystemType() );
    SHOW( header.getOriginStationID() );
    SHOW( header.getFileDateTime() );
    SHOW( header.getFileTitle() );
    SHOW( header.getClassification() );
    SHOW( header.getMessageCopyNum() );
    SHOW( header.getMessageNumCopies() );
    SHOW( header.getEncrypted() );
    SHOW( header.getBackgroundColor() );
    SHOW( header.getOriginatorName() );
    SHOW( header.getOriginatorPhone() );
    SHOWI( header.getFileLength() );
    SHOWI( header.getHeaderLength() );

    printf("The number of IMAGES contained in this file [%d]\n",
           (int)header.getNumImages());

    for (i = 0; i < (uint32_t)header.getNumImages(); i++)
    {
        printf("\tThe length of IMAGE subheader [%d]: %d bytes\n",
               i, (uint32_t)header.getImageInfo(i).getLengthSubheader());
        printf("\tThe length of the IMAGE data: %d bytes\n",
               (uint32_t)header.getImageInfo(i).getLengthData());
    }

    printf("The number of GRAPHICS contained in this file [%d]\n",
           (uint32_t)header.getNumGraphics());

    for (i = 0; i < (uint32_t)header.getNumGraphics(); i++)
    {
        printf("\tThe length of GRAPHICS subheader [%d]: %d bytes\n",
               i, (uint32_t)header.getGraphicInfo(i).getLengthSubheader());
        printf("\tThe length of the GRAPHICS data: %d bytes\n\n",
               (uint32_t)header.getGraphicInfo(i).getLengthData());
    }
    printf("The number of LABELS contained in this file [%d]\n",
           (uint32_t)header.getNumLabels());

    assert((uint32_t)header.getNumLabels() == 0);

    printf("The number of TEXTS contained in this file [%d]\n",
           (uint32_t)header.getNumTexts());

    for (i = 0; i < (uint32_t)header.getNumTexts(); i++)
    {
        printf("\tThe length of TEXT subheader [%d]: %d bytes\n",
               i, (int)header.getTextInfo(i).getLengthSubheader());
        printf("\tThe length of the TEXT data: %d bytes\n\n",
               (int)header.getTextInfo(i).getLengthData());
    }
    printf("The number of DATA EXTENSIONS contained in this file [%d]\n",
           (uint32_t)header.getNumDataExtensions());

    for (i = 0; i < (uint32_t)header.getNumDataExtensions(); i++)
    {
        printf("\tThe length of DATA EXTENSION subheader [%d]: %d bytes\n",
               i, (uint32_t)header.getDataExtensionInfo(i).getLengthSubheader());
        printf("\tThe length of the DATA EXTENSION data: %d bytes\n\n",
               (uint32_t)header.getDataExtensionInfo(i).getLengthData());

    }
    printf("The number of RESERVED EXTENSIONS contained in this file [%d]\n",
           (uint32_t)header.getNumReservedExtensions());

    for (i = 0; i < (uint32_t)header.getNumReservedExtensions(); i++)
    {
        printf("\tThe length of RESERVED EXTENSION subheader [%d]: %d bytes\n",
               i, (uint32_t)header.getReservedExtensionInfo(i).getLengthSubheader());
        printf("\tThe length of the RESERVED EXTENSION data: %d bytes\n\n",
               (uint32_t)header.getReservedExtensionInfo(i).getLengthData());

    }

    printf("The user-defined header length [%d]\n", (uint32_t)header.getUserDefinedHeaderLength());

    printf("The extended header length [%d]\n", (uint32_t)header.getExtendedHeaderLength());

}


int main(int argc, char** argv)
{
    try
    {
        if (argc != 2)
        {
            throw nitf::NITFException(Ctxt(str::Format("Usage: %s <nitf-file>\n", argv[0])));
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
    catch (const except::Throwable& t)
    {
        std::cout << t.getTrace() << std::endl;
    }
}
