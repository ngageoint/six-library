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

#define SHOW(X) std::cout << #X << " = [" << X << "]" << std::endl
#define SHOWI(X) printf("%s=[%d]\n", #X, (int)X)
#define SHOWRGB(X) printf("%s(R,G,B)=[%02x,%02x,%02x]\n", #X, \
       (unsigned char) X[0], (unsigned char) X[1], (unsigned char) X[2])


/*
 *  This function dumps a TRE using the TRE iterator (enumerator in C).
 *  The iterator is used to walk the fields of a TRE in order
 *  when the TRE enumerator is expired it will be set to NULL.
 *
 */

void printTRE(nitf::TRE tre)
{



    // This is so you know how long the TRE is
    nitf_Uint32 treLength = tre.getCurrentSize();

    /*
     *  This is the name for the description that was selected to field
     *  this TRE by the handler.
     */
    std::string treID = tre.getID();


    std::cout << std::endl << "--------------- " << tre.getTag()
              << " TRE (" << treLength << " - "
              << (treID.empty() ? "nullID" : treID)
              << ") ---------------" << std::endl;

    // Now walk the TRE
    for(nitf::TRE::Iterator it = tre.begin(); it != tre.end(); ++it)
    {
        std::string desc = it.getFieldDescription();
        nitf::Field field((*it).second());
        std::cout << (*it).first();
        if (!desc.empty())
            std::cout << " (" << desc << ")";
        std::cout << " = ["
                  << field.toString() << "]" << std::endl;
    }
    std::cout << "---------------------------------------------" << std::endl;

}

/*
 *  This function shows us the best way of walking through
 *  an extension segment (userDefined or extended)
 */
void showExtensions(nitf::Extensions extensions)
{
    for (nitf::ExtensionsIterator it = extensions.begin();
         it != extensions.end(); ++it)
    {
        nitf::TRE tre = *it;

        std::string treID = tre.getID();
        printTRE(tre);
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

    unsigned int num = header.getNumImages();
    std::cout << "The number of images contained in this file ["
              << num << "]" << std::endl;

    for (unsigned int i = 0; i < num; i++)
    {
        nitf::ComponentInfo info = header.getImageInfo(i);

        std::cout << "\tThe length of image subheader [" << i << "]: "
                  << (unsigned int)info.getLengthSubheader() << std::endl;
    }

    num = header.getNumGraphics();

    std::cout << "The number of graphics contained in this file ["
              << num << "]" << std::endl;

    for (unsigned int i = 0; i < num; i++)
    {
        nitf::ComponentInfo info = header.getGraphicInfo(i);

        std::cout << "\tThe length of graphics subheader [" << i << "]: "
                  << (unsigned int)info.getLengthSubheader() << std::endl;

        std::cout << "\tThe length of the graphics data: "
                  << (unsigned int)info.getLengthData()
                  << "bytes\n" << std::endl;
    }

    num = header.getNumLabels();

    std::cout << "The number of labels contained in this file ["
              << num << "]" << std::endl;

    for (unsigned int i = 0; i < num; i++)
    {
        nitf::ComponentInfo info = header.getLabelInfo(i);

        std::cout << "\tThe length of label subheader [" << i << "]: "
                  << (unsigned int)info.getLengthSubheader() << std::endl;

        std::cout << "\tThe length of the label data: "
                  << (unsigned int)info.getLengthData()
                  << "bytes\n" << std::endl;
    }

    num = header.getNumTexts();
    std::cout << "The number of texts contained in this file ["
              << num << "]" << std::endl;

    for (unsigned int i = 0; i < num; i++)
    {
        nitf::ComponentInfo info = header.getTextInfo(i);
        std::cout << "\tThe length of text subheader [" << i << "]: "
                  << (unsigned int)info.getLengthSubheader() << std::endl;

        std::cout << "\tThe length of the text data: "
                  << (unsigned int)info.getLengthData()
                  << "bytes\n" << std::endl;
    }

    num = header.getNumDataExtensions();
    std::cout << "The number of DES contained in this file ["
              << num << "]" << std::endl;

    for (unsigned int i = 0; i < num; i++)
    {
        nitf::ComponentInfo info = header.getDataExtensionInfo(i);

        std::cout << "\tThe length of DES subheader [" << i << "]: "
                  << (unsigned int)info.getLengthSubheader() << std::endl;

        std::cout << "\tThe length of the DES data: "
                  << (unsigned int)info.getLengthData()
                  << "bytes\n" << std::endl;
    }

    num = header.getNumReservedExtensions();
    std::cout << "The number of RES contained in this file ["
              << num << "]" << std::endl;

    for (unsigned int i = 0; i < num; i++)
    {
        nitf::ComponentInfo info = header.getReservedExtensionInfo(i);
        std::cout << "\tThe length of RES subheader [" << i << "]: "
                  << (unsigned int)info.getLengthSubheader() << std::endl;

        std::cout << "\tThe length of the RES data: "
                  << (unsigned int)info.getLengthData()
                  << "bytes\n" << std::endl;
    }

    SHOW(header.getUserDefinedHeaderLength().toString());

    nitf::Extensions udExts = header.getUserDefinedSection();

    showExtensions(udExts);

    SHOW(header.getExtendedHeaderLength().toString());

    nitf::Extensions exExts = header.getExtendedSection();
    showExtensions(exExts);

}


void showImageSubheader(nitf::ImageSubheader imsub)
{
    unsigned int i;
    std::cout << "image subheader" << std::endl;
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


/*
 *  This function dumps the security header.
 *
 */
void showSecurityGroup(nitf::FileSecurity securityGroup)
{
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

/*
 *  This section is for vector graphics.  Currently
 *  this will be CGM 1.0 (there is a spec for NITF CGM,
 *  but the original CGM 1.0 spec is out-of-print.
 *
 *  Note that this function does not dump the binary CGM
 *  You can use the NITRO CGM library to read the CGM data
 *  from the NITF (and dump it)
 */

void showGraphicSubheader(nitf::GraphicSubheader sub)
{

    std::cout << "graphic subheader" << std::endl;

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

}


void showLabelSubheader(nitf::LabelSubheader sub)
{
    printf("label subheader");

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

    nitf::Extensions exts = sub.getExtendedSection();
    showExtensions(exts);

}

/*
 *  This section contains raw text data.  You can put
 *  lots of stuff in here but most people never do.
 *
 *  Note that XML data is usually not contained in this section
 *  even though that might have made more sense.  XML data is
 *  typically found in the DES segment
 */
void showTextSubheader(nitf::TextSubheader sub)
{
    std::cout << "text subheader" << std::endl;
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


}

/*
 *  This section is for dumping the Data Extension Segment (DES)
 *  subheader.  It can hold up to 1GB worth of data, so its big
 *  enough for most things.  People stuff all kinds of things in
 *  the DESDATA block including
 *
 *  - TRE overflow:
 *      When a TRE is too big for the section its in.  In other words,
 *      if populating it properly would overflow the segment, it is
 *      dumped into the TRE overflow segment.
 *
 *      This is kind of a pain, and so NITRO 2.0 has functions to
 *      split this up for you, or merge it back into the header where it
 *      would go if it wasnt too big (see nitf_Record_mergeTREs() and
 *      nitf_Record_unmergeTREs).
 *
 *      However, by default, we assume that you want the data as it
 *      appeared in the file.  Therefore, when you dump a record, you
 *      might see overflow data
 *
 *  - Text data (especially XML)
 *
 *      XML data is getting more popular, and to make sure that they
 *      dont have to worry about the length of the XML surpassing the
 *      limits of a segment, most people decide to spec. it to go here
 *
 *  - Binary data
 *
 *      Since the DES is the wild west of the NITF, you can put anything
 *      you want here.
 *
 *  Confusingly, the DES subheader has its own little TRE-like
 *  arbitrary key-value params.  In NITRO we treat this as a TRE within
 *  the subheader.
 *
 *  This function prints the DE subheader, the extended TRE described above,
 *  and additionally, if the DESDATA is TRE overflow, we dump those too.
 */
void showDESubheader(nitf::DESubheader sub)
{
    printf("DES subheader\n");
    SHOW(sub.getFilePartType().toString());
    SHOW(sub.getTypeID().toString());
    SHOW(sub.getVersion().toString());
    SHOW(sub.getSecurityClass().toString());

    if (*(sub.getSecurityClass().getRawData()) != 'U')
        showSecurityGroup(sub.getSecurityGroup());

    SHOW(sub.getOverflowedHeaderType().toString());
    SHOW(sub.getDataItemOverflowed().toString());
    SHOW(sub.getSubheaderFieldsLength().toString());

    /*
     *  This is the user defined parameter section
     *  within the DES.  It contains only BCS-A/N type values
     *  so storing it in a 'TRE' struct is no big deal
     *
     *  It is only valid if the subheader fields length > 0; otherwise, it will
     *  throw an Exception since there is no TRE defining the subheader fields.
     */
    if (((nitf::Uint32)sub.getSubheaderFieldsLength()) > 0)
    {
        nitf::TRE tre = sub.getSubheaderFields();
        printTRE( tre );
    }

    SHOWI(sub.getDataLength());

    /*
     *  NITRO only populates this object if the DESDATA contains
     *  TRE overflow.  Otherwise, you need to use a DEReader to
     *  get at the DESDATA, since it can contain anything.
     *
     *  We wont bother to try and print whatever other things might
     *  have been put in here (e.g, text data or binary blobs)
     */
    nitf::Extensions exts = sub.getUserDefinedSection();
    showExtensions(exts);
}


/*
 *  This section is never really populated
 */
void showRESubheader(nitf::RESubheader sub)
{
    std::cout << "RES subheader\n" << std::endl;

    SHOW(sub.getFilePartType().toString());
    SHOW(sub.getTypeID().toString());
    SHOW(sub.getVersion().toString());
    SHOW(sub.getSecurityClass().toString());

    if (*(sub.getSecurityClass().getRawData()) != 'U')
        showSecurityGroup(sub.getSecurityGroup());

    SHOW(sub.getSubheaderFieldsLength().toString());

    SHOWI(sub.getDataLength());
}

int main(int argc, char **argv)
{

    try
    {
        //  This is the reader object
        nitf::Reader reader;

        if ( argc != 2 )
        {
            std::cout << "Usage: " << argv[0] << " <nitf-file>" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (nitf::Reader::getNITFVersion( argv[1] ) == NITF_VER_UNKNOWN)
        {
            std::cout << "This file does not appear to be a valid NITF"
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        /*
         *  Using an IO handle is one valid way to read a NITF in
         *
         *  NITRO 2.5 offers other ways, using the readIO() function
         *  in the Reader
         */
        nitf::IOHandle io(argv[1]);

        //  This parses all header data within the NITF
        nitf::Record record = reader.read(io);

        // Now show the header
        nitf::FileHeader fileHeader = record.getHeader();

        // Now show the header
        showFileHeader(fileHeader);

        // And now show the image information
        if (record.getNumImages())
        {
            nitf::List images = record.getImages();

            //  Walk each image and show
            for (nitf::ListIterator iter = images.begin();
                 iter != images.end(); ++iter)
            {
                nitf::ImageSegment segment = *iter;
                showImageSubheader(segment.getSubheader());
            }
        }
        else
        {
            std::cout << "No image in file!\n" << std::endl;
        }

        // Graphics
        if (record.getNumGraphics())
        {
            nitf::List graphics = record.getGraphics();

            //  Walk each graphic and show
            for (nitf::ListIterator iter = graphics.begin();
                 iter != graphics.end(); ++iter)
            {
                nitf::GraphicSegment segment = *iter;
                showGraphicSubheader(segment.getSubheader());
            }
        }
        else
        {
            std::cout << "No graphics in file" << std::endl;
        }

        // Labels
        if (record.getNumLabels())
        {
            nitf::List labels = record.getLabels();

            //  Walk each label and show
            for (nitf::ListIterator iter = labels.begin();
                 iter != labels.end(); ++iter)
            {
                nitf::LabelSegment segment = *iter;
                showLabelSubheader(segment.getSubheader());
            }
        }
        else
        {
            std::cout << "No label in file" << std::endl;
        }

        // Texts
        if (record.getNumTexts())
        {
            nitf::List texts = record.getTexts();

            //  Walk each label and show
            for (nitf::ListIterator iter = texts.begin();
                 iter != texts.end(); ++iter)
            {
                nitf::TextSegment segment = *iter;
                showTextSubheader(segment.getSubheader());
            }
        }
        else
        {
            std::cout << "No texts in file" << std::endl;
        }

        // Data Extensions
        if (record.getNumDataExtensions())
        {
            nitf::List des = record.getDataExtensions();

            //  Walk each label and show
            for (nitf::ListIterator iter = des.begin();
                 iter != des.end(); ++iter)
            {
                nitf::DESegment segment = *iter;
                showDESubheader(segment.getSubheader());
            }
        }
        else
        {
            std::cout << "No data extensions in file" << std::endl;
        }

        // Data Extensions
        if (record.getNumReservedExtensions())
        {
            nitf::List res = record.getReservedExtensions();

            //  Walk each label and show
            for (nitf::ListIterator iter = res.begin();
                 iter != res.end(); ++iter)
            {
                nitf::RESegment segment = *iter;
                showRESubheader(segment.getSubheader());
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
