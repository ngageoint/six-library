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

#ifndef __NITF_HASH_TABLE_H__
#define __NITF_HASH_TABLE_H__

#include "nitf/Pair.h"
#include "nitf/List.h"

/*  How many chains are in the hashes, by default  */
#   define NITF_TRE_HASH_SIZE 8

NITF_CXX_GUARD

/*!
 *  These are values for ownership.  You may elect either policy.
 *  Here is the difference.  In the hash table, the data field is
 *  a pointer to an object.  That object may have been allocated by
 *  you, or it may be static, I dont know.  You may wish to remove it
 *  yourself, or I may be told to remove it for you.  If you do not tell
 *  me, I will assume that you own it.
 */
enum { NITF_DATA_RETAIN_OWNER = 0, NITF_DATA_ADOPT = 1 };

/*!
 *  \struct nitf_HashTable
 *  \brief The hash table structure
 *
 *  This represents a non-unique hash table structure.
 */
typedef struct _nitf_HashTable
{
    nitf_List **buckets;
    int nbuckets;
    int adopt;
    unsigned int (*hash) (struct _nitf_HashTable *, const char *);
}
nitf_HashTable;

typedef unsigned int (*NITF_HASH_FUNCTION) (nitf_HashTable *,
        const char *);

typedef int (*NITF_HASH_FUNCTOR) (nitf_HashTable *, nitf_Pair * pair,
                                  NITF_DATA* userData, nitf_Error * error);


/*!
 *  \struct nitf_HashTableIterator
 *  \brief This is a basic iterator object for a HashTable
 */
typedef struct _nitf_HashTableIterator
{
    nitf_HashTable *hash; /*! The hash this is an iterator for */
    int curBucket; /*! The current bucket */
    nitf_ListIterator listIter; /*! The iterator for the current bucket chain */
}
nitf_HashTableIterator;



/*!
 *  Constructor.  This creates the hash table.
 *
 *  \param nbuckets The size of the hash
 *  \param error An error to populate on failure
 *  \return NULL (on failure), or a pointer to the hash table
 */
NITFAPI(nitf_HashTable *) nitf_HashTable_construct(int nbuckets,
        nitf_Error * error);


/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param cloner A NITF_DATA_ITEM_CLONE function that gets called foreach
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_HashTable *) nitf_HashTable_clone(nitf_HashTable * source,
        NITF_DATA_ITEM_CLONE cloner,
        nitf_Error * error);


/*!
 *  This function controls ownership.  You may elect either policy,
 *  NITF_DATA_ADOPT, or NITF_DATA_RETAIN_OWNER
 *  Here is the difference.  In the hash table, the data field is
 *  a pointer to an object.  That object may have been allocated by
 *  you, or it may be static, I dont know.  You may wish to remove it
 *  yourself, or I may be told to remove it for you.  If you do not tell
 *  me, I will assume that you own it.
 *  \param ht The hash table
 *  \param policy  The policy, either NITF_DATA_ADOPT
 *         or NITF_DATA_RETAIN_OWNER
 */
NITFAPI(void) nitf_HashTable_setPolicy(nitf_HashTable * ht, int policy);


/*!
 *  Remove data from the hash table.  Here is the removal policy:
 *  You ask for this object to be removed from the hash
 *    - We remove the pair from the hash and delete the pair associated
 *    with the storage
 *    - We give you back your data, if it exists
 *    - You are expected to delete the data we give you
 *
 *
 *  \note It doesnt matter if you asked us to adopt your object, if you
 *  remove it prematurely, you are required to delete it yourself.
 *
 *  \param ht The hash table
 *  \param key The key to search for
 *  \return The data for deletion.  If you remove it, you HAVE to delete it
 *
 */
NITFAPI(NITF_DATA *) nitf_HashTable_remove(nitf_HashTable * ht,
        const char *key);

/*!
 *  This is the default hashing function.  It gets bound when
 *  initDefaults() is called.  It is bound to the function pointer
 *  in the hash table
 *  \param ht The hash table object
 *  \param key The string key
 */
unsigned int __nitf_HashTable_defaultHash(nitf_HashTable * ht,
        const char *key);

/*!
 *  The default initializer.  This assigns the hash function to
 *  a "good default."
 *  \param ht The hash table
 *
 */
NITFAPI(void) nitf_HashTable_initDefaults(nitf_HashTable * ht);

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
 *      MAY be deleted (if policy is NITF_DATA_ADOPT).
 *
 *
 *
 *  The default behavior is to not adopt your data.  You need to change this
 *  if you wish for me to delete your data.
 *
 *  \param ht A pointer to a hash (ptr), so we can null it
 *
 */
NITFAPI(void) nitf_HashTable_destruct(nitf_HashTable ** ht);

/*!
 *  Check to see whether such a key exists in the hash table
 *  \param ht The hash to search
 *  \param key The key to lookup
 *  \return 1 if such a key exists, 0 if it does not
 */
NITFAPI(NITF_BOOL) nitf_HashTable_exists(nitf_HashTable * ht,
        const char *key);

/*!
 *  This is a debug tool to see what's in our hash
 *  \param ht The hash to print
 */
NITFAPI(void) nitf_HashTable_print(nitf_HashTable * ht);

/*!
 *  Foreach item in the hash table, do something (slow)
 *  \param ht The hash table
 *  \param fn The function to perform
 *  \param userData Optional user-defined data to pass to the functor
 *  \param error An error if one occurred
 *  \return Status
 */
NITFAPI(NITF_BOOL) nitf_HashTable_foreach(nitf_HashTable * ht,
        NITF_HASH_FUNCTOR fn,
        NITF_DATA* userData,
        nitf_Error * error);


/*!
 *  Insert this key/data pair into the hash table
 *  \param ht The hash table to insert to
 *  \param key The key to insert under
 *  \param data The data to insert into the second value of the pair
 *  \param error An error if one occurred
 *  \return 1 if success, 0 if failure
 */
NITFAPI(NITF_BOOL) nitf_HashTable_insert(nitf_HashTable * ht,
        const char *key,
        NITF_DATA * data,
        nitf_Error * error);

/*!
 *  Retrieve some key/value pair from the hash table
 *  \param ht The hash table to retrieve from
 *  \param key The key to retrieve by
 *  \return The chain link associated with the pair
 */
NITFAPI(nitf_Pair *)
nitf_HashTable_find(nitf_HashTable * ht, const char *key);



/*!
 *  Check to see if two iterators point at the same thing
 *
 *  \param it1  Iterator 1
 *  \param it2  Iterator 2
 *  \return 1 if they are equal, 0 if not
 */
NITFAPI(NITF_BOOL) nitf_HashTableIterator_equals(nitf_HashTableIterator * it1,
        nitf_HashTableIterator * it2);

/*!
 *  Check to see if two iterators are not pointing at the same thing
 *
 *  \param it1  Iterator 1
 *  \param it2  Iterator 2
 *  \return 1 if they are not equal, 0 if so
 */
NITFAPI(NITF_BOOL) nitf_HashTableIterator_notEqualTo(nitf_HashTableIterator * it1,
        nitf_HashTableIterator * it2);


/*!
 *  Return an iterator to the head of the chain
 *
 *  \param chain The chain to search
 *  \return An iterator to the head of the chain
 */
NITFAPI(nitf_HashTableIterator) nitf_HashTable_begin(nitf_HashTable * ht);


/*!
 *  Get an iterator to the tail of the chain
 *
 *  \param this_chain
 *  \return Iterator to chain tail
 */
NITFAPI(nitf_HashTableIterator) nitf_HashTable_end(nitf_HashTable * ht);


/*!
 *  Increment the iterator.  Eventually, this will point at NULL.
 *
 *  \param this_iter Iterator to increment
 *
 */
NITFAPI(void) nitf_HashTableIterator_increment(nitf_HashTableIterator * iter);


/*!
 *  Get the pair (key, value) this iterator points to
 *
 *  \return The data
 */
NITFAPI(nitf_Pair*) nitf_HashTableIterator_get(nitf_HashTableIterator * iter);


NITF_CXX_ENDGUARD

#endif
