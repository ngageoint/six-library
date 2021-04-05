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

#include "nitf/HashTable.hpp"

#include "nitf/NITFException.hpp"


nitf::HashTableIterator & nitf::HashTableIterator::operator=(const nitf::HashTableIterator & x) noexcept
{
    if (&x != this)
        handle = x.handle;
    return *this;
}

nitf_HashTableIterator & nitf::HashTableIterator::getHandle() noexcept { return handle; }
nitf_HashTableIterator& nitf::HashTableIterator::getHandle() const noexcept { return handle; }

bool nitf::HashTableIterator::equals(const nitf::HashTableIterator& it2) const noexcept
{
    const NITF_BOOL x = nitf_HashTableIterator_equals(&handle, &it2.getHandle());
    if (!x) return false;
    return true;
}

bool nitf::HashTableIterator::operator==(const nitf::HashTableIterator& it2) const noexcept
{
    return this->equals(it2);
}

bool nitf::HashTableIterator::notEqualTo(const nitf::HashTableIterator& it2) const noexcept
{
    const NITF_BOOL x = nitf_HashTableIterator_notEqualTo(&handle, &it2.getHandle());
    if (!x) return false;
    return true;
}

bool nitf::HashTableIterator::operator!=(const nitf::HashTableIterator& it2) const noexcept
{
    return this->notEqualTo(it2);
}

void nitf::HashTableIterator::increment() noexcept { nitf_HashTableIterator_increment(&handle); }

void nitf::HashTableIterator::operator++(int) noexcept { increment(); }

nitf::HashTableIterator & nitf::HashTableIterator::operator+=(int x) noexcept
{
    for (int i = 0; i < x; ++i)
        increment();
    return *this;
}

nitf::HashTableIterator nitf::HashTableIterator::operator+(int x) noexcept
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

nitf::HashTable::HashTable(int nbuckets) : HashTable(nitf_HashTable_construct(nbuckets, &error))
{
    setManaged(false);
}


nitf::HashTable nitf::HashTable::clone(NITF_DATA_ITEM_CLONE cloner)
{
    nitf::HashTable dolly(nitf_HashTable_clone(getNativeOrThrow(),
        cloner, &error));
    dolly.setManaged(false);
    return dolly;
}

void nitf::HashTable::setPolicy(int policy) noexcept
{
    nitf_HashTable_setPolicy(getNative(), policy);
}

NITF_DATA* nitf::HashTable::remove(const std::string& key) noexcept
{
    return nitf_HashTable_remove(getNative(), key.c_str());
}

void nitf::HashTable::initDefaults() noexcept
{
    nitf_HashTable_initDefaults(getNative());
}


bool nitf::HashTable::exists(const std::string& key) const noexcept
{
    return nitf_HashTable_exists(getNative(), key.c_str());
}

void nitf::HashTable::print() const noexcept
{
    nitf_HashTable_print(getNative());
}

void nitf::HashTable::forEach(HashIterator& fun, NITF_DATA* userData)
{
    const int numBuckets = getNumBuckets();
    for (int i = 0; i < numBuckets; i++)
    {
        nitf::List l = getBucket(i);
        for (nitf::ListIterator iter = l.begin();
                iter != l.end(); ++iter)
        {
            nitf::Pair pair = nitf::Pair(*iter);
            fun(this, pair, userData);
        }
    }
}

void nitf::HashTable::insert(const std::string& key, NITF_DATA* data)
{
    if (key.empty())
        throw except::NoSuchKeyException(Ctxt("Empty key value"));
    const NITF_BOOL x = nitf_HashTable_insert(getNative(), key.c_str(), data, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

nitf::Pair nitf::HashTable::find(const std::string& key) const
{
    if (key.empty())
        throw except::NoSuchKeyException(Ctxt("Empty key value"));
    nitf_Pair* x = nitf_HashTable_find(getNative(), key.c_str());
    if (!x)
        throw except::NoSuchKeyException(Ctxt(key));
    return nitf::Pair(x);
}

nitf::Pair nitf::HashTable::operator[] (const std::string& key)
{
    return find(key);
}

nitf::List nitf::HashTable::getBucket(int i) const
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

nitf::HashTableIterator nitf::HashTable::begin() const noexcept
{
    const nitf_HashTableIterator x = nitf_HashTable_begin(getNative());
    return nitf::HashTableIterator(x);
}

nitf::HashTableIterator nitf::HashTable::end() const noexcept
{
    const nitf_HashTableIterator x = nitf_HashTable_end(getNative());
    return nitf::HashTableIterator(x);
}

void nitf::HashTable::clearBuckets()
{
    std::vector<nitf::List *>::iterator i;
    i = mBuckets.begin();
    for (; i != mBuckets.end(); ++i)
    {
        delete *i;
    }
    mBuckets.clear();
}
