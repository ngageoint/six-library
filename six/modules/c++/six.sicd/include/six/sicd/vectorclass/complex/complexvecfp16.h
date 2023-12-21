/***************************  complexvecfp16.h   ******************************
* Author:        Agner Fog
* Date created:  2012-07-24
* Last modified: 2022-07-20
* Version:       2.02.00
* Project:       Extension to vector class library
* Description:
* Classes for complex number algebra and math with half precision
* using the AVX512_FP16 instruction set.
* The following classes are defined:
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

#ifndef COMPLEXVECFP16_H
#define COMPLEXVECFP16_H  200

#if INSTRSET < 10 || !defined(__AVX512FP16__)
// half precision instructions not supported. must emulate
#include "complexvecfp16e.h"

#else   // INSTRSET >= 10 && defined(__AVX512FP16__)
// half precision instructions supported.

#include "complexvec1.h"
#include "vectorfp16.h"

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
    __m128h xmm; // vector of 8 half precision floats. Only the first two are used
public:
    // default constructor
    Complex1h() = default;
    // construct from real, no imaginary part
    Complex1h(_Float16 re) {
        xmm = _mm_load_sh(&re);
    }
    Complex1h(float re) {
        xmm = Complex1h(_Float16(re));
    }
    // construct from real and imaginary part
    Complex1h(_Float16 re, _Float16 im) {
        xmm = _mm_setr_ph(re, im, 0, 0, 0, 0, 0, 0);
    }
    // Constructor to convert from type __m128h used in intrinsics:
    Complex1h(__m128h const x) {
        xmm = x;
    }
    // Assignment operator to convert from type __m128h used in intrinsics:
    Complex1h & operator = (__m128h const x) {
        xmm = x;
        return *this;
    }
    // Type cast operator to convert to __m128h used in intrinsics
    operator __m128h() const {
        return xmm;
    }
    // Member function to convert to vector
    Vec8h to_vector() const {
        return xmm;
    }
    // Member function to load from array (unaligned)
    Complex1h & load(void const * p) {
        xmm = Vec8h().load_partial(2, p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        Vec8h(xmm).store_partial(2, p);
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
    _Float16 real() const {
        return _mm_cvtsh_h(xmm);
    }
    // get imaginary part
    _Float16 imag() const {
        return Vec8h(xmm).extract(1);
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
*          Operators for Complex1h
*
*****************************************************************************/

// operator + : add
static inline Complex1h operator + (Complex1h const a, Complex1h const b) {
    return _mm_add_ph(a, b);
}

// operator += : add
static inline Complex1h & operator += (Complex1h & a, Complex1h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex1h operator - (Complex1h const a, Complex1h const b) {
    return _mm_sub_ph(a, b);
}

// operator - : unary minus
static inline Complex1h operator - (Complex1h const a) {
    return _mm_castps_ph(_mm_xor_ps(_mm_castph_ps(a), _mm_castsi128_ps(_mm_setr_epi32(0x80008000, 0, 0, 0))));
}

// operator -= : subtract
static inline Complex1h & operator -= (Complex1h & a, Complex1h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex1h operator * (Complex1h const a, Complex1h const b) {
    return _mm_fmul_sch(a, b);   // complex multiply instruction VFMULCSH
}

// operator *= : multiply
static inline Complex1h & operator *= (Complex1h & a, Complex1h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex1h operator / (Complex1h const a, Complex1h const b) {
    __m128h ab = _mm_cmul_sch(a, b);  // vfcmulcsh instruction = a * ~b
    __m128h bb = _mm_mul_ph(b, b);  // b.re * b.re, b.im * b.im
    __m128h bbflip = _mm_castsi128_ph(_mm_shufflelo_epi16(_mm_castph_si128(bb), 1));  // Swap bb.re and bb.im
    __m128h bb2 = _mm_add_ph(bb, bbflip);   // b.re * b.re + b.im * b.im
    __m128h dd = _mm_maskz_div_ph(3, ab, bb2); // ab / bb2
    return dd;
}

// operator /= : divide
static inline Complex1h & operator /= (Complex1h & a, Complex1h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex1h operator ~ (Complex1h const a) {
    return _mm_castps_ph(_mm_xor_ps(_mm_castph_ps(a), _mm_castsi128_ps(_mm_setr_epi32(0x80000000, 0, 0, 0))));
}

// operator == : returns true if a == b
static inline bool operator == (Complex1h const a, Complex1h const b) {
    __mmask8 m = _mm_cmp_ph_mask(a, b, 0);
    return (m & 3) == 3;
}

// operator != : returns true if a != b
static inline bool operator != (Complex1h const a, Complex1h const b) {
    __mmask8 m = _mm_cmp_ph_mask(a, b, 4);
    return (m & 3) != 0;
}

/*****************************************************************************
*
*          Operators mixing Complex1h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex1h operator + (Complex1h const a, _Float16 b) {
    return _mm_add_sh(a, _mm_set_sh(b));
}
static inline Complex1h operator + (_Float16 a, Complex1h const b) {
    return b + a;
}
static inline Complex1h & operator += (Complex1h & a, _Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex1h operator - (Complex1h const a, _Float16 b) {
    return _mm_sub_sh(a, _mm_set_sh(b));
}
static inline Complex1h operator - (_Float16 a, Complex1h const b) {
    return Complex1h(a) - b;
}
static inline Complex1h & operator -= (Complex1h & a, _Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex1h operator * (Complex1h const a, _Float16 b) {
    return _mm_mul_ph(a, _mm_set1_ph(b));
}
static inline Complex1h operator * (_Float16 a, Complex1h const b) {
    return b * a;
}
static inline Complex1h & operator *= (Complex1h & a, _Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex1h operator / (Complex1h const a, _Float16 b) {
    return _mm_div_ph(a, _mm_set1_ph(b));
}

static inline Complex1h operator / (_Float16 a, Complex1h const b) {
    __m128h bb = _mm_mul_ph(b, b);  // b.re * b.re, b.im * b.im
    __m128h bbflip = _mm_castsi128_ph(_mm_shufflelo_epi16(_mm_castph_si128(bb), 1));  // Swap bb.re and bb.im
    __m128h bb2 = _mm_add_sh(bb, bbflip);   // b.re * b.re + b.im * b.i 
    _Float16  b4 = _mm_cvtsh_h(bb2);
    return ~b * (a / b4);
}

static inline Complex1h & operator /= (Complex1h & a, _Float16 b) {
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
    __m128h bb = _mm_mul_ph(a, a);  // a.re * a.re, a.im * a.im
    __m128h bbflip = _mm_castsi128_ph(_mm_shufflelo_epi16(_mm_castph_si128(bb), 1));  // Swap bb.re and bb.im
    __m128h bb2 = _mm_add_sh(bb, bbflip);   // b.re * b.re + b.im * b.i 
    _Float16 b4 = _mm_cvtsh_h(bb2);
    return std::sqrt(float(b4));
}

// function csqrt: complex square root
static inline Complex1h csqrt(Complex1h const a) {
    Vec8h aa = a.to_vector();                    // vector of real and imaginary elements
    Vec8h t1 = aa * aa;                          // r*r, i*i
    Vec8h t2 = _mm_castsi128_ph(_mm_shufflelo_epi16(_mm_castph_si128(t1), 1));  // Swap t1.re and t1.im
    Vec8h t3 = t1 + t2;                          // pairwise horizontal sum
    Vec8h n = _mm_sqrt_ph(t3);                   // n = sqrt(r*r+i*i)
    __m128i t4 = _mm_unpacklo_epi16(_mm_castph_si128(aa), _mm_castph_si128(aa));  // copy real part of a
    __m128i sbithi = _mm_setr_epi32(0x80000000, 0, 0, 0);
    Vec8h t6 = _mm_castsi128_ph(_mm_xor_si128(t4, sbithi)); // r, -r
    Vec8h t7 = n + t6;                           // n+r, n-r
    Vec8h t8 = t7 * 0.5f;
    Vec8h t9 = _mm_sqrt_ph(t8);                  // sqrt((n+r)/2), sqrt((n-r)/2) 
    // (aa & sbithi) ^ t9 :
    __m128h t11 = _mm_castsi128_ph(_mm_ternarylogic_epi32(_mm_castph_si128(aa), sbithi, _mm_castph_si128(t9), 0x6A));
    return Complex1h(t11);
}

// function select
static inline Complex1h select (bool s, Complex1h const a, Complex1h const b) {
    return s ? a : b;
}
static inline Complex1h select (Vec8hb s, Complex1h const a, Complex1h const b) {
    return _mm_castsi128_ph(_mm_mask_mov_epi16(_mm_castph_si128(b), s, _mm_castph_si128(a)));  // s ? a : b
}

// interleave real and imag into complex vector
static inline Complex1h interleave_c (_Float16 const re, _Float16 const im) {
    return Complex1h(re, im);
}

/*****************************************************************************
*
*          Conversion functions
*
*****************************************************************************/

// function to_float: convert Complex1h to Complex1f
static inline Complex1f to_float (Complex1h const a) {
    return _mm_cvtph_ps(_mm_castph_si128(a));
}

// function to_double: convert Complex1h to Complex1d
static inline Complex1d to_double (Complex1h const a) {
    return _mm_cvtph_pd(a);
}

// function to_float16: convert Complex1h to Complex1f
static inline Complex1h to_float16 (Complex1f const a) {
    return _mm_castsi128_ph(_mm_cvtps_ph(a, 0));
}


/*****************************************************************************
*
*               Class Complex2h: two half precision complex numbers
*
*****************************************************************************/

class Complex2h {
protected:
    __m128h xmm; // vector of 8 half precision floats. Only the first four are used
public:
    // default constructor
    Complex2h() = default;
    // construct from real, no imaginary part
    Complex2h(_Float16 re) {
        xmm = _mm_castsi128_ph(_mm_maskz_mov_epi16(0b0101, _mm_castph_si128(_mm_set1_ph(re))));
    }
    Complex2h(float re) {
        xmm = Complex2h(_Float16(re));
    }
    // construct from real and imaginary parts
    Complex2h(_Float16 re0, _Float16 im0, _Float16 re1, _Float16 im1) {
        xmm = _mm_setr_ph(re0, im0, re1, im1, 0, 0, 0, 0);
    }
    // construct from one Complex1h, broadcast to all
    Complex2h(Complex1h const a) {
        xmm = _mm_castps_ph(_mm_maskz_broadcastss_ps(3, _mm_castph_ps(a)));
    }
    // construct from two Complex1h
    Complex2h(Complex1h const a, Complex1h const b) {
        xmm = _mm_castsi128_ph(_mm_unpacklo_epi32(_mm_castph_si128(a), _mm_castph_si128(b)));
    } 
    // construct from real and imaginary part, broadcast to all
    Complex2h(_Float16 re, _Float16 im) {
        xmm = Complex2h(Complex1h(re, im));
    }
    // Constructor to convert from type __m128h used in intrinsics:
    Complex2h(__m128h const x) {
        xmm = x;
    }
    // Assignment operator to convert from type __m128h used in intrinsics:
    Complex2h & operator = (__m128h const x) {
        xmm = x;
        return *this;
    }
    // Type cast operator to convert to __m128h used in intrinsics
    operator __m128h() const {
        return xmm;
    }
    // Member function to convert to vector
    Vec8h to_vector() const {
        return xmm;
    }
    // Member function to load from array (unaligned)
    Complex2h & load(void const * p) {
        xmm = Vec8h().load_partial(4, p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        Vec8h(xmm).store_partial(4, p);
    }
    // Member functions to split into two Complex1h:
    Complex1h get_low() const {
        return Complex1h(Vec8h(xmm).cutoff(2));
    }
    Complex1h get_high() const {
        __m128 t = _mm_shuffle_ps(_mm_castph_ps(xmm), _mm_castph_ps(xmm), 0xFD);
        return Complex1h(_mm_castps_ph(t));
    }
    // Member function to insert one complex scalar into complex vector
    Complex2h insert (int index, Complex1h x) {
        xmm = _mm_castsi128_ph(_mm_mask_broadcastd_epi32(_mm_castph_si128(xmm), __mmask8(1u << index), _mm_castph_si128(x)));
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1h extract (int i) const {
        return _mm_castps_ph(_mm_maskz_compress_ps(1 << i, _mm_castph_ps(xmm)));
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
*          Operators for Complex2h
*
*****************************************************************************/

// operator + : add
static inline Complex2h operator + (Complex2h const a, Complex2h const b) {
    return _mm_add_ph(a, b);
}

// operator += : add
static inline Complex2h & operator += (Complex2h & a, Complex2h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex2h operator - (Complex2h const a, Complex2h const b) {
    return _mm_sub_ph(a, b);
}

// operator - : unary minus
static inline Complex2h operator - (Complex2h const a) {
    return _mm_castps_ph(_mm_xor_ps(_mm_castph_ps(a), _mm_castsi128_ps(_mm_setr_epi32(0x80008000, 0x80008000, 0, 0))));
}

// operator -= : subtract
static inline Complex2h & operator -= (Complex2h & a, Complex2h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex2h operator * (Complex2h const a, Complex2h const b) {
    return _mm_fmul_pch(a, b);   // complex multiply instruction VFMULCSH
}

// operator *= : multiply
static inline Complex2h & operator *= (Complex2h & a, Complex2h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex2h operator / (Complex2h const a, Complex2h const b) {
    __m128h ab = _mm_cmul_pch(a, b);             // vfcmulcph instruction = a * ~b
    __m128h bb = _mm_mul_ph(b, b);               // b.re * b.re, b.im * b.im
    __m128h bbflip = _mm_castsi128_ph(_mm_shufflelo_epi16(_mm_castph_si128(bb), 0xB1));  // Swap bb.re and bb.im
    __m128h bb2 = _mm_add_ph(bb, bbflip);        // b.re * b.re + b.im * b.im
    __m128h dd = _mm_maskz_div_ph(0x0F, ab, bb2);// ab / bb2
    return dd;
}

// operator /= : divide
static inline Complex2h & operator /= (Complex2h & a, Complex2h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex2h operator ~ (Complex2h const a) {
    return _mm_castps_ph(_mm_xor_ps(_mm_castph_ps(a), _mm_castsi128_ps(_mm_setr_epi32(0x80000000, 0x80000000, 0, 0))));
}

// operator == : returns true if a == b
static inline Vec8hb operator == (Complex2h const a, Complex2h const b) {
    uint8_t k1 = _mm_cmp_ph_mask(a, b, 0);
    uint8_t k2 = k1 & (k1 >> 1) & 5;             // AND bits from real and imaginary parts
    return uint8_t(k2 * 3);                      // expand each into two bits
}

// operator != : returns true if a != b
static inline Vec8hb operator != (Complex2h const a, Complex2h const b) {
    return ~(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex2h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex2h operator + (Complex2h const a, _Float16 b) {
    return a + Complex2h(b);
}
static inline Complex2h operator + (_Float16 a, Complex2h const b) {
    return b + a;
}
static inline Complex2h & operator += (Complex2h & a, _Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex2h operator - (Complex2h const a, _Float16 b) {
    return a - Complex2h(b);
}
static inline Complex2h operator - (_Float16 a, Complex2h const b) {
    return Complex2h(a) - b;
}
static inline Complex2h & operator -= (Complex2h & a, _Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex2h operator * (Complex2h const a, _Float16 b) {
    return _mm_mul_ph(a, _mm_set1_ph(b));
}
static inline Complex2h operator * (_Float16 a, Complex2h const b) {
    return b * a;
}
static inline Complex2h & operator *= (Complex2h & a, _Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex2h operator / (Complex2h const a, _Float16 b) {
    return _mm_div_ph(a, _mm_set1_ph(b));
}

static inline Complex2h operator / (_Float16 a, Complex2h const b) {
    __m128h bb = _mm_mul_ph(b, b);  // b.re * b.re, b.im * b.im
    __m128h bbflip = _mm_castsi128_ph(_mm_shufflelo_epi16(_mm_castph_si128(bb), 0xB1));  // Swap bb.re and bb.im
    __m128h bb2 = _mm_add_ph(bb, bbflip);   // b.re * b.re + b.im * b.i
    __m128h cc = _mm_div_ph(_mm_set1_ph(a), bb2);
    return _mm_mul_ph(~b, cc);
}

static inline Complex2h & operator /= (Complex2h & a, _Float16 b) {
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
    __m128h bb = _mm_mul_ph(a, a);  // a.re * a.re, a.im * a.im
    __m128h bbflip = _mm_castsi128_ph(_mm_shufflelo_epi16(_mm_castph_si128(bb), 0xB1));  // Swap bb.re and bb.im
    __m128h bb2 = _mm_add_ph(bb, bbflip);   // b.re * b.re + b.im * b.i 
    return _mm_maskz_sqrt_ph(5, bb2);  // sqrt of real parts, set imag parts to 0
}

// function csqrt: complex square root
static inline Complex2h csqrt(Complex2h const a) {
    Vec8h aa = a.to_vector();                    // vector of real and imaginary elements
    Vec8h t1 = aa * aa;                          // r*r, i*i
    Vec8h t2 = _mm_castsi128_ph(_mm_shufflelo_epi16(_mm_castph_si128(t1), 0xB1));  // Swap t1.re and t1.im
    Vec8h t3 = t1 + t2;                          // pairwise horizontal sum
    Vec8h n = _mm_sqrt_ph(t3);                   // n = sqrt(r*r+i*i) 
    __m128i t4 = _mm_shufflelo_epi16(_mm_castph_si128(aa), 0xA0);  // copy reals part of a
    __m128i sbithi = _mm_setr_epi32(0x80000000, 0x80000000, 0, 0);
    Vec8h t6 = _mm_castsi128_ph(_mm_xor_si128(t4, sbithi)); // r, -r
    Vec8h t7 = n + t6;                           // n+r, n-r
    Vec8h t8 = t7 * 0.5f;
    Vec8h t9 = _mm_sqrt_ph(t8);                  // sqrt((n+r)/2), sqrt((n-r)/2) 
    // (aa & sbithi) ^ t9 :
    __m128h t11 = _mm_castsi128_ph(_mm_ternarylogic_epi32(_mm_castph_si128(aa), sbithi, _mm_castph_si128(t9), 0x6A));
    return Complex2h(t11);
}

// function select
static inline Complex2h select (Vec8hb s, Complex2h const a, Complex2h const b) {
    return _mm_castsi128_ph(_mm_mask_mov_epi16(_mm_castph_si128(b), s, _mm_castph_si128(a)));  // s ? a : b
}

// interleave real and imag into complex vector
static inline Complex2h interleave_c2 (Vec8h const re, Vec8h const im) {
    Vec8h c = blend8<0,8,1,9,-1,-1,-1,-1>(re, im);
    return __m128h(c);
}

/*****************************************************************************
*
*          Conversion functions
*
*****************************************************************************/

// function to_float: convert Complex2h to Complex2f
static inline Complex2f to_float (Complex2h const a) {
    return _mm_cvtph_ps(_mm_castph_si128(a));
}
#if MAX_VECTOR_SIZE >= 256
// function to_double: convert Complex2h to Complex2d
static inline Complex2d to_double (Complex2h const a) {
    return _mm256_cvtph_pd(a);
}
#endif
// function to_float16: convert Complex2h to Complex2f
static inline Complex2h to_float16 (Complex2f const a) {
    return _mm_castsi128_ph(_mm_cvtps_ph(a, 0));
}


/*****************************************************************************
*
*               Class Complex4h: four half precision complex numbers
*
*****************************************************************************/

class Complex4h {
protected:
    __m128h xmm; // vector of 8 half precision floats
public:
    // default constructor
    Complex4h() = default;
    // construct from real, no imaginary part
    Complex4h(_Float16 re) {
        xmm = _mm_castsi128_ph(_mm_maskz_mov_epi16(0b01010101, _mm_castph_si128(_mm_set1_ph(re))));
    }
    Complex4h(float re) {
        xmm = Complex4h(_Float16(re));
    }
    // construct from real and imaginary parts
    Complex4h(_Float16 re0, _Float16 im0, _Float16 re1, _Float16 im1, _Float16 re2, _Float16 im2, _Float16 re3, _Float16 im3) {
        xmm = _mm_setr_ph(re0, im0, re1, im1, re2, im2, re3, im3);
    }
    // construct from one Complex1h, broadcast to all
    Complex4h(Complex1h const a) {
        xmm = _mm_castps_ph(_mm_broadcastss_ps(_mm_castph_ps(a)));
    }
    // construct from two Complex2h
    Complex4h(Complex2h const c0, Complex2h const c1) {
        __m128i t = _mm_unpacklo_epi64(_mm_castph_si128(c0), _mm_castph_si128(c1));
        xmm = _mm_castsi128_ph(t);
    }
    // construct from four Complex1h
    Complex4h(Complex1h const c0, Complex1h const c1, Complex1h const c2, Complex1h const c3) {
        __m128i c01   = _mm_unpacklo_epi32(_mm_castph_si128(c0), _mm_castph_si128(c1));
        __m128i c23   = _mm_unpacklo_epi32(_mm_castph_si128(c2), _mm_castph_si128(c3));
        __m128i c0123 = _mm_unpacklo_epi64(c01, c23);
        xmm = _mm_castsi128_ph(c0123);
    }
    // construct from real and imaginary part, broadcast to all
    Complex4h(_Float16 re, _Float16 im) {
        xmm = Complex4h(Complex1h(re, im));
    }
    // Constructor to convert from type __m128h used in intrinsics:
    Complex4h(__m128h const x) {
        xmm = x;
    }
    // Assignment operator to convert from type __m128h used in intrinsics:
    Complex4h & operator = (__m128h const x) {
        xmm = x;
        return *this;
    }
    // Type cast operator to convert to __m128h used in intrinsics
    operator __m128h() const {
        return xmm;
    }
    // Member function to convert to vector
    Vec8h to_vector() const {
        return xmm;
    }
    // Member function to load from array (unaligned)
    Complex4h & load(void const * p) {
        xmm = _mm_loadu_ph(p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        _mm_storeu_ph(p, xmm);
    }
    // Member functions to split into two Complex2h:
    Complex2h get_low() const {
        return Complex2h(Vec8h(xmm).cutoff(4));
    }
    Complex2h get_high() const {
        __m128i t = _mm_maskz_shuffle_epi32(3, _mm_castph_si128(xmm), _MM_PERM_ENUM(0x0E));
        return Complex2h(_mm_castsi128_ph(t));
    }
    // Member function to insert one complex scalar into complex vector
    Complex4h insert (int index, Complex1h x) {
        xmm = _mm_castsi128_ph(_mm_mask_broadcastd_epi32(_mm_castph_si128(xmm), __mmask8(1u << index), _mm_castph_si128(x)));
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1h extract (int i) const {
        return _mm_castps_ph(_mm_maskz_compress_ps(1 << i, _mm_castph_ps(xmm)));
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
*          Operators for Complex4h
*
*****************************************************************************/

// operator + : add
static inline Complex4h operator + (Complex4h const a, Complex4h const b) {
    return _mm_add_ph(a, b);
}

// operator += : add
static inline Complex4h & operator += (Complex4h & a, Complex4h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex4h operator - (Complex4h const a, Complex4h const b) {
    return _mm_sub_ph(a, b);
}

// operator - : unary minus
static inline Complex4h operator - (Complex4h const a) {
    return _mm_castps_ph(_mm_xor_ps(_mm_castph_ps(a), _mm_castsi128_ps(_mm_set1_epi32(0x80008000))));
}

// operator -= : subtract
static inline Complex4h & operator -= (Complex4h & a, Complex4h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex4h operator * (Complex4h const a, Complex4h const b) {
    return _mm_fmul_pch(a, b);   // complex multiply instruction VFMULCSH
}

// operator *= : multiply
static inline Complex4h & operator *= (Complex4h & a, Complex4h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex4h operator / (Complex4h const a, Complex4h const b) {
    __m128h ab = _mm_cmul_pch(a, b);             // vfcmulcph instruction = a * ~b
    __m128h bb = _mm_mul_ph(b, b);               // b.re * b.re, b.im * b.im
    __m128h bbflip = _mm_castsi128_ph(_mm_rol_epi32(_mm_castph_si128(bb), 16));  // Swap bb.re and bb.im
    __m128h bb2 = _mm_add_ph(bb, bbflip);        // b.re * b.re + b.im * b.im
    __m128h dd = _mm_div_ph(ab, bb2);// ab / bb2
    return dd;
}

// operator /= : divide
static inline Complex4h & operator /= (Complex4h & a, Complex4h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex4h operator ~ (Complex4h const a) {
    return _mm_castps_ph(_mm_xor_ps(_mm_castph_ps(a), _mm_castsi128_ps(_mm_set1_epi32(0x80000000))));
}

// operator == : returns true if a == b
static inline Vec8hb operator == (Complex4h const a, Complex4h const b) {
    uint8_t k1 = _mm_cmp_ph_mask(a, b, 0);
    uint8_t k2 = k1 & (k1 >> 1) & 0x55;          // AND bits from real and imaginary parts
    return uint8_t(k2 * 3);                      // expand each into two bits
}

// operator != : returns true if a != b
static inline Vec8hb operator != (Complex4h const a, Complex4h const b) {
    return ~(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex4h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex4h operator + (Complex4h const a, _Float16 b) {
    return a + Complex4h(b);
}
static inline Complex4h operator + (_Float16 a, Complex4h const b) {
    return b + a;
}
static inline Complex4h & operator += (Complex4h & a, _Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex4h operator - (Complex4h const a, _Float16 b) {
    return a - Complex4h(b);
}
static inline Complex4h operator - (_Float16 a, Complex4h const b) {
    return Complex4h(a) - b;
}
static inline Complex4h & operator -= (Complex4h & a, _Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex4h operator * (Complex4h const a, _Float16 b) {
    return _mm_mul_ph(a, _mm_set1_ph(b));
}
static inline Complex4h operator * (_Float16 a, Complex4h const b) {
    return b * a;
}
static inline Complex4h & operator *= (Complex4h & a, _Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex4h operator / (Complex4h const a, _Float16 b) {
    return _mm_div_ph(a, _mm_set1_ph(b));
}

static inline Complex4h operator / (_Float16 a, Complex4h const b) {
    __m128h bb = _mm_mul_ph(b, b);  // b.re * b.re, b.im * b.im
    __m128h bbflip = _mm_castsi128_ph(_mm_rol_epi32(_mm_castph_si128(bb), 16));  // Swap bb.re and bb.im
    __m128h bb2 = _mm_add_ph(bb, bbflip);   // b.re * b.re + b.im * b.i
    __m128h cc = _mm_div_ph(_mm_set1_ph(a), bb2);
    return _mm_mul_ph(~b, cc);
}

static inline Complex4h & operator /= (Complex4h & a, _Float16 b) {
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
    __m128h bb = _mm_mul_ph(a, a);               // a.re * a.re, a.im * a.im
    __m128h bbflip = _mm_castsi128_ph(_mm_rol_epi32(_mm_castph_si128(bb), 16));  // Swap bb.re and bb.im
    __m128h bb2 = _mm_add_ph(bb, bbflip);        // b.re * b.re + b.im * b.i 
    return _mm_maskz_sqrt_ph(0x55, bb2);         // sqrt of real parts, set imag parts to 0
}

// function csqrt: complex square root
static inline Complex4h csqrt(Complex4h const a) {
    Vec8h aa = a.to_vector();                    // vector of real and imaginary elements
    Vec8h t1 = aa * aa;                          // r*r, i*i
    Vec8h t2 = _mm_castsi128_ph(_mm_rol_epi32(_mm_castph_si128(t1), 16));  // Swap t1.re and t1.im
    Vec8h t3 = t1 + t2;                          // pairwise horizontal sum
    Vec8h n = _mm_sqrt_ph(t3);                   // n = sqrt(r*r+i*i) 
    __m128i t4 = _mm_shufflelo_epi16(_mm_castph_si128(aa), 0xA0);  // copy reals part of a
    __m128i t5 = _mm_shufflehi_epi16(t4, 0xA0);  // copy reals part of a
    __m128i sbithi = _mm_set1_epi32(0x80000000);
    Vec8h t6 = _mm_castsi128_ph(_mm_xor_si128(t5, sbithi)); // r, -r
    Vec8h t7 = n + t6;                           // n+r, n-r
    Vec8h t8 = t7 * 0.5f;
    Vec8h t9 = _mm_sqrt_ph(t8);                  // sqrt((n+r)/2), sqrt((n-r)/2) 
    // (aa & sbithi) ^ t9 :
    __m128h t11 = _mm_castsi128_ph(_mm_ternarylogic_epi32(_mm_castph_si128(aa), sbithi, _mm_castph_si128(t9), 0x6A));
    return Complex4h(t11);
}

// function select
static inline Complex4h select (Vec8hb s, Complex4h const a, Complex4h const b) {
    return _mm_castsi128_ph(_mm_mask_mov_epi16(_mm_castph_si128(b), s, _mm_castph_si128(a)));  // s ? a : b
}

// interleave real and imag into complex vector
static inline Complex4h interleave_c4 (Vec8h const re, Vec8h const im) {
    Vec8h c = blend8<0,8,1,9,2,10,3,11>(re, im);
    return __m128h(c);
}

/*****************************************************************************
*
*          Conversion functions
*
*****************************************************************************/

#if MAX_VECTOR_SIZE >= 512
// function to_double: convert Complex4h to Complex4d
static inline Complex4d to_double (Complex4h const a) {
    return _mm512_cvtph_pd(a);
}
#endif

#if MAX_VECTOR_SIZE >= 256

// function to_float: convert Complex4h to Complex4f
static inline Complex4f to_float (Complex4h const a) {
    return _mm256_cvtph_ps(_mm_castph_si128(a));
}

// function to_float16: convert Complex4f to Complex4h
static inline Complex4h to_float16 (Complex4f const a) {
    return _mm_castsi128_ph(_mm256_cvtps_ph(a, 0));
}

/*****************************************************************************
*
*               Class Complex8h: four half precision complex numbers
*
*****************************************************************************/

class Complex8h {
protected:
    __m256h ymm; // vector of 8 half precision floats
public:
    // default constructor
    Complex8h() = default;
    // construct from real, no imaginary part
    Complex8h(_Float16 re) {
        ymm = _mm256_castsi256_ph(_mm256_maskz_mov_epi16(0x5555, _mm256_castph_si256(_mm256_set1_ph(re))));
    }
    Complex8h(float re) {
        ymm = Complex8h(_Float16(re));
    }
    // construct from real and imaginary parts
    Complex8h(_Float16 re0, _Float16 im0, _Float16 re1, _Float16 im1, _Float16 re2, _Float16 im2, _Float16 re3, _Float16 im3,
        _Float16 re4, _Float16 im4, _Float16 re5, _Float16 im5, _Float16 re6, _Float16 im6, _Float16 re7, _Float16 im7) {
        ymm = _mm256_setr_ph(re0, im0, re1, im1, re2, im2, re3, im3, re4, im4, re5, im5, re6, im6, re7, im7);
    }
    // construct from one Complex1h, broadcast to all
    Complex8h(Complex1h const a) {
        ymm = _mm256_castps_ph(_mm256_broadcastss_ps(_mm_castph_ps(a)));
    }
    // construct from two Complex4h
    Complex8h(Complex4h const c0, Complex4h const c1) {
        __m256 t = _mm256_insertf128_ps(_mm256_castps128_ps256(_mm_castph_ps(c0)), _mm_castph_ps(c1), 1);
        ymm = _mm256_castps_ph(t);
    }
    // construct from eight Complex1h
    Complex8h(Complex1h const c0, Complex1h const c1, Complex1h const c2, Complex1h const c3,
        Complex1h const c4, Complex1h const c5, Complex1h const c6, Complex1h const c7) {
        ymm = Complex8h(Complex4h(c0, c1, c2, c3), Complex4h(c4, c5, c6, c7));
    }
    // construct from real and imaginary part, broadcast to all
    Complex8h(_Float16 re, _Float16 im) {
        ymm = Complex8h(Complex1h(re, im));
    }
    // Constructor to convert from type __m256h used in intrinsics:
    Complex8h(__m256h const x) {
        ymm = x;
    }
    // Assignment operator to convert from type __m256h used in intrinsics:
    Complex8h & operator = (__m256h const x) {
        ymm = x;
        return *this;
    }
    // Type cast operator to convert to __m256h used in intrinsics
    operator __m256h() const {
        return ymm;
    }
    // Member function to convert to vector
    Vec16h to_vector() const {
        return ymm;
    }
    // Member function to load from array (unaligned)
    Complex8h & load(void const * p) {
        ymm = _mm256_loadu_ph(p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        _mm256_storeu_ph(p, ymm);
    }
    // Member functions to split into two Complex4h:
    Complex4h get_low() const {
        return  _mm256_castph256_ph128(ymm);
    }
    Complex4h get_high() const {
        return _mm_castps_ph(_mm256_extractf32x4_ps(_mm256_castph_ps(ymm), 1));
    }
    // Member function to insert one complex scalar into complex vector
    Complex8h insert (int index, Complex1h x) {
        ymm = _mm256_castsi256_ph(_mm256_mask_broadcastd_epi32(_mm256_castph_si256(ymm), __mmask8(1u << index), _mm_castph_si128(x)));
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1h extract (int i) const {
        return _mm_castps_ph(_mm256_castps256_ps128(_mm256_maskz_compress_ps(1 << i, _mm256_castph_ps(ymm))));
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
*          Operators for Complex8h
*
*****************************************************************************/

// operator + : add
static inline Complex8h operator + (Complex8h const a, Complex8h const b) {
    return _mm256_add_ph(a, b);
}

// operator += : add
static inline Complex8h & operator += (Complex8h & a, Complex8h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex8h operator - (Complex8h const a, Complex8h const b) {
    return _mm256_sub_ph(a, b);
}

// operator - : unary minus
static inline Complex8h operator - (Complex8h const a) {
    return _mm256_castps_ph(_mm256_xor_ps(_mm256_castph_ps(a), _mm256_castsi256_ps(_mm256_set1_epi32(0x80008000))));
}

// operator -= : subtract
static inline Complex8h & operator -= (Complex8h & a, Complex8h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex8h operator * (Complex8h const a, Complex8h const b) {
    return _mm256_fmul_pch(a, b);   // complex multiply instruction VFMULCSH
}

// operator *= : multiply
static inline Complex8h & operator *= (Complex8h & a, Complex8h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex8h operator / (Complex8h const a, Complex8h const b) {
    __m256h ab = _mm256_cmul_pch(a, b);             // vfcmulcph instruction = a * ~b
    __m256h bb = _mm256_mul_ph(b, b);               // b.re * b.re, b.im * b.im
    __m256h bbflip = _mm256_castsi256_ph(_mm256_rol_epi32(_mm256_castph_si256(bb), 16));  // Swap bb.re and bb.im
    __m256h bb2 = _mm256_add_ph(bb, bbflip);        // b.re * b.re + b.im * b.im
    __m256h dd = _mm256_div_ph(ab, bb2);// ab / bb2
    return dd;
}

// operator /= : divide
static inline Complex8h & operator /= (Complex8h & a, Complex8h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex8h operator ~ (Complex8h const a) {
    return _mm256_castps_ph(_mm256_xor_ps(_mm256_castph_ps(a), _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000))));
}

// operator == : returns true if a == b
static inline Vec16hb operator == (Complex8h const a, Complex8h const b) {
    uint16_t k1 = _mm256_cmp_ph_mask(a, b, 0);
    uint16_t k2 = k1 & (k1 >> 1) & 0x5555;       // AND bits from real and imaginary parts
    return uint16_t(k2 * 3);                     // expand each into two bits
}

// operator != : returns true if a != b
static inline Vec16hb operator != (Complex8h const a, Complex8h const b) {
    return ~(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex8h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex8h operator + (Complex8h const a, _Float16 b) {
    return a + Complex8h(b);
}
static inline Complex8h operator + (_Float16 a, Complex8h const b) {
    return b + a;
}
static inline Complex8h & operator += (Complex8h & a, _Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex8h operator - (Complex8h const a, _Float16 b) {
    return a - Complex8h(b);
}
static inline Complex8h operator - (_Float16 a, Complex8h const b) {
    return Complex8h(a) - b;
}
static inline Complex8h & operator -= (Complex8h & a, _Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex8h operator * (Complex8h const a, _Float16 b) {
    return _mm256_mul_ph(a, _mm256_set1_ph(b));
}
static inline Complex8h operator * (_Float16 a, Complex8h const b) {
    return b * a;
}
static inline Complex8h & operator *= (Complex8h & a, _Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex8h operator / (Complex8h const a, _Float16 b) {
    return _mm256_div_ph(a, _mm256_set1_ph(b));
}

static inline Complex8h operator / (_Float16 a, Complex8h const b) {
    __m256h bb = _mm256_mul_ph(b, b);  // b.re * b.re, b.im * b.im
    __m256h bbflip = _mm256_castsi256_ph(_mm256_rol_epi32(_mm256_castph_si256(bb), 16));  // Swap bb.re and bb.im
    __m256h bb2 = _mm256_add_ph(bb, bbflip);   // b.re * b.re + b.im * b.i
    __m256h cc = _mm256_div_ph(_mm256_set1_ph(a), bb2);
    return _mm256_mul_ph(~b, cc);
}

static inline Complex8h & operator /= (Complex8h & a, _Float16 b) {
    a = a / b;
    return a;
}

// interleave real and imag into complex vector
static inline Complex8h interleave_c (Vec8h const re, Vec8h const im) {
    Vec16h b(re, im);
    Vec16h c = permute16<0,8,1,9,2,10,3,11,4,12,5,13,6,14,7,15>(b);
    return __m256h(c);
}

/*****************************************************************************
*
*          Functions for Complex8h
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex8h abs(Complex8h const a) {
    __m256h bb = _mm256_mul_ph(a, a);               // a.re * a.re, a.im * a.im
    __m256h bbflip = _mm256_castsi256_ph(_mm256_rol_epi32(_mm256_castph_si256(bb), 16));  // Swap bb.re and bb.im
    __m256h bb2 = _mm256_add_ph(bb, bbflip);        // b.re * b.re + b.im * b.i 
    return _mm256_maskz_sqrt_ph(0x5555, bb2);       // sqrt of real parts, set imag parts to 0
}

// function csqrt: complex square root
static inline Complex8h csqrt(Complex8h const a) {
    Vec16h aa = a.to_vector();                    // vector of real and imaginary elements
    Vec16h t1 = aa * aa;                          // r*r, i*i
    Vec16h t2 = _mm256_castsi256_ph(_mm256_rol_epi32(_mm256_castph_si256(t1), 16));  // Swap t1.re and t1.im
    Vec16h t3 = t1 + t2;                          // pairwise horizontal sum
    Vec16h n = _mm256_sqrt_ph(t3);                // n = sqrt(r*r+i*i) 
    __m256i t4 = _mm256_shufflelo_epi16(_mm256_castph_si256(aa), 0xA0);  // copy reals part of a
    __m256i t5 = _mm256_shufflehi_epi16(t4, 0xA0);  // copy reals part of a
    __m256i sbithi = _mm256_set1_epi32(0x80000000);
    Vec16h t6 = _mm256_castsi256_ph(_mm256_xor_si256(t5, sbithi)); // r, -r
    Vec16h t7 = n + t6;                           // n+r, n-r
    Vec16h t8 = t7 * 0.5f;
    Vec16h t9 = _mm256_sqrt_ph(t8);               // sqrt((n+r)/2), sqrt((n-r)/2) 
    // (aa & sbithi) ^ t9 :
    __m256h t11 = _mm256_castsi256_ph(_mm256_ternarylogic_epi32(_mm256_castph_si256(aa), sbithi, _mm256_castph_si256(t9), 0x6A));
    return Complex8h(t11);
}

// function select
static inline Complex8h select (Vec16hb s, Complex8h const a, Complex8h const b) {
    return _mm256_castsi256_ph(_mm256_mask_mov_epi16(_mm256_castph_si256(b), s, _mm256_castph_si256(a)));  // s ? a : b
}

#endif  //  MAX_VECTOR_SIZE >= 256

#if MAX_VECTOR_SIZE >= 512

/*****************************************************************************
*
*          Conversion functions
*
*****************************************************************************/

// function to_float: convert Complex8h to Complex8f
static inline Complex8f to_float (Complex8h const a) {
    return _mm512_cvtph_ps(_mm256_castph_si256(a));
}

// function to_float16: convert Complex8f to Complex8h
static inline Complex8h to_float16 (Complex8f const a) {
    return _mm256_castsi256_ph(_mm512_cvtps_ph(a, 0));
} 


/*****************************************************************************
*
*               Class Complex16h: four half precision complex numbers
*
*****************************************************************************/

class Complex16h {
protected:
    __m512h zmm; // vector of 8 half precision floats
public:
    // default constructor
    Complex16h() = default;
    // construct from real, no imaginary part
    Complex16h(_Float16 re) {
        zmm = _mm512_castsi512_ph(_mm512_maskz_mov_epi16(0x55555555, _mm512_castph_si512(_mm512_set1_ph(re))));
    }
    Complex16h(float re) {
        zmm = Complex16h(_Float16(re));
    }
    // construct from real and imaginary parts
    Complex16h(_Float16 re0, _Float16 im0, _Float16 re1, _Float16 im1, _Float16 re2, _Float16 im2, _Float16 re3, _Float16 im3,
        _Float16 re4, _Float16 im4, _Float16 re5, _Float16 im5, _Float16 re6, _Float16 im6, _Float16 re7, _Float16 im7,
        _Float16 re8, _Float16 im8, _Float16 re9, _Float16 im9, _Float16 re10, _Float16 im10, _Float16 re11, _Float16 im11,
        _Float16 re12, _Float16 im12, _Float16 re13, _Float16 im13, _Float16 re14, _Float16 im14, _Float16 re15, _Float16 im15) {
        zmm = _mm512_setr_ph(re0, im0, re1, im1, re2, im2, re3, im3, re4, im4, re5, im5, re6, im6, re7, im7,
            re8, im8, re9, im9, re10, im10, re11, im11, re12, im12, re13, im13, re14, im14, re15, im15);
    }
    // construct from one Complex1h, broadcast to all
    Complex16h(Complex1h const a) {
        zmm = _mm512_castps_ph(_mm512_broadcastss_ps(_mm_castph_ps(a)));
    }
    // construct from two Complex8h
    Complex16h(Complex8h const c0, Complex8h const c1) {
        __m512 t = _mm512_insertf32x8(_mm512_castps256_ps512(_mm256_castph_ps(c0)), _mm256_castph_ps(c1), 1);
        zmm = _mm512_castps_ph(t);
    }
    // construct from 16 Complex1h
    Complex16h(Complex1h const c0, Complex1h const c1, Complex1h const c2, Complex1h const c3,
        Complex1h const c4, Complex1h const c5, Complex1h const c6, Complex1h const c7,
        Complex1h const c8, Complex1h const c9, Complex1h const c10, Complex1h const c11,
        Complex1h const c12, Complex1h const c13, Complex1h const c14, Complex1h const c15) {
        zmm = Complex16h(Complex8h(c0, c1, c2, c3, c4, c5, c6, c7), Complex8h(c8, c9, c10, c11, c12, c13, c14, c15));
    }
    // construct from real and imaginary part, broadcast to all
    Complex16h(_Float16 re, _Float16 im) {
        zmm = Complex16h(Complex1h(re, im));
    }
    // Constructor to convert from type __m512h used in intrinsics:
    Complex16h(__m512h const x) {
        zmm = x;
    }
    // Assignment operator to convert from type __m512h used in intrinsics:
    Complex16h & operator = (__m512h const x) {
        zmm = x;
        return *this;
    }
    // Type cast operator to convert to __m512h used in intrinsics
    operator __m512h() const {
        return zmm;
    }
    // Member function to convert to vector
    Vec32h to_vector() const {
        return zmm;
    }
    // Member function to load from array (unaligned)
    Complex16h & load(void const * p) {
        zmm = _mm512_loadu_ph(p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(void * p) const {
        _mm512_storeu_ph(p, zmm);
    }
    // Member functions to split into two Complex8h:
    Complex8h get_low() const {
        return  _mm512_castph512_ph256(zmm);
    }
    Complex8h get_high() const {
        return _mm256_castps_ph(_mm512_extractf32x8_ps(_mm512_castph_ps(zmm), 1));
    }
    // Member function to insert one complex scalar into complex vector
    Complex16h insert (int index, Complex1h x) {
        zmm = _mm512_castsi512_ph(_mm512_mask_broadcastd_epi32(_mm512_castph_si512(zmm), __mmask16(1u << index), _mm_castph_si128(x)));
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1h extract (int i) const {
        return _mm_castps_ph(_mm512_castps512_ps128(_mm512_maskz_compress_ps(1 << i, _mm512_castph_ps(zmm))));
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
*          Operators for Complex16h
*
*****************************************************************************/

// operator + : add
static inline Complex16h operator + (Complex16h const a, Complex16h const b) {
    return _mm512_add_ph(a, b);
}

// operator += : add
static inline Complex16h & operator += (Complex16h & a, Complex16h const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex16h operator - (Complex16h const a, Complex16h const b) {
    return _mm512_sub_ph(a, b);
}

// operator - : unary minus
static inline Complex16h operator - (Complex16h const a) {
    return _mm512_castps_ph(_mm512_xor_ps(_mm512_castph_ps(a), _mm512_castsi512_ps(_mm512_set1_epi32(0x80008000))));
}

// operator -= : subtract
static inline Complex16h & operator -= (Complex16h & a, Complex16h const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex16h operator * (Complex16h const a, Complex16h const b) {
    return _mm512_fmul_pch(a, b);   // complex multiply instruction VFMULCSH
}

// operator *= : multiply
static inline Complex16h & operator *= (Complex16h & a, Complex16h const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex16h operator / (Complex16h const a, Complex16h const b) {
    __m512h ab = _mm512_cmul_pch(a, b);             // vfcmulcph instruction = a * ~b
    __m512h bb = _mm512_mul_ph(b, b);               // b.re * b.re, b.im * b.im
    __m512h bbflip = _mm512_castsi512_ph(_mm512_rol_epi32(_mm512_castph_si512(bb), 16));  // Swap bb.re and bb.im
    __m512h bb2 = _mm512_add_ph(bb, bbflip);        // b.re * b.re + b.im * b.im
    __m512h dd = _mm512_div_ph(ab, bb2);// ab / bb2
    return dd;
}

// operator /= : divide
static inline Complex16h & operator /= (Complex16h & a, Complex16h const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex16h operator ~ (Complex16h const a) {
    return _mm512_castps_ph(_mm512_xor_ps(_mm512_castph_ps(a), _mm512_castsi512_ps(_mm512_set1_epi32(0x80000000))));
}

// operator == : returns true if a == b
static inline Vec32hb operator == (Complex16h const a, Complex16h const b) {
    uint32_t k1 = _mm512_cmp_ph_mask(a, b, 0);
    uint32_t k2 = k1 & (k1 >> 1) & 0x55555555;   // AND bits from real and imaginary parts
    return uint32_t(k2 * 3);                     // expand each into two bits
}

// operator != : returns true if a != b
static inline Vec32hb operator != (Complex16h const a, Complex16h const b) {
    return ~(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex16h and float16
*
*****************************************************************************/

// operator + : add
static inline Complex16h operator + (Complex16h const a, _Float16 b) {
    return a + Complex16h(b);
}
static inline Complex16h operator + (_Float16 a, Complex16h const b) {
    return b + a;
}
static inline Complex16h & operator += (Complex16h & a, _Float16 & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex16h operator - (Complex16h const a, _Float16 b) {
    return a - Complex16h(b);
}
static inline Complex16h operator - (_Float16 a, Complex16h const b) {
    return Complex16h(a) - b;
}
static inline Complex16h & operator -= (Complex16h & a, _Float16 & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex16h operator * (Complex16h const a, _Float16 b) {
    return _mm512_mul_ph(a, _mm512_set1_ph(b));
}
static inline Complex16h operator * (_Float16 a, Complex16h const b) {
    return b * a;
}
static inline Complex16h & operator *= (Complex16h & a, _Float16 & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex16h operator / (Complex16h const a, _Float16 b) {
    return _mm512_div_ph(a, _mm512_set1_ph(b));
}

static inline Complex16h operator / (_Float16 a, Complex16h const b) {
    __m512h bb = _mm512_mul_ph(b, b);            // b.re * b.re, b.im * b.im
    __m512h bbflip = _mm512_castsi512_ph(_mm512_rol_epi32(_mm512_castph_si512(bb), 16));  // Swap bb.re and bb.im
    __m512h bb2 = _mm512_add_ph(bb, bbflip);     // b.re * b.re + b.im * b.i
    __m512h cc = _mm512_div_ph(_mm512_set1_ph(a), bb2);
    return _mm512_mul_ph(~b, cc);
}

static inline Complex16h & operator /= (Complex16h & a, _Float16 b) {
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
    __m512h bb = _mm512_mul_ph(a, a);               // a.re * a.re, a.im * a.im
    __m512h bbflip = _mm512_castsi512_ph(_mm512_rol_epi32(_mm512_castph_si512(bb), 16));  // Swap bb.re and bb.im
    __m512h bb2 = _mm512_add_ph(bb, bbflip);        // b.re * b.re + b.im * b.i 
    return _mm512_maskz_sqrt_ph(0x55555555, bb2);   // sqrt of real parts, set imag parts to 0
}

// function csqrt: complex square root
static inline Complex16h csqrt(Complex16h const a) {
    Vec32h aa = a.to_vector();                    // vector of real and imaginary elements
    Vec32h t1 = aa * aa;                          // r*r, i*i
    Vec32h t2 = _mm512_castsi512_ph(_mm512_rol_epi32(_mm512_castph_si512(t1), 16));  // Swap t1.re and t1.im
    Vec32h t3 = t1 + t2;                          // pairwise horizontal sum
    Vec32h n = _mm512_sqrt_ph(t3);                // n = sqrt(r*r+i*i) 
    __m512i t4 = _mm512_shufflelo_epi16(_mm512_castph_si512(aa), 0xA0);  // copy reals part of a
    __m512i t5 = _mm512_shufflehi_epi16(t4, 0xA0);  // copy reals part of a
    __m512i sbithi = _mm512_set1_epi32(0x80000000);
    Vec32h t6 = _mm512_castsi512_ph(_mm512_xor_si512(t5, sbithi)); // r, -r
    Vec32h t7 = n + t6;                           // n+r, n-r
    Vec32h t8 = t7 * 0.5f;
    Vec32h t9 = _mm512_sqrt_ph(t8);               // sqrt((n+r)/2), sqrt((n-r)/2) 
    // (aa & sbithi) ^ t9 :
    __m512h t11 = _mm512_castsi512_ph(_mm512_ternarylogic_epi32(_mm512_castph_si512(aa), sbithi, _mm512_castph_si512(t9), 0x6A));
    return Complex16h(t11);
}

// function select
static inline Complex16h select (Vec32hb s, Complex16h const a, Complex16h const b) {
    return _mm512_castsi512_ph(_mm512_mask_mov_epi16(_mm512_castph_si512(b), s, _mm512_castph_si512(a)));  // s ? a : b
}

// interleave real and imag into complex vector
static inline Complex16h interleave_c (Vec16h const re, Vec16h const im) {
    Vec32h b(re, im);
    Vec32h c = permute32<0,16,1,17,2,18,3,19,4,20,5,21,6,22,7,23,    
        8,24,9,25,10,26,11,27,12,28,13,29,14,30,15,31>(b);
    return __m512h(c);
}

/*****************************************************************************
*
*          Conversion functions
*
*****************************************************************************/

// There are no conversion functions because Complex16f would be 1024 bits, which is currently not supported

#endif  //  MAX_VECTOR_SIZE >= 512

#ifdef VCL_NAMESPACE
}
#endif

#endif  // INSTRSET >= 10 && defined(__AVX512FP16__)

#endif  // COMPLEXVECFP16_H
