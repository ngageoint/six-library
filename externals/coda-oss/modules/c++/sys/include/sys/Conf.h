/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#ifndef CODA_OSS_sys_Conf_h_INCLUDED_
#define CODA_OSS_sys_Conf_h_INCLUDED_
#pragma once

// In case there is still a WIN32 (should be "_WIN32" with a leading '_') someplace.
#if defined(_WIN32) && !defined(WIN32)
#define WIN32 _WIN32
#endif

// POSIX is more-or-less "Unix"
// https://linux.die.net/man/7/feature_test_macros
// "If no feature test macros are explicitly defined, then the following feature test macros
// are defined by default: ... _POSIX_SOURCE, and _POSIX_C_SOURCE=200809L. [...] 
// _POSIX_SOURCE Defining this obsolete macro ... is equivalent to defining _POSIX_C_SOURCE ..."
#ifndef _WIN32
#include <features.h>
#endif

#undef CODA_OSS_POSIX_SOURCE
#if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 1)
#define CODA_OSS_POSIX_SOURCE _POSIX_C_SOURCE
#endif

#undef CODA_OSS_POSIX2001_SOURCE
#if defined(CODA_OSS_POSIX_SOURCE) && (_POSIX_C_SOURCE >= 200112L)
#define CODA_OSS_POSIX2001_SOURCE _POSIX_C_SOURCE
#endif

#undef CODA_OSS_POSIX2008_SOURCE
#if defined(CODA_OSS_POSIX2001_SOURCE) && (_POSIX_C_SOURCE >= 200809L)
#define CODA_OSS_POSIX2008_SOURCE _POSIX_C_SOURCE
#endif

#include <config/Version.h>
#include <config/Exports.h>
#include <sys/CPlusPlus.h>
#include <str/Convert.h>

#if defined (__APPLE_CC__)
#  include <iosfwd>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#ifdef __GNUC__
#include <byteswap.h> // "These functions are GNU extensions."
#endif

#include <iostream>
#include <algorithm>
#include <cstdarg>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include "coda_oss/span.h"
#include <complex>

#include "str/Format.h"
#include "sys/TimeStamp.h"


/*  Dance around the compiler to figure out  */
/*  if we have access to function macro...   */

/*  If we have gnu -- Yes!!  */
#if defined(__GNUC__)
    /*  We get a really nice function macro  */
#   define NativeLayer_func__ __PRETTY_FUNCTION__
#elif defined(_WIN32) && (_MSC_VER >= 1300)
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


namespace sys
{
    typedef char              byte;
    typedef unsigned char     ubyte;
    typedef uint8_t            Uint8_T;
    typedef uint16_t           Uint16_T;
    typedef uint32_t           Uint32_T;
    typedef uint64_t           Uint64_T;
    typedef size_t             Size_T;
    typedef int8_t             Int8_T;
    typedef int16_t            Int16_T;
    typedef int32_t            Int32_T;
    typedef int64_t            Int64_T;
}

#ifdef _WIN32
#  include <malloc.h>
#  include <windows.h>
#  include <process.h>


namespace sys
{
    typedef HANDLE            Handle_T;
    typedef Int64_T           Off_T;
    typedef DWORD             Pid_T;
#   if _WIN64 // SIZEOF_SIZE_T == 8
        static_assert(sizeof(size_t) == 8, "wrong sizeof(size_t)");
        typedef Int64_T   SSize_T;
#   else // SIZEOF_SIZE_T == 4
        static_assert(sizeof(size_t) == 4, "wrong sizeof(size_t)");
        typedef Int32_T   SSize_T;
#   endif
        static_assert(sizeof(size_t) == sizeof(SSize_T), "size_t and SSize_T should be the same size");
}
#else // !windows
#   include <sys/types.h>
#  include <signal.h>
#  include <errno.h>
#  include <sys/stat.h>
#  include <sys/wait.h>
#  include <unistd.h>
#  include <fcntl.h>
#  include <dirent.h>
#  include <stdint.h>
//#  include <sys/mman.h>

namespace sys
{
    typedef ssize_t            SSize_T;
    typedef off_t              Off_T;
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

#define SYS_FUNC NativeLayer_func__

#define Ctxt(MESSAGE) except::Context(__FILE__, __LINE__, SYS_FUNC, \
        sys::TimeStamp().local(), MESSAGE)

namespace sys
{
    /*!
     *  32 byte alignment is safe for now. Most SSE instruction sets
     *  require 16 byte alignment, but newer Intel chipsets require
     *  32 byte. All 32 byte alignments inherently confirm 16 byte
     *  alignment, so we default to 32 here to be safe.
     *
     *  NOTE: This should be updated as architectures require greater
     *        intervals. Alignments require base 2 increments.
     */
    static constexpr size_t SSE_INSTRUCTION_ALIGNMENT = 32;

    /*!
     * Returns true if the system is big-endian, otherwise false.
     * On Intel systems, we are usually small-endian, and on
     * RISC architectures we are big-endian.
     */
    bool CODA_OSS_API isBigEndianSystem();


   /*!
     *  Swap bytes in-place.  Note that a complex pixel
     *  is equivalent to two floats so elemSize and numElems
     *  must be adjusted accordingly.
     *
     *  \param [inout] buffer to transform
     *  \param elemSize
     *  \param numElems
     */
    void CODA_OSS_API byteSwap_(void* buffer, size_t elemSize, size_t numElems);
    template<typename T>
    inline void byteSwap(T* buffer, size_t elemSize, size_t numElems)
    {
        // Trying to byte-swap structs can result in garbage because of padding.
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "can only byte-swap numbers.");
        if (elemSize != sizeof(T))
        {
            throw std::invalid_argument("sizeof(T) != elemSize");
        }
        byteSwap_(buffer, elemSize, numElems);
    }
    template <typename T>
    inline void byteSwap(coda_oss::span<T> buffer)
    {
        constexpr auto elemSize = sizeof(T);
        const auto numElems = buffer.size();
        byteSwap(buffer.data(), elemSize, numElems);
    }
    template <typename T>
    inline void byteSwap(coda_oss::span<std::complex<T>> buffer)
    {
        void* pBuffer = buffer.data();
        const coda_oss::span<T> buffer_(static_cast<T*>(pBuffer), buffer.size() * 2);  // real and imag
        byteSwap(buffer_);
    }

    inline void byteSwapV(void* buffer, unsigned short elemSize, size_t numElems) // existing API
    {
        byteSwap_(buffer, elemSize, numElems);
    }
   
    /*!
     *  Swap bytes into output buffer.  Note that a complex pixel
     *  is equivalent to two floats so elemSize and numElems
     *  must be adjusted accordingly.
     *
     *  \param buffer to transform
     *  \param elemSize
     *  \param numElems
     *  \param[out] outputBuffer buffer to write swapped elements to
     */
    void CODA_OSS_API byteSwap_(const void* buffer, size_t elemSize, size_t numElems, void* outputBuffer);
    template <typename T, typename U = T>
    inline void byteSwap(const T* buffer, size_t elemSize, size_t numElems,
                         U* outputBuffer) // e.g., "unsigned int" && "int"
    {
        // Trying to byte-swap structs can result in garbage because of padding.
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "can only byte-swap numbers.");
        static_assert(std::is_arithmetic<U>::value || std::is_enum<U>::value, "can only byte-swap numbers.");
        static_assert(sizeof(T) > 1, "byte-swapping a single-byte value makes no sense.");
        //static_assert(sizeof(T) == sizeof(U), "sizeof(T) != sizeof(U)."); // outputBuffer could be std::byte
        if (elemSize != sizeof(T))
        {
            throw std::invalid_argument("sizeof(T) != elemSize");
        }
        byteSwap_(buffer, elemSize, numElems, outputBuffer);
    }
    template <typename T, typename U = T>
    inline void byteSwap(coda_oss::span<const T> buffer, coda_oss::span<U> outputBuffer) // e.g., "unsigned int" && "int"
    {
        // outputBuffer could be std::byte
        if (buffer.size_bytes() != outputBuffer.size_bytes())
        {
            throw std::invalid_argument("buffer.size_bytes() != outputBuffer.size_bytes()");
        }
        byteSwap(buffer.data(), sizeof(T), buffer.size(), outputBuffer.data());
    }
    template <typename T>
    inline void byteSwap(coda_oss::span<const std::complex<T>> buffer, coda_oss::span<std::complex<T>> outputBuffer)
    {
        const void* pBuffer = buffer.data();
        const coda_oss::span<const T> buffer_(static_cast<const T*>(pBuffer), buffer.size() * 2);  // real and imag
        void* pOutputBuffer = outputBuffer.data();
        const coda_oss::span<T> outputBuffer_(static_cast<T*>(pOutputBuffer), outputBuffer.size() * 2);  // real and imag

        byteSwap(buffer_, outputBuffer_);
    }

    inline void byteSwapV(const void* buffer, unsigned short elemSize, size_t numElems, void* outputBuffer) // existing API
    {
        byteSwap_(buffer, elemSize, numElems, outputBuffer);
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
    template <typename T> inline T byteSwap_(T val)
    {
        // Trying to byte-swap structs can result in garbage because of padding.
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "can only byte-swap numbers");

        constexpr auto size = sizeof(T);
        T out;

        unsigned char* cOut = reinterpret_cast<unsigned char*>(&out);
        unsigned char* cIn = reinterpret_cast<unsigned char*>(&val);
        for (size_t i = 0, j = size - 1; i < j; ++i, --j)
        {
            cOut[i] = cIn[j];
            cOut[j] = cIn[i];
        }
        return out;
    }
    template <typename T> inline T byteSwap(T val)
    {
        return byteSwap_(val);
    }
    inline uint8_t byteSwap(uint8_t val)
    {
        return val;  // no-op
    }
#if defined(_MSC_VER)
    // These routines should geneerate a single instruction; see https://devblogs.microsoft.com/cppblog/a-tour-of-4-msvc-backend-improvements/
    inline uint16_t byteSwap(uint16_t val)
    {
        return _byteswap_ushort(val);
    }
    inline uint32_t byteSwap(uint32_t val)
    {
        return _byteswap_ulong(val);
    }
    inline uint64_t byteSwap(uint64_t val)
    {
        return _byteswap_uint64(val);
    }
#elif defined(__GNUC__)
    inline uint16_t byteSwap(uint16_t val)
    {
        return bswap_16(val);
    }
    inline uint32_t byteSwap(uint32_t val)
    {
        return bswap_32(val);
    }
    inline uint64_t byteSwap(uint64_t val)
    {
        return bswap_64(val);
    }
#endif
    template <typename TUInt, typename T>
    inline T byteSwapValue_(T val)
    { 
        static_assert(sizeof(T) > 1, "byte-swapping a single-byte value makes no sense.");
        static_assert(sizeof(T) == sizeof(TUInt), "sizeof(T) != sizeof(<TUInt>)");
        static_assert(std::is_unsigned<TUInt>::value, "TUInt must be 'unsigned'");

        const void* pVal = &val;
        const auto pUInt = static_cast<const TUInt*>(pVal);
        const auto result = byteSwap(*pUInt);
        
        const void* pResult = &result;
        const auto pRetval = static_cast<const T*>(pResult);
        return *pRetval;
    }
    inline float byteSwap(float val)
    {
        return byteSwapValue_<uint32_t>(val);
    }
    inline double byteSwap(double val)
    {
        return byteSwapValue_<uint64_t>(val);
    }
    template<typename T>
    inline std::complex<T> byteSwap(std::complex<T> v)
    {
        std::complex<T> retval{byteSwap(v.real()), byteSwap(v.imag())};
        return retval;
    }

    /*!
     *  Method to create a block of memory on an alignment
     *  boundary specified by the user.
     *  This typically reduces the amount of moves that the
     *  OS has to do to get the data in the form that it needs
     *  to be in.  Since this method is non-standard, we present
     *  OS-specific alternatives.
     *
     *  \param sz The size (in bytes) of the buffer we wish to create
     *  \throw Exception if a bad allocation occurs
     *  \return a pointer to the data (this method never returns NULL)
     */
    inline void* alignedAlloc(size_t size,
                              size_t alignment = SSE_INSTRUCTION_ALIGNMENT)
    {
        void* p = nullptr;
#ifdef _WIN32
        p = _aligned_malloc(size, alignment);
#elif defined(CODA_OSS_POSIX2001_SOURCE)
        // https://linux.die.net/man/3/posix_memalign
        if (posix_memalign(&p, alignment, size) != 0)
        {
            p = nullptr;
        }
#elif defined(CODA_OSS_POSIX_SOURCE)
        // https://linux.die.net/man/3/posix_memalign
        // "The functions memalign(), ... have been available in all Linux libc libraries."
        p = memalign(alignment, size);
#else
        //! this is a basic unaligned allocation
        p = malloc(size);
        #error "Don't know how to implement alignedAlloc()."
#endif
        if (!p)
            throw except::Exception(Ctxt(
                "Aligned allocation failure of size [" +
                str::toString(size) + "] bytes"));
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
#ifdef _WIN32
        _aligned_free(p);
#else
        free(p);
#endif
    }


}

// https://en.wikipedia.org/wiki/Year_2038_problem
// "Most operating systems designed to run on 64-bit hardware already use signed 64-bit `time_t` integers. ..."
#include <time.h>
#include <stdint.h>
static_assert(sizeof(time_t) >= sizeof(int64_t), "Should have at least a 64-bit time_t.");

#endif // CODA_OSS_sys_Conf_h_INCLUDED_
