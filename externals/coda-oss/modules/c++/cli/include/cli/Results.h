/* =========================================================================
 * This file is part of cli-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cli-c++ is free software; you can redistribute it and/or modify
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

#ifndef __CLI_RESULTS_H__
#define __CLI_RESULTS_H__

#include <map>
#include "cli/Value.h"

namespace cli
{

class Results
{
protected:
    typedef std::map<std::string, cli::Value*> ValueStorage_T;
    typedef ValueStorage_T::iterator ValueIter_T;
    typedef ValueStorage_T::const_iterator ConstValueIter_T;
    typedef std::map<std::string, cli::Results*> ResultsStorage_T;
    typedef ResultsStorage_T::iterator ResultsIter_T;
    typedef ResultsStorage_T::const_iterator ConstResultsIter_T;

public:
    Results()
    {
    }
    ~Results()
    {
        destroy();
    }

    bool hasValue(const std::string& key) const
    {
        return mValues.find(key) != mValues.end();
    }

    bool hasSubResults(const std::string& key) const
    {
        return mResults.find(key) != mResults.end();
    }

    cli::Value* operator[](const std::string& key) const
            throw (except::NoSuchKeyException)
    {
        return getValue(key);
    }

    cli::Value* getValue(const std::string& key) const
            throw (except::NoSuchKeyException)
    {
        ConstValueIter_T p = mValues.find(key);
        if (p == mValues.end())
        {
            const std::string errorMessage = "No argument named " + key;
            throw except::NoSuchKeyException(Ctxt(errorMessage));
        }
        return p->second;
    }

    template<typename T>
    T get(const std::string& key, unsigned int index = 0) const
            throw (except::NoSuchKeyException)
    {
        return getValue(key)->get<T>(index);
    }

    template<typename T>
    T operator()(const std::string& key, unsigned int index = 0) const
            throw (except::NoSuchKeyException)
    {
        return get<T>(key, index);
    }

    cli::Results* getSubResults(const std::string& key) const
            throw (except::NoSuchKeyException)
    {
        ConstResultsIter_T p = mResults.find(key);
        if (p == mResults.end())
        {
            throw except::NoSuchKeyException(Ctxt(key));
        }
        return p->second;
    }

    void put(const std::string& key, cli::Value *value)
    {
        if (hasValue(key))
        {
            cli::Value* existing = getValue(key);
            if (existing != value)
                delete getValue(key);
        }
        mValues[key] = value;
    }

    void put(const std::string& key, cli::Results *args)
    {
        if (hasSubResults(key))
        {
            cli::Results *existing = getSubResults(key);
            if (existing != args)
                delete existing;
        }
        mResults[key] = args;
    }

    typedef ValueStorage_T::iterator iterator;
    typedef ValueStorage_T::const_iterator const_iterator;

    iterator begin() { return mValues.begin(); }
    const_iterator begin() const { return mValues.begin(); }
    iterator end() { return mValues.end(); }
    const_iterator end() const { return mValues.end(); }

protected:
    ValueStorage_T mValues;
    ResultsStorage_T mResults;

    void destroy()
    {
        for (ValueIter_T it = mValues.begin(), end = mValues.end(); it != end; ++it)
            delete it->second;
        for (ResultsIter_T it = mResults.begin(), end = mResults.end(); it
                != end; ++it)
            delete it->second;
        mValues.clear();
        mResults.clear();
    }
};

}

#endif
