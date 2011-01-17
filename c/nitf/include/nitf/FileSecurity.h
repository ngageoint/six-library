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

#ifndef __NITF_FILE_SECURITY_H__
#define __NITF_FILE_SECURITY_H__


/*!
 *  \file
 *  \brief Handle the file security groupings that appear throughout
 *
 *  Any of the headers or subheaders have security groupings.  The
 *  object declared in this header is the storage container for such
 *  information
 *
 */

#include "nitf/System.h"
#include "nitf/Field.h"

/*
 *  The constants below define the lengths of each field contained in
 *  the security info in any header for a NITF file
 *
 */

/*
 * These are the constant lengths for the 2.1 FileSecurity fields
 */
#define NITF_CLSY_SZ   2
#define NITF_CODE_SZ   11
#define NITF_CTLH_SZ   2
#define NITF_REL_SZ    20
#define NITF_DCTP_SZ   2
#define NITF_DCDT_SZ   8
#define NITF_DCXM_SZ   4
#define NITF_DG_SZ     1
#define NITF_DGDT_SZ   8
#define NITF_CLTX_SZ   43
#define NITF_CATP_SZ   1
#define NITF_CAUT_SZ   40
#define NITF_CRSN_SZ   1
#define NITF_RDT_SZ    8
#define NITF_CTLN_SZ   15

/*
 * The total block size for a 2.1 FileSecurity section
 */
#define NITF_SG_BLOCK_SZ \
    NITF_CLSY_SZ + \
    NITF_CODE_SZ + \
    NITF_CTLH_SZ + \
    NITF_REL_SZ  + \
    NITF_DCTP_SZ + \
    NITF_DCDT_SZ + \
    NITF_DCXM_SZ + \
    NITF_DG_SZ   + \
    NITF_DGDT_SZ + \
    NITF_CLTX_SZ + \
    NITF_CATP_SZ + \
    NITF_CAUT_SZ + \
    NITF_CRSN_SZ + \
    NITF_RDT_SZ  + \
    NITF_CTLN_SZ

/*
 * These are the constant lengths for the 2.0 FileSecurity fields
 * Notice that there are less fields in 2.0 than in 2.1...
 */
#define NITF_CODE_20_SZ   40
#define NITF_CTLH_20_SZ   40
#define NITF_REL_20_SZ    40
#define NITF_CAUT_20_SZ   20
#define NITF_CTLN_20_SZ   20
#define NITF_DGDT_20_SZ   6
#define NITF_CLTX_20_SZ   40


/*
 *  It is suitable to refer to the object field either by its meaningful
 *  name, or by its specification name.  The short (spec) names are just
 *  aliases to the full names.  They also help us internally to perform
 *  macros which require that the length field can be algorithmically
 *  constructed using the form {SHORTNAME}##SZ
 *
 */

#define NITF_CLSY classificationSystem
#define NITF_CODE codewords
#define NITF_CTLH controlAndHandling
#define NITF_REL  releasingInstructions
#define NITF_DCTP declassificationType
#define NITF_DCDT declassificationDate
#define NITF_DCXM declassificationExemption
#define NITF_DG   downgrade
#define NITF_DGDT downgradeDateTime
#define NITF_CLTX classificationText
#define NITF_CATP classificationAuthorityType
#define NITF_CAUT classificationAuthority
#define NITF_CRSN classificationReason
#define NITF_RDT  securitySourceDate
#define NITF_CTLN securityControlNumber

NITF_CXX_GUARD

/*!
 *  \struct nitf_FileSecurity
 *  \brief  This class keeps track of the file security info
 *
 *  The file security struct has a number of fields pertaining
 *  to security information from the nitf file.  You may access these
 *  by their longnames, as they are declared, or you may use the shorthand
 *  macros provided above
 *
 */
typedef struct _nitf_FileSecurity
{

    nitf_Field *classificationSystem;
    nitf_Field *codewords;
    nitf_Field *controlAndHandling;
    nitf_Field *releasingInstructions;
    nitf_Field *declassificationType;
    nitf_Field *declassificationDate;
    nitf_Field *declassificationExemption;
    nitf_Field *downgrade;
    nitf_Field *downgradeDateTime;
    nitf_Field *classificationText;
    nitf_Field *classificationAuthorityType;
    nitf_Field *classificationAuthority;
    nitf_Field *classificationReason;
    nitf_Field *securitySourceDate;
    nitf_Field *securityControlNumber;

}
nitf_FileSecurity;

/*!
 *  Create this object and memset components to zero.
 *
 *  \param error An error to populate on a NULL return
 *  \return the object
 */
NITFAPI(nitf_FileSecurity *) nitf_FileSecurity_construct(nitf_Error *
        error);

/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_FileSecurity *) nitf_FileSecurity_clone(nitf_FileSecurity *
        source,
        nitf_Error * error);


/*!
 *  Delete this object and null it.
 *  \param fs The file security object
 */
NITFAPI(void) nitf_FileSecurity_destruct(nitf_FileSecurity ** fs);


/*!
 *  Resizes the FileSecurity fields that are different sizes between
 *  different versions of the NITF specification. Calling with a valid
 *  nitf_Version will resize for that version. The default is to size
 *  for NITF 2.1 (NITF_VER_21)
 *
 *  \param fs The source object
 *  \param ver The nitf_Version to use
 *  \param error  An error to populate upon failure
 *  \return NITF_SUCCESS if valid, NITF_FAILURE otherwise
 */
NITFAPI(NITF_BOOL) nitf_FileSecurity_resizeForVersion
(
    nitf_FileSecurity *fs,
    nitf_Version ver,
    nitf_Error * error
);


NITF_CXX_ENDGUARD

#endif
