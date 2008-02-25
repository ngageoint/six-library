/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#include "nitf/HashTable.h"

NITFAPI(nitf_HashTable *) nitf_HashTable_construct(int nbuckets,
        nitf_Error * error)
{
    int i;
    int hashSize;

    /*  Create the hash table object itself  */
    nitf_HashTable *ht =
        (nitf_HashTable *) NITF_MALLOC(sizeof(nitf_HashTable));
    if (!ht)
    {
        /*  If we had problems, error population and return  */
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /*  Adopt the data by default  */
    ht->adopt = NITF_DATA_ADOPT;

    /*  Size of all of our chains  */
    hashSize = sizeof(nitf_List) * nbuckets;

    /*  Dont forget to set nbuckets in the object  */
    ht->nbuckets = nbuckets;

    /*  Allocate the list of lists (still need to allocate each list */
    ht->buckets = (nitf_List **) NITF_MALLOC(hashSize);
    if (!ht->buckets)
    {
        /*  If we had problems, error population, and  */
        /*  free the object we have and return         */
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        /*  Dont bother with the destructor  */
        NITF_FREE(ht);
        return NULL;
    }
    /*  Make sure if we have to call the destructor we are good   */
    memset(ht->buckets, 0, hashSize);

    /*  Foreach list, construct */
    for (i = 0; i < nbuckets; i++)
    {

        ht->buckets[i] = nitf_List_construct(error);

        /*  Our destructor is smart enough to delete only non-NULL vals  */
        /*  Not only that the constructor populated our error already    */
        if (!ht->buckets[i])
        {
            nitf_HashTable_destruct(&ht);
            return NULL;
        }
    }

    /*  Set ourselves up with a default hash  */
    /*  We can always change it !!            */
    nitf_HashTable_initDefaults(ht);

    /*  We are good, so return  */
    return ht;
}


NITFAPI(void) nitf_HashTable_setPolicy(nitf_HashTable * ht, int policy)
{
    assert(ht);
    ht->adopt = policy;
}


unsigned int __nitf_HashTable_defaultHash(nitf_HashTable * ht,
        const char *key)
{
    const char *p = key;
    const char *end = &key[strlen(key) - 1];
    char c;
    int hash = 0;
    
    while (p < end)
    {
        c = *p++;

        if (c > 0140)
            c -= 40;
        hash = ((hash << 3) + (hash >> 28) + c);
    }
    return (unsigned) ((hash & 07777777777) % ht->nbuckets);
}


NITFAPI(void) nitf_HashTable_initDefaults(nitf_HashTable * ht)
{
    /*  Point our hash function at the default  */
    ht->hash = &__nitf_HashTable_defaultHash;
}


NITFAPI(void) nitf_HashTable_destruct(nitf_HashTable ** ht)
{
    /*  If the hash table exists at all  */
    if (*ht)
    {
        /*  If the linked list of lists exists  */
        if ((*ht)->buckets)
        {
            int i;

            /*  This is tricky.  Because we are allocating  */
            /*  the list first, we may have */
            for (i = 0; i < (*ht)->nbuckets; i++)
            {
                nitf_List *l = (*ht)->buckets[i];

                /*  If this list is alive, we need to get rid of  */
                /*  its keys and then delete it                   */
                if (l != NULL)
                {
                    /*  While we have elements  */
                    while (!nitf_List_isEmpty(l))
                    {
                        /*  Get the next element  */
                        nitf_Pair *pair =
                            (nitf_Pair *) nitf_List_popFront(l);

                        /*  If there is a pair  */
                        if (pair)
                        {
                            /*  Extract the key data, which we KNOW   */
                            /*  That was dynamically allocated by us  */
                            char *key = pair->key;

                            /*  If its there  */
                            if (key)
                            {
                                /*  Free and NULL it  */
                                NITF_FREE(key);
                            }
                            /*  If the adoption policy is to adopt...  */
                            if ((*ht)->adopt)
                            {
                                /*  Then we have to do deletion ourselves  */
                                NITF_DATA *data = pair->data;
                                /*  If the data exists  */
                                if (data)
                                {
                                    /*  Free the data and NULL it  */
                                    NITF_FREE(data);
                                }
                            }
                            /*  Finally, we know that we allocated the  */
                            /*  pair, so lets free it                   */
                            NITF_FREE(pair);
                        }
                    }
                    /*  Now the list is empty, let's destroy it  */
                    nitf_List_destruct(&((*ht)->buckets[i]));
                }
                /*  Now go on to the next bucket  */
            }
            NITF_FREE((*ht)->buckets);
        }

        NITF_FREE(*ht);
        *ht = NULL;
    }
}


NITFAPI(NITF_BOOL) nitf_HashTable_exists(nitf_HashTable * ht,
        const char *key)
{
    /*  This is a SERIOUS copout!!!  */
    if (nitf_HashTable_find(ht, key) == NULL)
        return NITF_FAILURE;
    return NITF_SUCCESS;
}


NITFAPI(NITF_DATA *) nitf_HashTable_remove(nitf_HashTable * ht,
        const char *key)
{
    /*  Find out which list it would be in  */
    int bucket = ht->hash(ht, key);

    /*  Get the list at this bucket  */
    nitf_List *l = ht->buckets[bucket];

    /*  Set our iterator to the beginning  */
    nitf_ListIterator iter = nitf_List_begin(l);

    /*  Set an iterator at the end  */
    nitf_ListIterator end = nitf_List_end(l);

    /*  While the list iterator hasnt reached the end  */
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        /*  Get the pair at this node  */
        nitf_Pair *pair = (nitf_Pair *) nitf_ListIterator_get(&iter);

        /*  We'll see  */
        assert(pair);

        /*  We found a match, remove and go home  */
        if (strcmp(pair->key, key) == 0)
        {
            NITF_DATA *data = pair->data;

            /*  Remove at this point from l  */
            nitf_List_remove(l, &iter);

            /*  Delete the key -- that's ours  */
            NITF_FREE(pair->key);

            /* Free the pair */
            NITF_FREE(pair);

            /*  Return the value -- that's yours  */
            return data;
        }
        /*  We didn't get a match, so lets increment again  */
        nitf_ListIterator_increment(&iter);
    }
    /*  We slipped out of the while loop unsuccessfully  */
    return NULL;
}


NITFPRIV(int) printIt(nitf_HashTable * ht,
                      nitf_Pair * pair, NITF_DATA* userData, nitf_Error * error)
{
#ifdef NITF_DEBUG
    if (pair)
    {
        printf("{%p} [%s]: '%p'\n", pair, pair->key, pair->data);
    }
    else
    {
        printf("No pair defined at iter pos!\n");
    }
#endif
    return 1;
}


NITFAPI(void) nitf_HashTable_print(nitf_HashTable * ht)
{
    nitf_Error e;
    NITF_HASH_FUNCTOR fn = printIt;
    nitf_HashTable_foreach(ht, fn, NULL, &e);
}


NITFAPI(NITF_BOOL) nitf_HashTable_foreach(nitf_HashTable * ht,
        NITF_HASH_FUNCTOR fn,
        NITF_DATA* userData,
        nitf_Error * error)
{
    int i;
    for (i = 0; i < ht->nbuckets; i++)
    {
        nitf_List *l = ht->buckets[i];
        nitf_ListIterator iter = nitf_List_begin(l);
        nitf_ListIterator end = nitf_List_end(l);
        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_Pair *pair = (nitf_Pair *) nitf_ListIterator_get(&iter);
            if (!(*fn) (ht, pair, userData, error))
                return 0;
            nitf_ListIterator_increment(&iter);
        }
    }
    return 1;
}


NITFAPI(nitf_HashTable *) nitf_HashTable_clone(nitf_HashTable * source,
        NITF_DATA_ITEM_CLONE cloner,
        nitf_Error * error)
{
    int i;
    /*  This is the simplest way of setting up the hash  */
    nitf_HashTable *ht = NULL;

    if (source)
    {
        ht = nitf_HashTable_construct(source->nbuckets, error);
        if (!ht)
            return NULL;

        /*  Make sure the policy is the same!  */
        ht->adopt = source->adopt;

        /*  Now we want to walk the list and insert items into the hash */
        for (i = 0; i < source->nbuckets; i++)
        {
            nitf_List *l = source->buckets[i];
            nitf_ListIterator iter = nitf_List_begin(l);
            nitf_ListIterator end = nitf_List_end(l);
            while (nitf_ListIterator_notEqualTo(&iter, &end))
            {
                /*  Foreach item in each list... */
                nitf_Pair *pair =
                    (nitf_Pair *) nitf_ListIterator_get(&iter);

                /*  Use the function pointer to clone the object...  */
                NITF_DATA *newData =
                    (NITF_DATA *) cloner(pair->data, error);
                if (!newData)
                {
                    nitf_HashTable_destruct(&ht);
                    return NULL;
                }

                /*  ... and then insert it with the key into the new table  */
                if (!nitf_HashTable_insert(ht, pair->key, newData, error))
                {
                    nitf_HashTable_destruct(&ht);
                    return NULL;
                }

                nitf_ListIterator_increment(&iter);
            }
        }
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
    }
    return ht;
}


NITFAPI(NITF_BOOL) nitf_HashTable_insert(nitf_HashTable * ht,
        const char *key,
        NITF_DATA * data,
        nitf_Error * error)
{
    /*  Find the bucket  */
    int bucket = ht->hash(ht, key);

    /*  Malloc the pair -- that's our container item  */
    nitf_Pair *p = (nitf_Pair *) NITF_MALLOC(sizeof(nitf_Pair));
    if (!p)
    {
        /*  There was a memory allocation error  */
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        /*  Retreat!  */
        return 0;
    }

    /*  Initialize the new pair  */
    /*  This makes a copy of the key, but uses the data directly  */
    nitf_Pair_init(p, key, data);

    /*  Push the pair back into the list  */
    return nitf_List_pushBack(ht->buckets[bucket], p, error);
}


NITFAPI(nitf_Pair *)
nitf_HashTable_find(nitf_HashTable * ht, const char *key)
{
    /*  Retrieve the pocket  */
    int bucket = ht->hash(ht, key);

    /*  Get the list for it  */
    nitf_List *l = ht->buckets[bucket];

    /*  Get an iterator to the front   */
    nitf_ListIterator iter = nitf_List_begin(l);

    /*  Point one at the back  */
    nitf_ListIterator end = nitf_List_end(l);

    /*  While we are retrieving...  */
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {

        nitf_Pair *pair = (nitf_Pair *) nitf_ListIterator_get(&iter);

        /*  Should NOT get a null pair  */
        assert(pair);

        /*  We have a match!!!  */
        if (strcmp(pair->key, key) == 0)
        {
            return pair;
        }
        /*  Otherwise, increment  */
        nitf_ListIterator_increment(&iter);
    }
    return NULL;
}




NITFAPI(nitf_HashTableIterator) nitf_HashTable_begin(nitf_HashTable * ht)
{
    nitf_HashTableIterator hash_iterator;
    /*  Be ruthless with our assertions  */
    assert(ht);
    
    hash_iterator.curBucket = -1; /* default to the 'end' */
    hash_iterator.listIter.current = NULL;
    hash_iterator.hash = ht;
    
    if (ht->buckets)
    {
        int i;
        for (i = 0; i < ht->nbuckets && hash_iterator.curBucket < 0; i++)
        {
            nitf_List *l = ht->buckets[i];
            if (l != NULL && nitf_List_size(l) > 0)
            {
                hash_iterator.curBucket = i;
                hash_iterator.listIter = nitf_List_begin(l);
            }
        }
    }
    return hash_iterator;
}


NITFAPI(nitf_HashTableIterator) nitf_HashTable_end(nitf_HashTable * ht)
{
    nitf_HashTableIterator hash_iterator;
    hash_iterator.curBucket = -1;
    hash_iterator.listIter.current = NULL;
    hash_iterator.hash = ht;
    return hash_iterator;
}



NITFAPI(NITF_BOOL) nitf_HashTableIterator_equals(nitf_HashTableIterator * it1,
        nitf_HashTableIterator * it2)
{
    return it1->curBucket == it2->curBucket
        && nitf_ListIterator_equals(&it1->listIter, &it2->listIter)
        && it1->hash == it2->hash;
}


NITFAPI(NITF_BOOL) nitf_HashTableIterator_notEqualTo(nitf_HashTableIterator * it1,
        nitf_HashTableIterator * it2)
{
    return !nitf_HashTableIterator_equals(it1, it2);
}


NITFAPI(void) nitf_HashTableIterator_increment(nitf_HashTableIterator * iter)
{
    NITF_BOOL found = 0;
    if (iter->hash->buckets && iter->curBucket >= 0
        && iter->curBucket < iter->hash->nbuckets)
    {
        nitf_List *l = iter->hash->buckets[iter->curBucket];
        nitf_ListIterator end = nitf_List_end(l);
        nitf_ListIterator_increment(&iter->listIter);
        if (nitf_ListIterator_notEqualTo(&iter->listIter, &end))
            found = 1;
        else
        {
            int i;
            for (i = iter->curBucket + 1; i < iter->hash->nbuckets && !found; i++)
            {
                nitf_List *l = iter->hash->buckets[i];
                if (l != NULL && nitf_List_size(l) > 0)
                {
                    iter->curBucket = i;
                    iter->listIter = nitf_List_begin(l);
                    found = 1;
                }
            }
        }
    }
    
    if (!found)
    {
        /* set to "end" */
        iter->curBucket = -1;
        iter->listIter.current = NULL;
    }
}

NITFAPI(nitf_Pair*) nitf_HashTableIterator_get(nitf_HashTableIterator * iter)
{
    if (iter->curBucket >= 0 && iter->listIter.current)
        return (nitf_Pair*)nitf_ListIterator_get(&iter->listIter);
    return NULL;
}

