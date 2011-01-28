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

#include "nrt/HashTable.h"

NRTAPI(nrt_HashTable *) nrt_HashTable_construct(int nbuckets, nrt_Error * error)
{
    int i;
    int hashSize;

    /* Create the hash table object itself */
    nrt_HashTable *ht = (nrt_HashTable *) NRT_MALLOC(sizeof(nrt_HashTable));
    if (!ht)
    {
        /* If we had problems, error population and return */
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NULL;
    }

    /* Adopt the data by default */
    ht->adopt = NRT_DATA_ADOPT;

    /* Size of all of our chains */
    hashSize = sizeof(nrt_List) * nbuckets;

    /* Dont forget to set nbuckets in the object */
    ht->nbuckets = nbuckets;

    /* Allocate the list of lists (still need to allocate each list */
    ht->buckets = (nrt_List **) NRT_MALLOC(hashSize);
    if (!ht->buckets)
    {
        /* If we had problems, error population, and */
        /* free the object we have and return */
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        /* Dont bother with the destructor */
        NRT_FREE(ht);
        return NULL;
    }
    /* Make sure if we have to call the destructor we are good */
    memset(ht->buckets, 0, hashSize);

    /* Foreach list, construct */
    for (i = 0; i < nbuckets; i++)
    {

        ht->buckets[i] = nrt_List_construct(error);

        /* Our destructor is smart enough to delete only non-NULL vals */
        /* Not only that the constructor populated our error already */
        if (!ht->buckets[i])
        {
            nrt_HashTable_destruct(&ht);
            return NULL;
        }
    }

    /* Set ourselves up with a default hash */
    /* We can always change it !! */
    nrt_HashTable_initDefaults(ht);

    /* We are good, so return */
    return ht;
}

NRTAPI(void) nrt_HashTable_setPolicy(nrt_HashTable * ht, int policy)
{
    assert(ht);
    ht->adopt = policy;
}

unsigned int __NRT_HashTable_defaultHash(nrt_HashTable * ht, const char *key)
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

NRTAPI(void) nrt_HashTable_initDefaults(nrt_HashTable * ht)
{
    /* Point our hash function at the default */
    ht->hash = &__NRT_HashTable_defaultHash;
}

NRTAPI(void) nrt_HashTable_destruct(nrt_HashTable ** ht)
{
    /* If the hash table exists at all */
    if (*ht)
    {
        /* If the linked list of lists exists */
        if ((*ht)->buckets)
        {
            int i;

            /* This is tricky.  Because we are allocating */
            /* the list first, we may have */
            for (i = 0; i < (*ht)->nbuckets; i++)
            {
                nrt_List *l = (*ht)->buckets[i];

                /* If this list is alive, we need to get rid of */
                /* its keys and then delete it */
                if (l != NULL)
                {
                    /* While we have elements */
                    while (!nrt_List_isEmpty(l))
                    {
                        /* Get the next element */
                        nrt_Pair *pair = (nrt_Pair *) nrt_List_popFront(l);

                        /* If there is a pair */
                        if (pair)
                        {
                            /* Extract the key data, which we KNOW */
                            /* That was dynamically allocated by us */
                            char *key = pair->key;

                            /* If its there */
                            if (key)
                            {
                                /* Free and NULL it */
                                NRT_FREE(key);
                            }
                            /* If the adoption policy is to adopt...  */
                            if ((*ht)->adopt)
                            {
                                /* Then we have to do deletion ourselves */
                                NRT_DATA *data = pair->data;
                                /* If the data exists */
                                if (data)
                                {
                                    /* Free the data and NULL it */
                                    NRT_FREE(data);
                                }
                            }
                            /* Finally, we know that we allocated the */
                            /* pair, so lets free it */
                            NRT_FREE(pair);
                        }
                    }
                    /* Now the list is empty, let's destroy it */
                    nrt_List_destruct(&((*ht)->buckets[i]));
                }
                /* Now go on to the next bucket */
            }
            NRT_FREE((*ht)->buckets);
        }

        NRT_FREE(*ht);
        *ht = NULL;
    }
}

NRTAPI(NRT_BOOL) nrt_HashTable_exists(nrt_HashTable * ht, const char *key)
{
    /* This is a SERIOUS copout!!! */
    if (nrt_HashTable_find(ht, key) == NULL)
        return NRT_FAILURE;
    return NRT_SUCCESS;
}

NRTAPI(NRT_DATA *) nrt_HashTable_remove(nrt_HashTable * ht, const char *key)
{
    /* Find out which list it would be in */
    int bucket = ht->hash(ht, key);

    /* Get the list at this bucket */
    nrt_List *l = ht->buckets[bucket];

    /* Set our iterator to the beginning */
    nrt_ListIterator iter = nrt_List_begin(l);

    /* Set an iterator at the end */
    nrt_ListIterator end = nrt_List_end(l);

    /* While the list iterator hasnt reached the end */
    while (nrt_ListIterator_notEqualTo(&iter, &end))
    {
        /* Get the pair at this node */
        nrt_Pair *pair = (nrt_Pair *) nrt_ListIterator_get(&iter);

        /* We'll see */
        assert(pair);

        /* We found a match, remove and go home */
        if (strcmp(pair->key, key) == 0)
        {
            NRT_DATA *data = pair->data;

            /* Remove at this point from l */
            nrt_List_remove(l, &iter);

            /* Delete the key -- that's ours */
            NRT_FREE(pair->key);

            /* Free the pair */
            NRT_FREE(pair);

            /* Return the value -- that's yours */
            return data;
        }
        /* We didn't get a match, so lets increment again */
        nrt_ListIterator_increment(&iter);
    }
    /* We slipped out of the while loop unsuccessfully */
    return NULL;
}

NRTPRIV(int) printIt(nrt_HashTable * ht, nrt_Pair * pair, NRT_DATA * userData,
                     nrt_Error * error)
{
#ifdef NRT_DEBUG
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

NRTAPI(void) nrt_HashTable_print(nrt_HashTable * ht)
{
    nrt_Error e;
    NRT_HASH_FUNCTOR fn = printIt;
    nrt_HashTable_foreach(ht, fn, NULL, &e);
}

NRTAPI(NRT_BOOL) nrt_HashTable_foreach(nrt_HashTable * ht, NRT_HASH_FUNCTOR fn,
                                       NRT_DATA * userData, nrt_Error * error)
{
    int i;
    for (i = 0; i < ht->nbuckets; i++)
    {
        nrt_List *l = ht->buckets[i];
        nrt_ListIterator iter = nrt_List_begin(l);
        nrt_ListIterator end = nrt_List_end(l);
        while (nrt_ListIterator_notEqualTo(&iter, &end))
        {
            nrt_Pair *pair = (nrt_Pair *) nrt_ListIterator_get(&iter);
            if (!(*fn) (ht, pair, userData, error))
                return 0;
            nrt_ListIterator_increment(&iter);
        }
    }
    return 1;
}

NRTAPI(nrt_HashTable *) nrt_HashTable_clone(nrt_HashTable * source,
                                            NRT_DATA_ITEM_CLONE cloner,
                                            nrt_Error * error)
{
    int i;
    /* This is the simplest way of setting up the hash */
    nrt_HashTable *ht = NULL;

    if (source)
    {
        ht = nrt_HashTable_construct(source->nbuckets, error);
        if (!ht)
            return NULL;

        /* Make sure the policy is the same! */
        ht->adopt = source->adopt;

        /* Now we want to walk the list and insert items into the hash */
        for (i = 0; i < source->nbuckets; i++)
        {
            nrt_List *l = source->buckets[i];
            nrt_ListIterator iter = nrt_List_begin(l);
            nrt_ListIterator end = nrt_List_end(l);
            while (nrt_ListIterator_notEqualTo(&iter, &end))
            {
                /* Foreach item in each list... */
                nrt_Pair *pair = (nrt_Pair *) nrt_ListIterator_get(&iter);

                /* Use the function pointer to clone the object...  */
                NRT_DATA *newData = (NRT_DATA *) cloner(pair->data, error);
                if (!newData)
                {
                    nrt_HashTable_destruct(&ht);
                    return NULL;
                }

                /* ... and then insert it with the key into the new table */
                if (!nrt_HashTable_insert(ht, pair->key, newData, error))
                {
                    nrt_HashTable_destruct(&ht);
                    return NULL;
                }

                nrt_ListIterator_increment(&iter);
            }
        }
    }
    else
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INVALID_OBJECT,
                        "Trying to clone NULL pointer");
    }
    return ht;
}

NRTAPI(NRT_BOOL) nrt_HashTable_insert(nrt_HashTable * ht, const char *key,
                                      NRT_DATA * data, nrt_Error * error)
{
    /* Find the bucket */
    int bucket = ht->hash(ht, key);

    /* Malloc the pair -- that's our container item */
    nrt_Pair *p = (nrt_Pair *) NRT_MALLOC(sizeof(nrt_Pair));
    if (!p)
    {
        /* There was a memory allocation error */
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        /* Retreat! */
        return 0;
    }

    /* Initialize the new pair */
    /* This makes a copy of the key, but uses the data directly */
    nrt_Pair_init(p, key, data);

    /* Push the pair back into the list */
    return nrt_List_pushBack(ht->buckets[bucket], p, error);
}

NRTAPI(nrt_Pair *) nrt_HashTable_find(nrt_HashTable * ht, const char *key)
{
    /* Retrieve the pocket */
    int bucket = ht->hash(ht, key);

    /* Get the list for it */
    nrt_List *l = ht->buckets[bucket];

    /* Get an iterator to the front */
    nrt_ListIterator iter = nrt_List_begin(l);

    /* Point one at the back */
    nrt_ListIterator end = nrt_List_end(l);

    /* While we are retrieving...  */
    while (nrt_ListIterator_notEqualTo(&iter, &end))
    {

        nrt_Pair *pair = (nrt_Pair *) nrt_ListIterator_get(&iter);

        /* Should NOT get a null pair */
        assert(pair);

        /* We have a match!!! */
        if (strcmp(pair->key, key) == 0)
        {
            return pair;
        }
        /* Otherwise, increment */
        nrt_ListIterator_increment(&iter);
    }
    return NULL;
}

NRTAPI(nrt_HashTableIterator) nrt_HashTable_begin(nrt_HashTable * ht)
{
    nrt_HashTableIterator hash_iterator;
    /* Be ruthless with our assertions */
    assert(ht);

    hash_iterator.curBucket = -1;       /* default to the 'end' */
    hash_iterator.listIter.current = NULL;
    hash_iterator.hash = ht;

    if (ht->buckets)
    {
        int i;
        for (i = 0; i < ht->nbuckets && hash_iterator.curBucket < 0; i++)
        {
            nrt_List *l = ht->buckets[i];
            if (l != NULL && nrt_List_size(l) > 0)
            {
                hash_iterator.curBucket = i;
                hash_iterator.listIter = nrt_List_begin(l);
            }
        }
    }
    return hash_iterator;
}

NRTAPI(nrt_HashTableIterator) nrt_HashTable_end(nrt_HashTable * ht)
{
    nrt_HashTableIterator hash_iterator;
    hash_iterator.curBucket = -1;
    hash_iterator.listIter.current = NULL;
    hash_iterator.hash = ht;
    return hash_iterator;
}

NRTAPI(NRT_BOOL) nrt_HashTableIterator_equals(nrt_HashTableIterator * it1,
                                              nrt_HashTableIterator * it2)
{
    return it1->curBucket == it2->curBucket
        && nrt_ListIterator_equals(&it1->listIter, &it2->listIter)
        && it1->hash == it2->hash;
}

NRTAPI(NRT_BOOL) nrt_HashTableIterator_notEqualTo(nrt_HashTableIterator * it1,
                                                  nrt_HashTableIterator * it2)
{
    return !nrt_HashTableIterator_equals(it1, it2);
}

NRTAPI(void) nrt_HashTableIterator_increment(nrt_HashTableIterator * iter)
{
    NRT_BOOL found = 0;
    if (iter->hash->buckets && iter->curBucket >= 0
        && iter->curBucket < iter->hash->nbuckets)
    {
        nrt_List *l = iter->hash->buckets[iter->curBucket];
        nrt_ListIterator end = nrt_List_end(l);
        nrt_ListIterator_increment(&iter->listIter);
        if (nrt_ListIterator_notEqualTo(&iter->listIter, &end))
            found = 1;
        else
        {
            int i;
            for (i = iter->curBucket + 1; i < iter->hash->nbuckets && !found;
                 i++)
            {
                nrt_List *l = iter->hash->buckets[i];
                if (l != NULL && nrt_List_size(l) > 0)
                {
                    iter->curBucket = i;
                    iter->listIter = nrt_List_begin(l);
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

NRTAPI(nrt_Pair *) nrt_HashTableIterator_get(nrt_HashTableIterator * iter)
{
    if (iter->curBucket >= 0 && iter->listIter.current)
        return (nrt_Pair *) nrt_ListIterator_get(&iter->listIter);
    return NULL;
}
