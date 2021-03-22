/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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


#ifndef __SYS_DIRECTORY_ENTRY_H__
#define __SYS_DIRECTORY_ENTRY_H__

#include "except/Exception.h"
#include "sys/OS.h"
#include "sys/Path.h"


namespace sys
{
class DirectoryEntry
{
public:
    class Iterator;
    friend class Iterator;

    DirectoryEntry(const Path& path)
    {
        if (!path.exists() || !path.isDirectory())
            throw except::FileNotFoundException(Ctxt(path.getPath()));

        mCurrent = mDir.findFirstFile(path.getPath());
        mFirst.reset(this);
        mLast.reset(nullptr);

    }

    /* Dont worry about this for now
    DirectoryEntry& operator=(const Path& path)
    {
        if (!path.exists() || !path.isDirectory())
            throw except::NoSuchFileException(Ctxt(path));

        mCurrent = mDir.findFirstFile(path.getPath());
        mFirst.reset(this);
        mLast.reset(nullptr);
        return *this;
    }
    */

    DirectoryEntry(const std::string& dirName) : 
        mDirName(dirName)
    {
        mCurrent = mDir.findFirstFile(dirName);
        mFirst.reset(this);
        mLast.reset(nullptr);
    }
    virtual ~DirectoryEntry()
    {}
    virtual void next()
    {
        mCurrent = mDir.findNextFile();
    }
    virtual std::string getCurrent() const
    {
        return mCurrent;
    }
    virtual std::string getName() const
    {
        return mDirName;
    }

    class Iterator
    {
    public:
        Iterator() : mEntry(nullptr)
        {}
        explicit Iterator(DirectoryEntry* dirEntry) : mEntry(dirEntry)
        {}


        void reset(DirectoryEntry* dirEntry)
        {
            mEntry = dirEntry;
        }
        Iterator& operator++()
        {
            mEntry->next();
            if (mEntry->mCurrent.empty()) 
                mEntry = nullptr;
            return *this;
        }
        std::string operator*() const
        {
            if (mEntry->mCurrent.empty())
                throw except::NullPointerReference(Ctxt(
                    "DirectoryEntry::Iterator NULL entry not allowed"));
            return std::string(mEntry->mCurrent);
        }
        DirectoryEntry* get() const
        {
            return mEntry;
        }

        DirectoryEntry* operator->() const
        {
            return get();
        }

    private:
        DirectoryEntry* mEntry;

    };

    const Iterator& begin() const
    {
        return mFirst;
    }
    const Iterator& end() const
    {
        return mLast;
    }


private:
    Iterator mFirst;
    Iterator mLast;
    std::string mCurrent;
    std::string mDirName;
    Directory mDir;
    //   DirectoryEntry mDirLast;


};



}

bool operator==(const sys::DirectoryEntry::Iterator& lhs,
                const sys::DirectoryEntry::Iterator& rhs);

bool operator!=(const sys::DirectoryEntry::Iterator& lhs,
                const sys::DirectoryEntry::Iterator& rhs);

#endif
