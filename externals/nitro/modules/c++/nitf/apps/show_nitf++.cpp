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

#include <sstream>
#include <iostream>

#include <import/nitf.hpp>
#include <io/FileInputStream.h>

bool format_as_xml = false;

static size_t count_nul(const std::string& s)
{
    // Sometimes the field contains a bunch of NUL characters, which isn't valid in XML.
    size_t nul_count = 0;
    constexpr char nul = '\0';
    for (const char& ch : s)
    {
        if (ch == nul)
        {
            nul_count++;
        }
    }
    return nul_count;
}

template<typename T>
inline void show(const std::string& x_, const T& x)
{
    std::stringstream ss;
    ss << x;
    const std::string str_x = ss.str();

    if (format_as_xml)
    {
        std::cout << "\t";
        std::cout << "<" << x_;

        // Sometimes the field contains a bunch of NUL characters, which isn't valid in XML.
        const size_t nul_count = count_nul(str_x);
        if (nul_count == str_x.size())
        {
            std::cout << " length=\"" << nul_count << "\" />";
        }
        else
        {
            std::cout << ">";
            std::cout << str_x;
            std::cout << "</" << x_ << ">";
        }
    }
    else
    {
        std::cout << x_ << " = [" << str_x << "]";
    }
    std::cout << "\n";
}
#define SHOW(X) show(#X, X)
//#define SHOW(X) std::cout << #X << " = [" << X << "]\n"

#define SHOWI(X) show(#X, (int)X)
//#define SHOWI(X) std::cout << #X << "=[" << (int)X << "]\n"
#define SHOWRGB(X) printf("%s(R,G,B)=[%02x,%02x,%02x]\n", #X, \
       (unsigned char) X[0], (unsigned char) X[1], (unsigned char) X[2])


/*
 *  This function dumps a TRE using the TRE iterator (enumerator in C).
 *  The iterator is used to walk the fields of a TRE in order
 *  when the TRE enumerator is expired it will be set to NULL.
 *
 */

static void printTREField(const nitf::Field& field)
{
    const std::string strField = field.toString();

    if (!format_as_xml)
    {
        std::cout << " = [" << strField << "]\n";
        return;
    }

    // Sometimes the field contains a bunch of NUL characters, which isn't valid in XML.
    const size_t nul_count = count_nul(strField);
    if (nul_count == strField.size())
    {
        std::cout << " length=\"";
        std::cout << nul_count;
        std::cout << "\"";
    }
    else
    {
        std::cout << " value=\"";
        std::cout << strField;
        std::cout << "\"";
    }
    
    std::cout << " />";
    std::cout << "\n";
}

void printTRE(const nitf::TRE& tre)
{
    // This is so you know how long the TRE is
    const auto treLength = tre.getCurrentSize();

    /*
     *  This is the name for the description that was selected to field
     *  this TRE by the handler.
     */
    const std::string treID = tre.getID();
    const std::string str_treID = (treID.empty() ? "nullID" : treID);

    if (!format_as_xml)
    {
        std::cout << "\n--------------- " << tre.getTag()
            << " TRE (" << treLength << " - "
            << str_treID
            << ") ---------------\n";
    }
    else
    {
        std::cout << "\t";
        std::cout << "<TRE tag=\"" << tre.getTag() << "\"";
        std::cout << " length=\"" << treLength << "\"";
        std::cout << " ID=\"" << str_treID << "\"";
        std::cout << ">";
        std::cout << "\n";
    }

    // Now walk the TRE
    for(nitf::TRE::Iterator it = tre.begin(); it != tre.end(); ++it)
    {
        const std::string desc = it.getFieldDescription();
        const nitf::Field field((*it).second());

        if (format_as_xml)
        {
            std::cout << "\t\t";
            std::cout << "<field name=\"";
        }
        std::cout << (*it).first();
        if (format_as_xml)
        {
            std::cout << "\"";
        }

        const nitf::Field::FieldType fieldType = field.getType();
        if (format_as_xml)
        {
            std::string strType;
            switch (fieldType)
            {
            case  nitf::Field::BCS_A: strType = "BCS_A"; break;
            case  nitf::Field::BCS_N: strType = "BCS_N"; break;
            case  nitf::Field::BINARY: strType = "BINARY"; break;
            default: strType = "Unknown"; break;
            }
            std::cout << " type=\"" << strType << "\"";
        }

        if (!desc.empty())
        {
            if (!format_as_xml)
            {
                std::cout << " (" << desc << ")";
            }
            else
            {
                std::cout << " desc=\"" << desc << "\"";
            }
        }

        printTREField(field);
    }

    if (!format_as_xml)
    {
        std::cout << "---------------------------------------------";
    }
    else
    {
        std::cout << "\t";
        std::cout << "</TRE>";
    }
    std::cout << "\n";

}

/*
 *  This function shows us the best way of walking through
 *  an extension segment (userDefined or extended)
 */
void showExtensions(const nitf::Extensions& extensions)
{
    if (format_as_xml)
    {
        std::cout << "\t";
        std::cout << "<TREs>";
        std::cout << "\n";
    }

    for (nitf::ExtensionsIterator it = extensions.begin();
         it != extensions.end(); ++it)
    {
        nitf::TRE tre = *it;

        std::string treID = tre.getID();
        printTRE(tre);
    }

    if (format_as_xml)
    {
        std::cout << "\t";
        std::cout << "</TREs>";
        std::cout << "\n";
    }
}


void showFileHeader(const nitf::FileHeader& header)
{
    if (format_as_xml)
    {
        std::cout << "<FileHeader>\n";
    }
    //#define SHOW_FILE_HEADER(X) SHOW(header.get ## X(). toString())
    #define SHOW_FILE_HEADER(X) show(#X, header.get ## X(). toString())
    SHOW_FILE_HEADER(FileHeader);
    SHOW_FILE_HEADER(FileVersion);
    SHOW_FILE_HEADER(ComplianceLevel);
    SHOW_FILE_HEADER(SystemType);
    SHOW_FILE_HEADER(OriginStationID);
    SHOW_FILE_HEADER(FileDateTime);
    SHOW_FILE_HEADER(FileTitle);
    SHOW_FILE_HEADER(Classification);
    SHOW_FILE_HEADER(MessageCopyNum);
    SHOW_FILE_HEADER(MessageNumCopies);
    SHOW_FILE_HEADER(Encrypted);
    SHOW_FILE_HEADER(BackgroundColor);
    SHOW_FILE_HEADER(OriginatorName);
    SHOW_FILE_HEADER(OriginatorPhone);
    SHOW_FILE_HEADER(FileLength);
    SHOW_FILE_HEADER(HeaderLength);

    if (!format_as_xml) // don't need XML output right now
    {
        int num = header.getNumImages();
        std::cout << "The number of images contained in this file ["
            << num << "]" << std::endl;

        for (int i = 0; i < num; i++)
        {
            nitf::ComponentInfo info = header.getImageInfo(i);

            std::cout << "\tThe length of image subheader [" << i << "]: "
                << (unsigned int)info.getLengthSubheader() << std::endl;
        }

        num = header.getNumGraphics();
        std::cout << "The number of graphics contained in this file ["
            << num << "]" << std::endl;

        for (int i = 0; i < num; i++)
        {
            nitf::ComponentInfo info = header.getGraphicInfo(i);

            std::cout << "\tThe length of graphics subheader [" << i << "]: "
                << (unsigned int)info.getLengthSubheader() << std::endl;

            std::cout << "\tThe length of the graphics data: "
                << (unsigned int)info.getLengthData()
                << " bytes\n" << std::endl;
        }

        num = header.getNumLabels();
        std::cout << "The number of labels contained in this file ["
            << num << "]" << std::endl;

        for (int i = 0; i < num; i++)
        {
            nitf::ComponentInfo info = header.getLabelInfo(i);

            std::cout << "\tThe length of label subheader [" << i << "]: "
                << (unsigned int)info.getLengthSubheader() << std::endl;

            std::cout << "\tThe length of the label data: "
                << (unsigned int)info.getLengthData()
                << " bytes\n" << std::endl;
        }

        num = header.getNumTexts();
        std::cout << "The number of texts contained in this file ["
            << num << "]" << std::endl;

        for (int i = 0; i < num; i++)
        {
            nitf::ComponentInfo info = header.getTextInfo(i);
            std::cout << "\tThe length of text subheader [" << i << "]: "
                << (unsigned int)info.getLengthSubheader() << std::endl;

            std::cout << "\tThe length of the text data: "
                << (unsigned int)info.getLengthData()
                << " bytes\n" << std::endl;
        }

        num = header.getNumDataExtensions();
        std::cout << "The number of DES contained in this file ["
            << num << "]" << std::endl;

        for (int i = 0; i < num; i++)
        {
            nitf::ComponentInfo info = header.getDataExtensionInfo(i);

            std::cout << "\tThe length of DES subheader [" << i << "]: "
                << (unsigned int)info.getLengthSubheader() << std::endl;

            std::cout << "\tThe length of the DES data: "
                << (unsigned int)info.getLengthData()
                << " bytes\n" << std::endl;
        }

        num = header.getNumReservedExtensions();
        std::cout << "The number of RES contained in this file ["
            << num << "]" << std::endl;

        for (int i = 0; i < num; i++)
        {
            nitf::ComponentInfo info = header.getReservedExtensionInfo(i);
            std::cout << "\tThe length of RES subheader [" << i << "]: "
                << (unsigned int)info.getLengthSubheader() << std::endl;

            std::cout << "\tThe length of the RES data: "
                << (unsigned int)info.getLengthData()
                << " bytes\n" << std::endl;
        }
    }

    SHOW_FILE_HEADER(UserDefinedHeaderLength);
    const nitf::Extensions udExts = header.getUserDefinedSection();
    if (format_as_xml)
    {
        std::cout << "\t<UserDefinedHeader>\n";
    }
    showExtensions(udExts);
    if (format_as_xml)
    {
        std::cout << "\t</UserDefinedHeader>\n";
    }

    SHOW_FILE_HEADER(ExtendedHeaderLength);
    const nitf::Extensions exExts = header.getExtendedSection();
    if (format_as_xml)
    {
        std::cout << "\t<ExtendedHeader>\n";
    }
    showExtensions(udExts);
    if (format_as_xml)
    {
        std::cout << "\t</ExtendedHeader>\n";
    }

    if (format_as_xml)
    {
        std::cout << "</FileHeader>\n";
    }
}


void showImageSubheader(const nitf::ImageSubheader& imsub)
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

    SHOW( (uint32_t)imsub.getNumImageComments() );
    const nitf::List comments = imsub.getImageComments();
    for (nitf::ListIterator it = comments.begin(); it != comments.end(); ++it)
        SHOW(((nitf::Field)*it).toString());

    SHOW( imsub.getImageCompression().toString() );
    SHOW( imsub.getCompressionRate().toString() );

    SHOW( imsub.getNumImageBands().toString() );
    SHOW( (uint32_t)imsub.getNumMultispectralImageBands() );

    for (i = 0; i < (unsigned int)imsub.getNumImageBands(); i++)
    {
        SHOW( imsub.getBandInfo(i).getRepresentation().toString());
        SHOW( imsub.getBandInfo(i).getSubcategory().toString() );
        SHOW( imsub.getBandInfo(i).getImageFilterCondition().toString() );
        SHOW( imsub.getBandInfo(i).getImageFilterCode().toString() );
        SHOW( (uint32_t)imsub.getBandInfo(i).getNumLUTs() );
        SHOW( (uint32_t)imsub.getBandInfo(i).getBandEntriesPerLUT() );
    }

    SHOW( imsub.getImageSyncCode().toString() );
    SHOW( imsub.getImageMode().toString() );
    SHOW( (uint32_t)imsub.getNumBlocksPerRow() );
    SHOW( (uint32_t)imsub.getNumBlocksPerCol() );
    SHOW( (uint32_t)imsub.getNumPixelsPerHorizBlock() );
    SHOW( (uint32_t)imsub.getNumPixelsPerVertBlock() );
    SHOW( (uint32_t)imsub.getNumBitsPerPixel() );
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
    //    htUd.forEach(showTRE);

    SHOW( (uint32_t)imsub.getExtendedHeaderLength() );
    SHOW( imsub.getExtendedHeaderOverflow().toString() );

    nitf::Extensions exExts = imsub.getExtendedSection();
    showExtensions(exExts);
    // nitf::HashTable htEx = exExts.getHash();
    //    htEx.forEach(showTRE);

}

void showImages(const nitf::Record& record)
{
    if (format_as_xml)
    {
        return; // don't need XML output right now
    }

    if (record.getNumImages())
    {
        const nitf::List images = record.getImages();

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
}

/*
 *  This function dumps the security header.
 *
 */
void showSecurityGroup(const nitf::FileSecurity& securityGroup)
{
    if (format_as_xml)
    {
        return; // don't need XML output right now
    }

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

void showGraphicSubheader(const nitf::GraphicSubheader& sub)
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

void showGraphics(const nitf::Record& record)
{
    if (format_as_xml)
    {
        return; // don't need XML output right now
    }

    // Graphics
    if (record.getNumGraphics())
    {
        const nitf::List graphics = record.getGraphics();

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
}

void showLabelSubheader(const nitf::LabelSubheader& sub)
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

void showLabels(const nitf::Record& record)
{
    if (format_as_xml)
    {
        return; // don't need XML output right now
    }

    if (record.getNumLabels())
    {
        const nitf::List labels = record.getLabels();

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
}

/*
 *  This section contains raw text data.  You can put
 *  lots of stuff in here but most people never do.
 *
 *  Note that XML data is usually not contained in this section
 *  even though that might have made more sense.  XML data is
 *  typically found in the DES segment
 */
void showTextSubheader(const nitf::TextSubheader& sub)
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

void showTexts(const nitf::Record& record)
{
    if (format_as_xml)
    {
        return; // don't need XML output right now
    }

    if (record.getNumTexts())
    {
        const nitf::List texts = record.getTexts();

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
void showDESubheader(const nitf::DESubheader& sub)
{
    if (!format_as_xml)
    {
        printf("DES subheader\n");
    }
    else
    {
        std::cout << "<DESubheader>\n";
    }
    
    //#define SHOW_DES_SUBHEADER(X) SHOW(sub.get ## X(). toString())
   #define SHOW_DES_SUBHEADER(X) show(#X, sub.get ## X(). toString())
    #define SHOWI_DES_SUBHEADER(X) show(#X, sub.get ## X())

    SHOW_DES_SUBHEADER(FilePartType);
    SHOW_DES_SUBHEADER(TypeID);
    SHOW_DES_SUBHEADER(Version);
    SHOW_DES_SUBHEADER(SecurityClass);

    if (*(sub.getSecurityClass().getRawData()) != 'U')
        showSecurityGroup(sub.getSecurityGroup());

    SHOW_DES_SUBHEADER(OverflowedHeaderType);
    SHOW_DES_SUBHEADER(DataItemOverflowed);
    SHOW_DES_SUBHEADER(SubheaderFieldsLength);

    /*
     *  This is the user defined parameter section
     *  within the DES.  It contains only BCS-A/N type values
     *  so storing it in a 'TRE' struct is no big deal
     *
     *  It is only valid if the subheader fields length > 0; otherwise, it will
     *  throw an Exception since there is no TRE defining the subheader fields.
     */
    if (((uint32_t)sub.getSubheaderFieldsLength()) > 0)
    {
        const nitf::TRE tre = sub.getSubheaderFields();
        printTRE( tre );
    }

    SHOWI_DES_SUBHEADER(DataLength);

    if (!format_as_xml)
    {
        SHOWI(sub.getDataLength());
    }

    /*
     *  NITRO only populates this object if the DESDATA contains
     *  TRE overflow.  Otherwise, you need to use a DEReader to
     *  get at the DESDATA, since it can contain anything.
     *
     *  We wont bother to try and print whatever other things might
     *  have been put in here (e.g, text data or binary blobs)
     */
    const nitf::Extensions exts = sub.getUserDefinedSection();
    showExtensions(exts);

    if (format_as_xml)
    {
        std::cout << "</DESubheader>\n";
    }
}

void showDataExtensions(const nitf::Record& record)
{
    if (record.getNumDataExtensions())
    {
        if (format_as_xml)
        {
            std::cout << "<DataExtensions>\n";
        }

        const nitf::List des = record.getDataExtensions();

        //  Walk each label and show
        for (nitf::ListIterator iter = des.begin();
            iter != des.end(); ++iter)
        {
            const nitf::DESegment segment = *iter;
            showDESubheader(segment.getSubheader());
        }

        if (format_as_xml)
        {
            std::cout << "</DataExtensions>\n";
        }
    }
    else
    {
        if (!format_as_xml)
        {
            std::cout << "No data extensions in file\n";
        }
    }
}

/*
 *  This section is never really populated
 */
void showRESubheader(const nitf::RESubheader& sub)
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

void showReservedExtensions(const nitf::Record& record)
{
    if (format_as_xml)
    {
        return; // no XML output right now
    }

    if (record.getNumReservedExtensions())
    {
        const nitf::List res = record.getReservedExtensions();

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
}

void showWarnings(const nitf::Reader& reader)
{
    if (format_as_xml)
    {
        return; // don't need XML output right now
    }

    const nitf::List warnings = reader.getWarningList();
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
            char* p = (char*)*iter;

            //  Make sure
            assert(p != NULL);

            //  Show the data
            std::cout << "\tFound problem: [" << p << "]\n" << std::endl;

            ++iter;
        }
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
}

const std::string str_xml_option = "--xml";
static int usage(const std::string& argv0)
{
    std::cout << "Usage: " << argv0 << " [ " << str_xml_option << " ] <nitf-file>\n";
    return EXIT_FAILURE;
}

static int main_(int argc, char** argv)
{
    if ((argc != 2) && (argc != 3))
    {
        return usage(argv[0]);
    }
    
    std::string nitfPathname;
    if (argc == 3)
    {
        if (argv[1] != str_xml_option)
        {
            return usage(argv[0]);
        }
        format_as_xml = true;
        nitfPathname = argv[2];
    }
    else
    {
        nitfPathname = argv[1];
    }

    io::FileInputStream fis(nitfPathname);
    nitf::IOStreamReader io(fis);
    if (nitf::Reader::getNITFVersion(io) == NITF_VER_UNKNOWN)
    {
        std::cout << "This file does not appear to be a valid NITF\n";
        exit(EXIT_FAILURE);
    }

    //  This is the reader object
    nitf::Reader reader;

    //  This parses all header data within the NITF
    const nitf::Record record = reader.readIO(io);

    // Now show the header
    const nitf::FileHeader fileHeader = record.getHeader();

    if (format_as_xml)
    {
        std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>" << "\n";
        std::cout << "<NITF>" << "\n";
    }

    // Now show the header
    showFileHeader(fileHeader);

    // And now show the image information
    showImages(record);

    showGraphics(record);
    showLabels(record);
    showTexts(record);
    showDataExtensions(record);
    showReservedExtensions(record);

    io.close();
    
    showWarnings(reader);

    if (format_as_xml)
    {
        std::cout << "</NITF>" << "\n";
    }
    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    try
    {
        return main_(argc, argv);
     }
    catch (const except::Throwable& t)
    {
        std::cerr << t.getTrace() << "\n";
    }
}
