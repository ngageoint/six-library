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

#ifndef __NITF_FIELD_WARNING_H__
#define __NITF_FIELD_WARNING_H__

#include "nitf/System.h"
#include "nitf/Field.h"

NITF_CXX_GUARD


/*!
 *  \struct nitf_FieldWarning
 *  \brief Describes a warning associated with a field in the NITF
 *
 *  The FieldWarning  object stores information necessary to describe a problem
 *  associated with a NITF field.
 */
typedef struct _nitf_FieldWarning
{
    /* Offset in bytes to the field in the NITF file */
    nitf_Off fileOffset;

    /* Name of the offending field in the following format
       hdr.section(i).  ...  .fieldName
       section  - Any of the following section abbreviations.
       If a section can occur more than once (i)
       will appear where "i" is the occurance number
       in the order they appear in the file
       hdr     - Main file header (Identification & Organization Segment)
       ish(i)  - ith image sub-header
       imd(i)  - ith image data section
       gsh(i)  - ith graphics sub-header
       grd(i)  - ith graphics data section
       tsh(i)  - ith text sub-header
       ted(i)  - ith text data section
       esh(i)  - ith extended data sub-header
       edd(i)  - ith extended data section
       
       <TRE>(i)- ith occurance of a particular TRE in a particular
       section (i.e. hdr.ish(2).ACFTB(1).SCNUM 
    */
    
    char *fieldName;

    /* Original field found in the file */
    nitf_Field *field;

    /* Description of what was expected */
    char *expectation;

}
nitf_FieldWarning;


/*!
 *  Construct a FieldWarning object
 *  This function creates a FieldWarning object.
 *  \param fileOffset Offset in the file to the field
 *  \param field A string which represents the NITF field
 *  \param value The NITF field value
 *  \param expectation A string describing the expected field value
 *  \param error An error object to fill if a problem occurs
 *  \return A FieldWarning object or NULL upon failure
 */
NITFAPI(nitf_FieldWarning *) 
nitf_FieldWarning_construct(nitf_Off fileOffset,
                            const char *fieldName,
                            nitf_Field *field,
                            const char
                            *expectation,
                            nitf_Error *
                            error);


/*!
 *  Destruct a FieldWarning object
 *  This function destructs a FieldWarning object and NULL sets its pointer
 *  \param fieldWarningPtr A pointer to a FieldWarning object
 */
NITFAPI(void) nitf_FieldWarning_destruct(nitf_FieldWarning **fieldWarningPtr);


/*!
 *  Clone a FieldWarning object
 *  This function clones (deep copies) a FieldWarning object
 *  \param source The source object
 *  \param error An error object to fill if a problem occurs
 *  \return A FieldWarning object or NULL upon failure
 */
NITFAPI(nitf_FieldWarning *) nitf_FieldWarning_clone(nitf_FieldWarning *source,
                                                     nitf_Error * error);


/* HELP:  This object should probably have 1 or more print methods, and ? */

NITF_CXX_ENDGUARD

#endif
