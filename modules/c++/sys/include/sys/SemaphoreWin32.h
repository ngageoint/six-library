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


#ifndef __SYS_SEMAPHORE_WIN32_H__
#define __SYS_SEMAPHORE_WIN32_H__

#include "config/Exports.h"

#if defined(WIN32) || defined(_WIN32)

#if !defined(USE_NSPR_THREADS)

#include "sys/Conf.h"
#include "sys/SemaphoreInterface.h"

namespace sys
{

class CODA_OSS_API SemaphoreWin32 : public SemaphoreInterface
{
public:
    SemaphoreWin32(unsigned int count = 0, size_t maxCount = std::numeric_limits<size_t>::max());

    void wait();

    void signal();
    
    HANDLE& getNative();
    
    /*!
     *  Return the type name.  This function is essentially free,
     *  because it is static RTTI.
     */
    const char* getNativeType() const
    {
        return typeid(mNative).name();
    }
private:
    HANDLE mNative;
};

}
#endif
#endif
#endif
