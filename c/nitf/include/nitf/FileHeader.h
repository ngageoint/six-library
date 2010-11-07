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

#ifndef __NITF_FILE_HEADER_H__
#define __NITF_FILE_HEADER_H__

/*!
 *  \file
 *  \brief Contains the API for objects in a NITF file header
 *
 *  This file contains the declarations for all of the objects which
 *  are loaded or saved during file read/write operations.
 *
 *
 */

#include "nitf/System.h"
#include "nitf/FileSecurity.h"
#include "nitf/ComponentInfo.h"
#include "nitf/Extensions.h"

#define NITF_FHDR_SZ   4
#define NITF_FVER_SZ   5
#define NITF_CLEVEL_SZ 2
#define NITF_STYPE_SZ  4
#define NITF_OSTAID_SZ 10
#define NITF_FDT_SZ    14
#define NITF_FTITLE_SZ 80
#define NITF_FSCLAS_SZ 1
#define NITF_FSCOP_SZ  5
#define NITF_FSCPYS_SZ 5
#define NITF_ENCRYP_SZ 1
#define NITF_FBKGC_SZ  3        /*  Nobody else is using this one! */
#define NITF_ONAME_SZ  24
#define NITF_OPHONE_SZ 18
#define NITF_FL_SZ     12
#define NITF_HL_SZ     6

#define NITF_LISH_SZ  6
#define NITF_LI_SZ    10
#define NITF_LSSH_SZ  4
#define NITF_LS_SZ    6
#define NITF_LLSH_SZ  4
#define NITF_LL_SZ    3
#define NITF_LTSH_SZ  4
#define NITF_LT_SZ    5
#define NITF_LDSH_SZ  4
#define NITF_LD_SZ    9
#define NITF_LRESH_SZ 4
#define NITF_LRE_SZ   7
/*  This is the tricky part...  */
#define NITF_UDHDL_SZ   5
#define NITF_UDHOFL_SZ  3
#define NITF_XHDL_SZ    5
#define NITF_XHDLOFL_SZ 3

/* new ones added by TZ */
#define NITF_NUMI_SZ 3
#define NITF_NUMS_SZ 3
#define NITF_NUMX_SZ 3
#define NITF_NUMT_SZ 3
#define NITF_NUMDES_SZ 3
#define NITF_NUMRES_SZ 3


#define NITF_FHDR     fileHeader
#define NITF_FVER     fileVersion
#define NITF_CLEVEL   complianceLevel
#define NITF_STYPE    systemType
#define NITF_OSTAID   originStationID
#define NITF_FDT      fileDateTime
#define NITF_FTITLE   fileTitle
#define NITF_FSCLAS   classification
#define NITF_FSCOP    messageCopyNum
#define NITF_FSCPYS   messageNumCopies
#define NITF_ENCRYP   encrypted
#define NITF_FBKGC    backgroundColor
#define NITF_ONAME    originatorName
#define NITF_OPHONE   originatorPhone
#define NITF_FL       fileLength
#define NITF_HL       headerLength
#define NITF_LISH(i)  imageInfo[i]->lengthSubheader
#define NITF_LI(i)    imageInfo[i]->lengthData
#define NITF_LSSH(i)  graphicInfo[i]->lengthSubheader
#define NITF_LS(i)    graphicInfo[i]->lengthData
#define NITF_LLSH(i)  labelInfo[i]->lengthSubheader
#define NITF_LL(i)    labelInfo[i]->lengthData
#define NITF_LTSH(i)  textInfo[i]->lengthSubheader
#define NITF_LT(i)    textInfo[i]->lengthData
#define NITF_LDSH(i)  dataExtensionInfo[i]->lengthSubheader
#define NITF_LD(i)    dataExtensionInfo[i]->lengthData
#define NITF_LRESH(i) reservedExtensionInfo[i]->lengthSubheader
#define NITF_LRE(i)   reservedExtensionInfo[i]->lengthData

#define NITF_NUMI     numImages
#define NITF_NUMS     numGraphics
#define NITF_NUMX     numLabels
#define NITF_NUMT     numTexts
#define NITF_NUMDES   numDataExtensions
#define NITF_NUMRES   numReservedExtensions

#define NITF_UDHDL    userDefinedHeaderLength
#define NITF_UDHOFL   userDefinedOverflow
#define NITF_XHDL     extendedHeaderLength
#define NITF_XHDLOFL  extendedHeaderOverflow

NITF_CXX_GUARD

/*!
 *  \struct nitf_FileHeader
 *  \brief  A structure representing the in-memory layout of the NITF file
 *
 *  This object is the in-core representation of an NITF file header
 *  its accessor fields may be called by their longnames (the manner in
 *  which they are declared) or by their 'nicknames,'  which are provided
 *  above.  The shortcuts are also for algorithmically determining a length
 *  given only the name, using the construction {SHORTNAME}##SZ
 *
 */
typedef struct _nitf_FileHeader
{
    nitf_Field *fileHeader;
    nitf_Field *fileVersion;
    nitf_Field *complianceLevel;
    nitf_Field *systemType;
    nitf_Field *originStationID;
    nitf_Field *fileDateTime;
    nitf_Field *fileTitle;
    nitf_Field *classification;

    nitf_FileSecurity *securityGroup;

    nitf_Field *messageCopyNum;
    nitf_Field *messageNumCopies;
    nitf_Field *encrypted;
    nitf_Field *backgroundColor;
    nitf_Field *originatorName;
    nitf_Field *originatorPhone;

    nitf_Field *fileLength;
    nitf_Field *headerLength;

    nitf_Field *numImages;
    nitf_Field *numGraphics;
    nitf_Field *numLabels;
    nitf_Field *numTexts;
    nitf_Field *numDataExtensions;
    nitf_Field *numReservedExtensions;

    nitf_ComponentInfo **imageInfo;
    nitf_ComponentInfo **graphicInfo;
    nitf_ComponentInfo **labelInfo;
    nitf_ComponentInfo **textInfo;
    nitf_ComponentInfo **dataExtensionInfo;
    nitf_ComponentInfo **reservedExtensionInfo;

    nitf_Field *userDefinedHeaderLength;
    nitf_Field *userDefinedOverflow;
    nitf_Field *extendedHeaderLength;
    nitf_Field *extendedHeaderOverflow;

    nitf_Extensions *userDefinedSection;
    nitf_Extensions *extendedSection;
    /* udhdl, udhofl, udhd */
    /* xhdl, xhdlofl, xhd */

}
nitf_FileHeader;

/*!
 *  We want to construct and init the whole object.
 *  We NULL-initialize all sub-objects before creating them, so that,
 *  in the event of an exception, memory deallocation is easier.
 *  This is tedious, but prevents bug-prone problems
 *
 *  \param error An error to populate on a NULL return
 *  \return The newly allocated file header, or NULL upon failure
 *
 */
NITFAPI(nitf_FileHeader *) nitf_FileHeader_construct(nitf_Error * error);


/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_FileHeader *) nitf_FileHeader_clone(nitf_FileHeader * source,
        nitf_Error * error);

/*!
 *  Destruct all sub-objects, and make sure that we
 *  kill this object too.
 *
 *  \param fh The file header to destroy.  We point fh at NULL.
 */
NITFAPI(void) nitf_FileHeader_destruct(nitf_FileHeader ** fh);


NITF_CXX_ENDGUARD

#endif
