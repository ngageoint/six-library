/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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


#ifndef __MT_SINGLETON_H__
#define __MT_SINGLETON_H__

#include <import/sys.h>
#include "mt/CriticalSection.h"

namespace mt
{

/*!
 * \brief Templated, thread-safe Singleton
 * 
 * The Singleton class guarantees that at any given time in an application's
 * life cycle, only 1 instance of the templated object will be in existence.
 * Singleton is a great way to implement the Manager design pattern.
 * 
 * The Singleton takes in the Object class type as the first template parameter.
 * So, for example, if you had a Logger class and you wanted only one instance
 * of it throughout your program, you could do this:
 * 
 * typedef mt::Singleton<Logger> LoggerManager;
 * ...
 * LoggerManager::getInstance().log("singleton log message");
 * 
 * The second template parameter is a bool which denotes whether the Singleton
 * should automatically destroy the owned object at program exit. By default,
 * this flag is set to false, thus NOT destroying the internal object at exit.
 * The public destroy() method of the Singleton can be used to manually destroy
 * the singleton object. This lets the user facilitate how/when the Singleton
 * gets destroyed.
 * 
 * Most users will want to set AutoDestroy to true, since they want the memory
 * to be freed up automatically at exit. Be careful when keeping AutoDestroy
 * set to false, for this means that you, the user, are in charge of calling
 * destroy() when you are finished using the Singleton. Going back to the
 * previous example of a singleton Logger, here is how you would create one
 * that gets destroyed at program exit:
 * 
 * typedef mt::Singleton<Logger, true> LoggerManager; 
 * 
 * Notice that this implementation of the Singleton pattern uses the
 * atexit method for deleting the singleton instance, rather than an
 * unique_ptr. This is done b/c there is a valid use case the unique_ptr
 * gets destroyed (as well as the underlying memory), but if the Singleton
 * gets revived, the unique_ptr does NOT get recreated, and returns an invalid
 * memory address.
 * 
 */
template <bool AutoDestroy> struct SingletonAutoDestroyer
{
    static void registerAtExit(void (*)(void))
    {
        /* When AutoDestroy is false, we want to do nothing */
    }
};

template <> struct SingletonAutoDestroyer<true>
{
    static void registerAtExit(void (*function)(void))
    {
#if defined(__SunOS_5_10)
/*
 * Fix for Solaris bug where atexit is not extern C++
 * http://bugs.opensolaris.org/bugdatabase/view_bug.do;jsessionid=9c8c03419fb896b730de20cd53ae?bug_id=6455603
 */
#   if !defined(ELIMINATE_BROKEN_LINKAGE)
        atexit(function);
#   endif
#else
        std::atexit(function);
#endif
    }
};

template<typename T, bool AutoDestroy = false>
class Singleton
{
public:
    /*!
     * Returns the one and only instance of the object
     */
    static T& getInstance();

    /*!
     * This attempts to destroy the internal singleton object. If it has
     * already been destroyed, it does nothing. Usually you only should
     * call this method if AutoDestroy is set to false. Otherwise, this method
     * will get automatically called at program exit.
     */
    static void destroy();

protected:
    //! Constructor
    inline explicit Singleton()
    {
        assert(mInstance == 0);
        mInstance = static_cast<T*>(this);
    }
    //! Destructor
    inline ~Singleton() { mInstance = 0; }

private:
    static T* mInstance; //static instance
    static sys::Mutex mMutex; //static mutex for locking access to the instance
    inline explicit Singleton(Singleton const&) {}
    inline Singleton& operator=(Singleton const&) { return *this; }
};

template<typename T, bool AutoDestroy>
T& Singleton<T, AutoDestroy>::getInstance()
{
    //double-checked locking
    if (mInstance == nullptr)
    {
        CriticalSection<sys::Mutex> obtainLock(&mMutex);
        if (mInstance == nullptr)
        {
            mInstance = new T; //create the instance
            SingletonAutoDestroyer<AutoDestroy>::registerAtExit(destroy);
        }
    }
    return *mInstance;
}

template<typename T, bool AutoDestroy>
void Singleton<T, AutoDestroy>::destroy()
{
    //double-checked locking
    if (mInstance != nullptr)
    {
        CriticalSection<sys::Mutex> obtainLock(&mMutex);
        if (mInstance != nullptr)
        {
            //we are OK to delete it
            delete mInstance;
            mInstance = nullptr;
        }
    }
}

template<typename T, bool AutoDestroy> T* Singleton<T, AutoDestroy>::mInstance = nullptr;
template<typename T, bool AutoDestroy> sys::Mutex Singleton<T, AutoDestroy>::mMutex;

}
#endif
