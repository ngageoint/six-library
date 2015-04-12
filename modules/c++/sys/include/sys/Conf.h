/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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


#ifndef __SYS_CONF_H__
#define __SYS_CONF_H__

#if defined (__APPLE_CC__)
#  include <iosfwd>
#endif

#include <assert.h>
#include <iostream>
#include <stdio.h>

#if defined(__sgi) || defined(__sgi__)
#   include <stdarg.h>
#   include <stdlib.h>
#else
#   include <cstdarg>
#   include <cstdlib>
#endif

#include <memory>
#include "str/Format.h"
#include "sys/TimeStamp.h"

#ifdef HAVE_CONFIG_H
#  include "sys/config.h"
#endif

/*  Dance around the compiler to figure out  */
/*  if we have access to function macro...   */

/*  If we have gnu -- Yes!!  */
#if defined(__GNUC__)
    /*  We get a really nice function macro  */
#   define NativeLayer_func__ __PRETTY_FUNCTION__
#elif defined(WIN32) && (_MSC_VER >= 1300)
#   define NativeLayer_func__ __FUNCSIG__
/*  Otherwise, lets look for C99 compatibility  */
#elif defined (__STDC_VERSION__)
    /*  The above line may not be necessary but   */
    /*  Im not about to find out...               */
    /*  Check to see if the compiler is doing C99 */
#   if __STDC_VERSION__ < 199901
        /*  If not, define an empty function signature  */
#       define NativeLayer_func__ ""
#   else
        /*  If so -- Yes!! Use it!  */
#       define NativeLayer_func__ __func__
#   endif
#else
/*  If STDC stuff isnt defined, that would be lame, but  */
/*  lets make sure its still okay                        */
#   define NativeLayer_func__ ""
#endif



#if defined(WIN32) || defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#      define WIN32_LEAN_AND_MEAN
#  endif
#  define NOMINMAX
#  include <malloc.h>
#  include <windows.h>
#  include <process.h>


namespace sys
{
    typedef char              byte;
    typedef unsigned char     ubyte;
    typedef HANDLE            Handle_T;
    typedef unsigned char     Uint8_T;
    typedef unsigned __int16  Uint16_T;
    typedef unsigned __int32  Uint32_T;
    typedef unsigned __int64  Uint64_T;
    typedef signed char       Int8_T;
    typedef __int16           Int16_T;
    typedef __int32           Int32_T;
    typedef __int64           Int64_T;
    typedef Int64_T           Off_T;
    typedef DWORD             Pid_T;
#   ifndef _SIZE_T_DEFINED
#       ifdef  _WIN64
            typedef unsigned __int64    Size_T;
#       else
            typedef _W64 unsigned int   Size_T;
#       endif
#   else
        typedef size_t Size_T;
#   endif
#   ifdef  _WIN64
        typedef __int64    SSize_T;
#   else
        typedef _W64 int   SSize_T;
#   endif
}
#else // !windows
#   include <sys/types.h>
#   if defined(__sgi) || defined(__sgi__)
#       if defined(__GNUC__)
#           ifdef _FIX_BROKEN_HEADERS
                typedef __int64_t jid_t;
#           endif
#       endif
#   endif
#   if defined(__sun) || defined(__sun__) || defined(__sparc) || defined(__sparc) || defined(__sparc__)
#       if !defined(__SunOS_5_6) && !defined(__SunOS_5_7) && !defined(__SunOS_5_8) && defined(__GNUC__)
#           ifdef _FIX_BROKEN_HEADERS
                typedef id_t projid_t;
#           endif
#       endif
#       include <sys/stream.h>
#  endif
#  include <signal.h>
#  include <errno.h>
#  include <sys/stat.h>
#  include <sys/wait.h>
#  include <unistd.h>
#  include <fcntl.h>
#  include <dirent.h>
#  if defined(_USE_STDINT)
#      include <stdint.h>
#  else
#      include <inttypes.h>
#  endif
//#  include <sys/mman.h>

namespace sys
{
    
    typedef char               byte;
    typedef unsigned char      ubyte;
    typedef uint8_t            Uint8_T;
    typedef uint16_t           Uint16_T;
    typedef uint32_t           Uint32_T;
    typedef uint64_t           Uint64_T;
    typedef size_t             Size_T;
    typedef ssize_t            SSize_T;
    typedef off_t              Off_T;
    typedef int8_t             Int8_T;
    typedef int16_t            Int16_T;
    typedef int32_t            Int32_T;
    typedef int64_t            Int64_T;
    typedef int                Handle_T;
    // Should we remove this?
    typedef pid_t              Pid_T;
}
#endif // *nix

// For strerror
#include <string.h>

#ifdef signal //  Apache defines this
#    undef  signal
#endif

#include "except/Exception.h"

#define FmtX str::format

#define SYS_TIME sys::TimeStamp().local()

#define SYS_FUNC NativeLayer_func__

#define Ctxt(MESSAGE) except::Context(__FILE__, __LINE__, SYS_FUNC, SYS_TIME, MESSAGE)

namespace sys
{
    /*!
     * Returns true if the system is big-endian, otherwise false.
     * On Intel systems, we are usually small-endian, and on
     * RISC architectures we are big-endian.
     */
    bool isBigEndianSystem();


   /*!
     *  Swap bytes in-place.  Note that a complex pixel
     *  is equivalent to two floats so elemSize and numElems
     *  must be adjusted accordingly.
     *
     *  \param [inout] buffer to transform
     *  \param elemSize
     *  \param numElems
     */
    inline void byteSwap(sys::byte *buffer,
                         const unsigned short elemSize,
                         const size_t numElems)
    {
        if (!buffer || elemSize < 2 || !numElems)
            return;

        unsigned short half = elemSize >> 1;
        long offset = 0, innerOff = 0, innerSwap = 0;

        for(size_t i = 0; i < numElems; ++i, offset += elemSize)
        {
            for(unsigned short j = 0; j < half; ++j)
            {
                innerOff = offset + j;
                innerSwap = offset + elemSize - 1 - j;
                
                sys::byte tmp       = buffer[innerOff];
                buffer[innerOff]    = buffer[innerSwap];
                buffer[innerSwap]   = tmp;
            }
        }
    }

    /*!
     *  Function to swap one element irrespective of size.  The inplace 
     *  buffer function should be preferred.
     *
     *  To specialize complex float, first include the complex library
     *  \code
        #include <complex>
     *  \endcode
     *
     *  Then put an overload in as specified below:
     *  \code
        template <typename T> std::complex<T> byteSwap(std::complex<T> val)
        {
            std::complex<T> out(byteSwap<T>(val.real()),
                                byteSwap<T>(val.imag()));
            return out;
        }
     *  \endcode
     *
     */
    template <typename T> T byteSwap(T val)
    {
        size_t size = sizeof(T);
        T out;
        
        unsigned char* cOut = reinterpret_cast<unsigned char*>(&out);
        unsigned char* cIn = reinterpret_cast<unsigned char*>(&val);
        for (int i = 0, j = size - 1; i < j; ++i, --j)
        {
            cOut[i] = cIn[j];
            cOut[j] = cIn[i];
        }
        return out;
    }


#ifdef WIN32

    /*!
     *  Method to create a block of memory on 16-byte boundary.
     *  This typically reduces the amount of moves that the
     *  OS has to do to get the data in the form that it needs
     *  to be in.  Since this method is non-standard, we present
     *  OS-specific alternatives.
     *
     *  \param sz The size (in bytes) of the buffer we wish to create
     *  \throw Exception if a bad allocation occurs
     *  \return a pointer to the data (this method never returns NULL)
     */
    inline void* alignedAlloc(size_t sz)
    {
        void* p = _aligned_malloc(sz, 16);
        if (!p)
            throw except::Exception("_aligned_malloc: bad alloc");
        
        return p;
    }
    
    /*!
     *  Free memory that was allocated with alignedAlloc
     *  This method behaves like free
     *
     *  \param p A pointer to the data allocated using alignedAlloc
     */
    inline void alignedFree(void* p)
    {
        _aligned_free(p);
    }
#elif defined(__POSIX) && !defined(__sun)

    /*!
     *  Method to create a block of memory on 16-byte boundary.
     *  This typically reduces the amount of moves that the
     *  OS has to do to get the data in the form that it needs
     *  to be in.  Since this method is non-standard, we present
     *  OS-specific alternatives.
     *
     *  \param sz The size (in bytes) of the buffer we wish to create
     *  \throw Exception if a bad allocation occurs
     *  \return a pointer to the data (this method never returns NULL)
     */
    inline void* alignedAlloc(size_t sz)
    {
        void* p = NULL;
        if (posix_memalign(&p, 16, sz) != 0)
            throw except::Exception("posix_memalign: bad alloc");
        memset(p, 0, sz);
        return p;
    
    }

    /*!
     *  Free memory that was allocated with alignedAlloc
     *  This method behaves like free
     *
     *  \param p A pointer to the data allocated using alignedAlloc
     */
    inline void alignedFree(void* p)
    {
        free(p);
    }
#elif defined(__sun)
    /*!
     *  Method to create a block of memory on 16-byte boundary.
     *  This typically reduces the amount of moves that the
     *  OS has to do to get the data in the form that it needs
     *  to be in.  Since this method is non-standard, we present
     *  OS-specific alternatives.
     *
     *  \param sz The size (in bytes) of the buffer we wish to create
     *  \throw Exception if a bad allocation occurs
     *  \return a pointer to the data (this method never returns NULL)
     */
    inline void* alignedAlloc(size_t sz)
    {
        void* const p = memalign(16, sz);
        if (p == NULL)
            throw except::Exception("memalign: bad alloc");
        memset(p, 0, sz);
        return p;
    }

    /*!
     *  Free memory that was allocated with alignedAlloc
     *  This method behaves like free
     *
     *  \param p A pointer to the data allocated using alignedAlloc
     */
    inline void alignedFree(void* p)
    {
        free(p);
    }
#else

    /*!
     *  Method to create a block of memory on 16-byte boundary.
     *  This typically reduces the amount of moves that the
     *  OS has to do to get the data in the form that it needs
     *  to be in.  Since this method is non-standard, we present
     *  OS-specific alternatives.
     *
     *  \param sz The size (in bytes) of the buffer we wish to create
     *  \throw Exception if a bad allocation occurs
     *  \return a pointer to the data (this method never returns NULL)
     */
    inline void* alignedAlloc(size_t sz)
    {
        void* p = calloc(sz, 1);
        if (p == NULL)
            throw except::Exception("calloc: bad alloc");
        return p;
    }

    /*!
     *  Free memory that was allocated with alignedAlloc
     *  This method behaves like free
     *
     *  \param p A pointer to the data allocated using alignedAlloc
     */
    inline void alignedFree(void* p)
    {
        free(p);
    }
#endif


}

#endif

