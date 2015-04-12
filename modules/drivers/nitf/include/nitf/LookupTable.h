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
 * License along with this program; if not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_LOOKUP_TABLE_H__
#define __NITF_LOOKUP_TABLE_H__

#include "nitf/System.h"

NITF_CXX_GUARD

/*!
  \brief nitf_LookupTable - Object representing the image sub-header lookup
   table

  The nitf_LookupTable object represents the image sub-header lookup these
  tables are stored in the sub-header and are used to map the image data
  in some representations. These objects appear as a field in the
  nitf_BandInfo object.

*/
typedef struct _nitf_LookupTable
{
    nitf_Uint32 tables;         /*!< Number of tables */
    nitf_Uint32 entries;        /*!< Number of entries per table */
    nitf_Uint8 *table;          /*!< The tables */
}
nitf_LookupTable;

/*!
 *  Constructor for a LookupTable object.  The numTables and numEntries are
 *  both set, but the LookupTable is set to NULL to start.
 *
 *  \param numTables the number of lookupTables
 *  \param numEntries the number of entries per lookup table
 *  \param error An error to populate on a NULL return
 *  \return The newly allocated bandInfo object, or NULL upon failure
 *
 */
NITFAPI(nitf_LookupTable *) nitf_LookupTable_construct
(
    nitf_Uint32 numTables,
    nitf_Uint32 numEntries,
    nitf_Error * error
);

/*!
 *  Clone this object.  This is a deep copy operation.  The lut
 *  is set to NULL if the source has none, and otherwise, is a
 *  deep copy of the LUT (clone).
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_LookupTable *) nitf_LookupTable_clone
(
    nitf_LookupTable * source,
    nitf_Error * error
);

/*!
 *  Destruct this and all sub-objects.
 *
 *  \param lut The LookupTable to destroy.  We point lut at NULL.
 */
NITFAPI(void) nitf_LookupTable_destruct(nitf_LookupTable ** lut);


/*!
  \brief nitf_LookupTable_init - Initialize a lookup table object

  nitf_LookupTable_init initialize a lookup table object. The caller must
  supply an existing object, this function does not construct one.

  The tables are stored as one contiguous array. This function makes a copy
  of the tables and does not retain a reference to the user argument.

  \return FALSE is returned on error and the supplied error object is set
*/
NITFAPI(NITF_BOOL) nitf_LookupTable_init(
    nitf_LookupTable * lut,      /*!< The lookup table to initialize */
    nitf_Uint32 numTables,       /*!< Number of tables */
    nitf_Uint32 numEntries,      /*!< Number of entries in the tablt */
    NITF_DATA * tables,          /*!< The tables */
    nitf_Error * error           /*!< Error object for error returns */
);

NITF_CXX_ENDGUARD

#endif
