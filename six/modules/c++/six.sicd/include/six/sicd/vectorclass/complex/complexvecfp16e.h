/***************************  complexvecfp16e.h   *****************************
* Author:        Agner Fog
* Date created:  2012-07-24
* Last modified: 2022-07-20
* Version:       2.02.00
* Project:       Extension to vector class library
* Description:
* Classes for complex number algebra and math with half precision.
* These classes are emulated for processors without the AVX512_FP16 instruction set.
* Complex1h:  A scalar of  1 complex number made from 2 half precision floats
* Complex2h:  A vector of  2 complex numbers made from 4 half precision floats
* Complex4h:  A vector of  4 complex numbers made from 8 half precision floats
* Complex8h:  A vector of  8 complex numbers made from 16 half precision floats
* Complex16h: A vector of 16 complex numbers made from 32 half precision floats
* This file defines operators and functions for these classes
* See complexvec_manual.pdf for detailed instructions.
*
* (c) Copyright 2012-2022. Apache License version 2.0 or later
******************************************************************************/

#ifndef COMPLEXVECFP16E_H
#define COMPLEXVECFP16E_H  200

#include "complexvec1.h"
#include <vectorfp16e.h>

#ifdef VCL_NAMESPACE
namespace VCL_NAMESPACE {
#endif

// The Complex classes do not inherit from the corresponding vector classes
// because that would lead to undesired implicit conversions when calling
// functions that are defined only for the vector class. Explicit conversion 
// between these classes is still possible.

/*****************************************************************************
*
*               Class Complex1h: one half precision complex number
*
*****************************************************************************/

class Complex1h {
protected:
    __m128i xmm; // vector of 8 half precision floats. Only the first two are used
public:
    // default constructor
    Complex1h() = default;
    // construct from real, no imaginary part
    Complex1h(Float16 re) {
        xmm = Vec8h(re, 0, 0, 0, 0, 0, 0, 0);
    }
    Complex1h(float re) {
        xmm = Vec8h(Float16(re), 0, 0, 0, 0, 0, 0, 0);
    }
    // construct from real and imaginary part
    Complex1h(Float16 re, Float16 im) {
        xmm = Vec8h(re, im, 0, 0, 0, 0, 0, 0);
    }
    // Constructor to convert from type __m128i used in intrinsics:
    Complex1h(__m128i const x) {
        xmm = x;
    }
    // Assignment operator to convert from type __m128i used in intrinsics:
    Complex1h & operator = (__m128i const x) {
        xmm = x;
        return *this;
    }
    // Type cast operator to convert to __m128i used in intrinsics
    operator __m128i() const {
        return xmm;
    }
    // Member function to convert to vector
    Vec8h to_vector() const {
        return xmm;
    }
    // Member function to load from array (unaligned)
    Complex1h & load(void const * p) {
        xmm = Vec8s().load_partial(2, p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        Vec8s(xmm).store_partial(2, p);
    }
    // Member function to insert one complex scalar into complex vector
    Complex1h insert (int index, Complex1h x) {
        xmm = x;
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    // (for compatibility with higher vectors)
    Complex1h extract (int i) const {
        return *this;
    }
    // get real part
    Float16 real() const {
        return casts2fp16(uint16_t(_mm_extract_epi16(xmm, 0)));
    }
    // get imaginary part
    Float16 imag() const {
        return casts2fp16(uint16_t(_mm_extract_epi16(xmm, 1)));
    }
    static constexpr int size() {
        return 1;
    }
    static constexpr int elementtype() {
        return 0x10F;
    }
};

/*****************************************************************************
*
*          conversions Complex1h <-> Complex1f
*
*****************************************************************************/
// function to_float: convert Complex1h to Complex1f
static inline Complex1f to_float (Complex1h const x) {
    return __m128(convert8h_4f(__m128i(x)));
}

// function to_float16: convert Complex1h to Complex1f
static inline Complex1h to_float16 (Complex1f const x) {
    return __m128i(convert4f_8h(__m128(x)));
} 

/*****************************************************************************
*
*          Operators for Complex1h
*
*****************************************************************************/

// operator + : add
static inline Complex1h operator + (Complex1h const a, Complex1h const b) {
    return to_float16(to_float(a) + to_float(b));
}

// operator += : add
static inline Complex1h & operator += (Complex1h & a, Complex1h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex1h operator - (Complex1h const a, Complex1h const b) {
    return to_float16(to_float(a) - to_float(b));
}

// operator - : unary minus
static inline Complex1h operator - (Complex1h const a) {
    return __m128i(-a.to_vector());
}

// operator -= : subtract
static inline Complex1h & operator -= (Complex1h & a, Complex1h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex1h operator * (Complex1h const a, Complex1h const b) {
    return to_float16(to_float(a) * to_float(b));
}

// operator *= : multiply
static inline Complex1h & operator *= (Complex1h & a, Complex1h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex1h operator / (Complex1h const a, Complex1h const b) {
    return to_float16(to_float(a) / to_float(b));
}

// operator /= : divide
static inline Complex1h & operator /= (Complex1h & a, Complex1h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex1h operator ~ (Complex1h const a) {
    return __m128i(change_sign<0,1,0,0,0,0,0,0>(a.to_vector()));
}

// operator == : returns true if a == b
static inline bool operator == (Complex1h const a, Complex1h const b) {
    return to_float(a) == to_float(b);
}

// operator != : returns true if a != b
static inline bool operator != (Complex1h const a, Complex1h const b) {
    return to_float(a) != to_float(b);
}

/*****************************************************************************
*
*          Operators mixing Complex1h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex1h operator + (Complex1h const a, Float16 b) {
    return a + Complex1h(b);
}
static inline Complex1h operator + (Float16 a, Complex1h const b) {
    return b + a;
}
static inline Complex1h & operator += (Complex1h & a, Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex1h operator - (Complex1h const a, Float16 b) {
    return a - Complex1h(b);
}
static inline Complex1h operator - (Float16 a, Complex1h const b) {
    return Complex1h(a) - b;
}
static inline Complex1h & operator -= (Complex1h & a, Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex1h operator * (Complex1h const a, Float16 b) {
    return __m128i(a.to_vector() * b);
}
static inline Complex1h operator * (Float16 a, Complex1h const b) {
    return b * a;
}
static inline Complex1h & operator *= (Complex1h & a, Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex1h operator / (Complex1h const a, Float16 b) {
    return __m128i(a.to_vector() / b);
}

static inline Complex1h operator / (Float16 a, Complex1h const b) {
    return to_float16(float(a) / to_float(b));
}

static inline Complex1h & operator /= (Complex1h & a, Float16 b) {
    a = a / b;
    return a;
}

/*****************************************************************************
*
*          Functions for Complex1h
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline float abs(Complex1h const a) {
    return abs(to_float(a));
}

// function csqrt: complex square root
static inline Complex1h csqrt(Complex1h const a) {
    return to_float16(csqrt(to_float(a)));
}

// function select
static inline Complex1h select (bool s, Complex1h const a, Complex1h const b) {
    return s ? a : b;
}
static inline Complex1h select (Vec8sb s, Complex1h const a, Complex1h const b) {
    return __m128i(select(s, Vec8s(a), Vec8s(b)));
}

// interleave real and imag into complex vector
static inline Complex1h interleave_c (Float16 const re, Float16 const im) {
    return Complex1h(re, im);
}


/*****************************************************************************
*
*               Class Complex2h: two half precision complex numbers
*
*****************************************************************************/


class Complex2h {
protected:
    __m128i xmm; // vector of 8 half precision floats. Only the first two are used
public:
    // default constructor
    Complex2h() = default;
    // construct from real, no imaginary part
    Complex2h(Float16 re) {
        xmm = Vec8h(re, 0, re, 0, 0, 0, 0, 0);
    }
    Complex2h(float re) {
        xmm = Vec8h(Float16(re), 0, Float16(re), 0, 0, 0, 0, 0);
    }
    // construct from real and imaginary parts
    Complex2h(Float16 re0, Float16 im0, Float16 re1, Float16 im1) {
        xmm = Vec8h(re0, im0, re1, im1, 0, 0, 0, 0);
    }
    // construct from one Complex1h, broadcast to all
    Complex2h(Complex1h const a) {
        xmm = _mm_unpacklo_epi32(a, a);
    }
    // construct from two Complex1h
    Complex2h(Complex1h const a, Complex1h const b) {
        xmm = _mm_unpacklo_epi32(a, b);
    }
    // construct from real and imaginary part, broadcast to all
    Complex2h(Float16 re, Float16 im) {
        xmm = Complex2h(Complex1h(re, im));
    }
    // Constructor to convert from type __m128i used in intrinsics:
    Complex2h(__m128i const x) {
        xmm = x;
    }
    // Assignment operator to convert from type __m128i used in intrinsics:
    Complex2h & operator = (__m128i const x) {
        xmm = x;
        return *this;
    }
    // Type cast operator to convert to __m128i used in intrinsics
    operator __m128i() const {
        return xmm;
    }
    // Member function to convert to vector
    Vec8h to_vector() const {
        return xmm;
    }
    // Member function to load from array (unaligned)
    Complex2h & load(void const * p) {
        xmm = Vec8s().load_partial(4, p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        Vec8s(xmm).store_partial(4, p);
    }
    // Member functions to split into two Complex1h:
    Complex1h get_low() const {
        return Complex1h(Vec8h(xmm).cutoff(2));
    }
    Complex1h get_high() const {
        return _mm_shuffle_epi32(xmm, 0xFD);
    } 
    // Member function to insert one complex scalar into complex vector
    Complex2h insert (int index, Complex1h x) {
#if INSTRSET >= 10   // AVX512VL
        xmm = _mm_mask_broadcastd_epi32(xmm, __mmask8(1u << index), x);
#else
        int32_t f[4];
        store(f);
        if (uint32_t(index) < 2) {
            x.store(f+index);
            load(f);
        }
#endif
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1h extract (int i) const {
        int32_t x[4];
        store(x);
        return Complex1h().load(x + i);
    }
    // get real parts
    Vec8h real() const {
        return permute8<0,2,-1,-1,-1,-1,-1,-1>(to_vector());
    }
    // get imaginary parts
    Vec8h imag() const {
        return permute8<1,3,-1,-1,-1,-1,-1,-1>(to_vector());
    }
    static constexpr int size() {
        return 2;
    }
    static constexpr int elementtype() {
        return 0x10F;
    }
};

/*****************************************************************************
*
*          conversions Complex2h <-> Complex2f
*
*****************************************************************************/
// function to_float: convert Complex2h to Complex2f
static inline Complex2f to_float (Complex2h const x) {
    return __m128(convert8h_4f(__m128i(x)));
}

// function to_float16: convert Complex2h to Complex1f
static inline Complex2h to_float16 (Complex2f const x) {
    return __m128i(convert4f_8h(__m128(x)));
} 

/*****************************************************************************
*
*          Operators for Complex2h
*
*****************************************************************************/

// operator + : add
static inline Complex2h operator + (Complex2h const a, Complex2h const b) {
    return to_float16(to_float(a) + to_float(b));
}

// operator += : add
static inline Complex2h & operator += (Complex2h & a, Complex2h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex2h operator - (Complex2h const a, Complex2h const b) {
    return to_float16(to_float(a) - to_float(b));
}

// operator - : unary minus
static inline Complex2h operator - (Complex2h const a) {
    return __m128i(-a.to_vector());
}

// operator -= : subtract
static inline Complex2h & operator -= (Complex2h & a, Complex2h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex2h operator * (Complex2h const a, Complex2h const b) {
    return to_float16(to_float(a) * to_float(b));
}

// operator *= : multiply
static inline Complex2h & operator *= (Complex2h & a, Complex2h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex2h operator / (Complex2h const a, Complex2h const b) {
    return to_float16(to_float(a) / to_float(b));
}

// operator /= : divide
static inline Complex2h & operator /= (Complex2h & a, Complex2h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex2h operator ~ (Complex2h const a) {
    return __m128i(change_sign<0,1,0,1,0,0,0,0>(a.to_vector()));
}

// operator == : returns true if a == b
static inline Vec8hb operator == (Complex2h const a, Complex2h const b) {
    Vec4fb eq = to_float(a) == to_float(b);
#if INSTRSET >= 10  // compact boolean vectors
    return __mmask8(eq);
#else
    return Vec8sb(_mm_packs_epi32( _mm_castps_si128(eq), _mm_setzero_si128()));
#endif
}

// operator != : returns true if a != b
static inline Vec8hb operator != (Complex2h const a, Complex2h const b) {
    return !(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex2h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex2h operator + (Complex2h const a, Float16 b) {
    return a + Complex2h(b);
}
static inline Complex2h operator + (Float16 a, Complex2h const b) {
    return b + a;
}
static inline Complex2h & operator += (Complex2h & a, Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex2h operator - (Complex2h const a, Float16 b) {
    return a - Complex2h(b);
}
static inline Complex2h operator - (Float16 a, Complex2h const b) {
    return Complex2h(a) - b;
}
static inline Complex2h & operator -= (Complex2h & a, Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex2h operator * (Complex2h const a, Float16 b) {
    return __m128i(a.to_vector() * b);
}
static inline Complex2h operator * (Float16 a, Complex2h const b) {
    return b * a;
}
static inline Complex2h & operator *= (Complex2h & a, Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex2h operator / (Complex2h const a, Float16 b) {
    return __m128i(a.to_vector() / b);
}

static inline Complex2h operator / (Float16 a, Complex2h const b) {
    return to_float16(float(a) / to_float(b));
}

static inline Complex2h & operator /= (Complex2h & a, Float16 b) {
    a = a / b;
    return a;
}

/*****************************************************************************
*
*          Functions for Complex2h
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex2h abs(Complex2h const a) {
    return to_float16(abs(to_float(a)));
}

// function csqrt: complex square root
static inline Complex2h csqrt(Complex2h const a) {
    return to_float16(csqrt(to_float(a)));
}

// function select
static inline Complex2h select (Vec8hb s, Complex2h const a, Complex2h const b) {
    return __m128i(select(s, Vec8s(a), Vec8s(b)));
}

// interleave real and imag into complex vector
static inline Complex2h interleave_c2 (Vec8h const re, Vec8h const im) {
    Vec8h c = blend8<0,8,1,9,-1,-1,-1,-1>(re, im);
    return __m128i(c);
}


/*****************************************************************************
*
*               Class Complex4h: four half precision complex numbers
*
*****************************************************************************/

class Complex4h {
protected:
    __m128i xmm; // vector of 8 half precision floats
public:
    // default constructor
    Complex4h() = default;
    // construct from real, no imaginary part
    Complex4h(Float16 re) {
        xmm = Vec8h(re, 0, re, 0, re, 0, re, 0);
    }
    Complex4h(float re) {
        xmm = Vec8h(Float16(re), 0, Float16(re), 0, Float16(re), 0, Float16(re), 0);
    }
    // construct from real and imaginary parts
    Complex4h(Float16 re0, Float16 im0, Float16 re1, Float16 im1, Float16 re2, Float16 im2, Float16 re3, Float16 im3) {
        xmm = Vec8h(re0, im0, re1, im1, re2, im2, re3, im3);
    }
    // construct from one Complex1h, broadcast to all
    Complex4h(Complex1h const a) {
        xmm = _mm_shuffle_epi32(a, 0);
    }
    // construct from two Complex2h
    Complex4h(Complex2h const a, Complex2h const b) {
        xmm = _mm_unpacklo_epi64(a, b);
    }
    // construct from four Complex1h
    Complex4h(Complex1h const a, Complex1h const b, Complex1h const c, Complex1h const d) {
        xmm = Complex4h(Complex2h(a, b), Complex2h(c, d));
    }
    // construct from real and imaginary part, broadcast to all
    Complex4h(Float16 re, Float16 im) {
        xmm = Complex4h(Complex1h(re, im));
    }
    // Constructor to convert from type __m128i used in intrinsics:
    Complex4h(__m128i const x) {
        xmm = x;
    }
    // Assignment operator to convert from type __m128i used in intrinsics:
    Complex4h & operator = (__m128i const x) {
        xmm = x;
        return *this;
    }
    // Type cast operator to convert to __m128i used in intrinsics
    operator __m128i() const {
        return xmm;
    }
    // Member function to convert to vector
    Vec8h to_vector() const {
        return xmm;
    }
    // Member function to load from array (unaligned)
    Complex4h & load(void const * p) {
        xmm = _mm_loadu_si128((__m128i const*)p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        _mm_storeu_si128((__m128i *)p, xmm);
    }
    // Member functions to split into two Complex2h:
    Complex2h get_low() const {
        return _mm_unpacklo_epi64(xmm, _mm_setzero_si128());
    }
    Complex2h get_high() const {
        return _mm_unpackhi_epi64(xmm, _mm_setzero_si128());
    }
    // Member function to insert one complex scalar into complex vector
    Complex4h insert (int index, Complex1h x) {
#if INSTRSET >= 10   // AVX512VL
        xmm = _mm_mask_broadcastd_epi32(xmm, __mmask8(1u << index), x);
#else
        int32_t f[4];
        store(f);
        if (uint32_t(index) < 4) {
            x.store(f+index);
            load(f);
        }
#endif
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1h extract (int i) const {
        int32_t x[4];
        store(x);
        return Complex1h().load(x + i);
    }
    // get real parts
    Vec8h real() const {
        return permute8<0,2,4,6,-1,-1,-1,-1>(to_vector());
    }
    // get imaginary parts
    Vec8h imag() const {
        return permute8<1,3,5,7,-1,-1,-1,-1>(to_vector());
    }
    static constexpr int size() {
        return 4;
    }
    static constexpr int elementtype() {
        return 0x10F;
    }
};

/*****************************************************************************
*
*          conversions Complex4h <-> Complex4f
*
*****************************************************************************/
// function to_float: convert Complex4h to Complex4f
static inline Complex4f to_float (Complex4h const x) {
    return Complex4f(to_float(x.to_vector()));
}

// function to_float16: convert Complex4f to Complex4h
static inline Complex4h to_float16 (Complex4f const x) {
    return Complex4h(to_float16(x.to_vector()));
} 

/*****************************************************************************
*
*          Operators for Complex4h
*
*****************************************************************************/

// operator + : add
static inline Complex4h operator + (Complex4h const a, Complex4h const b) {
    return to_float16(to_float(a) + to_float(b));
}

// operator += : add
static inline Complex4h & operator += (Complex4h & a, Complex4h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex4h operator - (Complex4h const a, Complex4h const b) {
    return to_float16(to_float(a) - to_float(b));
}

// operator - : unary minus
static inline Complex4h operator - (Complex4h const a) {
    return __m128i(-a.to_vector());
}

// operator -= : subtract
static inline Complex4h & operator -= (Complex4h & a, Complex4h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex4h operator * (Complex4h const a, Complex4h const b) {
    return to_float16(to_float(a) * to_float(b));
}

// operator *= : multiply
static inline Complex4h & operator *= (Complex4h & a, Complex4h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex4h operator / (Complex4h const a, Complex4h const b) {
    return to_float16(to_float(a) / to_float(b));
}

// operator /= : divide
static inline Complex4h & operator /= (Complex4h & a, Complex4h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex4h operator ~ (Complex4h const a) {
    return __m128i(change_sign<0,1,0,1,0,1,0,1>(a.to_vector()));
}

// operator == : returns true if a == b
static inline Vec8hb operator == (Complex4h const a, Complex4h const b) {
#if INSTRSET >= 10  // compact boolean vectors
    Vec8fb eq = to_float(a) == to_float(b);
    return __mmask16(eq);
#else   // broad boolean vectors
    Vec4i e0 = reinterpret_i(to_float(a.get_low()) == to_float(b.get_low()));
    Vec4i e1 = reinterpret_i(to_float(a.get_high()) == to_float(b.get_high()));
    return Vec8sb(_mm_packs_epi32(e0, e1));
#endif
}

// operator != : returns true if a != b
static inline Vec8hb operator != (Complex4h const a, Complex4h const b) {
    return !(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex4h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex4h operator + (Complex4h const a, Float16 b) {
    return a + Complex4h(b);
}
static inline Complex4h operator + (Float16 a, Complex4h const b) {
    return b + a;
}
static inline Complex4h & operator += (Complex4h & a, Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex4h operator - (Complex4h const a, Float16 b) {
    return a - Complex4h(b);
}
static inline Complex4h operator - (Float16 a, Complex4h const b) {
    return Complex4h(a) - b;
}
static inline Complex4h & operator -= (Complex4h & a, Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex4h operator * (Complex4h const a, Float16 b) {
    return __m128i(a.to_vector() * b);
}
static inline Complex4h operator * (Float16 a, Complex4h const b) {
    return b * a;
}
static inline Complex4h & operator *= (Complex4h & a, Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex4h operator / (Complex4h const a, Float16 b) {
    return __m128i(a.to_vector() / b);
}

static inline Complex4h operator / (Float16 a, Complex4h const b) {
    return to_float16(float(a) / to_float(b));
}

static inline Complex4h & operator /= (Complex4h & a, Float16 b) {
    a = a / b;
    return a;
}

/*****************************************************************************
*
*          Functions for Complex4h
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex4h abs(Complex4h const a) {
    return to_float16(abs(to_float(a)));
}

// function csqrt: complex square root
static inline Complex4h csqrt(Complex4h const a) {
    return to_float16(csqrt(to_float(a)));
}

// function select
static inline Complex4h select (Vec8hb s, Complex4h const a, Complex4h const b) {
    return __m128i(select(s, Vec8s(a), Vec8s(b)));
}

// interleave real and imag into complex vector
static inline Complex4h interleave_c4 (Vec8h const re, Vec8h const im) {
    Vec8h c = blend8<0,8,1,9,2,10,3,11>(re, im);
    return __m128i(c);
}


#if MAX_VECTOR_SIZE >= 512

/*****************************************************************************
*
*               Class Complex8h: eight half precision complex numbers
*
*****************************************************************************/

class Complex8h {
protected:
    Vec16h yy;
public:
    // default constructor
    Complex8h() = default;
    // construct from two Complex4h
    Complex8h(Complex4h const a, Complex4h const b) {
#if INSTRSET >= 8
        yy = set_m128ir(a, b);
#else
        yy = Vec16h(Vec8h(a), Vec8h(b));
#endif
    }
    // Member function to convert to vector
    Vec16h to_vector() const {
        return yy;
    }
    // construct from real, no imaginary part
    Complex8h(Float16 re) {
        yy = Complex8h(Complex4h(re), Complex4h(re)).to_vector();
    }
    Complex8h(float re) {
        yy = Complex8h(Complex4h(Float16(re)), Complex4h(Float16(re))).to_vector();
    }
    // construct from real and imaginary parts
    Complex8h(Float16 re0, Float16 im0, Float16 re1, Float16 im1, Float16 re2, Float16 im2, Float16 re3, Float16 im3,
        Float16 re4, Float16 im4, Float16 re5, Float16 im5, Float16 re6, Float16 im6, Float16 re7, Float16 im7) :
        yy(re0, im0, re1, im1, re2, im2, re3, im3, re4, im4, re5, im5, re6, im6, re7, im7) {
    }
    // construct from one Complex1h, broadcast to all
    Complex8h(Complex1h const a) {
        *this = Complex8h(Complex4h(a), Complex4h(a));
    }
    // construct from eight Complex1h
    Complex8h(Complex1h const a0, Complex1h const a1, Complex1h const a2, Complex1h const a3,
        Complex1h const a4, Complex1h const a5, Complex1h const a6, Complex1h const a7) {
        *this = Complex8h(Complex4h(a0, a1, a2, a3), Complex4h(a4, a5, a6, a7));
    }
    // construct from real and imaginary part, broadcast to all
    Complex8h(Float16 re, Float16 im) {
        *this = Complex8h(Complex1h(re, im));
    }
#if INSTRSET >= 8
    // Constructor to convert from type __m256i used in intrinsics:
    Complex8h(__m256i const x) {
        yy = x;
    }
    // Assignment operator to convert from type __m128i used in intrinsics:
    Complex8h & operator = (__m256i const x) {
        yy = x;
        return *this;
    }
    // Type cast operator to convert to __m256i used in intrinsics
    operator __m256i() const {
        return yy;
    }
#endif
    // Constructor to convert from type Vec16h
    Complex8h(Vec16h const x) {
        yy = x;
    }
    // Member function to load from array (unaligned)
    Complex8h & load(void const * p) {
        yy.load(p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        yy.store(p);
    }
    // Member functions to split into two Complex2h:
    Complex4h get_low() const {
        return __m128i(yy.get_low());
    }
    Complex4h get_high() const {
        return __m128i(yy.get_high());
    }
    // Member function to insert one complex scalar into complex vector
    Complex8h insert (int index, Complex1h x) {
#if INSTRSET >= 10   // AVX512VL
        yy = _mm256_mask_broadcastd_epi32(yy, __mmask8(1u << index), x);
#else
        int32_t f[8];
        store(f);
        if (uint32_t(index) < 8) {
            x.store(f+index);
            load(f);
        }
#endif
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1h extract (int i) const {
        int32_t x[8];
        yy.store(x);
        return Complex1h().load(x + i);
    }
    // get real parts
    Vec8h real() const {
        return permute16<0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15>(to_vector()).get_low();
    }
    // get imaginary parts
    Vec8h imag() const {
        // make the permute a common subexpression that can be eliminated 
        // by the compiler if both real() and imag() are called
        return permute16<0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15>(to_vector()).get_high();
    }
    static constexpr int size() {
        return 8;
    }
    static constexpr int elementtype() {
        return 0x10F;
    }
};

/*****************************************************************************
*
*          conversions Complex8h <-> Complex4f
*
*****************************************************************************/
// function to_float: convert Complex8h to Complex8f
static inline Complex8f to_float (Complex8h const x) {
    return Complex8f(to_float(x.to_vector()));
}

// function to_float16: convert Complex8f to Complex8h
static inline Complex8h to_float16 (Complex8f const x) {
    return Complex8h(to_float16(x.to_vector()));
} 

/*****************************************************************************
*
*          Operators for Complex8h
*
*****************************************************************************/

// operator + : add
static inline Complex8h operator + (Complex8h const a, Complex8h const b) {
    return to_float16(to_float(a) + to_float(b));
}

// operator += : add
static inline Complex8h & operator += (Complex8h & a, Complex8h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex8h operator - (Complex8h const a, Complex8h const b) {
    return to_float16(to_float(a) - to_float(b));
}

// operator - : unary minus
static inline Complex8h operator - (Complex8h const a) {
    return (-a.to_vector());
}

// operator -= : subtract
static inline Complex8h & operator -= (Complex8h & a, Complex8h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex8h operator * (Complex8h const a, Complex8h const b) {
    return to_float16(to_float(a) * to_float(b));
}

// operator *= : multiply
static inline Complex8h & operator *= (Complex8h & a, Complex8h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex8h operator / (Complex8h const a, Complex8h const b) {
    return to_float16(to_float(a) / to_float(b));
}

// operator /= : divide
static inline Complex8h & operator /= (Complex8h & a, Complex8h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex8h operator ~ (Complex8h const a) {
    return (change_sign<0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1>(a.to_vector()));
}

// operator == : returns true if a == b
static inline Vec16hb operator == (Complex8h const a, Complex8h const b) {
#if INSTRSET >= 10  // compact boolean vectors
    Vec16fb eq = to_float(a) == to_float(b);
    return __mmask16(eq);
#else   // broad boolean vectors
    Vec8sb e0 = a.get_low() == b.get_low();
    Vec8sb e1 = a.get_high() == b.get_high();
    Vec16sb r(e0, e1);
    return r;
#endif
}

// operator != : returns true if a != b
static inline Vec16hb operator != (Complex8h const a, Complex8h const b) {
    return !(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex8h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex8h operator + (Complex8h const a, Float16 b) {
    return a + Complex8h(b);
}
static inline Complex8h operator + (Float16 a, Complex8h const b) {
    return b + a;
}
static inline Complex8h & operator += (Complex8h & a, Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex8h operator - (Complex8h const a, Float16 b) {
    return a - Complex8h(b);
}
static inline Complex8h operator - (Float16 a, Complex8h const b) {
    return Complex8h(a) - b;
}
static inline Complex8h & operator -= (Complex8h & a, Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex8h operator * (Complex8h const a, Float16 b) {
    return a.to_vector() * b;
}
static inline Complex8h operator * (Float16 a, Complex8h const b) {
    return b * a;
}
static inline Complex8h & operator *= (Complex8h & a, Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex8h operator / (Complex8h const a, Float16 b) {
    return a.to_vector() / b;
}

static inline Complex8h operator / (Float16 a, Complex8h const b) {
    return to_float16(float(a) / to_float(b));
}

static inline Complex8h & operator /= (Complex8h & a, Float16 b) {
    a = a / b;
    return a;
}

/*****************************************************************************
*
*          Functions for Complex8h
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex8h abs(Complex8h const a) {
    return to_float16(abs(to_float(a)));
}

// function csqrt: complex square root
static inline Complex8h csqrt(Complex8h const a) {
    return to_float16(csqrt(to_float(a)));
}

// function select
static inline Complex8h select (Vec16hb s, Complex8h const a, Complex8h const b) {
    return reinterpret_h(select(s, Vec16s(a.to_vector()), Vec16s(b.to_vector())));
}

// interleave real and imag into complex vector
static inline Complex8h interleave_c (Vec8h const re, Vec8h const im) {
    Vec16h b(re, im);
    Vec16h c = permute16<0,8,1,9,2,10,3,11,4,12,5,13,6,14,7,15>(b);
#if INSTRSET >= 8
    return __m256i(c);
#else
    return Complex8h(c);
#endif
}


/*****************************************************************************
*
*               Class Complex16h: 16 half precision complex numbers
*
*****************************************************************************/

class Complex16h {
protected:
    Vec32h zz;
public:
    // default constructor
    Complex16h() = default;
    // construct from two Complex8h
    Complex16h(Complex8h const a, Complex8h const b) {
#if INSTRSET >= 10
        zz = _mm512_inserti64x4(_mm512_castsi256_si512(a), b, 1);
#else
        zz = Vec32h(a.to_vector(), b.to_vector());
#endif
    }
    // Member function to convert to vector
    Vec32h to_vector() const {
        return zz;
    }
    // construct from real, no imaginary part
    Complex16h(Float16 re) {
        zz = Complex16h(Complex8h(re), Complex8h(re)).to_vector();
    }
    Complex16h(float re) {
        zz = Complex16h(Complex8h(Float16(re)), Complex8h(Float16(re))).to_vector();
    }
    // construct from real and imaginary parts
    Complex16h(Float16 re0, Float16 im0, Float16 re1, Float16 im1, Float16 re2, Float16 im2, Float16 re3, Float16 im3,
        Float16 re4, Float16 im4, Float16 re5, Float16 im5, Float16 re6, Float16 im6, Float16 re7, Float16 im7,        
        Float16 re8, Float16 im8, Float16 re9, Float16 im9, Float16 re10, Float16 im10, Float16 re11, Float16 im11,
        Float16 re12, Float16 im12, Float16 re13, Float16 im13, Float16 re14, Float16 im14, Float16 re15, Float16 im15) :
        zz(re0, im0, re1, im1, re2, im2, re3, im3,re4, im4, re5, im5, re6, im6, re7, im7,        
            re8, im8, re9, im9, re10, im10, re11, im11,re12, im12, re13, im13, re14, im14, re15, im15) {
    }
    // construct from one Complex1h, broadcast to all
    Complex16h(Complex1h const a) {
        *this = Complex16h(Complex8h(a), Complex8h(a));
    }
    // construct from 16 Complex1h
    Complex16h(Complex1h const a0, Complex1h const a1, Complex1h const a2, Complex1h const a3,
        Complex1h const a4, Complex1h const a5, Complex1h const a6, Complex1h const a7,
        Complex1h const a8, Complex1h const a9, Complex1h const a10, Complex1h const a11,
        Complex1h const a12, Complex1h const a13, Complex1h const a14, Complex1h const a15) {
        *this = Complex16h(Complex8h(a0, a1, a2, a3, a4, a5, a6, a7), Complex8h(a8, a9, a10, a11, a12, a13, a14, a15));
    }
    // construct from real and imaginary part, broadcast to all
    Complex16h(Float16 re, Float16 im) {
        *this = Complex16h(Complex1h(re, im));
    }
#if INSTRSET >= 10
    // Constructor to convert from type __m512i used in intrinsics:
    Complex16h(__m512i const x) {
        zz = x;
    }
    // Assignment operator to convert from type __m128i used in intrinsics:
    Complex16h & operator = (__m512i const x) {
        zz = x;
        return *this;
    }
    // Type cast operator to convert to __m512i used in intrinsics
    operator __m512i() const {
        return zz;
    }
#endif
    // Constructor to convert from type Vec32h
    Complex16h(Vec32h const x) {
        zz = x;
    }
    // Member function to load from array (unaligned)
    Complex16h & load(void const * p) {
        zz.load(p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        zz.store(p);
    }
    // Member functions to split into two Complex2h:
    Complex8h get_low() const {
        return zz.get_low();
    }
    Complex8h get_high() const {
        return zz.get_high();
    }
    // Member function to insert one complex scalar into complex vector
    Complex16h insert (int index, Complex1h x) {
#if INSTRSET >= 10   // AVX512VL
        zz = _mm512_mask_broadcastd_epi32(zz, __mmask16(1u << index), x);
#else
        int32_t f[16];
        store(f);
        if (uint32_t(index) < 16) {
            x.store(f+index);
            load(f);
        }
#endif
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1h extract (int i) const {
        int32_t x[16];
        zz.store(x);
        return Complex1h().load(x + i);
    }
    // get real parts
    Vec16h real() const {
        return permute32<0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,1,3,5,7,9,11,13,15, 17,19,21,23,25,27,29,31>(to_vector()).get_low();
    }
    // get imaginary parts
    Vec16h imag() const {
        // make the permute a common subexpression that can be eliminated 
        // by the compiler if both real() and imag() are called
        return permute32<0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,1,3,5,7,9,11,13,15, 17,19,21,23,25,27,29,31>(to_vector()).get_high();
    }
    static constexpr int size() {
        return 16;
    }
    static constexpr int elementtype() {
        return 0x10F;
    }
};

/*****************************************************************************
*
*          conversions Complex16h <-> 
*
*****************************************************************************/
// There are no conversions because there is no Complex16f

/*****************************************************************************
*
*          Operators for Complex16h
*
*****************************************************************************/

// operator + : add
static inline Complex16h operator + (Complex16h const a, Complex16h const b) {
    return Complex16h(a.get_low() + b.get_low(), a.get_high() + b.get_high());
}

// operator += : add
static inline Complex16h & operator += (Complex16h & a, Complex16h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex16h operator - (Complex16h const a, Complex16h const b) {
    return Complex16h(a.get_low() - b.get_low(), a.get_high() - b.get_high());
}

// operator - : unary minus
static inline Complex16h operator - (Complex16h const a) {
    return (-a.to_vector());
}

// operator -= : subtract
static inline Complex16h & operator -= (Complex16h & a, Complex16h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex16h operator * (Complex16h const a, Complex16h const b) {
    return Complex16h(a.get_low() * b.get_low(), a.get_high() * b.get_high());
}

// operator *= : multiply
static inline Complex16h & operator *= (Complex16h & a, Complex16h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex16h operator / (Complex16h const a, Complex16h const b) {
    return Complex16h(a.get_low() / b.get_low(), a.get_high() / b.get_high());
}

// operator /= : divide
static inline Complex16h & operator /= (Complex16h & a, Complex16h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex16h operator ~ (Complex16h const a) {
    return (change_sign<0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1>(a.to_vector()));
}

// operator == : returns true if a == b
static inline Vec32hb operator == (Complex16h const a, Complex16h const b) {
    Vec16sb e0 = a.get_low() == b.get_low();
    Vec16sb e1 = a.get_high() == b.get_high();
#if INSTRSET >= 10  // compact boolean vectors
    return __mmask32(__mmask16(e0) | __mmask16(e1) << 16);
#else   // broad boolean vectors
    Vec32sb r(e0, e1);
    return r;
#endif
}

// operator != : returns true if a != b
static inline Vec32hb operator != (Complex16h const a, Complex16h const b) {
    return !(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex16h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex16h operator + (Complex16h const a, Float16 b) {
    return a + Complex16h(b);
}
static inline Complex16h operator + (Float16 a, Complex16h const b) {
    return b + a;
}
static inline Complex16h & operator += (Complex16h & a, Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex16h operator - (Complex16h const a, Float16 b) {
    return a - Complex16h(b);
}
static inline Complex16h operator - (Float16 a, Complex16h const b) {
    return Complex16h(a) - b;
}
static inline Complex16h & operator -= (Complex16h & a, Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex16h operator * (Complex16h const a, Float16 b) {
    return a.to_vector() * b;
}
static inline Complex16h operator * (Float16 a, Complex16h const b) {
    return b * a;
}
static inline Complex16h & operator *= (Complex16h & a, Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex16h operator / (Complex16h const a, Float16 b) {
    return a.to_vector() / b;
}

static inline Complex16h operator / (Float16 a, Complex16h const b) {
    return Complex16h(a / b.get_low(), a / b.get_high());
}

static inline Complex16h & operator /= (Complex16h & a, Float16 b) {
    a = a / b;
    return a;
}

/*****************************************************************************
*
*          Functions for Complex16h
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex16h abs(Complex16h const a) {
    return Complex16h(abs(a.get_low()), abs(a.get_high()));
}

// function csqrt: complex square root
static inline Complex16h csqrt(Complex16h const a) {
    return Complex16h(csqrt(a.get_low()), csqrt(a.get_high()));
}

// function select
static inline Complex16h select (Vec32hb s, Complex16h const a, Complex16h const b) {
    return Complex16h(reinterpret_h(select(s, Vec32s(a.to_vector()), Vec32s(b.to_vector()))));
}

// interleave real and imag into complex vector
static inline Complex16h interleave_c (Vec16h const re, Vec16h const im) {
    Vec32h b(re, im);
    Vec32h c = permute32<0,16,1,17,2,18,3,19,4,20,5,21,6,22,7,23,    
        8,24,9,25,10,26,11,27,12,28,13,29,14,30,15,31>(b);
#if INSTRSET >= 10
    return __m512i(c);
#else
    return Complex16h(c);
#endif
}


#endif  //  MAX_VECTOR_SIZE >= 512

#ifdef VCL_NAMESPACE
}
#endif

#endif  // COMPLEXVECFP16E_H
