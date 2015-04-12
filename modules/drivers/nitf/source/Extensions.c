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

#include "nitf/Extensions.h"


NITFPRIV(int) eraseIt(nitf_HashTable * ht,
                      nitf_Pair * pair, NITF_DATA* userData, nitf_Error * error);

NITFPRIV(NITF_BOOL) insertToHash( nitf_HashTable* hash, const char* name,
                                  nitf_TRE* tre, nitf_Error* error);

NITFAPI(nitf_Extensions *) nitf_Extensions_construct(nitf_Error * error)
{
    /*  Allocate the extensions section  */
    nitf_Extensions *ext =
        (nitf_Extensions *) NITF_MALLOC(sizeof(nitf_Extensions));
    if (!ext)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    /* Make sure we NULL-set this so we dont have any problems */
    ext->ref = NULL;

    /*  Construct the hash -- it WILL, autodelete its chains  */
    /*  And we will autodelete our list items                 */
    ext->hash = nitf_HashTable_construct(NITF_TRE_HASH_SIZE, error);
    if (!ext->hash)
    {
        /*  If there is failure, cleanup  */
        nitf_Extensions_destruct(&ext);
        return NULL;
    }
    ext->ref = nitf_List_construct(error);
    if (!ext->ref)
    {
        nitf_Extensions_destruct(&ext);
        return NULL;
    }

    nitf_HashTable_setPolicy(ext->hash, NITF_DATA_RETAIN_OWNER);
    return ext;
}


NITFAPI(nitf_Extensions *) nitf_Extensions_clone(nitf_Extensions * source,
        nitf_Error * error)
{

    /*  In this case, using the constructor is easiest here */
    nitf_Extensions *ext = NULL;
    if (source)
    {
        nitf_ListIterator currentRef;
        nitf_ListIterator treRefEnd;

        ext = (nitf_Extensions *) NITF_MALLOC(sizeof(nitf_Extensions));
        if (!ext)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }

        /* Make sure to set this to NULL to avoid any problems */
        ext->ref = NULL;

        /*  Create the hash  */
        ext->hash =
            nitf_HashTable_construct(source->hash->nbuckets, error);
        if (!ext->hash)
        {
            nitf_Extensions_destruct(&ext);
            return NULL;
        }


        /*
         *  First, clone the list of TREs. This is the easiest thing
         *  to do anyway
         *
         */
        if ( (ext->ref = nitf_List_clone(source->ref,
                                         (NITF_DATA_ITEM_CLONE)
                                         nitf_TRE_clone,
                                         error)) == NULL )
        {
            nitf_Extensions_destruct(&ext);
            return NULL;
        }

        currentRef = nitf_List_begin(ext->ref);
        treRefEnd   = nitf_List_end(ext->ref);

        /* Go through and insert each thing */
        while ( nitf_ListIterator_notEqualTo( &currentRef,
                                              &treRefEnd ) )
        {

            nitf_TRE* tre = (nitf_TRE*) nitf_ListIterator_get(&currentRef);/*(ext->ref);*/
            if (!insertToHash( ext->hash, tre->tag, tre, error ))
            {
                nitf_Extensions_destruct(&ext);
                return NULL;
            }
            nitf_ListIterator_increment(&currentRef);
        }
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
    }
    return ext;
}


NITFAPI(void) nitf_Extensions_destruct(nitf_Extensions ** ext)
{

    /*  If we have a handle still  */
    if ((*ext))
    {

        /*  If it owns a valid hash  */
        if ((*ext)->hash)
        {

            /*  We want to delete the chain  */
            nitf_Error e;
            NITF_HASH_FUNCTOR deleteHashChain = eraseIt;

            /*  Each hash value is a linked list                        */
            /*  Even though we told the hash to kill our list           */
            /*  It doesnt know how to kill our list node values         */
            /*  So we kill each one of them in this iterative function  */
            /*  And we let the hash table think its doing the real work */
            nitf_HashTable_foreach((*ext)->hash, deleteHashChain, NULL, &e);

            /*  Now we kill the hash table  */
            nitf_HashTable_destruct(&((*ext)->hash));

        }
        /* These are all just dead references, so we just want
           to get rid of the list, not delete anything */
        if ((*ext)->ref)
        {
            nitf_List *l = (*ext)->ref;
            while (!nitf_List_isEmpty(l))
            {
                (void) nitf_List_popFront(l);
            }
            nitf_List_destruct(&l);
        }
        NITF_FREE(*ext);

        /*  Null-set this so it doesnt happen again  */
        *ext = NULL;
    }

}

NITFAPI(NITF_BOOL) nitf_Extensions_appendTRE(nitf_Extensions * ext,
        nitf_TRE * tre,
        nitf_Error * error)
{

    if (!insertToHash(ext->hash, tre->tag, tre, error))
        return NITF_FAILURE;

    /* Okay this is pretty unlikely */
    if (!nitf_List_pushBack(ext->ref, tre, error))
    {
        /* Im not addressing the potentially inconsistent state
           here.  The net effect is that if you can't push it on
           here, but you somehow got it in the hash, you can modify
           that till the cows come home, but it won't show up on a write */
        return NITF_FAILURE;
    }

    return NITF_SUCCESS;
}


NITFAPI(nitf_List *) nitf_Extensions_getTREsByName(nitf_Extensions * ext,
        const char *name)
{
    nitf_Pair *pair = nitf_HashTable_find(ext->hash, name);
    if (!pair)
        return NULL;
    else
    {
        return (nitf_List *) pair->data;
    }
}


NITFAPI(NITF_BOOL) nitf_Extensions_exists(nitf_Extensions * ext,
        const char *name)
{
    return nitf_HashTable_exists(ext->hash, name);
}



NITFAPI(void) nitf_Extensions_removeTREsByName(nitf_Extensions * ext,
        const char *name)
{
    /* Do LL first, its easy */
    nitf_Pair* pair;
    nitf_Error error;
    nitf_ListIterator iter = nitf_List_begin(ext->ref);
    nitf_ListIterator end = nitf_List_end(ext->ref);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        /* We remove this but don't delete it */
        nitf_TRE* tre = (nitf_TRE*)nitf_ListIterator_get(&iter);
        /*  If this reference is to a TRE of this name */
        if ( strcmp(tre->tag, name) == 0 )
        {
            /* Remove it, which advances the iterator */
            nitf_List_remove(ext->ref, &iter);
        }
        else
        {
            /* Advance the iterator */
            nitf_ListIterator_increment(&iter);
        }
    }
    /* Find the list that matches and obliterate it */
    pair = nitf_HashTable_find(ext->hash, name);

    (void) eraseIt(ext->hash, pair, NULL, &error);
    nitf_HashTable_remove(ext->hash, name);
}


NITFAPI(nitf_ExtensionsIterator) nitf_Extensions_begin(nitf_Extensions *
        ext)
{
    /*  The iterator to return  */
    nitf_ExtensionsIterator extIt;

    /*  Be ruthless with our assertions  */
    assert(ext);

    extIt.iter = nitf_List_begin(ext->ref);
    return extIt;
}


NITFAPI(nitf_ExtensionsIterator) nitf_Extensions_end(nitf_Extensions * ext)
{
    nitf_ExtensionsIterator extIt;

    /*  Be ruthless with our assertions  */
    assert(ext);
    extIt.iter = nitf_List_end(ext->ref);
    return extIt;
}


NITFAPI(void) nitf_ExtensionsIterator_increment(nitf_ExtensionsIterator *
        extIt)
{
    nitf_ListIterator_increment(&extIt->iter);
}


NITFAPI(nitf_TRE *) nitf_ExtensionsIterator_get(nitf_ExtensionsIterator *
        extIt)
{
    return (nitf_TRE *) nitf_ListIterator_get(&extIt->iter);
}

NITFAPI(NITF_BOOL) nitf_Extensions_insert(nitf_Extensions* ext,
        nitf_ExtensionsIterator* extIt,
        nitf_TRE* tre,
        nitf_Error* error)
{
    /* The easy part, put in list */
    if (!nitf_List_insert(ext->ref, extIt->iter, tre, error))
        return NITF_FAILURE;
    /* Now insert to hash */
    return insertToHash( ext->hash, tre->tag, tre, error);
}

NITFAPI(nitf_TRE *) nitf_Extensions_remove(nitf_Extensions* ext,
        nitf_ExtensionsIterator* extIt,
        nitf_Error* error)
{
    /* The easy part, remove from list */
    nitf_TRE* tre = nitf_List_remove(ext->ref, &(extIt->iter));
    nitf_List* treInstances;
    nitf_ListIterator it;
    nitf_ListIterator end;

    if ( tre == NULL )
    {
        nitf_Error_init(error, "Undefined TRE at iterator position",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NULL;
    }
    /* Trick here is to remove the correct TRE from the hash */

    /* Start by getting them all... */
    treInstances = nitf_Extensions_getTREsByName(ext, tre->tag);

    /* If this happens we have a big problem: */
    /* Somehow the reference list is different from the hash */
    assert( ! nitf_List_isEmpty(treInstances) );


    it = nitf_List_begin( treInstances );
    end = nitf_List_end( treInstances );

    /* Compare TRE pointers, and drop the right one */
    while ( nitf_ListIterator_notEqualTo(&it, &end) )
    {

        if ( tre == (nitf_TRE*)nitf_ListIterator_get(&it) )
        {
            nitf_List_remove(treInstances, &it);
            break;
        }
        nitf_ListIterator_increment(&it);
    }

    /* Then if the list is empty, blow away the hash entry */
    if (nitf_List_isEmpty(treInstances))
    {
        nitf_List_destruct(&treInstances);
        nitf_HashTable_remove(ext->hash, tre->tag);
    }

    return tre;
}

NITFAPI(NITF_BOOL) nitf_ExtensionsIterator_equals(nitf_ExtensionsIterator *
        it1,
        nitf_ExtensionsIterator *
        it2)
{
    return nitf_ListIterator_equals(&it1->iter, &it2->iter);
}


NITFAPI(NITF_BOOL)
nitf_ExtensionsIterator_notEqualTo(nitf_ExtensionsIterator * it1,
                                   nitf_ExtensionsIterator * it2)
{
    return nitf_ListIterator_notEqualTo(&it1->iter, &it2->iter);
}


NITFAPI(nitf_Uint32) nitf_Extensions_computeLength
(nitf_Extensions * ext, nitf_Version fver, nitf_Error * error)
{
    nitf_Uint32 dataLength = 0;
    nitf_ExtensionsIterator iter, end;
    nitf_TRE *tre;

    if (ext != NULL)
    {
        /* set up iterators */
        iter = nitf_Extensions_begin(ext);
        end = nitf_Extensions_end(ext);

        /* First, figure out if we have to write any TREs */
        while (nitf_ExtensionsIterator_notEqualTo(&iter, &end))
        {
            tre = (nitf_TRE *) nitf_ExtensionsIterator_get(&iter);

            /* if unknown length, we need to compute it */
            dataLength += (nitf_Uint32)tre->handler->getCurrentSize(tre, error);
            dataLength += NITF_ETAG_SZ + NITF_EL_SZ;

            /* increment */
            nitf_ExtensionsIterator_increment(&iter);
        }
    }
    return dataLength;
}

NITFPRIV(NITF_BOOL) insertToHash( nitf_HashTable* hash, const char* name,
                                  nitf_TRE* tre, nitf_Error* error)
{

    int createdTREList = 0;
    nitf_List* tres = NULL;
    nitf_Pair* pair = nitf_HashTable_find(hash, name);
    /* First thing's first, do we have a list or don't we? */

    if (pair)
    {
        tres = (nitf_List*)pair->data;
    }
    else
    {
        createdTREList = 1;
        tres = nitf_List_construct(error);
        if (! tres )
        {
            return NITF_FAILURE;
        }
        if (!nitf_HashTable_insert(hash, name, (NITF_DATA *) tres, error))
        {

            nitf_List_destruct(&tres);
            return NITF_FAILURE;
        }
    }

    /* Now that we know we have a list, make sure we insert into it */
    if ( !nitf_List_pushBack( tres, tre, error ) )
    {
        if ( createdTREList )
        {
            nitf_List_destruct(&tres);
            nitf_HashTable_remove(hash, name );
        }
        return NITF_FAILURE;
    }

    return NITF_SUCCESS;

}

NITFPRIV(int) eraseIt(nitf_HashTable * ht,
                      nitf_Pair * pair, NITF_DATA* userData, nitf_Error * error)
{
    if (!pair)
        return 0;
    else
    {

        nitf_List *list = (nitf_List *) pair->data;
        nitf_ListIterator iter = nitf_List_begin(list);
        nitf_ListIterator end = nitf_List_end(list);
        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {

            /*  This auto-increments for us  */
            nitf_TRE *tre = (nitf_TRE *) nitf_List_remove(list, &iter);
            nitf_TRE_destruct(&tre);
        }

        nitf_List_destruct((nitf_List **) & (pair->data));
        return 1;
    }
}
