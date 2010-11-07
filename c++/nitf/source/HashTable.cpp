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

#include "nitf/HashTable.hpp"

nitf::HashTableIterator & nitf::HashTableIterator::operator=(const nitf::HashTableIterator & x)
{
    if (&x != this)
        handle = x.handle;
    return *this;
}

nitf_HashTableIterator & nitf::HashTableIterator::getHandle() { return handle; }

bool nitf::HashTableIterator::equals(nitf::HashTableIterator& it2)
{
    NITF_BOOL x = nitf_HashTableIterator_equals(&handle, &it2.getHandle());
    if (!x) return false;
    return true;
}

bool nitf::HashTableIterator::operator==(const nitf::HashTableIterator& it2)
{
    return this->equals((nitf::HashTableIterator&)it2);
}

bool nitf::HashTableIterator::notEqualTo(nitf::HashTableIterator& it2)
{
    NITF_BOOL x = nitf_HashTableIterator_notEqualTo(&handle, &it2.getHandle());
    if (!x) return false;
    return true;
}

bool nitf::HashTableIterator::operator!=(const nitf::HashTableIterator& it2)
{
    return this->notEqualTo((nitf::HashTableIterator&)it2);
}

void nitf::HashTableIterator::increment() { nitf_HashTableIterator_increment(&handle); }

void nitf::HashTableIterator::operator++(int x) { increment(); }

nitf::HashTableIterator & nitf::HashTableIterator::operator+=(int x)
{
    for (int i = 0; i < x; ++i)
        increment();
    return *this;
}

nitf::HashTableIterator nitf::HashTableIterator::operator+(int x)
{
    nitf::HashTableIterator it = HashTableIterator(*this);
    it += x;
    return it;
}




nitf::HashTable::HashTable(const HashTable & x)
{
    setNative(x.getNative());
}

nitf::HashTable & nitf::HashTable::operator=(const HashTable & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}


nitf::HashTable::HashTable(nitf_HashTable * x)
{
    setNative(x);
    getNativeOrThrow();
}

nitf::HashTable::HashTable(int nbuckets) throw(nitf::NITFException)
{
    setNative(nitf_HashTable_construct(nbuckets, &error));
    getNativeOrThrow();
    setManaged(false);
}


nitf::HashTable nitf::HashTable::clone(NITF_DATA_ITEM_CLONE cloner) throw(nitf::NITFException)
{
    nitf::HashTable dolly(nitf_HashTable_clone(getNativeOrThrow(),
        cloner, &error));
    dolly.setManaged(false);
    return dolly;
}

void nitf::HashTable::setPolicy(int policy)
{
    nitf_HashTable_setPolicy(getNative(), policy);
}

NITF_DATA* nitf::HashTable::remove(const std::string& key)
{
    return nitf_HashTable_remove(getNative(), key.c_str());
}

void nitf::HashTable::initDefaults()
{
    nitf_HashTable_initDefaults(getNative());
}

nitf::HashTable::~HashTable(){}

bool nitf::HashTable::exists(const std::string& key)
{
    return nitf_HashTable_exists(getNative(), key.c_str());
}

void nitf::HashTable::print()
{
    nitf_HashTable_print(getNative());
}

void nitf::HashTable::foreach(HashIterator& fun, NITF_DATA* userData)
    throw(nitf::NITFException)
{
    int numBuckets = getNumBuckets();
    for (int i = 0; i < numBuckets; i++)
    {
        nitf::List l = getBucket(i);
        for (nitf::ListIterator iter = l.begin();
                iter != l.end(); ++iter)
        {
            nitf::Pair pair = nitf::Pair((nitf_Pair*)(*iter));
            fun(this, pair, userData);
        }
    }
}

void nitf::HashTable::insert(const std::string& key, NITF_DATA* data) throw(nitf::NITFException)
{
    if (key.length() == 0)
        throw except::NoSuchKeyException(Ctxt("Empty key value"));
    NITF_BOOL x = nitf_HashTable_insert(getNative(), key.c_str(), data, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

nitf::Pair nitf::HashTable::find(const std::string& key) throw(except::NoSuchKeyException)
{
    if (key.length() == 0)
        throw except::NoSuchKeyException(Ctxt("Empty key value"));
    nitf_Pair* x = nitf_HashTable_find(getNative(), key.c_str());
    if (!x)
        throw except::NoSuchKeyException(Ctxt(key));
    return nitf::Pair(x);
}

nitf::Pair nitf::HashTable::operator[] (const std::string& key) throw(except::NoSuchKeyException)
{
    return find(key);
}

nitf::List nitf::HashTable::getBucket(int i) throw(nitf::NITFException)
{
    //return *mBuckets.at(i);
    if (!getNativeOrThrow()->buckets || !getNativeOrThrow()->buckets[i])
        throw nitf::NITFException(Ctxt("No such bucket"));
    return nitf::List(getNativeOrThrow()->buckets[i]);

}

int nitf::HashTable::getNumBuckets() const
{
    return getNativeOrThrow()->nbuckets;
}

int nitf::HashTable::getAdopt() const
{
    return getNativeOrThrow()->adopt;
}

nitf::HashTableIterator nitf::HashTable::begin()
{
    nitf_HashTableIterator x = nitf_HashTable_begin(getNative());
    return nitf::HashTableIterator(x);
}

nitf::HashTableIterator nitf::HashTable::end()
{
    nitf_HashTableIterator x = nitf_HashTable_end(getNative());
    return nitf::HashTableIterator(x);
}

void nitf::HashTable::clearBuckets()
{
    std::vector<nitf::List *>::iterator i;
    i = mBuckets.begin();
    for (; i != mBuckets.end(); ++i)
    {
        if (*i) delete *i;
    }
    mBuckets.clear();
}
