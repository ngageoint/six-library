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

#pragma once
#ifndef CODA_OSS_cli_Results_h_INCLUDED_
#define CODA_OSS_cli_Results_h_INCLUDED_

#include <map>
#include <memory>
#include <utility>
#include <stdexcept>

#include "sys/Conf.h"

#include "cli/Value.h"

namespace cli
{

class Results final
{
    typedef std::map<std::string, std::unique_ptr<cli::Value>> ValueStorage_T;
    typedef ValueStorage_T::iterator ValueIter_T;
    typedef ValueStorage_T::const_iterator ConstValueIter_T;
    typedef std::map<std::string, cli::Results*> ResultsStorage_T;
    typedef ResultsStorage_T::iterator ResultsIter_T;
    typedef ResultsStorage_T::const_iterator ConstResultsIter_T;

public:
    Results() = default;
    ~Results()
    {
        destroy();
    }
    Results(const Results&) = delete;
    Results& operator=(const Results&) = delete;
    Results(Results&&) = default;
    Results& operator=(Results&&) = default;

    bool hasValue(const std::string& key) const
    {
        return mValues.find(key) != mValues.end();
    }

    bool hasSubResults(const std::string& key) const
    {
        return mResults.find(key) != mResults.end();
    }

    const cli::Value* operator[](const std::string& key) const
    {
        return getValue(key);
    }

    const cli::Value* getValue(const std::string& key) const
    {
        auto const p = mValues.find(key);
        if (p == mValues.end())
        {
            const std::string errorMessage = "No argument named " + key;
            throw except::NoSuchKeyException(Ctxt(errorMessage));
        }
        return p->second.get();
    }
    cli::Value* getValue(const std::string& key)
    {
        auto const p = mValues.find(key);
        if (p == mValues.end())
        {
            const std::string errorMessage = "No argument named " + key;
            throw except::NoSuchKeyException(Ctxt(errorMessage));
        }
        return p->second.get();
    }

    template<typename T>
    T get(const std::string& key, unsigned int index = 0) const
    {
        return getValue(key)->get<T>(index);
    }

    template<typename T>
    T operator()(const std::string& key, unsigned int index = 0) const
    {
        return get<T>(key, index);
    }

    cli::Results* getSubResults(const std::string& key) const
    {
        ConstResultsIter_T p = mResults.find(key);
        if (p == mResults.end())
        {
            throw except::NoSuchKeyException(Ctxt(key));
        }
        return p->second;
    }

    void put(const std::string& key, std::unique_ptr<cli::Value> value)
    {
        if (hasValue(key) && (getValue(key) == value.get()))
        {
            return;
        }
        mValues[key] = std::move(value);
    }
    void put(const std::string& key, cli::Value* value)
    {
        cli::Value* pExistingValue = hasValue(key) ? getValue(key) : nullptr;
        if ((pExistingValue == nullptr) || (pExistingValue != value))
        {
            // Either 1) we didn't already have a value or 2) the existing value is different
            put(key, std::unique_ptr<cli::Value>(value));
        }
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

    auto begin() { return mValues.begin(); }
    auto begin() const { return mValues.begin(); }
    auto end() { return mValues.end(); }
    auto end() const { return mValues.end(); }

private:
    ValueStorage_T mValues;
    ResultsStorage_T mResults;

    void destroy()
    {
        mValues.clear();
        for (ResultsIter_T it = mResults.begin(), end = mResults.end(); it
                != end; ++it)
            delete it->second;
        mResults.clear();
    }
};

}

#endif  // CODA_OSS_cli_Results_h_INCLUDED_
