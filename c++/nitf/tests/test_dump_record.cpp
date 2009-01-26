/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#define SHOW(X) std::cout << #X << " = [" << X << "]" << std::endl
#define SHOWI(X) printf("%s=[%ld]\n", #X, X)
#define SHOWRGB(X) printf("%s(R,G,B)=[%02x,%02x,%02x]\n", #X, (unsigned char) X[0], (unsigned char) X[1], (unsigned char) X[2])


void printTRE(nitf::TRE tre)
{
    for(nitf::TRE::Iterator it = tre.begin(); it != tre.end(); ++it)
    {
        nitf::Field field((*it).second());
        std::cout << (*it).first() << " = [" << field.toString() << "]" << std::endl;
    }
}


void showExtensions(nitf::Extensions extensions)
{
    //TREPrintFunctor treTraverser;
    for (nitf::ExtensionsIterator it = extensions.begin();
            it != extensions.end(); ++it)
    {
        nitf::TRE tre = *it;
        nitf_Uint32 treLength = tre.getCurrentSize();
        std::string treID = tre.getID();
//        if (treLength <= 0)
//            treLength = tre.computeLength();
        std::cout << std::endl << "--------------- " << tre.getTag()
            << " TRE (" << treLength << " - "
            << (treID.empty() ? "nullID" : treID)
            << ") ---------------" << std::endl;
        //        for(nitf::TRE::FieldIterator it = tre.begin(); it != tre.end(); ++it)
        //            std::cout << it->first << " = [" << it->second.toString() << "]" << std::endl;
        printTRE(tre);
        //tre.foreach(treTraverser);
        std::cout << "---------------------------------------------" << std::endl;
    }
}


void showFileHeader(nitf::FileHeader header)
{
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
    SHOW( header.getFileLength().toString() );
    SHOW( header.getHeaderLength().toString() );

    std::cout << "The number of IMAGES contained in this file ["
    << (unsigned int)header.getNumImages() << "]" << std::endl;

    for (unsigned int i = 0; i < (unsigned int)header.getNumImages(); i++)
    {
        std::cout << "\tThe length of IMAGE subheader ["
        << i
        << "]: "
        << (unsigned int)header.getImageInfo(i).getLengthSubheader()
        << std::endl;
    }

    std::cout << "The number of GRAPHICS contained in this file ["
    << (unsigned int)header.getNumGraphics() << "]" << std::endl;

    for (unsigned int i = 0; i < (unsigned int)header.getNumGraphics(); i++)
    {
        std::cout << "\tThe length of GRAPHIC subheader ["
        << i << "]: "
        << (unsigned int)header.getGraphicInfo(i).getLengthSubheader()
        << std::endl;

        std::cout << "\tThe length of the GRAPHIC data: "
        << (unsigned int)header.getGraphicInfo(i).getLengthData()
        << "bytes\n" << std::endl;
    }

    std::cout << "The number of LABELS contained in this file ["
    << (unsigned int)header.getNumLabels() << "]" << std::endl;

    for (unsigned int i = 0; i < (unsigned int)header.getNumLabels(); i++)
    {
        std::cout << "\tThe length of LABEL subheader ["
        << i << "]: "
        << (unsigned int)header.getLabelInfo(i).getLengthSubheader()
        << std::endl;

        std::cout << "\tThe length of the LABEL data: "
        << (unsigned int)header.getLabelInfo(i).getLengthData()
        << "bytes\n" << std::endl;
    }

    std::cout << "The number of TEXTS contained in this file ["
    << (unsigned int)header.getNumTexts() << "]" << std::endl;

    for (unsigned int i = 0; i < (unsigned int)header.getNumTexts(); i++)
    {
        std::cout << "\tThe length of TEXT subheader ["
        << i << "]: "
        << (unsigned int)header.getTextInfo(i).getLengthSubheader()
        << std::endl;

        std::cout << "\tThe length of the TEXT data: "
        << (unsigned int)header.getTextInfo(i).getLengthData()
        << "bytes\n" << std::endl;
    }

    std::cout << "The number of DATA EXTENSIONS contained in this file ["
    << (unsigned int)header.getNumDataExtensions() << "]" << std::endl;

    for (unsigned int i = 0; i < (unsigned int)header.getNumDataExtensions(); i++)
    {
        std::cout << "\tThe length of DATA EXTENSION subheader ["
        << i << "]: "
        << (unsigned int)header.getDataExtensionInfo(i).getLengthSubheader()
        << std::endl;

        std::cout << "\tThe length of the DATA EXTENSION data: "
        << (unsigned int)header.getDataExtensionInfo(i).getLengthData()
        << "bytes\n" << std::endl;
    }

    std::cout << "The number of RESERVED EXTENSIONS contained in this file ["
    << (unsigned int)header.getNumReservedExtensions() << "]" << std::endl;

    for (unsigned int i = 0; i < (unsigned int)header.getNumReservedExtensions(); i++)
    {
        std::cout << "\tThe length of RESERVED EXTENSION subheader ["
        << i << "]: "
        << (unsigned int)header.getReservedExtensionInfo(i).getLengthSubheader()
        << std::endl;

        std::cout << "\tThe length of the RESERVED EXTENSION data: "
        << (unsigned int)header.getReservedExtensionInfo(i).getLengthData()
        << "bytes\n" << std::endl;
    }

    //the hash functor
    //    ShowTREFunctor showTRE;
    SHOW(header.getUserDefinedHeaderLength().toString());

    nitf::Extensions udExts = header.getUserDefinedSection();
    showExtensions(udExts);
    // nitf::HashTable htUd = udExts.getHash();
    //    htUd.foreach(showTRE);

    SHOW(header.getExtendedHeaderLength().toString());

    nitf::Extensions exExts = header.getExtendedSection();
    showExtensions(exExts);
    // nitf::HashTable htEx = exExts.getHash();
    //    htEx.foreach(showTRE);


}


void showImageSubheader(nitf::ImageSubheader imsub)
{
    unsigned int i;
    std::cout << "Read image into imsub" << std::endl;
    SHOW( imsub.getFilePartType().toString() );
    SHOW( imsub.getImageId().toString() );
    SHOW( imsub.getImageDateAndTime().toString() );
    SHOW( imsub.getTargetId().toString() );
    SHOW( imsub.getImageTitle().toString() );
    SHOW( imsub.getImageSecurityClass().toString() );
    SHOW( imsub.getEncrypted().toString() );
    SHOW( imsub.getImageSource().toString() );
    SHOW( imsub.getNumRows().toString() );
    SHOW( imsub.getNumCols().toString() );
    SHOW( imsub.getPixelValueType().toString() );
    SHOW( imsub.getImageRepresentation().toString() );
    SHOW( imsub.getImageCategory().toString() );
    SHOW( imsub.getActualBitsPerPixel().toString() );
    SHOW( imsub.getPixelJustification().toString() );
    SHOW( imsub.getImageCoordinateSystem().toString() );
    SHOW( imsub.getCornerCoordinates().toString() );

    SHOW( (nitf::Uint32)imsub.getNumImageComments() );
    nitf::List comments = imsub.getImageComments();
    for (nitf::ListIterator it = comments.begin(); it != comments.end(); ++it)
        SHOW(((nitf::Field)*it).toString());

    SHOW( imsub.getImageCompression().toString() );
    SHOW( imsub.getCompressionRate().toString() );

    SHOW( imsub.getNumImageBands().toString() );
    SHOW( (nitf::Uint32)imsub.getNumMultispectralImageBands() );

    for (i = 0; i < (unsigned int)imsub.getNumImageBands(); i++)
    {
        SHOW( imsub.getBandInfo(i).getRepresentation().toString());
        SHOW( imsub.getBandInfo(i).getSubcategory().toString() );
        SHOW( imsub.getBandInfo(i).getImageFilterCondition().toString() );
        SHOW( imsub.getBandInfo(i).getImageFilterCode().toString() );
        SHOW( (nitf::Uint32)imsub.getBandInfo(i).getNumLUTs() );
        SHOW( (nitf::Uint32)imsub.getBandInfo(i).getBandEntriesPerLUT() );
    }

    SHOW( imsub.getImageSyncCode().toString() );
    SHOW( imsub.getImageMode().toString() );
    SHOW( (nitf::Uint32)imsub.getNumBlocksPerRow() );
    SHOW( (nitf::Uint32)imsub.getNumBlocksPerCol() );
    SHOW( (nitf::Uint32)imsub.getNumPixelsPerHorizBlock() );
    SHOW( (nitf::Uint32)imsub.getNumPixelsPerVertBlock() );
    SHOW( (nitf::Uint32)imsub.getNumBitsPerPixel() );
    SHOW( imsub.getImageDisplayLevel().toString() );
    SHOW( imsub.getImageAttachmentLevel().toString() );
    SHOW( imsub.getImageLocation().toString() );
    SHOW( imsub.getImageMagnification().toString() );
    SHOW( imsub.getUserDefinedImageDataLength().toString() );
    SHOW( imsub.getUserDefinedOverflow().toString() );

    //the hash functor
    //    ShowTREFunctor showTRE;
    nitf::Extensions udExts = imsub.getUserDefinedSection();
    showExtensions(udExts);
    // nitf::HashTable htUd = udExts.getHash();
    //    htUd.foreach(showTRE);

    SHOW( (nitf::Uint32)imsub.getExtendedHeaderLength() );
    SHOW( imsub.getExtendedHeaderOverflow().toString() );

    nitf::Extensions exExts = imsub.getExtendedSection();
    showExtensions(exExts);
    // nitf::HashTable htEx = exExts.getHash();
    //    htEx.foreach(showTRE);

}


void showSecurityGroup(nitf::FileSecurity securityGroup)
{
    /*  Attention: If the classification is U, the security group  */
    /*  section should be empty!  (boring!)                        */

    SHOW(securityGroup.getClassificationSystem().toString());
    SHOW(securityGroup.getCodewords().toString());
    SHOW(securityGroup.getControlAndHandling().toString());
    SHOW(securityGroup.getReleasingInstructions().toString());
    SHOW(securityGroup.getDeclassificationType().toString());
    SHOW(securityGroup.getDeclassificationDate().toString());
    SHOW(securityGroup.getDeclassificationExemption().toString());
    SHOW(securityGroup.getDowngrade().toString());
    SHOW(securityGroup.getDowngradeDateTime().toString());
    SHOW(securityGroup.getClassificationText().toString());
    SHOW(securityGroup.getClassificationAuthorityType().toString());
    SHOW(securityGroup.getClassificationAuthority().toString());
    SHOW(securityGroup.getClassificationReason().toString());
    SHOW(securityGroup.getSecuritySourceDate().toString());
    SHOW(securityGroup.getSecurityControlNumber().toString());
}

void showGraphicSubheader(nitf::GraphicSubheader sub)
{

    std::cout << "Read graphic into sub\n" << std::endl;

    SHOW(sub.getFilePartType().toString());
    SHOW(sub.getGraphicID().toString());
    SHOW(sub.getName().toString());
    SHOW(sub.getSecurityClass().toString());

    if (*(sub.getSecurityClass().getRawData()) != 'U')
        showSecurityGroup(sub.getSecurityGroup());

    SHOW(sub.getEncrypted().toString());
    SHOW(sub.getStype().toString());
    SHOW(sub.getRes1().toString());
    SHOW(sub.getDisplayLevel().toString());
    SHOW(sub.getAttachmentLevel().toString());
    SHOW(sub.getLocation().toString());
    SHOW(sub.getBound1Loc().toString());
    SHOW(sub.getColor().toString());
    SHOW(sub.getBound2Loc().toString());
    SHOW(sub.getRes2().toString());
    SHOW(sub.getExtendedHeaderLength().toString());
    SHOW(sub.getExtendedHeaderOverflow().toString());

    nitf::Extensions exts = sub.getExtendedSection();
    showExtensions(exts);
    //the hash functor
    //    ShowTREFunctor showTRE;
    // nitf::HashTable ht = exts.getHash();
    //    ht.foreach(showTRE);

}


void showLabelSubheader(nitf::LabelSubheader sub)
{

    std::cout << "Read label into sub\n" << std::endl;
    SHOW(sub.getFilePartType().toString());
    SHOW(sub.getLabelID().toString());
    SHOW(sub.getSecurityClass().toString());

    if (*(sub.getSecurityClass().getRawData()) != 'U')
        showSecurityGroup(sub.getSecurityGroup());

    SHOW(sub.getEncrypted().toString());
    SHOW(sub.getFontStyle().toString());
    SHOW(sub.getCellWidth().toString());
    SHOW(sub.getCellHeight().toString());
    SHOW(sub.getDisplayLevel().toString());
    SHOW(sub.getAttachmentLevel().toString());
    SHOW(sub.getLocationRow().toString());
    SHOW(sub.getLocationColumn().toString());
    SHOWRGB(sub.getTextColor().getRawData());
    SHOWRGB(sub.getBackgroundColor().getRawData());
    SHOW(sub.getExtendedHeaderLength().toString());
    SHOW(sub.getExtendedHeaderOverflow().toString());

    //the hash functor
    //    ShowTREFunctor showTRE;
    nitf::Extensions exts = sub.getExtendedSection();
    showExtensions(exts);
    // nitf::HashTable ht = exts.getHash();
    // ht.foreach(showTRE);

}

void showTextSubheader(nitf::TextSubheader sub)
{
    std::cout << "Read text into sub\n" << std::endl;
    SHOW(sub.getFilePartType().toString());
    SHOW(sub.getTextID().toString());
    SHOW(sub.getAttachmentLevel().toString());
    SHOW(sub.getDateTime().toString());
    SHOW(sub.getTitle().toString());
    SHOW(sub.getSecurityClass().toString());

    if (*(sub.getSecurityClass().getRawData()) != 'U')
        showSecurityGroup(sub.getSecurityGroup());

    SHOW(sub.getEncrypted().toString());
    SHOW(sub.getFormat().toString());
    SHOW(sub.getExtendedHeaderLength().toString());
    SHOW(sub.getExtendedHeaderOverflow().toString());

    nitf::Extensions exts = sub.getExtendedSection();
    showExtensions(exts);

    //the hash functor
    //    ShowTREFunctor showTRE;
    // nitf::HashTable ht = exts.getHash();
    // ht.foreach(showTRE);

}

void showDESubheader(nitf::DESubheader sub)
{

    std::cout << "Read DE into sub\n" << std::endl;
    SHOW(sub.getFilePartType().toString());
    SHOW(sub.getTypeID().toString());
    SHOW(sub.getVersion().toString());
    SHOW(sub.getSecurityClass().toString());

    if (*(sub.getSecurityClass().getRawData()) != 'U')
        showSecurityGroup(sub.getSecurityGroup());

    SHOW(sub.getOverflowedHeaderType().toString());
    SHOW(sub.getDataItemOverflowed().toString());
    SHOW(sub.getSubheaderFieldsLength().toString());

    //TREPrintFunctor treTraverser;
    //sub.getSubheaderFields().foreach(treTraverser);

    SHOWI(sub.getDataLength());

    nitf::Extensions exts = sub.getUserDefinedSection();
    showExtensions(exts);
    // nitf::HashTable ht = exts.getHash();
    //the hash functor
    //    ShowTREFunctor showTRE;
    // ht.foreach(showTRE);
}


/* No way to test this anyway
 */
void showRESubheader(nitf::RESubheader sub)
{
    nitf_Error error;

    std::cout << "Read RE into sub\n" << std::endl;

    SHOW(sub.getFilePartType().toString());
    SHOW(sub.getTypeID().toString());
    SHOW(sub.getVersion().toString());
    SHOW(sub.getSecurityClass().toString());

    if (*(sub.getSecurityClass().getRawData()) != 'U')
        showSecurityGroup(sub.getSecurityGroup());

    SHOW(sub.getSubheaderFieldsLength().toString());

    // ?! this returns the raw info, so we can leave it as it is in the C
    //nitf_TRE_print((nitf_TRE*)sub.getSubheaderFields(), &error);
    SHOWI(sub.getDataLength());
}

int main(int argc, char **argv)
{

    try
    {
        /*  This is the reader object  */
        nitf::Reader reader;

        /*  Check argv and make sure we are happy  */
        if ( argc != 2 )
        {
            throw nitf::NITFException(Ctxt(FmtX("Usage: %s <nitf-file>\n", argv[0])));
        }

        nitf::IOHandle io(argv[1]);

        nitf::Record record = reader.read(io);
        //return 0;
        // Now show the header
        nitf::FileHeader fileHeader = record.getHeader();

        // warn if not version 2.00 or 2.10
        std::string version = fileHeader.getFileVersion().toString();

        if (strcmp(version.c_str(), "02.10") != 0 &&
                strcmp(version.c_str(), "02.00") != 0 &&
                strncmp(fileHeader.getFileHeader().getRawData(), "NSIF", 4) != 0)
        {
            std::cout << "!!! unhandled NITF version: " << version << std::endl;
        }

        // May have coredumped already
        // Fileheader
        showFileHeader(fileHeader);

        // Images
        if ((unsigned int)fileHeader.getNumImages())
        {
            nitf::ListIterator iter = record.getImages().begin();
            nitf::ListIterator end = record.getImages().end();

            while (iter != end)
            {
                printf("Image segment\n");
                nitf::ImageSegment segment = *iter;
                printf("Show image subheader\n");
                showImageSubheader(segment.getSubheader());
                iter++;
            }
        }
        else
        {
            std::cout << "No image in file!\n" << std::endl;
        }

        // Graphics
        if ((unsigned int)fileHeader.getNumGraphics())
        {
            /*  Walk each graphic and show  */
            nitf::ListIterator iter = record.getGraphics().begin();
            nitf::ListIterator end = record.getGraphics().end();

            while (iter != end)
            {
                printf("Graphic segment\n");
                nitf::GraphicSegment segment = *iter;
                printf("Show graphic subheader\n");
                showGraphicSubheader(segment.getSubheader());
                iter++;
            }
        }
        else
        {
            std::cout << "No graphics in file" << std::endl;
        }

        // Labels
        if ((unsigned int)fileHeader.getNumLabels())
        {
            //  Walk each label and show
            nitf::ListIterator iter = record.getLabels().begin();
            nitf::ListIterator end = record.getLabels().end();

            while (iter != end)
            {
                printf("Label segment\n");
                nitf::LabelSegment segment = *iter;
                printf("Show label subheader\n");
                showLabelSubheader(segment.getSubheader());
                iter++;
            }
        }
        else
        {
            std::cout << "No label in file" << std::endl;
        }

        // Texts
        if ((unsigned int)fileHeader.getNumTexts())
        {
            //  Walk each text and show
            nitf::ListIterator iter = record.getTexts().begin();
            nitf::ListIterator end = record.getTexts().end();

            while (iter != end)
            {
                printf("Text segment\n");
                nitf::TextSegment segment = *iter;
                printf("Show text subheader\n");
                showTextSubheader(segment.getSubheader());
                iter++;
            }
        }
        else
        {
            std::cout << "No texts in file" << std::endl;
        }

        // Data Extensions
        if ((unsigned int)fileHeader.getNumDataExtensions())
        {
            //  Walk each dataExtension and show
            nitf::ListIterator iter = record.getDataExtensions().begin();
            nitf::ListIterator end = record.getDataExtensions().end();

            while (iter != end)
            {
                printf("Data Extensions segment\n");
                nitf::DESegment segment = *iter;
                printf("Show data extensions subheader\n");
                showDESubheader(segment.getSubheader());
                iter++;
            }
        }
        else
        {
            std::cout << "No data extensions in file: " << (unsigned int)fileHeader.getNumDataExtensions() << std::endl;
        }

        // Reserved Extensions
        if ((unsigned int)fileHeader.getNumReservedExtensions())
        {
            //  Walk each reservedextension and show
            nitf::ListIterator iter = record.getReservedExtensions().begin();
            nitf::ListIterator end = record.getReservedExtensions().end();

            while (iter != end)
            {
                printf("Reserved Extensions segment\n");
                nitf::RESegment segment = *iter;
                printf("Show reserved extensions subheader\n");
                showRESubheader(segment.getSubheader());
                iter++;
            }
        }
        else
        {
            std::cout << "No reserved extensions in file" << std::endl;
        }

        io.close();

        // Warnings
        nitf::List warnings = reader.getWarningList();
        if (!warnings.isEmpty())
        {
            //  Iterator to a list
            nitf::ListIterator iter = warnings.begin();

            //  Iterator to the end of list
            nitf::ListIterator end = warnings.end();

            std::cout << "WARNINGS: ";
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

            //  While we are not at the end
            while (iter != end)
            {
                //char *p = (char *) nitf_ListIterator_get(&it);
                char *p = (char *) * iter;

                //  Make sure
                assert(p != NULL);

                //  Show the data
                std::cout << "\tFound problem: [" << p << "]\n" << std::endl;

                ++iter;
            }

            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        }
        return 0;
    }
    catch (except::Throwable& t)
    {
        std::cout << t.getTrace() << std::endl;
    }
}
