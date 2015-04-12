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

#ifndef __NRT_HASH_TABLE_H__
#define __NRT_HASH_TABLE_H__

#include "nrt/Pair.h"
#include "nrt/List.h"

NRT_CXX_GUARD
/*
 * These are values for ownership.  You may elect either policy.
 * Here is the difference.  In the hash table, the datafield is
 * a pointer to an object. That object may have been allocated by
 * you, or it may be static. You may wish to remove it
 * yourself, or it may be told to remove it for you.
 */
enum
{
    NRT_DATA_RETAIN_OWNER = 0, NRT_DATA_ADOPT = 1
};

/*!
 *  \struct nrt_HashTable
 *  \brief The hash table structure
 *
 *  This represents a non-unique hash table structure.
 */
typedef struct _NRT_HashTable
{
    nrt_List **buckets;
    int nbuckets;
    int adopt;
    unsigned int (*hash) (struct _NRT_HashTable *, const char *);
} nrt_HashTable;

typedef unsigned int (*NRT_HASH_FUNCTION) (nrt_HashTable *, const char *);

typedef int (*NRT_HASH_FUNCTOR) (nrt_HashTable *, nrt_Pair * pair,
                                 NRT_DATA * userData, nrt_Error * error);

/*!
 *  \struct nrt_HashTableIterator
 *  \brief This is a basic iterator object for a HashTable
 */
typedef struct _NRT_HashTableIterator
{
    nrt_HashTable *hash;        /* ! The hash this is an iterator for */
    int curBucket;              /* ! The current bucket */
    nrt_ListIterator listIter;  /* ! The iterator for the current bucket chain */
} nrt_HashTableIterator;

/*!
 *  Constructor.  This creates the hash table.
 *
 *  \param nbuckets The size of the hash
 *  \param error An error to populate on failure
 *  \return NULL (on failure), or a pointer to the hash table
 */
NRTAPI(nrt_HashTable *) nrt_HashTable_construct(int nbuckets,
                                                nrt_Error * error);

/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param cloner A NRT_DATA_ITEM_CLONE function that gets called foreach
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NRTAPI(nrt_HashTable *) nrt_HashTable_clone(nrt_HashTable * source,
                                            NRT_DATA_ITEM_CLONE cloner,
                                            nrt_Error * error);

/*!
 *  This function controls ownership.  You may elect either policy,
 *  NRT_DATA_ADOPT, or NRT_DATA_RETAIN_OWNER
 *  Here is the difference.  In the hash table, the data field is
 *  a pointer to an object.  That object may have been allocated by
 *  you, or it may be static, I dont know.  You may wish to remove it
 *  yourself, or I may be told to remove it for you.  If you do not tell
 *  me, I will assume that you own it.
 *  \param ht The hash table
 *  \param policy  The policy, either NRT_DATA_ADOPT
 *         or NRT_DATA_RETAIN_OWNER
 */
NRTAPI(void) nrt_HashTable_setPolicy(nrt_HashTable * ht, int policy);

/*!
 *  Remove data from the hash table.  Here is the removal policy:
 *  You ask for this object to be removed from the hash
 *    - We remove the pair from the hash and delete the pair associated
 *    with the storage
 *    - We give you back your data, if it exists
 *    - You are expected to delete the data we give you
 *
 *
 *  \note It doesn't matter if you asked us to adopt your object, if you
 *  remove it prematurely, you are required to delete it yourself.
 *
 *  \param ht The hash table
 *  \param key The key to search for
 *  \return The data for deletion.  If you remove it, you HAVE to delete it
 *
 */
NRTAPI(NRT_DATA *) nrt_HashTable_remove(nrt_HashTable * ht, const char *key);

/*!
 *  This is the default hashing function.  It gets bound when
 *  initDefaults() is called.  It is bound to the function pointer
 *  in the hash table
 *  \param ht The hash table object
 *  \param key The string key
 */
unsigned int __NRT_HashTable_defaultHash(nrt_HashTable * ht, const char *key);

/*!
 *  The default initializer.  This assigns the hash function to
 *  a "good default."
 *  \param ht The hash table
 *
 */
NRTAPI(void) nrt_HashTable_initDefaults(nrt_HashTable * ht);

/*!
 *  The destructor.  This function is fairly complex.  Here is its
 *  contract:
 *
 *  - A value MUST be non-zero to be deletable
 *  - A non-deletable value must be zero with the exception of data
 *  - If there is a hash it will be deleted
 *  - If there is a list of lists (buckets), they will be deleted
 *  - If there is a list within the list, it will be traversed
 *      # During traversal, foreach node, each pair will be deleted
 *      # In the process of deleting the pair, the key will be deleted
 *      # At the same time, depending on the policy, the data (the value)
 *      MAY be deleted (if policy is NRT_DATA_ADOPT).
 *
 *
 *
 *  The default behavior is to not adopt your data.  You need to change this
 *  if you wish for me to delete your data.
 *
 *  \param ht A pointer to a hash (ptr), so we can null it
 *
 */
NRTAPI(void) nrt_HashTable_destruct(nrt_HashTable ** ht);

/*!
 *  Check to see whether such a key exists in the hash table
 *  \param ht The hash to search
 *  \param key The key to lookup
 *  \return 1 if such a key exists, 0 if it does not
 */
NRTAPI(NRT_BOOL) nrt_HashTable_exists(nrt_HashTable * ht, const char *key);

/*!
 *  This is a debug tool to see what's in our hash
 *  \param ht The hash to print
 */
NRTAPI(void) nrt_HashTable_print(nrt_HashTable * ht);

/*!
 *  Foreach item in the hash table, do something (slow)
 *  \param ht The hash table
 *  \param fn The function to perform
 *  \param userData Optional user-defined data to pass to the functor
 *  \param error An error if one occurred
 *  \return Status
 */
NRTAPI(NRT_BOOL) nrt_HashTable_foreach(nrt_HashTable * ht, NRT_HASH_FUNCTOR fn,
                                       NRT_DATA * userData, nrt_Error * error);

/*!
 *  Insert this key/data pair into the hash table
 *  \param ht The hash table to insert to
 *  \param key The key to insert under
 *  \param data The data to insert into the second value of the pair
 *  \param error An error if one occurred
 *  \return 1 if success, 0 if failure
 */
NRTAPI(NRT_BOOL) nrt_HashTable_insert(nrt_HashTable * ht, const char *key,
                                      NRT_DATA * data, nrt_Error * error);

/*!
 *  Retrieve some key/value pair from the hash table
 *  \param ht The hash table to retrieve from
 *  \param key The key to retrieve by
 *  \return The chain link associated with the pair
 */
NRTAPI(nrt_Pair *) nrt_HashTable_find(nrt_HashTable * ht, const char *key);

/*!
 *  Check to see if two iterators point at the same thing
 *
 *  \param it1  Iterator 1
 *  \param it2  Iterator 2
 *  \return 1 if they are equal, 0 if not
 */
NRTAPI(NRT_BOOL) nrt_HashTableIterator_equals(nrt_HashTableIterator * it1,
                                              nrt_HashTableIterator * it2);

/*!
 *  Check to see if two iterators are not pointing at the same thing
 *
 *  \param it1  Iterator 1
 *  \param it2  Iterator 2
 *  \return 1 if they are not equal, 0 if so
 */
NRTAPI(NRT_BOOL) nrt_HashTableIterator_notEqualTo(nrt_HashTableIterator * it1,
                                                  nrt_HashTableIterator * it2);

/*!
 *  Return an iterator to the head of the chain
 *
 *  \param chain The chain to search
 *  \return An iterator to the head of the chain
 */
NRTAPI(nrt_HashTableIterator) nrt_HashTable_begin(nrt_HashTable * ht);

/*!
 *  Get an iterator to the tail of the chain
 *
 *  \param this_chain
 *  \return Iterator to chain tail
 */
NRTAPI(nrt_HashTableIterator) nrt_HashTable_end(nrt_HashTable * ht);

/*!
 *  Increment the iterator.  Eventually, this will point at NULL.
 *
 *  \param this_iter Iterator to increment
 *
 */
NRTAPI(void) nrt_HashTableIterator_increment(nrt_HashTableIterator * iter);

/*!
 *  Get the pair (key, value) this iterator points to
 *
 *  \return The data
 */
NRTAPI(nrt_Pair *) nrt_HashTableIterator_get(nrt_HashTableIterator * iter);

NRT_CXX_ENDGUARD
#endif
