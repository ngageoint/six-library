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

#ifndef __NITF_EXTENSIONS_H__
#define __NITF_EXTENSIONS_H__

#include "nitf/System.h"
#include "nitf/TRE.h"

NITF_CXX_GUARD

/*!
 *  \struct nitf_Extensions
 *  \brief Object containing listing of TREs discovered in segment
 *
 *  The Extensions object represents the user defined and extended header
 *  portions of the various segments.  The representation internally is
 *  cross-indexed -- once as an associative array, to yield fast lookups
 *  and once as a linked list to maintain TRE ordering.  The hash table
 *  points at a list of TREs matching the key (which is the TRE name).
 *  The list is of references to the TREs containing the TRE and some
 *  context information.
 */
typedef struct _nitf_Extensions
{
    /* A hash of tre's (char*, [nitf_List{nitf_TRE*}]) */
    nitf_HashTable *hash;
    nitf_List *ref;
}
nitf_Extensions;


/*!
 *  \struct nitf_ExtensionsIterator
 *  \brief The iterator defined for traversing an extension linearly
 *
 *  The ExtensionsIterator makes use of the reference list in the Extensions
 *  object, in order to track a position within the extensions object
 *  in order.
 */
typedef struct _nitf_ExtensionsIterator
{
    nitf_ListIterator iter;
}
nitf_ExtensionsIterator;

/*!
 *  Construct the extensions section.  These may exist in almost
 *  any part of the NITF file.  In essence, this object is a
 *  data store of information that was provided in data extensions.
 *
 *  \param error An object to be populated on error
 *  \return NULL on failure, the object otherwise
 *
 */
NITFAPI(nitf_Extensions *) nitf_Extensions_construct(nitf_Error * error);

/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_Extensions *) nitf_Extensions_clone(nitf_Extensions * source,
        nitf_Error * error);



/*!
 *  Destruct the extensions section.  This removes the hash values
 *  and deletes everything.
 *  \param ext The address of an extensions pointer
 *
 */
NITFAPI(void) nitf_Extensions_destruct(nitf_Extensions ** ext);

/*!
 *  Insert a tre into the extensions section.  The object will
 *  be added as follows.  If such an object exists in the hash already,
 *  this object will be added to the next pointer link.
 *
 *  1.3 Release
 *
 *  This function replaces the 1.2 nitf_Extensions_insert()
 *  There is a new nitf_Extensions_insert() function which uses
 *  iterators, but this appendTRE function will simply place it
 *  at the end
 *
 *  \param name The name of the TRE to insert
 *  \param tre The tre to insert
 *  \return  1 on success, 0 on failure
 */
NITFAPI(NITF_BOOL) nitf_Extensions_appendTRE(nitf_Extensions * ext,
        nitf_TRE * tre,
        nitf_Error * error);

/*!
 *  \param name The name of the TRE
 *  \return The TRE that is retrieved from the hash
 */
NITFAPI(nitf_List *) nitf_Extensions_getTREsByName(nitf_Extensions * ext,
        const char *name);


/*! For backwards compatibility
 *  \define nitf_Extensions_get
 *  1.3 mapping to 1.2 function
 */
#define nitf_Extensions_get(ext, name) nitf_Extensions_getTREsByName(ext, name)

/*!
 *  This function replaces the 1.2 nitf_Extensions_erase() function,
 *  which was incorrectly implemented.
 *
 *  Remove all instances of a given TRE from this extensions object.
 *  \param ext The name of the TRE to remove
 *
 */
NITFAPI(void) nitf_Extensions_removeTREsByName(nitf_Extensions* ext,
        const char* name);

/*!
 *  Checks if the TRE of the given name exists
 *  \param name The name of the TRE
 *  \return 1 if exists, 0 otherwise
 */
NITFAPI(NITF_BOOL) nitf_Extensions_exists(nitf_Extensions * ext,
        const char *name);


/*!
 *  The begin method produces an ExtensionsIterator that points
 *  at the beginning of the Extensions object.  If "dereferenced,"
 *  the iterator will produce the first TRE within the extension.
 *
 *  \param ext The extensions segment
 *  \return An iterator pointing at the beginning of the extension
 */
NITFAPI(nitf_ExtensionsIterator) nitf_Extensions_begin(nitf_Extensions *ext);

/*!
 *  The end method produces an ExtensionsIterator that points
 *  at the end of the Extensions object.  If "dereferenced,"
 *  the iterator will produce the final TRE within the extension.
 *
 *  \param ext The extensions segment
 *  \return An iterator pointing at the end of the extension
 */
NITFAPI(nitf_ExtensionsIterator) nitf_Extensions_end(nitf_Extensions *ext);

/*!
 *  Increments the extensions iterator
 *  \param extIt the iterator to increment
 */
NITFAPI(void)
nitf_ExtensionsIterator_increment(nitf_ExtensionsIterator *extIt);

/*!
 *  Produce the TRE at this location ("dereference").
 *  This function modified for 1.3
 *
 *  \param extIt The iterator
 *  \return The TRE that the iterator is pointing at
 */
NITFAPI(nitf_TRE *)
nitf_ExtensionsIterator_get(nitf_ExtensionsIterator *extIt);



/*!
 *  Insert a TRE instance into the extensions location after this position
 *  \see nitf_List_insert
 *  at the iterator location.  This function is new for 1.3
 *
 *  \param ext The name of the extensions object
 *  \param extIter The iterator
 *  \param tre The TRE to insert
 *  \param An error to populate on failure
 *  \return NITF_SUCCESS or NITF_FAILURE
 */
NITFAPI(NITF_BOOL) nitf_Extensions_insert(nitf_Extensions* ext,
        nitf_ExtensionsIterator* extIter,
        nitf_TRE* tre,
        nitf_Error* error);

/*!
 *  Remove a TRE instance at the iterator location.  This function
 *  is new for 1.3
 *
 *  \param ext The name of the extensions object
 *  \param extIter The iterator
 *  \param tre The TRE to insert
 *  \param An error to populate on failure
 *  \return NITF_SUCCESS or NITF_FAILURE
 */
NITFAPI(nitf_TRE*) nitf_Extensions_remove(nitf_Extensions* ext,
        nitf_ExtensionsIterator* extIter,
        nitf_Error* error);


/*!
 *  Check for equality in iterators.  This is typically done for
 *  looping purposes.
 *
 *  \param it1 The first iterator to compare
 *  \param it2 The second iterator to compare
 *  \return True if the iterators are equal
 */
NITFAPI(NITF_BOOL)
nitf_ExtensionsIterator_equals(nitf_ExtensionsIterator *it1,
                               nitf_ExtensionsIterator *it2);

/*!
 *  Check for inequality in iterators.  This is typically done for
 *  conditional looping purposes.
 *
 *  \param it1 The first iterator to compare
 *  \param it2 The second iterator to compare
 *  \return True if the iterators are not equal
 */
NITFAPI(NITF_BOOL)
nitf_ExtensionsIterator_notEqualTo(nitf_ExtensionsIterator *it1,
                                   nitf_ExtensionsIterator *it2);



/*!
 * Computes the length of the extension based on its current state.
 *
 * This basically just adds up the lengths of all of the TREs it holds.
 *
 * \param ext The extension segment
 * \param fver The version
 * \param error The error to populate on failure
 * \return the length
 */
NITFAPI(nitf_Uint32)
nitf_Extensions_computeLength(nitf_Extensions * ext,
                              nitf_Version fver,
                              nitf_Error * error);

NITF_CXX_ENDGUARD

#endif
