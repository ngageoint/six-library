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

#ifndef __NITF_BAND_INFO_H__
#define __NITF_BAND_INFO_H__

#include "nitf/LookupTable.h"
#include "nitf/System.h"
#include "nitf/Field.h"


#define NITF_IREPBAND_SZ 2
#define NITF_ISUBCAT_SZ  6
#define NITF_IFC_SZ      1
#define NITF_IMFLT_SZ    3
#define NITF_NLUTS_SZ    1
#define NITF_NELUT_SZ    5

#define NITF_IREPBAND representation
#define NITF_ISUBCAT  subcategory
#define NITF_IFC      imageFilterCondition
#define NITF_IMFLT    imageFilterCode
#define NITF_NLUTS    numLUTs
#define NITF_NELUT    bandEntriesPerLUT

NITF_CXX_GUARD

/*!
 *  \struct nitf_BandInfo
 *  \brief The object representing image band information
 *
 *  The BandInfo object is stored in the image subheader, and
 *  contains information about the bands, including any Lookup
 *  Tables, and information about band representation.
 */
typedef struct _nitf_BandInfo
{
    nitf_Field *representation;
    nitf_Field *subcategory;
    nitf_Field *imageFilterCondition;
    nitf_Field *imageFilterCode;
    nitf_Field *numLUTs;
    nitf_Field *bandEntriesPerLUT;
    nitf_LookupTable *lut;
}
nitf_BandInfo;

/*!
 *  Constructor for a BandInfo object.  All internal values
 *  are constructed to size, with the default fill.  The lookup
 *  table is set to NULL to start.
 *
 *  \param error An error to populate on a NULL return
 *  \return The newly allocated bandInfo object, or NULL upon failure
 *
 */
NITFAPI(nitf_BandInfo *) nitf_BandInfo_construct(nitf_Error * error);


/*!
 *  Destruct all sub-objects, and make sure that we
 *  kill this object too.
 *
 *  \param info The BandInfo to destroy.  We point info at NULL.
 */
NITFAPI(void) nitf_BandInfo_destruct(nitf_BandInfo ** info);


/*!
 *  Clone this object.  This is a deep copy operation.  The lut
 *  is set to NULL if the source has none, and otherwise, is a
 *  deep copy of the LUT (clone).
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFPROT(nitf_BandInfo *) nitf_BandInfo_clone(nitf_BandInfo * source,
                                              nitf_Error * error);

/*!
 *  \brief nitf_BandInfo_init - Initialize a band info object
 *
 *  nitf_BandInfo_init initialize a band info table object. The caller must
 *  ( supply an existing object, this function does not construct one.
 *
 *  Any of the string arguments can be NULL in which case the default value
 *  is used.
 *
 *  The object retains a refference to the lut argument, it does not make a
 *  copy
 *
 *  \return FALSE is returned on error and the supplied error object is set
 */
NITFAPI(NITF_BOOL) nitf_BandInfo_init(nitf_BandInfo * bandInfo, /*!< The band info to initialize */
                                      const char *representation,       /*!< The band representation */
                                      const char *subcategory,          /*!< The band subcategory */
                                      const char *imageFilterCondition, /*!< The band filter condition */
                                      const char *imageFilterCode,      /*!< The band standard image filter code */
                                      nitf_Uint32 numLUTs,      /*!< The number of look-up tables */
                                      nitf_Uint32 bandEntriesPerLUT,    /*!< The number of entries/LUT */
                                      nitf_LookupTable * lut,   /*!< The look-up tables */
                                      nitf_Error * error        /*!< Error object for error returns */
                                     );


NITF_CXX_ENDGUARD

#endif
