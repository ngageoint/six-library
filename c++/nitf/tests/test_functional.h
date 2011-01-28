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

#ifndef __NITF_TEST_FUNCTIONAL_H__
#define __NITF_TEST_FUNCTIONAL_H__

#include <import/except.h>
#include <functional>
#include <algorithm>
#include <map>
#include <vector>

template <typename T>
class FieldAdapterMap
{
private:
    std::map<std::string, std::mem_fun_ref_t<nitf::Field, T> > mMap;
    std::vector< std::pair< std::string,
        std::mem_fun_ref_t<nitf::Field, T> > > mVec;

public:
typedef typename std::vector< std::pair< std::string,
        std::mem_fun_ref_t<nitf::Field, T> > >::iterator Iterator;

    FieldAdapterMap() {}
    virtual ~FieldAdapterMap() {}

    Iterator begin() { return mVec.begin(); }
    Iterator end() { return mVec.end(); }

    std::mem_fun_ref_t<nitf::Field, T> operator[] (const std::string& key)
        throw(except::NoSuchKeyException)
    {
        if (!exists(key))
            throw except::NoSuchKeyException(key);
        return mMap.find(key)->second;
    }

    bool exists(const std::string& key)
    {
        return mMap.find(key) != mMap.end();
    }

    void insert(const std::string& key, std::mem_fun_ref_t<nitf::Field, T> func)
    {
        mMap.insert(std::pair<std::string,
            std::mem_fun_ref_t<nitf::Field, T> >(key, func));
        mVec.push_back(std::pair<std::string,
            std::mem_fun_ref_t<nitf::Field, T> >(key, func));
    }

    void remove(const std::string& key)
    {
        if (exists(key))
        {
            mMap.erase(mMap.find(key));
            for (Iterator it = begin(); it != end(); ++it)
            {
                if (it->first == key)
                {
                    mVec.erase(it);
                    break;
                }
            }
        }
    }

};


#endif
