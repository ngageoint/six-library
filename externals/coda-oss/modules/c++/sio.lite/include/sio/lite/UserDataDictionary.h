/* =========================================================================
 * This file is part of sio.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __SIO_LITE_USER_DATA_DICTIONARY_H__
#define __SIO_LITE_USER_DATA_DICTIONARY_H__

#include <import/except.h>
#include <import/sys.h>
#include <map>
#include <list>
#include <vector>
#include <string>

namespace sio
{
namespace lite
{
    
/*!
 * \class OrderedDictionary
 * 
 * An OrderedDictionary keeps track of the order that items are added to
 * the dictionary, allowing you to iterate in order.
 * 
 * This class probably belongs in a utility library.
 * 
 */
template < typename Key_T, typename Value_T >
class OrderedDictionary
{
protected:
    std::list< std::pair<Key_T, Value_T> > mList;
    std::map < Key_T, Value_T > mMap;

public:
    typedef typename std::list<std::pair<Key_T, Value_T> >::iterator Iterator;
    typedef typename std::list<std::pair<Key_T, Value_T> >::const_iterator ConstIterator;
    
    OrderedDictionary(){}
    virtual ~OrderedDictionary(){}

    Iterator begin() { return mList.begin(); }
    Iterator end() { return mList.end(); }
    
    ConstIterator begin() const { return mList.begin(); }
    ConstIterator end() const { return mList.end(); }

    virtual Value_T& operator[] (const Key_T& key)
        throw(except::NoSuchKeyException)
    {
        typename std::map < Key_T, Value_T >::iterator it = mMap.find(key);
        if (it == mMap.end())
            throw except::NoSuchKeyException();
        return it->second;
    }

    virtual bool exists(const Key_T& key) const
    {
        return mMap.find(key) != mMap.end();
    }
    
    virtual size_t size() const { return mList.size(); } 
    
    virtual void add(Key_T key, const Value_T& value)
    {
        if (exists(key))
        {
            remove(key);
        }
        mMap[key] = value;
        mList.push_back(std::pair<Key_T, Value_T>(key, value));
    }

    virtual void remove(const Key_T& key)
    {
        mMap.erase(key);
        for (Iterator it = begin(); it != end(); ++it)
        {
            if (it->first == key)
            {
                mList.erase(it);
                break;
            }
        }
    }
};

//! UserDataDictionary is an OrderedDictionary mapping string keys to string values
typedef OrderedDictionary<std::string, std::vector<sys::byte> > UserDataDictionary;

}
}
#endif

