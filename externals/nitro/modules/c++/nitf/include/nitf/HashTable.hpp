/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __NITF_HASHTABLE_HPP__
#define __NITF_HASHTABLE_HPP__
#pragma once

#include <string>
#include <vector>

#include "nitf/coda-oss.hpp"
#include "nitf/System.hpp"
#include "nitf/Pair.hpp"
#include "nitf/List.hpp"
#include "nitf/Object.hpp"

/*!
 *  \file HashTable.hpp
 *  \brief  Contains wrapper implementation for HashTable
 */

namespace nitf
{
class HashTable; //forward declaration

/*!
 * HashTable Iterator Functor
 */
class HashIterator
{
public:
    virtual ~HashIterator() {}
    virtual void operator()(nitf::HashTable* ht,
                            nitf::Pair& pair,
                            NITF_DATA* userData) = 0;
};


/*!
 *  \class HashTableIterator
 *  \brief  The C++ wrapper for the nitf_HashTableIterator
 *
 *  Iterates a hash table, unordered.
 */
struct HashTableIterator /*final*/   // no "final", SWIG doesn't like it
{
    HashTableIterator() = default;
    ~HashTableIterator() = default;

    //! Copy constructor
    HashTableIterator(const HashTableIterator & x) noexcept { handle = x.handle; }

    //! Assignment Operator
    HashTableIterator & operator=(const HashTableIterator & x) noexcept;

    //! Set native object
    HashTableIterator(nitf_HashTableIterator x) noexcept { setHandle(x); }

    //! Get native object
    nitf_HashTableIterator& getHandle() noexcept;
    nitf_HashTableIterator & getHandle() const noexcept;

    //! Check to see if two iterators are equal
    bool equals(const nitf::HashTableIterator& it2) const noexcept;

    //! Check to see if two iterators are not equal
    bool notEqualTo(const nitf::HashTableIterator& it2) const noexcept;

    //! Increment the iterator
    void increment() noexcept;

    bool operator==(const nitf::HashTableIterator& it2) const noexcept;

    bool operator!=(const nitf::HashTableIterator& it2) const noexcept;

    //! Increment the iterator (postfix);
    void operator++(int x) noexcept;

    //! Increment the iterator by a specified amount
    HashTableIterator & operator+=(int x) noexcept;

    HashTableIterator operator+(int x) noexcept;

    //! Increment the iterator (prefix);
    void operator++() noexcept { increment(); }

    //! Get the data
    nitf::Pair operator*() { return get(); }

    //! Get the data
    nitf::Pair get() const { return nitf_HashTableIterator_get(&handle); }


private:
    mutable nitf_HashTableIterator handle{};

    //! Set native object
    void setHandle(nitf_HashTableIterator x) noexcept  { handle = x; }
};


/*!
 *  \class HashTable
 *  \brief  The C++ wrapper for the nitf_HashTable
 */
DECLARE_CLASS(HashTable)
{
public:

    //! Copy constructor
    HashTable(const HashTable & x);

    //! Assignment Operator
    HashTable & operator=(const HashTable & x);

    //! Set native object
    HashTable(nitf_HashTable * x);

    /*!
     *  Constructor
     *  \param nbuckets  The size of the hash
     */
    HashTable(int nbuckets = 5);

    //! Clone
    nitf::HashTable clone(NITF_DATA_ITEM_CLONE cloner);

    /*!
     *  This function controls ownership.  You may elect either
     *  policy, NITF_DATA_ADOPT, or NITF_DATA_RETAIN_OWNER.
     *  Here is the difference.  In the hash table, the data field
     *  is a pointer to an object.  That object may have been allocated
     *  by you, or it may be static, I don't know.  You may wish to
     *  remove it yourself, or I may be told to remove it for you.
     *  If you do not tell me, I will assume that you own it.
     */
    void setPolicy(int policy) noexcept;

    /*!
     *  Remove data from the hash table.  Here is the removal policy:
     *  You ask for this object to be removed from the hash
     *    - We remove the pair from the hash and delete the pair
     *      associated with the storage
     *    - We give you back your data, if it exists
     *    - You are expected to delete the data we give you
     *
     */
    NITF_DATA* remove(const std::string& key) noexcept;

    /*!
     *  Assigns the hash function to a "good" default.
     */
    void initDefaults() noexcept;

    ~HashTable() = default;

    /*!
     *  Check to see whether such a key exists in the table
     *  \param  The key to lookup
     *  \return  True if exists, False otherwise.
     */
    bool exists(const std::string& key) const noexcept;

    /*!
     *  Debug tool to find out whats in the hash table
     */
    void print() const noexcept;

    /*!
     *  For each item in the hash table, do something (slow);
     *  \param fn  The function to perform
     */
    void forEach(HashIterator& fun, NITF_DATA* userData = nullptr);

    /*!
     *  Insert this key/data pair into the hash table
     *  \param key  The key to insert under
     *  \param data  The data to insert into the second value of the pair
     */
    void insert(const std::string& key, NITF_DATA* data);

    /*!
     * Templated insert, allowing us to insert nitf::Objects
     */
    template <typename T>
    void insert(const std::string& key, nitf::Object<T> object)
    {
        insert(key, (NITF_DATA*)object.getNative());
    }

    /*!
     *  Retrieve some key/value pair from the hash table
     *  \param key  The key to retrieve by
     *  \return  The key/value pair
     */
    nitf::Pair find(const std::string& key) const;

    nitf::Pair operator[] (const std::string& key);

    //! Get the buckets
    nitf::List getBucket(int i) const;

    //! Get the nbuckets
    int getNumBuckets() const;

    //! Get the adopt flag
    int getAdopt() const;

    /*!
     *  Get the begin iterator
     *  \return  The iterator pointing to the first item in the list
     */
    nitf::HashTableIterator begin() const noexcept;

    /*!
     *  Get the end iterator
     *  \return  The iterator pointing to PAST the last item in the list (null);
     */
    nitf::HashTableIterator end() const noexcept;

private:

    std::vector<nitf::List *> mBuckets;
    nitf_Error error{};

    //! Clear the buckets
    void clearBuckets();

};

}
#endif
