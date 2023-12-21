/***************************  complexvec1.h   *********************************
* Author:        Agner Fog
* Date created:  2012-07-24
* Last modified: 2022-07-20
* Version:       2.02.00
* Project:       Extension to vector class library
* Description:
* Classes for complex number algebra and math:
* Complex1f:  One complex number consisting of two single precision floats
* Complex2f:  A vector of 2 complex numbers made from 4 single precision floats
* Complex4f:  A vector of 4 complex numbers made from 8 single precision floats
* Complex8f:  A vector of 8 complex numbers made from 16 single precision floats
* Complex1d:  One complex number made from 2 double precision floats
* Complex2d:  A vector of 2 complex numbers made from 4 double precision floats
* Complex4d:  A vector of 4 complex numbers made from 8 double precision floats
*
* This file defines operators and functions for these classes
* See complexvec_manual.pdf for detailed instructions.
*
* (c) Copyright 2012-2022. Apache License version 2.0 or later
******************************************************************************/

// The Complex classes do not inherit from the corresponding vector classes
// because that would lead to undesired implicit conversions when calling
// functions that are defined only for the vector class. For example, if the
// sin function is defined for Vec2d but not for Complex1d then an attempt to
// call sin(Complex1d) would in fact call sin(Vec2d) which would be mathematically
// wrong. This is avoided by not using inheritance. Explicit conversion between
// these classes is still possible.

#ifndef COMPLEXVEC_H
#define COMPLEXVEC_H  200

#ifndef VECTORCLASS_H
#include "vectorclass.h"
#endif
#include <cmath>

#ifdef VCL_NAMESPACE
namespace VCL_NAMESPACE {
#endif

#if INSTRSET >= 10 && VECTORCLASS_H >= 20000
#define COMPACT_BOOL true              // all boolean vectors are compact
#else
#define COMPACT_BOOL false             // only the biggest boolean vectors are compact
#endif

/*****************************************************************************
*
*               Class Complex1f: one single precision complex number
*
*****************************************************************************/

class Complex1f {
protected:
    __m128 xmm; // vector of 4 single precision floats. Only the first two are used
public:
    // default constructor
    Complex1f() = default;
    // construct from real, no imaginary part
    Complex1f(float re) {
        xmm = _mm_load_ss(&re);
    }
    // construct from real and imaginary part
    Complex1f(float re, float im) {
        xmm = Vec4f(re, im, 0.f, 0.f);
    }
    // Constructor to convert from type __m128 used in intrinsics:
    Complex1f(__m128 const x) {
        xmm = x;
    }
    // Assignment operator to convert from type __m128 used in intrinsics:
    Complex1f & operator = (__m128 const x) {
        xmm = x;
        return *this;
    }
    // Type cast operator to convert to __m128 used in intrinsics
    operator __m128() const {
        return xmm;
    }
    // Member function to convert to vector
    Vec4f to_vector() const {
        return xmm;
    }
    // Member function to load from array (unaligned)
    Complex1f & load(float const * p) {
        xmm = Vec4f().load_partial(2, p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(float * p) const {
        Vec4f(xmm).store_partial(2, p);
    }
    // Member function to insert one complex scalar into complex vector
    Complex1f insert (int index, Complex1f x) {
        xmm = x;
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1f extract (int i) const {
        return *this;
    }
    // get real part
    float real() const {
        return _mm_cvtss_f32(xmm);
    }
    // get imaginary part
    float imag() const {
        return Vec4f(xmm).extract(1);
    }
    static constexpr int size() {
        return 1;
    }
    static constexpr int elementtype() {
        return 0x110;
    }
};

/*****************************************************************************
*
*          Operators for Complex1f
*
*****************************************************************************/

// operator + : add
static inline Complex1f operator + (Complex1f const a, Complex1f const b) {
    return Complex1f(Vec4f(a) + Vec4f(b));
}

// operator += : add
static inline Complex1f & operator += (Complex1f & a, Complex1f const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex1f operator - (Complex1f const a, Complex1f const b) {
    return Complex1f(Vec4f(a) - Vec4f(b));
}

// operator - : unary minus
static inline Complex1f operator - (Complex1f const a) {
    return Complex1f(- Vec4f(a));
}

// operator -= : subtract
static inline Complex1f & operator -= (Complex1f & a, Complex1f const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex1f operator * (Complex1f const a, Complex1f const b) {
    __m128 b_flip = _mm_shuffle_ps(b,b,0xB1);   // Swap b.re and b.im
    __m128 a_im   = _mm_shuffle_ps(a,a,0xF5);   // Imag. part of a in both
    __m128 a_re   = _mm_shuffle_ps(a,a,0xA0);   // Real part of a in both
    __m128 aib    = _mm_mul_ps(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
#if defined(__FMA__) || INSTRSET >= 8           // FMA3 or AVX2
    return  _mm_fmaddsub_ps(a_re, b, aib);      // a_re * b +/- aib
#elif defined (__FMA4__)  // FMA4
    return  _mm_maddsub_ps(a_re, b, aib);       // a_re * b +/- aib
#elif  INSTRSET >= 3  // SSE3
    __m128 arb    = _mm_mul_ps(a_re, b);        // (a.re*b.re, a.re*b.im)
    return _mm_addsub_ps(arb, aib);             // subtract/add
#else
    __m128 arb    = _mm_mul_ps(a_re, b);        // (a.re*b.re, a.re*b.im)
    __m128 aib_m  = change_sign<1,0,1,0>(Vec4f(aib)); // change sign of low part
    return _mm_add_ps(arb, aib_m);              // add
#endif
}

// operator *= : multiply
static inline Complex1f & operator *= (Complex1f & a, Complex1f const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex1f operator / (Complex1f const a, Complex1f const b) {
    // The following code is made similar to the operator * to enable common 
    // subexpression elimination in code that contains both operator * and 
    // operator / where one or both operands are the same
    __m128 a_re   = _mm_shuffle_ps(a,a,0);      // Real part of a in both
    __m128 arb    = _mm_mul_ps(a_re, b);        // (a.re*b.re, a.re*b.im)
    __m128 b_flip = _mm_shuffle_ps(b,b,1);      // Swap b.re and b.im
    __m128 a_im   = _mm_shuffle_ps(a,a,5);      // Imag. part of a in both
#if defined(__FMA__) || INSTRSET >= 8           // FMA3 or AVX2
    __m128 n      = _mm_fmsubadd_ps(a_im, b_flip, arb); 
#elif defined (__FMA4__)  // FMA4
    __m128 n      = _mm_msubadd_ps (a_im, b_flip, arb);
#else
    __m128 aib    = _mm_mul_ps(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
    __m128 arbm   = change_sign<0,1,0,1>(Vec4f(arb)); // change sign of high part
    __m128 n      = _mm_add_ps(aib, arbm);      // arbm + aib
#endif  // FMA
    __m128 bb     = _mm_mul_ps(b, b);           // (b.re*b.re, b.im*b.im)
#if INSTRSET >= 3  // SSE3
    __m128 bb2    = _mm_hadd_ps(bb,bb);         // (b.re*b.re + b.im*b.im) 
#else
    __m128 bb1    = _mm_shuffle_ps(bb,bb,1);
    __m128 bb2    = _mm_add_ss(bb,bb1);
#endif
    __m128 bb3    = _mm_shuffle_ps(bb2,bb2,0);  // copy into all positions to avoid division by zero
    return          _mm_div_ps(n, bb3);         // n / bb3
}

// operator /= : divide
static inline Complex1f & operator /= (Complex1f & a, Complex1f const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex1f operator ~ (Complex1f const a) {
    // The parameters for change_sign are made identical to the ones used in Complex2f so that the constant mask will share the same memory position
    return Complex1f(change_sign<0,1,0,1>(Vec4f(a))); // change sign of high part
}

// operator == : returns true if a == b
static inline bool operator == (Complex1f const a, Complex1f const b) {
    Vec4fb t1 = Vec4f(a) == Vec4f(b);
#if COMPACT_BOOL                                 // compact boolean vector
    return (to_bits(t1) & 3) == 3;
#else                                            // broad boolean vector
    Vec4fb t2 = _mm_shuffle_ps(t1, t1, 0x44);
    return horizontal_and(t2);
#endif
}

// operator != : returns true if a != b
static inline bool operator != (Complex1f const a, Complex1f const b) {
    return !(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex1f and float
*
*****************************************************************************/

// operator + : add
static inline Complex1f operator + (Complex1f const a, float b) {
    return _mm_add_ss(a, _mm_set_ss(b));
}
static inline Complex1f operator + (float a, Complex1f const b) {
    return b + a;
}
static inline Complex1f & operator += (Complex1f & a, float & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex1f operator - (Complex1f const a, float b) {
    return _mm_sub_ss(a, _mm_set_ss(b));
}
static inline Complex1f operator - (float a, Complex1f const b) {
    return Complex1f(a) - b;
}
static inline Complex1f & operator -= (Complex1f & a, float & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex1f operator * (Complex1f const a, float b) {
    return _mm_mul_ps(a, _mm_set1_ps(b));
}
static inline Complex1f operator * (float a, Complex1f const b) {
    return b * a;
}
static inline Complex1f & operator *= (Complex1f & a, float & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex1f operator / (Complex1f const a, float b) {
    return _mm_div_ps(a, _mm_set1_ps(b));
}

static inline Complex1f operator / (float a, Complex1f const b) {
    Vec4f b2(b);
    Vec4f b3 = b2 * b2;
#if  INSTRSET >= 3  // SSE3
    __m128 t2 = _mm_hadd_ps(b3,b3);
#else
    __m128 t1 = _mm_shuffle_ps(b3,b3,1);
    __m128 t2 = _mm_add_ss(t1,b3);
#endif
    float  b4 = _mm_cvtss_f32(t2);
    return ~b * (a / b4);
}

static inline Complex1f & operator /= (Complex1f & a, float b) {
    a = a / b;
    return a;
}


/*****************************************************************************
*
*          Functions for Complex1f
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline float abs(Complex1f const a) {
    Vec4f a1 = Vec4f(a);
    Vec4f a2 = a1 * a1;
#if  INSTRSET >= 3  // SSE3
    __m128 t2 = _mm_hadd_ps(a2,a2);
#else
    __m128 t1 = _mm_shuffle_ps(a2,a2,1);
    __m128 t2 = _mm_add_ss(t1,a2);
#endif
    float  a3 = _mm_cvtss_f32(t2);
    return std::sqrt(a3);
}

// function csqrt: complex square root
static inline Complex1f csqrt(Complex1f const a) {
    Vec4f aa = a.to_vector();                    // vector of real and imaginary elements
    Vec4f t1 = aa * aa;                          // r*r, i*i
    Vec4f t2 = _mm_shuffle_ps(t1, t1, 1);        // swap real and imaginary parts
    Vec4f t3 = t1 + t2;                          // pairwise horizontal sum
    Vec4f t4 = sqrt(t3);                         // n = sqrt(r*r+i*i)
    Vec4f t5 = _mm_unpacklo_ps(a, a);            // copy real part of a
    Vec4f sbithi = _mm_castsi128_ps(constant4ui<0, 0x80000000u, 0, 0x80000000u>()); // (0.,-0.)
    Vec4f t6 = t5 ^ sbithi;                      // r, -r
    Vec4f t7 = t4 + t6;                          // n+r, n-r
    // Vec4f t7 = _mm_addsub_ps(t4, -t5);        // no advantage
    Vec4f t8 = t7 * 0.5f;
    Vec4f t9 = sqrt(t8);                         // sqrt((n+r)/2), sqrt((n-r)/2)
#if INSTRSET < 10
    Vec4f t10 = aa & sbithi;                     // 0, signbit of i
    Vec4f t11 = t9 ^ t10;                        // sqrt((n+r)/2), sign(i)*sqrt((n-r)/2)
#else   // AVX512VL                              // merge two instructions
    Vec4f t11 = _mm_castsi128_ps(_mm_ternarylogic_epi32(_mm_castps_si128(aa), _mm_castps_si128(sbithi), _mm_castps_si128(t9), 0x6A));
#endif
    return Complex1f(t11);
}


// function select
static inline Complex1f select (bool s, Complex1f const a, Complex1f const b) {
    return s ? a : b;
}
static inline Complex1f select (Vec4fb const s, Complex1f const a, Complex1f const b) {
    return Complex1f(select(s, Vec4f(a), Vec4f(b)));
}


// interleave real and imag into complex vector
static inline Complex1f interleave_c (float const re, float const im) {
    return Complex1f(re, im);
}

/*****************************************************************************
*
*               Class Complex2f: two single precision complex numbers
*
*****************************************************************************/

class Complex2f {
protected:
    __m128 xmm; // vector of 4 single precision floats
public:
    // default constructor
    Complex2f() = default;
    // construct from real and imaginary parts
    Complex2f(float re0, float im0, float re1, float im1) {
        xmm = Vec4f(re0, im0, re1, im1);
    }
    // construct from real, no imaginary part
    Complex2f(float re) {
        xmm = Vec4f(re, 0.f, re, 0.f);
    }
    // construct from real and imaginary part, broadcast to all
    Complex2f(float re, float im) {
        xmm = Vec4f(re, im, re, im);
    }
    // construct from one Complex1f, broadcast
    Complex2f(Complex1f const a0) {
        xmm = _mm_movelh_ps(a0, a0);
    }
    // construct from two Complex1f
    Complex2f(Complex1f const a0, Complex1f const a1) {
        xmm = _mm_movelh_ps(a0, a1);
    }
    // Constructor to convert from type __m128 used in intrinsics:
    Complex2f(__m128 const x) {
        xmm = x;
    }
    // Assignment operator to convert from type __m128 used in intrinsics:
    Complex2f & operator = (__m128 const x) {
        xmm = x;
        return *this;
    }
    // Type cast operator to convert to __m128 used in intrinsics
    operator __m128() const {
        return xmm;
    }
    // Member function to convert to vector
    Vec4f to_vector() const {
        return xmm;
    }
    // Member function to load from array
    Complex2f & load(float const * p) {
        xmm = Vec4f().load(p);
        return *this;
    }
    // Member function to store into array
    void store(float * p) const {
        Vec4f(xmm).store(p);
    }
    // Member functions to split into two Complex1f:
    Complex1f get_low() const {
        return Complex1f(Vec4f(xmm).cutoff(2));
    }
    Complex1f get_high() const {
        __m128 t = _mm_movehl_ps(_mm_setzero_ps(), xmm);
        return Complex1f(t);
    }
    // Member function to insert one complex scalar into complex vector
    Complex2f insert (int index, Complex1f x) {
#if INSTRSET >= 10   // AVX512VL
        xmm = _mm_castsi128_ps(_mm_mask_broadcastq_epi64(_mm_castps_si128(xmm), __mmask8(1u << index), _mm_castps_si128(x)));
#else
        float f[4];
        store(f);
        if (uint32_t(index) < 2) {
            x.store(f+2*index);
            load(f);
        }
#endif
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1f extract (int i) const {
        return i ? get_high() : get_low();
    }
    // get real parts
    Vec4f real() const {
        return permute4<0,2,-1,-1>(to_vector());
    }
    // get imaginary parts
    Vec4f imag() const {
        return permute4<1,3,-1,-1>(to_vector());
    }
    static constexpr int size() {
        return 2;
    }
    static constexpr int elementtype() {
        return 0x110;
    }
};

/*****************************************************************************
*
*          Operators for Complex2f
*
*****************************************************************************/

// operator + : add
static inline Complex2f operator + (Complex2f const a, Complex2f const b) {
    return Complex2f(Vec4f(a) + Vec4f(b));
}

// operator += : add
static inline Complex2f & operator += (Complex2f & a, Complex2f const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex2f operator - (Complex2f const a, Complex2f const b) {
    return Complex2f(Vec4f(a) - Vec4f(b));
}

// operator - : unary minus
static inline Complex2f operator - (Complex2f const a) {
    return Complex2f(- Vec4f(a));
}

// operator -= : subtract
static inline Complex2f & operator -= (Complex2f & a, Complex2f const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex2f operator * (Complex2f const a, Complex2f const b) {
    __m128 b_flip = _mm_shuffle_ps(b,b,0xB1);   // Swap b.re and b.im
    __m128 a_im   = _mm_shuffle_ps(a,a,0xF5);   // Imag. part of a in both
    __m128 a_re   = _mm_shuffle_ps(a,a,0xA0);   // Real part of a in both
    __m128 aib    = _mm_mul_ps(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
#if defined(__FMA__) || INSTRSET >= 8           // FMA3 or AVX2
    return  _mm_fmaddsub_ps(a_re, b, aib);      // a_re * b +/- aib
#elif defined (__FMA4__)  // FMA4
    return  _mm_maddsub_ps (a_re, b, aib);      // a_re * b +/- aib
#elif  INSTRSET >= 3  // SSE3
    __m128 arb    = _mm_mul_ps(a_re, b);        // (a.re*b.re, a.re*b.im)
    return _mm_addsub_ps(arb, aib);             // subtract/add
#else
    __m128 arb     = _mm_mul_ps(a_re, b);       // (a.re*b.re, a.re*b.im)
    __m128 aib_m   = change_sign<1,0,1,0>(Vec4f(aib)); // change sign of low part
    return _mm_add_ps(arb, aib_m);              // add
#endif
}

// operator *= : multiply
static inline Complex2f & operator *= (Complex2f & a, Complex2f const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex2f operator / (Complex2f const a, Complex2f const b) {
    // The following code is made similar to the operator * to enable common 
    // subexpression elimination in code that contains both operator * and 
    // operator / where one or both operands are the same
    __m128 a_re   = _mm_shuffle_ps(a,a,0xA0);   // Real part of a in both
    __m128 arb    = _mm_mul_ps(a_re, b);        // (a.re*b.re, a.re*b.im)
    __m128 b_flip = _mm_shuffle_ps(b,b,0xB1);   // Swap b.re and b.im
    __m128 a_im   = _mm_shuffle_ps(a,a,0xF5);   // Imag. part of a in both
#if defined(__FMA__) || INSTRSET >= 8           // FMA3 or AVX2
    __m128 n      = _mm_fmsubadd_ps(a_im, b_flip, arb); 
#elif defined (__FMA4__)  // FMA4
    __m128 n      = _mm_msubadd_ps (a_im, b_flip, arb);
#else
    __m128 aib    = _mm_mul_ps(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
    __m128 arbm   = change_sign<0,1,0,1>(Vec4f(arb)); // change sign of high part
    __m128 n      = _mm_add_ps(arbm, aib);      // arbm + aib
#endif  // FMA
    __m128 bb     = _mm_mul_ps(b, b);           // (b.re*b.re, b.im*b.im)
    __m128 bb1    = _mm_shuffle_ps(bb,bb,0xB1); // Swap bb.re and bb.im
    __m128 bb2    = _mm_add_ps(bb,bb1);         // add pairwise into both positions
    return          _mm_div_ps(n, bb2);         // n / bb3
}

// operator /= : divide
static inline Complex2f & operator /= (Complex2f & a, Complex2f const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex2f operator ~ (Complex2f const a) {
    return Complex2f(change_sign<0,1,0,1>(Vec4f(a))); // change sign of high part
}

// operator == : returns true if a == b
// The boolean vector has two identical elements for real and imaginary part, respectively
static inline Vec4fb operator == (Complex2f const a, Complex2f const b) {
    Vec4fb t1 = Vec4f(a) == Vec4f(b);
#if COMPACT_BOOL
    uint8_t k1 = __mmask8(t1);                   // convert to bits
    uint8_t k2 = k1 & (k1 >> 1) & 5;             // AND bits from real and imaginary parts
    return uint8_t(k2 * 3);                      // expand each into two bits
#else                                            // broad boolean vector
    Vec4fb t2 = _mm_shuffle_ps(t1, t1, 0xB1);    // swap real and imaginary parts
    return t1 & t2;
#endif
}

// operator != : returns true if a != b
static inline Vec4fb operator != (Complex2f const a, Complex2f const b) {
    return ~(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex2f and float
*
*****************************************************************************/

// operator + : add
static inline Complex2f operator + (Complex2f const a, float b) {
    return a + Complex2f(b);
}
static inline Complex2f operator + (float a, Complex2f const b) {
    return b + a;
}
static inline Complex2f & operator += (Complex2f & a, float & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex2f operator - (Complex2f const a, float b) {
    return a - Complex2f(b);
}
static inline Complex2f operator - (float a, Complex2f const b) {
    return Complex2f(a) - b;
}
static inline Complex2f & operator -= (Complex2f & a, float & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex2f operator * (Complex2f const a, float b) {
    return _mm_mul_ps(a, _mm_set1_ps(b));
}
static inline Complex2f operator * (float a, Complex2f const b) {
    return b * a;
}
static inline Complex2f & operator *= (Complex2f & a, float & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex2f operator / (Complex2f const a, float b) {
    return _mm_div_ps(a, _mm_set1_ps(b));
}

static inline Complex2f operator / (float a, Complex2f const b) {
    Vec4f b2(b);
    Vec4f b3 = b2 * b2;
    Vec4f t1 = _mm_shuffle_ps(b3,b3,0xB1); // swap real and imaginary parts
    Vec4f t2 = t1 + b3;
    Vec4f t3 = Vec4f(a) / t2;
    Vec4f t4 = Vec4f(~b) * t3;
    return Complex2f(t4);
}

static inline Complex2f & operator /= (Complex2f & a, float b) {
    a = a / b;
    return a;
}


/*****************************************************************************
*
*          Functions for Complex2f
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex2f abs(Complex2f const a) {
    Vec4f a1 = Vec4f(a);
    Vec4f a2 = a1 * a1;
    Vec4f t1 = _mm_shuffle_ps(a2,a2,0xB1); // swap real and imaginary parts
    Vec4f t2 = t1 + a2;
    Vec4f mask = _mm_castsi128_ps(constant4ui<0xFFFFFFFFu,0,0xFFFFFFFFu,0>());
    Vec4f t3 = t2 & mask;                  // set imaginary parts to zero
    Vec4f t4 = sqrt(t3);
    return Complex2f(t4);
}

// function csqrt: complex square root
static inline Complex2f csqrt(Complex2f const a) {
    Vec4f aa = a.to_vector();                    // vector of real and imaginary elements
    Vec4f t1 = aa * aa;                          // r*r, i*i
    Vec4f t2 = _mm_shuffle_ps(t1, t1, 0xB1);     // swap real and imaginary parts
    Vec4f t3 = t1 + t2;                          // pairwise horizontal sum
    Vec4f t4 = sqrt(t3);                         // n = sqrt(r*r+i*i)
    Vec4f t5 = _mm_shuffle_ps(a, a, 0xA0);       // copy real part of a
    Vec4f sbithi = _mm_castsi128_ps(constant4ui<0, 0x80000000u, 0, 0x80000000u>()); // (0.,-0.)
    Vec4f t6 = t5 ^ sbithi;                      // r, -r
    Vec4f t7 = t4 + t6;                          // n+r, n-r
    Vec4f t8 = t7 * 0.5f;
    Vec4f t9 = sqrt(t8);                         // sqrt((n+r)/2), sqrt((n-r)/2)
#if INSTRSET < 10
    Vec4f t10 = aa & sbithi;                     // 0, signbit of i
    Vec4f t11 = t9 ^ t10;                        // sqrt((n+r)/2), sign(i)*sqrt((n-r)/2)
#else   // AVX512VL                              // merge two instructions
    Vec4f t11 = _mm_castsi128_ps(_mm_ternarylogic_epi32(_mm_castps_si128(aa), _mm_castps_si128(sbithi), _mm_castps_si128(t9), 0x6A));
#endif
    return Complex2f(t11);
}

// function select
static inline Complex2f select (Vec4fb const s, Complex2f const a, Complex2f const b) {
    return Complex2f(select(s, Vec4f(a), Vec4f(b)));
}

// interleave real and imag into complex vector
static inline Complex2f interleave_c2 (Vec4f const re, Vec4f const im) {
    Vec4f c = blend4<0,4,1,5>(re, im);
    return __m128(c);
}


#if MAX_VECTOR_SIZE >= 256
/*****************************************************************************
*
*               Class Complex4f: four single precision complex numbers
*
*****************************************************************************/

class Complex4f {
protected:
    Vec8f y; // vector of 8 floats
public:
    // default constructor
    Complex4f() = default;
    // construct from real and imaginary parts
    Complex4f(float re0, float im0, float re1, float im1, float re2, float im2, float re3, float im3) 
        : y(re0, im0, re1, im1, re2, im2, re3, im3) {}
    // construct from one Complex1f, broadcast to all
    Complex4f(Complex1f const a) {
#if INSTRSET >= 10  // AVX512DQ
        y = _mm256_broadcast_f32x2(a);
#elif INSTRSET >= 7  // AVX
        __m128 x = _mm_castpd_ps(_mm_unpacklo_pd(_mm_castps_pd(a),_mm_castps_pd(a)));
        y = Vec8f(Vec4f(x), Vec4f(x));
#else
        Complex2f a2(a, a);        
        y = Vec8f(Vec4f(a2), Vec4f(a2));
#endif
    }
    // construct from two Complex2f
    Complex4f(Complex2f const a, Complex2f const b) {
        y = Vec8f(Vec4f(a), Vec4f(b));
    }
    // construct from four Complex1f
    Complex4f(Complex1f const a0, Complex1f const a1, Complex1f const a2, Complex1f const a3) {
        *this = Complex4f(Complex2f(a0,a1), Complex2f(a2,a3));
    }
    // construct from real, no imaginary part
    Complex4f(float re) {
        Complex2f a2(re);
        *this = Complex4f(a2, a2);
    }
    // construct from real and imaginary part, broadcast to all
    Complex4f(float re, float im) {
        Complex2f t(re, im);
        *this = Complex4f(t, t);
    }
    // Constructor to convert from type __m256 used in intrinsics or Vec256fe used in emulation
#if INSTRSET >= 7  // AVX
    Complex4f(__m256 const x) {
        y = x;
#else
    Complex4f(Vec256fe const x) {
        y = x;
#endif
    }
    // Assignment operator to convert from type __m256 used in intrinsics or Vec256fe used in emulation
#if INSTRSET >= 7  // AVX
    Complex4f & operator = (__m256 const x) {
#else
    Complex4f & operator = (Vec256fe const x) {
#endif
        *this = Complex4f(x);
        return *this;
    }
    // Type cast operator to convert to __m256 used in intrinsics or Vec256fe used in emulation
#if INSTRSET >= 7  // AVX
    operator __m256() const {
#else
    operator Vec256fe() const {
#endif
        return y;
    }
    // Member function to convert to vector
    Vec8f to_vector() const {
        return y;
    }
    // Member function to load from array (unaligned)
    Complex4f & load(float const * p) {
        y = Vec8f().load(p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(float * p) const {
        y.store(p);
    }
    // Member functions to split into two Complex1f:
    Complex2f get_low() const {
        return Complex2f(y.get_low());
    }
    Complex2f get_high() const {
        return Complex2f(y.get_high());
    }
    // Member function to insert one complex scalar into complex vector
    Complex4f insert (int index, Complex1f x) {
#if INSTRSET >= 10   // AVX512VL
        y = _mm256_castsi256_ps(_mm256_mask_broadcastq_epi64(_mm256_castps_si256(y), __mmask8(1u << index), _mm_castps_si128(x)));
#else
        float f[8];
        store(f);
        if (uint32_t(index) < 4) {
            x.store(f+2*index);
            load(f);
        }
#endif
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1f extract (int i) const {
#if INSTRSET >= 10  // AVX512VL
        __m256 x = _mm256_castpd_ps(_mm256_maskz_compress_pd(uint8_t(1 << i), _mm256_castps_pd(y)));
        return Complex1f(_mm256_castps256_ps128(x));
#else
        float x[8];
        store(x);
        return Complex1f().load(x + 2*i);
#endif
    }
    // get real parts
    Vec4f real() const {
        return permute8<0,2,4,6,1,3,5,7>(to_vector()).get_low();
    }
    // get imaginary parts
    Vec4f imag() const {
        // make the permute a common subexpression that can be eliminated 
        // by the compiler if both real() and imag() are called
        return permute8<0,2,4,6,1,3,5,7>(to_vector()).get_high();
    }
    static constexpr int size() {
        return 4;
    }
    static constexpr int elementtype() {
        return 0x110;
    }
};


/*****************************************************************************
*
*          Operators for Complex4f
*
*****************************************************************************/

// operator + : add
static inline Complex4f operator + (Complex4f const a, Complex4f const b) {
    return Complex4f(Vec8f(a) + Vec8f(b));
}

// operator += : add
static inline Complex4f & operator += (Complex4f & a, Complex4f const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex4f operator - (Complex4f const a, Complex4f const b) {
    return Complex4f(Vec8f(a) - Vec8f(b));
}

// operator - : unary minus
static inline Complex4f operator - (Complex4f const a) {
    return Complex4f(- Vec8f(a));
}

// operator -= : subtract
static inline Complex4f & operator -= (Complex4f & a, Complex4f const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex4f operator * (Complex4f const a, Complex4f const b) {
#if INSTRSET >= 7  // AVX
    __m256 b_flip = _mm256_shuffle_ps(b,b,0xB1);   // Swap b.re and b.im
    __m256 a_im   = _mm256_shuffle_ps(a,a,0xF5);   // Imag part of a in both
    __m256 a_re   = _mm256_shuffle_ps(a,a,0xA0);   // Real part of a in both
    __m256 aib    = _mm256_mul_ps(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
#if defined(__FMA__) || INSTRSET >= 8              // FMA3 or AVX2
    return  _mm256_fmaddsub_ps(a_re, b, aib);      // a_re * b +/- aib
#else
    __m256 arb    = _mm256_mul_ps(a_re, b);        // (a.re*b.re, a.re*b.im)
    return          _mm256_addsub_ps(arb, aib);    // subtract/add
#endif  // FMA
#else   // AVX
    return Complex4f(a.get_low()*b.get_low(), a.get_high()*b.get_high());
#endif
}

// operator *= : multiply
static inline Complex4f & operator *= (Complex4f & a, Complex4f const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex4f operator / (Complex4f const a, Complex4f const b) {
#if INSTRSET >= 7  // AVX
    __m256 a_re   = _mm256_shuffle_ps(a,a,0xA0);   // Real part of a in both
    __m256 arb    = _mm256_mul_ps(a_re, b);        // (a.re*b.re, a.re*b.im)
    __m256 b_flip = _mm256_shuffle_ps(b,b,0xB1);   // Swap b.re and b.im
    __m256 a_im   = _mm256_shuffle_ps(a,a,0xF5);   // Imag part of a in both
#if defined(__FMA__) || INSTRSET >= 8              // FMA3 or AVX2
    __m256 n      = _mm256_fmsubadd_ps(a_im, b_flip, arb); 
#else
    __m256 aib    = _mm256_mul_ps(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
    __m256 arbm   = change_sign<0,1,0,1,0,1,0,1>(Vec8f(arb)); // (a.re*b.re,-a.re*b.im)
    __m256 n      = _mm256_add_ps(arbm, aib);      // arbm + aib
#endif  // FMA
    __m256 bb     = _mm256_mul_ps(b, b);           // (b.re*b.re, b.im*b.im)
    __m256 bb2    = _mm256_shuffle_ps(bb,bb,0xB1); // Swap bb.re and bb.im
    __m256 bsq    = _mm256_add_ps(bb,bb2);         // (b.re*b.re + b.im*b.im) dublicated
    return          _mm256_div_ps(n, bsq);         // n / bsq
#else
    return Complex4f(a.get_low()/b.get_low(), a.get_high()/b.get_high());
#endif
}

// operator /= : divide
static inline Complex4f & operator /= (Complex4f & a, Complex4f const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex4f operator ~ (Complex4f const a) {
    return Complex4f(change_sign<0,1,0,1,0,1,0,1>(Vec8f(a)));
}

// operator == : returns true if a == b
// The boolean vector has two identical elements for real and imaginary part, respectively
static inline Vec8fb operator == (Complex4f const a, Complex4f const b) {
    Vec8fb eq0 = Vec8f(a) == Vec8f(b);
#if COMPACT_BOOL
    uint8_t k1 = __mmask8(eq0);                  // convert to bits
    uint8_t k2 = k1 & (k1 >> 1) & 0x55;          // AND bits from real and imaginary parts
    return uint8_t(k2 * 3);                      // expand each into two bits
#else
    Vec8fb eq1 = (Vec8fb)permute8<1,0,3,2,5,4,7,6>(Vec8f(eq0));
    return Vec8fb(eq0 & eq1);
#endif
}

// operator != : returns true if a != b
static inline Vec8fb operator != (Complex4f const a, Complex4f const b) {
    return ~(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex4f and float
*
*****************************************************************************/

// operator + : add
static inline Complex4f operator + (Complex4f const a, float b) {
    return a + Complex4f(b);
}
static inline Complex4f operator + (float a, Complex4f const b) {
    return b + a;
}
static inline Complex4f & operator += (Complex4f & a, float & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex4f operator - (Complex4f const a, float b) {
    return a - Complex4f(b);
}
static inline Complex4f operator - (float a, Complex4f const b) {
    return Complex4f(a) - b;
}
static inline Complex4f & operator -= (Complex4f & a, float & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex4f operator * (Complex4f const a, float b) {
    return Complex4f(Vec8f(a) * Vec8f(b));
}
static inline Complex4f operator * (float a, Complex4f const b) {
    return b * a;
}
static inline Complex4f & operator *= (Complex4f & a, float & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex4f operator / (Complex4f const a, float b) {
    return Complex4f(Vec8f(a) / Vec8f(b));
}

static inline Complex4f operator / (float a, Complex4f const b) {
#if INSTRSET >= 7  // AVX
    Vec8f b2(b);
    Vec8f b3 = b2 * b2;
    Vec8f t1 = _mm256_shuffle_ps(b3,b3,0xB1); // swap real and imaginary parts
    Vec8f t2 = t1 + b3;
    Vec8f t3 = Vec8f(a) / t2;
    Vec8f t4 = Vec8f(~b) * t3;
    return Complex4f(t4);
#else
    return Complex4f(a / b.get_low(), a / b.get_high());
#endif
}
static inline Complex4f & operator /= (Complex4f & a, float b) {
    a = a / b;
    return a;
}


/*****************************************************************************
*
*          Functions for Complex4f
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex4f abs(Complex4f const a) {
#if INSTRSET >= 7  // AVX
    Vec8f a1 = Vec8f(a);
    Vec8f a2 = a1 * a1;
    Vec8f t1 = _mm256_shuffle_ps(a2,a2,0xB1);    // swap real and imaginary parts
    Vec8f t2 = t1 + a2;
    Vec8f mask = constant8f<0xFFFFFFFFu,0,0xFFFFFFFFu,0,0xFFFFFFFFu,0,0xFFFFFFFFu,0>();
    Vec8f t3 = t2 & mask;                        // set imaginary parts to zero
    Vec8f t4 = sqrt(t3);
    return Complex4f(t4);
#else
    return Complex4f(abs(a.get_low()), abs(a.get_high()));
#endif
}

// function csqrt: square root
static inline Complex4f csqrt(Complex4f const a) {
#if INSTRSET >= 9  // AVX512
    Vec8f aa = a.to_vector();                    // vector of real and imaginary elements
    Vec8f t1 = aa * aa;                          // r*r, i*i
    Vec8f t2 = _mm256_shuffle_ps(t1, t1, 0xB1);  // swap real and imaginary parts
    Vec8f t3 = t1 + t2;                          // pairwise horizontal sum
    Vec8f t4 = sqrt(t3);                         // n = sqrt(r*r+i*i)
    Vec8f t5 = _mm256_shuffle_ps(a, a, 0xA0);    // copy real part of a
    Vec8f sbithi = _mm256_castsi256_ps(constant8ui<0,0x80000000,0,0x80000000,0,0x80000000,0,0x80000000>());
    Vec8f t6 = t5 ^ sbithi;                      // r, -r
    Vec8f t7 = t4 + t6;                          // n+r, n-r
    Vec8f t8 = t7 * 0.5f;
    Vec8f t9 = sqrt(t8);                         // sqrt((n+r)/2), sqrt((n-r)/2)
#if INSTRSET < 10
    Vec8f t10 = aa & sbithi;                     // 0, signbit of i
    Vec8f t11 = t9 ^ t10;                        // sqrt((n+r)/2), sign(i)*sqrt((n-r)/2)
#else   // AVX512VL                              // merge two instructions
    Vec8f t11 = _mm256_castsi256_ps(_mm256_ternarylogic_epi32(_mm256_castps_si256(aa), _mm256_castps_si256(sbithi), _mm256_castps_si256(t9), 0x6A));
#endif
    return Complex4f(t11);
#else
    return Complex4f(csqrt(a.get_low()), csqrt(a.get_high()));
#endif
}

// function select.
// Note: s must contain a vector of four booleans of 64 bits each. 
static inline Complex4f select (Vec8fb const s, Complex4f const a, Complex4f const b) {
    return Complex4f(select(s, Vec8f(a), Vec8f(b)));
}

// interleave real and imag into complex vector
static inline Complex4f interleave_c (Vec4f const re, Vec4f const im) {
    Vec8f b(re, im);
    Vec8f c = permute8<0,4,1,5,2,6,3,7>(b);
#if INSTRSET >= 7  // AVX    
    return __m256(c);
#else
    return Vec256fe(c);
#endif
}

#endif  // MAX_VECTOR_SIZE >= 256

#if MAX_VECTOR_SIZE >= 512
/*****************************************************************************
*
*               Class Complex8f: 8 single precision complex numbers
*
*****************************************************************************/

class Complex8f {
protected:
    Vec16f y; // vector of 8 floats
public:
    // default constructor
    Complex8f() = default;
    // construct from real and imaginary parts
    Complex8f(float re0, float im0, float re1, float im1, float re2, float im2, float re3, float im3,
        float re4, float im4, float re5, float im5, float re6, float im6, float re7, float im7) 
        : y(re0, im0, re1, im1, re2, im2, re3, im3, re4, im4, re5, im5, re6, im6, re7, im7) {}
    // construct from real, no imaginary part
    Complex8f(float const a) {
#if INSTRSET >= 10  // AVX512DQ
        y = _mm512_broadcast_f32x2(_mm_load_ss(&a));
#else
        Complex4f a2(a);
        y = Vec16f(Vec8f(a2), Vec8f(a2));
#endif
    }
    // construct from one Complex1f, broadcast to all
    Complex8f(Complex1f const a) {
#if INSTRSET >= 10  // AVX512DQ
        y = _mm512_broadcast_f32x2(__m128(a));
#else
        Complex4f a2(a);
        y = Vec16f(Vec8f(a2), Vec8f(a2));
#endif
    }
    // construct from two Complex4f
    Complex8f(Complex4f const a, Complex4f const b) {
        y = Vec16f(Vec8f(a), Vec8f(b));
    }
    // construct from eight Complex1f
    Complex8f(Complex1f const a0, Complex1f const a1, Complex1f const a2, Complex1f const a3,
        Complex1f const a4, Complex1f const a5, Complex1f const a6, Complex1f const a7) {
        y = Vec16f(Vec8f(Complex4f(Complex2f(a0,a1),Complex2f(a2,a3))),
                   Vec8f(Complex4f(Complex2f(a4,a5),Complex2f(a6,a7))));
    }

    // construct from real and imaginary part, broadcast to all
    Complex8f(float re, float im) {
        *this = Complex8f(Complex1f(re, im));
    }
    // Constructor to convert from type __m512 used in intrinsics or Vec256fe used in emulation
#if INSTRSET >= 9  // AVX512
    Complex8f(__m512 const x) {
        y = x;
    }
#else
    Complex8f(Vec16f const x) {  // constructor to convert from emulated Vec16f
        y = x;
    }
#endif
    // Assignment operator to convert from type __m512 used in intrinsics or Vec256fe used in emulation
#if INSTRSET >= 9  // AVX512
    Complex8f & operator = (__m512 const x) {
        *this = Complex8f(x);
        return *this;
    }
#endif
    // Type cast operator to convert to __m512 used in intrinsics or Vec256fe used in emulation
#if INSTRSET >= 9  // AVX512
    operator __m512() const {
        return y;
    }
#endif
    // Member function to convert to vector
    Vec16f to_vector() const {
        return y;
    }
    // Member function to load from array
    Complex8f & load(float const * p) {
        y = Vec16f().load(p);
        return *this;
    }
    // Member function to store into array
    void store(float * p) const {
        y.store(p);
    }
    // Member functions to split into two Complex1f:
    Complex4f get_low() const {
        return Complex4f(y.get_low());
    }
    Complex4f get_high() const {
        return Complex4f(y.get_high());
    }
    // Member function to insert one complex scalar into complex vector
    Complex8f insert (int index, Complex1f x) {
#if INSTRSET >= 10   // AVX512VL
        y = _mm512_castsi512_ps(_mm512_mask_broadcastq_epi64(_mm512_castps_si512(y), __mmask16(1u << index), _mm_castps_si128(x)));
#else
        float f[16];
        store(f);
        if (uint32_t(index) < 8) {
            x.store(f+2*index);
            load(f);
        }
#endif
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1f extract (int i) const {
#if INSTRSET >= 9  // AVX512F
        __m512 x = _mm512_castpd_ps(_mm512_maskz_compress_pd(uint8_t(1 << i), _mm512_castps_pd(y)));
        return Complex1f(_mm512_castps512_ps128(x));
#else
        float x[16];
        store(x);
        return Complex1f().load(x + 2*i);
#endif
    }
    // get real parts
    Vec8f real() const {
        return permute16<0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15>(to_vector()).get_low();
    }
    // get imaginary parts
    Vec8f imag() const {
        // make the permute a common subexpression that can be eliminated 
        // by the compiler if both real() and imag() are called
        return permute16<0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15>(to_vector()).get_high();
    }
    static constexpr int size() {
        return 8;
    }
    static constexpr int elementtype() {
        return 0x110;
    }
};


/*****************************************************************************
*
*          Operators for Complex8f
*
*****************************************************************************/

// operator + : add
static inline Complex8f operator + (Complex8f const a, Complex8f const b) {
    return Complex8f(Vec16f(a.to_vector()) + Vec16f(b.to_vector()));
}

// operator += : add
static inline Complex8f & operator += (Complex8f & a, Complex8f const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex8f operator - (Complex8f const a, Complex8f const b) {
    return Complex8f(Vec16f(a.to_vector()) - Vec16f(b.to_vector()));
}

// operator - : unary minus
static inline Complex8f operator - (Complex8f const a) {
    return Complex8f(- Vec16f(a.to_vector()));
}

// operator -= : subtract
static inline Complex8f & operator -= (Complex8f & a, Complex8f const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex8f operator * (Complex8f const a, Complex8f const b) {
#if INSTRSET >= 9  // AVX512
    __m512 b_flip = _mm512_shuffle_ps(b,b,0xB1);   // Swap b.re and b.im
    __m512 a_im   = _mm512_shuffle_ps(a,a,0xF5);   // Imag part of a in both
    __m512 a_re   = _mm512_shuffle_ps(a,a,0xA0);   // Real part of a in both
    __m512 aib    = _mm512_mul_ps(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
    return     _mm512_fmaddsub_ps(a_re, b, aib);   // a_re * b +/- aib
#else   // AVX
    return Complex8f(a.get_low()*b.get_low(), a.get_high()*b.get_high());
#endif
}

// operator *= : multiply
static inline Complex8f & operator *= (Complex8f & a, Complex8f const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex8f operator / (Complex8f const a, Complex8f const b) {
#if INSTRSET >= 9  // AVX512
    __m512 a_re   = _mm512_shuffle_ps(a,a,0xA0);   // Real part of a in both
    __m512 arb    = _mm512_mul_ps(a_re, b);        // (a.re*b.re, a.re*b.im)
    __m512 b_flip = _mm512_shuffle_ps(b,b,0xB1);   // Swap b.re and b.im
    __m512 a_im   = _mm512_shuffle_ps(a,a,0xF5);   // Imag part of a in both
    __m512 n      = _mm512_fmsubadd_ps(a_im, b_flip, arb); 
    __m512 bb     = _mm512_mul_ps(b, b);           // (b.re*b.re, b.im*b.im)
    __m512 bb2    = _mm512_shuffle_ps(bb,bb,0xB1); // Swap bb.re and bb.im
    __m512 bsq    = _mm512_add_ps(bb,bb2);         // (b.re*b.re + b.im*b.im) dublicated
    return          _mm512_div_ps(n, bsq);         // n / bsq
#else
    return Complex8f(a.get_low()/b.get_low(), a.get_high()/b.get_high());
#endif
}

// operator /= : divide
static inline Complex8f & operator /= (Complex8f & a, Complex8f const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex8f operator ~ (Complex8f const a) {
    return Complex8f(change_sign<0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1>(a.to_vector()));
    //return Complex8f(a.to_vector() ^ Vec16f(0,-0.,0,-0.,0,-0.,0,-0.,0,-0.,0,-0.,0,-0.,0,-0.));
}

// operator == : returns true if a == b
// The boolean vector has two identical elements for real and imaginary part, respectively
static inline Vec16fb operator == (Complex8f const a, Complex8f const b) {
#if COMPACT_BOOL
    Vec16fb  eq0 = a.to_vector() == b.to_vector(); // compare
    uint16_t k1  = __mmask16(eq0);               // convert to bits
    uint16_t k2  = k1 & (k1 >> 1) & 0x5555;      // AND bits from real and imaginary parts
    return uint16_t(k2 * 3);                     // expand each into two bits
#else
    return Vec16fb(a.get_low() == b.get_low(), a.get_high() == b.get_high());
#endif
}

// operator != : returns true if a != b
static inline Vec16fb operator != (Complex8f const a, Complex8f const b) {
    return ~(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex8f and float
*
*****************************************************************************/

// operator + : add
static inline Complex8f operator + (Complex8f const a, float b) {
    return a + Complex8f(b);
}
static inline Complex8f operator + (float a, Complex8f const b) {
    return b + a;
}
static inline Complex8f & operator += (Complex8f & a, float & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex8f operator - (Complex8f const a, float b) {
    return a - Complex8f(b);
}
static inline Complex8f operator - (float a, Complex8f const b) {
    return Complex8f(a) - b;
}
static inline Complex8f & operator -= (Complex8f & a, float & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex8f operator * (Complex8f const a, float b) {
    return Complex8f(a.to_vector() * b);
}
static inline Complex8f operator * (float a, Complex8f const b) {
    return b * a;
}
static inline Complex8f & operator *= (Complex8f & a, float & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex8f operator / (Complex8f const a, float b) {
    return Complex8f(a.to_vector() / Vec16f(b));
}

static inline Complex8f operator / (float a, Complex8f const b) {
#if INSTRSET >= 9  // AVX512
    Vec16f b2(b);
    Vec16f b3 = b2 * b2;
    Vec16f t1 = _mm512_shuffle_ps(b3,b3,0xB1); // swap real and imaginary parts
    Vec16f t2 = t1 + b3;
    Vec16f t3 = Vec16f(a) / t2;
    Vec16f t4 = Vec16f(~b) * t3;
    return Complex8f(t4);
#else
    return Complex8f(a / b.get_low(), a / b.get_high());
#endif
}
static inline Complex8f & operator /= (Complex8f & a, float b) {
    a = a / b;
    return a;
}


/*****************************************************************************
*
*          Functions for Complex8f
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex8f abs(Complex8f const a) {
#if INSTRSET >= 9  // AVX512
    Vec16f a1 = Vec16f(a);
    Vec16f a2 = a1 * a1;                          // square real and imaginary parts
    Vec16f t1 = _mm512_shuffle_ps(a2,a2,0xB1);    // swap real and imaginary squares
    Vec16f t2 = _mm512_maskz_add_ps(0x5555, t1, a2); // add real and imaginary squares, set imaginary to zero
    Vec16f t4 = sqrt(t2);
    return Complex8f(t4);
#else
    return Complex8f(abs(a.get_low()), abs(a.get_high()));
#endif
}

// function csqrt: complex square root
static inline Complex8f csqrt(Complex8f const a) {
#if INSTRSET >= 9  // AVX512
    Vec16f aa = a.to_vector();                    // vector of real and imaginary elements
    Vec16f t1 = aa * aa;                          // r*r, i*i
    Vec16f t2 = _mm512_shuffle_ps(t1, t1, 0xB1);  // swap real and imaginary parts
    Vec16f t3 = t1 + t2;                          // pairwise horizontal sum
    Vec16f t4 = sqrt(t3);                         // n = sqrt(r*r+i*i)
    Vec16f t5 = _mm512_shuffle_ps(a, a, 0xA0);    // copy real part of a
    Vec16f sbithi = _mm512_castsi512_ps(Vec8uq(0x8000000000000000u)); // sign bit of imag parts
    Vec16f t6 = t5 ^ sbithi;                      // r, -r
    Vec16f t7 = t4 + t6;                          // n+r, n-r
    Vec16f t8 = t7 * 0.5f;
    Vec16f t9 = sqrt(t8);                         // sqrt((n+r)/2), sqrt((n-r)/2)
#if INSTRSET < 10
    Vec16f t10 = aa & sbithi;                     // 0, signbit of i
    Vec16f t11 = t9 ^ t10;                        // sqrt((n+r)/2), sign(i)*sqrt((n-r)/2)
#else   // AVX512VL                               // merge two instructions
    Vec16f t11 = _mm512_castsi512_ps(_mm512_ternarylogic_epi32(_mm512_castps_si512(aa), _mm512_castps_si512(sbithi), _mm512_castps_si512(t9), 0x6A));
#endif
    return Complex8f(t11);
#else
    return Complex8f(csqrt(a.get_low()), csqrt(a.get_high()));
#endif
}

// function select
static inline Complex8f select (Vec16fb const s, Complex8f const a, Complex8f const b) {
    return Complex8f(select(s, a.to_vector(), b.to_vector()));
}

// interleave real and imag into complex vector
static inline Complex8f interleave_c (Vec8f const re, Vec8f const im) {
    Vec16f b(re, im);
    Vec16f c = permute16<0,8,1,9,2,10,3,11,4,12,5,13,6,14,7,15>(b);
#if INSTRSET >= 9  // AVX512
    return __m512(c);
#else
    return Complex8f(c);
#endif
}

#endif // MAX_VECTOR_SIZE >= 512

/*****************************************************************************
*
*               Class Complex1d: one double precision complex number
*
*****************************************************************************/

class Complex1d {
protected:
    __m128d xmm; // double vector
public:
    // default constructor
    Complex1d() = default;
    // construct from real and imaginary part
    Complex1d(double re, double im) {
        xmm = Vec2d(re, im);
    }
    // construct from real, no imaginary part
    Complex1d(double re) {
        xmm = _mm_load_sd(&re);
    }
    // Constructor to convert from type __m128d used in intrinsics:
    Complex1d(__m128d const x) {
        xmm = x;
    }
    // Assignment operator to convert from type __m128d used in intrinsics:
    Complex1d & operator = (__m128d const x) {
        xmm = x;
        return *this;
    }
    // Type cast operator to convert to __m128d used in intrinsics
    operator __m128d() const {
        return xmm;
    }
    // Member function to convert to vector
    Vec2d to_vector() const {
        return xmm;
    }
    // Member function to load from array
    Complex1d & load(double const * p) {
        xmm = Vec2d().load(p);
        return *this;
    }
    // Member function to store into array
    void store(double * p) const {
        Vec2d(xmm).store(p);
    }
    // get real part
    double real() const {
        return _mm_cvtsd_f64(xmm);
    }
    // get imaginary part
    double imag() const {
        return Vec2d(xmm).extract(1);
    }
    // Member function to insert one complex scalar into complex vector
    Complex1d insert (int index, Complex1d x) {
        xmm = x;
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1d extract (int i) const {
        return *this;
    }
    static constexpr int size() {
        return 1;
    }
    static constexpr int elementtype() {
        return 0x111;
    }
};


/*****************************************************************************
*
*          Operators for Complex1d
*
*****************************************************************************/

// operator + : add
static inline Complex1d operator + (Complex1d const a, Complex1d const b) {
    return Complex1d(Vec2d(a) + Vec2d(b));
}

// operator += : add
static inline Complex1d & operator += (Complex1d & a, Complex1d const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex1d operator - (Complex1d const a, Complex1d const b) {
    return Complex1d(Vec2d(a) - Vec2d(b));
}

// operator - : unary minus
static inline Complex1d operator - (Complex1d const a) {
    return Complex1d(- Vec2d(a));
}

// operator -= : subtract
static inline Complex1d & operator -= (Complex1d & a, Complex1d const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex1d operator * (Complex1d const a, Complex1d const b) {
    __m128d b_flip = _mm_shuffle_pd(b,b,1);      // Swap b.re and b.im
    __m128d a_im   = _mm_shuffle_pd(a,a,3);      // Imag. part of a in both
    __m128d a_re   = _mm_shuffle_pd(a,a,0);      // Real part of a in both
    __m128d aib    = _mm_mul_pd(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
#if defined(__FMA__) || INSTRSET >= 8             // FMA3 or AVX2
    return  _mm_fmaddsub_pd(a_re, b, aib);       // a_re * b +/- aib
#elif defined (__FMA4__)  // FMA4
    return  _mm_maddsub_pd (a_re, b, aib);       // a_re * b +/- aib
#elif  INSTRSET >= 3  // SSE3
    __m128d arb    = _mm_mul_pd(a_re, b);        // (a.re*b.re, a.re*b.im)
    return _mm_addsub_pd(arb, aib);              // subtract/add
#else
    __m128d arb    = _mm_mul_pd(a_re, b);        // (a.re*b.re, a.re*b.im)
    __m128d aib_m  = change_sign<1,0>(Vec2d(aib)); // change sign of low part
    return _mm_add_pd(arb, aib_m);               // add
#endif
}

// operator *= : multiply
static inline Complex1d & operator *= (Complex1d & a, Complex1d const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex1d operator / (Complex1d const a, Complex1d const b) {
    __m128d a_re   = _mm_shuffle_pd(a,a,0);      // Real part of a in both
    __m128d arb    = _mm_mul_pd(a_re, b);        // (a.re*b.re, a.re*b.im)
    __m128d b_flip = _mm_shuffle_pd(b,b,1);      // Swap b.re and b.im
    __m128d a_im   = _mm_shuffle_pd(a,a,3);      // Imag. part of a in both
#if defined(__FMA__) || INSTRSET >= 8            // FMA3 or AVX2
    __m128d n      = _mm_fmsubadd_pd(a_im, b_flip, arb); 
#elif defined (__FMA4__)  // FMA4
    __m128d n      = _mm_msubadd_pd (a_im, b_flip, arb);
#else
    __m128d aib    = _mm_mul_pd(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
    __m128d arbm   = change_sign<0,1>(Vec2d(arb));
    __m128d n      = _mm_add_pd(arbm, aib);      // arbm + aib
#endif  // FMA
    __m128d bb     = _mm_mul_pd(b, b);           // (b.re*b.re, b.im*b.im)
#if INSTRSET >= 3  // SSE3
    __m128d bsq    = _mm_hadd_pd(bb,bb);         // (b.re*b.re + b.im*b.im) dublicated
#else
    __m128d bb1    = _mm_shuffle_pd(bb,bb,1);
    __m128d bsq    = _mm_add_pd(bb,bb1);
#endif // SSE3
    return           _mm_div_pd(n, bsq);         // n / bsq
}

// operator /= : divide
static inline Complex1d & operator /= (Complex1d & a, Complex1d const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex1d operator ~ (Complex1d const a) {
    return Complex1d(change_sign<0,1>(Vec2d(a)));
}

// operator == : returns true if a == b
static inline bool operator == (Complex1d const a, Complex1d const b) {
    return horizontal_and(Vec2d(a) == Vec2d(b));
}

// operator != : returns true if a != b
static inline bool operator != (Complex1d const a, Complex1d const b) {
    return horizontal_or(Vec2d(a) != Vec2d(b));
}

/*****************************************************************************
*
*          Operators mixing Complex1d and double
*
*****************************************************************************/

// operator + : add
static inline Complex1d operator + (Complex1d const a, double b) {
    return _mm_add_sd(a, _mm_set_sd(b));
}
static inline Complex1d operator + (double a, Complex1d const b) {
    return b + a;
}
static inline Complex1d & operator += (Complex1d & a, double & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex1d operator - (Complex1d const a, double b) {
    return _mm_sub_sd(a, _mm_set_sd(b));
}
static inline Complex1d operator - (double a, Complex1d const b) {
    return Complex1d(a) - b;
}
static inline Complex1d & operator -= (Complex1d & a, double & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex1d operator * (Complex1d const a, double b) {
    return _mm_mul_pd(a, _mm_set1_pd(b));
}
static inline Complex1d operator * (double a, Complex1d const b) {
    return b * a;
}
static inline Complex1d & operator *= (Complex1d & a, double & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex1d operator / (Complex1d const a, double b) {
    return _mm_div_pd(a, _mm_set1_pd(b));
}
static inline Complex1d operator / (double a, Complex1d const b) {
    Vec2d b2(b);
    double b3 = horizontal_add(b2 * b2);
    return ~b * (a / b3);
}
static inline Complex1d & operator /= (Complex1d & a, double b) {
    a = a / b;
    return a;
}


/*****************************************************************************
*
*          Functions for Complex1d
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline double abs(Complex1d const a) {
    Vec2d t = Vec2d(a);
    return std::sqrt(horizontal_add(t*t));
}

// function csqrt: complex square root
static inline Complex1d csqrt(Complex1d const a) {
    Vec2d aa = a.to_vector();
    Vec2d t1 = aa * aa;                          // r*r, i*i
    Vec2d t2  = _mm_shuffle_pd(t1,t1,1);         // swap real and imaginary parts
    Vec2d t3  = t1 + t2;                         // pairwise horizontal sum
    Vec2d t4  = sqrt(t3);                        // n = sqrt(r*r+i*i)
    Vec2d t5  = _mm_shuffle_pd(a,a,0);        // copy real part of a
    Vec2d sbithi = _mm_castsi128_pd (constant4ui<0,0,0,0x80000000u>()); // (0.,-0.)
    Vec2d t6  = t5 ^ sbithi;                     // r, -r
    Vec2d t7  = t4 + t6;                         // n+r, n-r
    // Vec2d t7 = _mm256_addsub_pd(t4, -t5);     // no advantage
    Vec2d t8 = t7 * 0.5;
    Vec2d t9 = sqrt(t8);                         // sqrt((n+r)/2), sqrt((n-r)/2)
#if INSTRSET < 10
    Vec2d t10 = aa & sbithi;                     // 0, signbit of i
    Vec2d t11 = t9 ^ t10;                        // sqrt((n+r)/2), sign(i)*sqrt((n-r)/2)
#else   // AVX512VL                              // merge two instructions
    Vec2d t11 = _mm_castsi128_pd(_mm_ternarylogic_epi32(_mm_castpd_si128(aa), _mm_castpd_si128(sbithi), _mm_castpd_si128(t9), 0x6A));
#endif
    return Complex1d(t11);
}

// function select
static inline Complex1d select (bool s, Complex1d const a, Complex1d const b) {
    return s ? a : b;
}
static inline Complex1d select (Vec2db const s, Complex1d const a, Complex1d const b) {
    return Complex1d(select(s, Vec2d(a), Vec2d(b)));
}

// interleave real and imag into complex vector
static inline Complex1d interleave_c(double const re, double const im) {
    return Complex1d(re, im);
}


#if MAX_VECTOR_SIZE >= 256
/*****************************************************************************
*
*               Class Complex2d: one double precision complex number
*
*****************************************************************************/

class Complex2d {
protected:
    Vec4d y; // vector of 4 doubles
public:
    // default constructor
    Complex2d() = default;
    // construct from real and imaginary parts
    Complex2d(double re0, double im0, double re1, double im1) 
        : y(re0, im0, re1, im1) {}
    // construct from one Complex1d
    Complex2d(Complex1d const a) {
#if INSTRSET >= 7  // AVX
        __m128d aa = a;
        y = _mm256_broadcast_pd(&aa);  // VBROADCASTF128
#else
        y = Vec4d(Vec2d(a), Vec2d(a));
#endif
    }
    // construct from two Complex1d
    Complex2d(Complex1d const a, Complex1d const b) {
        y = Vec4d(Vec2d(a), Vec2d(b));
    }
    // construct from real, no imaginary part
    Complex2d(double re) {
        *this = Complex2d(Complex1d(re), Complex1d(re));
    }
    // construct from real and imaginary part, broadcast to all
    Complex2d(double re, double im) {
        *this = Complex2d(Complex1d(re, im));
    }
    // Constructor to convert from type __m256d used in intrinsics or Vec256de used in emulation
#if INSTRSET >= 7  // AVX
    Complex2d(__m256d const x) {
        y = x;
#else
    Complex2d(Vec256de const x) {
        y = x;
#endif
    }
    // Assignment operator to convert from type __m256d used in intrinsics or Vec256de used in emulation
#if INSTRSET >= 7  // AVX
    Complex2d & operator = (__m256d const x) {
#else
    Complex2d & operator = (Vec256de const x) {
#endif
        *this = Complex2d(x);
        return *this;
    }
    // Type cast operator to convert to __m256d used in intrinsics or Vec256de used in emulation
#if INSTRSET >= 7  // AVX
    operator __m256d() const {
#else
    operator Vec256de() const {
#endif
        return y;
    }
    // Member function to convert to vector
    Vec4d to_vector() const {
        return y;
    }
    // Member function to load from array (unaligned)
    Complex2d & load(double const * p) {
        y = Vec4d().load(p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(double * p) const {
        y.store(p);
    }
    // Member functions to split into two Complex1d:
    Complex1d get_low() const {
        return Complex1d(y.get_low());
    }
    Complex1d get_high() const {
        return Complex1d(y.get_high());
    }
    // Member function to insert one complex scalar into complex vector
    Complex2d insert (int index, Complex1d x) {
#if INSTRSET >= 10   // AVX512VL
        y = _mm256_mask_broadcast_f64x2(y, __mmask8(3 << (index*2)), x);
#else
        if (index == 0) {
            *this = Complex2d(x, get_high());
        }
        else if (index == 1) {
            *this = Complex2d(get_low(), x);
        }
#endif
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1d extract (int i) const {
#if INSTRSET >= 10  // AVX512VL
        __m256d x = _mm256_maskz_compress_pd(uint8_t(3 << (2*i)), y);
        return Complex1d(_mm256_castpd256_pd128(x));
#else
        double x[4];
        store(x);
        return Complex1d().load(x + 2*i);
#endif
    }
    // get real parts
    Vec2d real() const {
        return permute4<0,2,1,3>(to_vector()).get_low();
    }
    // get imaginary parts
    Vec2d imag() const {
        // make the permute a common subexpression that can be eliminated 
        // by the compiler if both real() and imag() are called
        return permute4<0,2,1,3>(to_vector()).get_high();
    }
    static constexpr int size() {
        return 2;
    }
    static constexpr int elementtype() {
        return 0x111;
    }
};


/*****************************************************************************
*
*          Operators for Complex2d
*
*****************************************************************************/

// operator + : add
static inline Complex2d operator + (Complex2d const a, Complex2d const b) {
    return Complex2d(Vec4d(a) + Vec4d(b));
}

// operator += : add
static inline Complex2d & operator += (Complex2d & a, Complex2d const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex2d operator - (Complex2d const a, Complex2d const b) {
    return Complex2d(Vec4d(a) - Vec4d(b));
}

// operator - : unary minus
static inline Complex2d operator - (Complex2d const a) {
    return Complex2d(- Vec4d(a));
}

// operator -= : subtract
static inline Complex2d & operator -= (Complex2d & a, Complex2d const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex2d operator * (Complex2d const a, Complex2d const b) {
#if INSTRSET >= 7  // AVX
    __m256d b_flip = _mm256_shuffle_pd(b,b,5);      // Swap b.re and b.im
    __m256d a_im   = _mm256_shuffle_pd(a,a,0xF);    // Imag part of a in both
    __m256d a_re   = _mm256_shuffle_pd(a,a,0);      // Real part of a in both
    __m256d aib    = _mm256_mul_pd(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
#if defined(__FMA__) || INSTRSET >= 8               // FMA3 or AVX2
    return  _mm256_fmaddsub_pd(a_re, b, aib);       // a_re * b +/- aib
#else
    __m256d arb    = _mm256_mul_pd(a_re, b);        // (a.re*b.re, a.re*b.im)
    return           _mm256_addsub_pd(arb, aib);    // subtract/add
#endif // FMA
#else
    return Complex2d(a.get_low()*b.get_low(), a.get_high()*b.get_high());
#endif // AVX
}

// operator *= : multiply
static inline Complex2d & operator *= (Complex2d & a, Complex2d const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex2d operator / (Complex2d const a, Complex2d const b) {
#if INSTRSET >= 7  // AVX
    __m256d a_re   = _mm256_shuffle_pd(a,a,0);      // Real part of a in both
    __m256d arb    = _mm256_mul_pd(a_re, b);        // (a.re*b.re, a.re*b.im)
    __m256d b_flip = _mm256_shuffle_pd(b,b,5);      // Swap b.re and b.im
    __m256d a_im   = _mm256_shuffle_pd(a,a,0xF);    // Imag part of a in both
#if defined(__FMA__) || INSTRSET >= 8               // FMA3 or AVX2
    __m256d n      = _mm256_fmsubadd_pd(a_im, b_flip, arb); 
#else
    __m256d aib    = _mm256_mul_pd(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
    __m256d arbm   = change_sign<0,1,0,1>(Vec4d(arb));
    __m256d n      = _mm256_add_pd(arbm, aib);      // arbm + aib
#endif  // FMA
    __m256d bb     = _mm256_mul_pd(b, b);           // (b.re*b.re, b.im*b.im)
    __m256d bsq    = _mm256_hadd_pd(bb,bb);         // (b.re*b.re + b.im*b.im) dublicated
    return           _mm256_div_pd(n, bsq);         // n / bsq
#else
    return Complex2d(a.get_low()/b.get_low(), a.get_high()/b.get_high());
#endif
}

// operator /= : divide
static inline Complex2d & operator /= (Complex2d & a, Complex2d const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex2d operator ~ (Complex2d const a) {
    return Complex2d(change_sign<0,1,0,1>(Vec4d(a)));
}

// operator == : returns true if a == b
static inline Vec4db operator == (Complex2d const a, Complex2d const b) {
    Vec4db eq0 = Vec4d(a) == Vec4d(b);
#if COMPACT_BOOL
    uint8_t k1 = __mmask8(eq0);                  // convert to bits
    uint8_t k2 = k1 & (k1 >> 1) & 5;             // AND bits from real and imaginary parts
    return uint8_t(k2 * 3);                      // expand each into two bits
#else
    Vec4db eq1 = (Vec4db)permute4<1,0,3,2>(Vec4d(eq0));
    return eq0 & eq1;
#endif
}

// operator != : returns true if a != b
static inline Vec4db operator != (Complex2d const a, Complex2d const b) {
    return ~(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex2d and double
*
*****************************************************************************/

// operator + : add
static inline Complex2d operator + (Complex2d const a, double b) {
    return a + Complex2d(b);
}
static inline Complex2d operator + (double a, Complex2d const b) {
    return b + a;
}
static inline Complex2d & operator += (Complex2d & a, double & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex2d operator - (Complex2d const a, double b) {
    return a - Complex2d(b);
}
static inline Complex2d operator - (double a, Complex2d const b) {
    return Complex2d(a) - b;
}
static inline Complex2d & operator -= (Complex2d & a, double & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex2d operator * (Complex2d const a, double b) {
    return Complex2d(Vec4d(a) * Vec4d(b));
}
static inline Complex2d operator * (double a, Complex2d const b) {
    return b * a;
}
static inline Complex2d & operator *= (Complex2d & a, double & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex2d operator / (Complex2d const a, double b) {
    return Complex2d(Vec4d(a) / Vec4d(b));
}

static inline Complex2d operator / (double a, Complex2d const b) {
#if INSTRSET >= 7  // AVX
    Vec4d b2 = Vec4d(b) * Vec4d(b);
    Vec4d b3 = _mm256_hadd_pd(b2, b2);       // horizontal add each two elements
    return Complex2d(Vec4d(~b) * (Vec4d(a) / b3));
#else
    return Complex2d(a / b.get_low(), a / b.get_high());
#endif
}
static inline Complex2d & operator /= (Complex2d & a, double b) {
    a = a / b;
    return a;
}


/*****************************************************************************
*
*          Functions for Complex2d
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex2d abs(Complex2d const a) {
#if INSTRSET >= 7  // AVX
    Vec4d a1 = Vec4d(a) * Vec4d(a);
    Vec4d a2 = _mm256_hadd_pd(a1, _mm256_setzero_pd());   // horizontal add each two elements
    return Complex2d(sqrt(a2));
#else
    return Complex2d(abs(a.get_low()), 0., abs(a.get_high()), 0.);
#endif
}

// function csqrt: complex square root
static inline Complex2d csqrt(Complex2d const a) {
#if INSTRSET >= 7  // AVX
    Vec4d aa = a.to_vector();
    Vec4d t1 = aa * aa;                          // r*r, i*i
    Vec4d t2  = _mm256_shuffle_pd(t1,t1,5);      // swap real and imaginary parts
    Vec4d t3  = t1 + t2;                         // pairwise horizontal sum
    Vec4d t4  = sqrt(t3);                        // n = sqrt(r*r+i*i)
    Vec4d t5  = _mm256_shuffle_pd(a,a,0);        // copy real part of a
    Vec4d sbithi = _mm256_castps_pd (constant8f<0,0,0,0x80000000u,0,0,0,0x80000000u>()); // (0.,-0.,0.,-0.)
    Vec4d t6  = t5 ^ sbithi;                     // r, -r
    Vec4d t7  = t4 + t6;                         // n+r, n-r
    // Vec4d t7 = _mm256_addsub_pd(t4, -t5);     // no advantage
    Vec4d t8 = t7 * 0.5;
    Vec4d t9 = sqrt(t8);                         // sqrt((n+r)/2), sqrt((n-r)/2)
#if INSTRSET < 10
    Vec4d t10 = aa & sbithi;                     // 0, signbit of i
    Vec4d t11 = t9 ^ t10;                        // sqrt((n+r)/2), sign(i)*sqrt((n-r)/2)
#else   // AVX512VL                              // merge two instructions
    Vec4d t11 = _mm256_castsi256_pd(_mm256_ternarylogic_epi32(_mm256_castpd_si256(aa), _mm256_castpd_si256(sbithi), _mm256_castpd_si256(t9), 0x6A));
#endif
    return Complex2d(t11);
#else
    return Complex2d(csqrt(a.get_low()), csqrt(a.get_high()));
#endif
}


// function select
static inline Complex2d select (Vec4db const s, Complex2d const a, Complex2d const b) {
    return Complex2d(select(s, Vec4d(a), Vec4d(b)));
}

// interleave real and imag into complex vector
static inline Complex2d interleave_c (Vec2d const re, Vec2d const im) {
    Vec4d c(re, im);
    Vec4d d = permute4<0,2,1,3>(c);
#if INSTRSET >= 7  // AVX
    return __m256d(d);
#else
    return  Vec256de(d);
#endif
}


#endif // if MAX_VECTOR_SIZE >= 256

#if MAX_VECTOR_SIZE >= 512
/*****************************************************************************
*
*               Class Complex4d: one double precision complex number
*
*****************************************************************************/

class Complex4d {
protected:
    Vec8d y; // vector of 4 doubles
public:
    // default constructor
    Complex4d() = default;
    // construct from real and imaginary parts
    Complex4d(double re0, double im0, double re1, double im1,
        double re2, double im2, double re3, double im3) 
        : y(re0, im0, re1, im1, re2, im2, re3, im3) {}
    // construct from one Complex1d
    Complex4d(Complex1d const a) {
#if INSTRSET >= 9  // AVX512
        y = _mm512_castps_pd(_mm512_broadcast_f32x4(_mm_castpd_ps(a)));
#else
        y = Vec8d(Complex2d(a).to_vector(), Complex2d(a).to_vector());
#endif
    }
    // construct from four Complex1d
    Complex4d(Complex1d const a0, Complex1d const a1, Complex1d const a2, Complex1d const a3) {
        y = Vec8d(Vec4d(Complex2d(a0,a1)), Vec4d(Complex2d(a2,a3)));
    }
    // construct from two Complex2d
    Complex4d(Complex2d const a0, Complex2d const a1) {
        y = Vec8d(a0.to_vector(), a1.to_vector());
    }
    // construct from real, no imaginary part
    Complex4d(double re) {
        *this = Complex4d(Complex1d(re));
    }
    // construct from real and imaginary part, broadcast to all
    Complex4d(double re, double im) {
        *this = Complex4d(Complex1d(re, im));
    }
    // Constructor to convert from type __m512d used in intrinsics or Vec256de used in emulation
#if INSTRSET >= 9  // AVX512
    Complex4d(__m512d const x) {
        y = x;
#else
    Complex4d(Vec8d const x) {
        y = x;
#endif
    }
    // Assignment operator to convert from type __m512d used in intrinsics or Vec256de used in emulation
#if INSTRSET >= 9  // AVX512
    Complex4d & operator = (__m512d const x) {
#else
    Complex4d & operator = (Vec8d const x) {
#endif
        *this = Complex4d(x);
        return *this;
    }
    // Type cast operator to convert to __m512d used in intrinsics or Vec256de used in emulation
#if INSTRSET >= 9  // AVX512
    operator __m512d() const {
        return y;
    }
#endif
    // Member function to convert to vector
    Vec8d to_vector() const {
        return y;
    }
    // Member function to load from array (unaligned)
    Complex4d & load(double const * p) {
        y = Vec8d().load(p);
        return *this;
    }
    // Member function to store into array (unaligned)
    void store(double * p) const {
        y.store(p);
    }
    // Member functions to split into two Complex1d:
    Complex2d get_low() const {
        return Complex2d(y.get_low());
    }
    Complex2d get_high() const {
        return Complex2d(y.get_high());
    }
    // Member function to insert one complex scalar into complex vector
    Complex4d insert (int index, Complex1d x) {
#if INSTRSET >= 10   // AVX512VL
        y = _mm512_mask_broadcast_f64x2(y, __mmask8(3 << (index*2)), x);
#else
        Complex4d a(x,x,x,x);
        Vec8db s;
        s.load_bits(3 << (2*index));
        *this = Complex4d(select(s, a.to_vector(), to_vector()));
#endif
        return *this;
    }
    // Member function to extract one complex scalar from complex vector
    Complex1d extract (int i) const {
#if INSTRSET >= 9  // AVX512VL
        __m512d x = _mm512_maskz_compress_pd(uint8_t(3 << (2*i)), y);
        return Complex1d(_mm512_castpd512_pd128(x));
#else
        double x[8];
        store(x);
        return Complex1d().load(x + 2*i);
#endif
    }
    // get real parts
    Vec4d real() const {
        return permute8<0,2,4,6,1,3,5,7>(to_vector()).get_low();
    }
    // get imaginary parts
    Vec4d imag() const {
        // make the permute a common subexpression that can be eliminated 
        // by the compiler if both real() and imag() are called
        return permute8<0,2,4,6,1,3,5,7>(to_vector()).get_high();
    }
    static constexpr int size() {
        return 4;
    }
    static constexpr int elementtype() {
        return 0x111;
    }
};


/*****************************************************************************
*
*          Operators for Complex4d
*
*****************************************************************************/

// operator + : add
static inline Complex4d operator + (Complex4d const a, Complex4d const b) {
    return Complex4d(a.to_vector() + b.to_vector());
}

// operator += : add
static inline Complex4d & operator += (Complex4d & a, Complex4d const b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex4d operator - (Complex4d const a, Complex4d const b) {
    return Complex4d(a.to_vector() - b.to_vector());
}

// operator - : unary minus
static inline Complex4d operator - (Complex4d const a) {
    return Complex4d(- a.to_vector());
}

// operator -= : subtract
static inline Complex4d & operator -= (Complex4d & a, Complex4d const b) {
    a = a - b;
    return a;
}

// operator * : complex multiply is defined as
// (a.re * b.re - a.im * b.im,  a.re * b.im + b.re * a.im)
static inline Complex4d operator * (Complex4d const a, Complex4d const b) {
#if INSTRSET >= 9  // AVX512
    Vec8d aa = a.to_vector();
    Vec8d bb = b.to_vector();
    __m512d b_flip = _mm512_shuffle_pd(bb,bb,0x55);  // Swap b.re and b.im
    __m512d a_im   = _mm512_shuffle_pd(aa,aa,0xFF);  // Imag part of a in both
    __m512d a_re   = _mm512_shuffle_pd(aa,aa,0);     // Real part of a in both
    __m512d aib    = _mm512_mul_pd(a_im, b_flip);    // (a.im*b.im, a.im*b.re)
#if defined(__FMA__) || INSTRSET >= 8                // FMA3 or AVX2
    return  _mm512_fmaddsub_pd(a_re, bb, aib);       // a_re * b +/- aib
#else
    __m512d arb    = _mm512_mul_pd(a_re, b);        // (a.re*b.re, a.re*b.im)
    return           _mm512_addsub_pd(arb, aib);    // subtract/add
#endif // FMA
#else
    return Complex4d(a.get_low()*b.get_low(), a.get_high()*b.get_high());
#endif // AVX
}

// operator *= : multiply
static inline Complex4d & operator *= (Complex4d & a, Complex4d const b) {
    a = a * b;
    return a;
}

// operator / : complex divide is defined as
// (a.re * b.re + a.im * b.im, b.re * a.im - a.re * b.im) / (b.re * b.re + b.im * b.im)
static inline Complex4d operator / (Complex4d const a, Complex4d const b) {
#if INSTRSET >= 9  // AVX512
    __m512d a_re   = _mm512_shuffle_pd(a,a,0);      // Real part of a in both
    __m512d arb    = _mm512_mul_pd(a_re, b);        // (a.re*b.re, a.re*b.im)
    __m512d b_flip = _mm512_shuffle_pd(b,b,0x55);   // Swap b.re and b.im
    __m512d a_im   = _mm512_shuffle_pd(a,a,0xFF);   // Imag part of a in both
#if defined(__FMA__) || INSTRSET >= 8               // FMA3 or AVX2
    __m512d n      = _mm512_fmsubadd_pd(a_im, b_flip, arb); 
#else
    __m512d aib    = _mm512_mul_pd(a_im, b_flip);   // (a.im*b.im, a.im*b.re)
    __m512d arbm   = change_sign<0,1,0,1>(Vec8d(arb));
    __m512d n      = _mm512_add_pd(arbm, aib);      // arbm + aib
#endif  // FMA
    __m512d bb     = _mm512_mul_pd(b, b);           // (b.re*b.re, b.im*b.im)
    __m512d bbfl   = _mm512_shuffle_pd(bb,bb,0x55); // (b.im*b.im, b.re*b.re)
    __m512d bsq    = _mm512_add_pd(bb,bbfl);        // (b.re*b.re + b.im*b.im) dublicated
    return           _mm512_div_pd(n, bsq);         // n / bsq
#else
    return Complex4d(a.get_low() / b.get_low(), a.get_high() / b.get_high());
#endif
}

// operator /= : divide
static inline Complex4d & operator /= (Complex4d & a, Complex4d const b) {
    a = a / b;
    return a;
}

// operator ~ : complex conjugate
// ~(a,b) = (a,-b)
static inline Complex4d operator ~ (Complex4d const a) {
    return Complex4d(change_sign<0,1,0,1,0,1,0,1>(a.to_vector()));
    //return Complex4d(a.to_vector() ^ Vec8d(0,-0.0,0,-0.0,0,-0.0,0,-0.0));
}

// operator == : returns true if a == b
static inline Vec8db operator == (Complex4d const a, Complex4d const b) {
#if COMPACT_BOOL
    Vec8db eq0 = a.to_vector() == b.to_vector();
    uint8_t k1 = __mmask8(eq0);                  // convert to bits
    uint8_t k2 = k1 & (k1 >> 1) & 0x55;          // AND bits from real and imaginary parts
    return uint8_t(k2 * 3);                      // expand each into two bits
#else
    return Vec8db(a.get_low() == b.get_low(), a.get_high() == b.get_high());
#endif
}

// operator != : returns true if a != b
static inline Vec8db operator != (Complex4d const a, Complex4d const b) {
    return ~(a == b);
}

/*****************************************************************************
*
*          Operators mixing Complex4d and double
*
*****************************************************************************/

// operator + : add
static inline Complex4d operator + (Complex4d const a, double b) {
    return a + Complex4d(b);
}
static inline Complex4d operator + (double a, Complex4d const b) {
    return b + a;
}
static inline Complex4d & operator += (Complex4d & a, double & b) {
    a = a + b;
    return a;
}

// operator - : subtract
static inline Complex4d operator - (Complex4d const a, double b) {
    return a - Complex4d(b);
}
static inline Complex4d operator - (double a, Complex4d const b) {
    return Complex4d(a) - b;
}
static inline Complex4d & operator -= (Complex4d & a, double & b) {
    a = a - b;
    return a;
}

// operator * : multiply
static inline Complex4d operator * (Complex4d const a, double b) {
    return Complex4d(a.to_vector() * b);
}
static inline Complex4d operator * (double a, Complex4d const b) {
    return b * a;
}
static inline Complex4d & operator *= (Complex4d & a, double & b) {
    a = a * b;
    return a;
}

// operator / : divide
static inline Complex4d operator / (Complex4d const a, double b) {
    return Complex4d(a.to_vector() / b);
}

static inline Complex4d operator / (double a, Complex4d const b) {
#if INSTRSET >= 9  // AVX512
    Vec8d b2 = Vec8d(b) * Vec8d(b);            // square each element
    Vec8d b3 = _mm512_shuffle_pd(b2,b2,0x55);  // swap elements
    Vec8d b4 = b2 + b3;                        // horizontal add each two elements
    return Complex4d(Vec8d(~b) * (Vec8d(a) / b4));
#else
    return Complex4d(a / b.get_low(), a / b.get_high());
#endif
}
static inline Complex4d & operator /= (Complex4d & a, double b) {
    a = a / b;
    return a;
}


/*****************************************************************************
*
*          Functions for Complex4d
*
*****************************************************************************/

// function abs: absolute value
// abs(a,b) = sqrt(a*a+b*b);
static inline Complex4d abs(Complex4d const a) {
#if INSTRSET >= 9  // AVX512
    Vec8d a1  = Vec8d(a) * Vec8d(a);
    Vec8d afl = _mm512_shuffle_pd(a1,a1,0x55);       // swap real and imaginary part
    Vec8d bsq = _mm512_maskz_add_pd(0x55, a1, afl);  // sum. zero imaginary parts
    return Complex4d(sqrt(bsq));
#else
    return Complex4d(abs(a.get_low()), abs(a.get_high()));
#endif
}

// function csqrt: complex square root
static inline Complex4d csqrt(Complex4d const a) {
#if INSTRSET >= 9  // AVX512
    Vec8d aa = a.to_vector();
    Vec8d t1 = aa * aa;                          // r*r, i*i
    Vec8d t2  = _mm512_shuffle_pd(t1,t1,0x55);   // swap real and imaginary parts
    Vec8d t3  = t1 + t2;                         // pairwise horizontal sum
    Vec8d t4  = sqrt(t3);                        // n = sqrt(r*r+i*i)
    Vec8d t5  = _mm512_shuffle_pd(a,a,0);        // copy real part of a
    Vec8d sbithi = _mm512_castsi512_pd(
        constant16ui<0,0,0,0x80000000u,0,0,0,0x80000000u,0,0,0,0x80000000u,0,0,0,0x80000000u>()); // (0.,-0.,0.,-0.,0.,-0.,0.,-0.)
    Vec8d t6  = t5 ^ sbithi;                     // r, -r
    Vec8d t7  = t4 + t6;                         // n+r, n-r
    Vec8d t8 = t7 * 0.5;
    Vec8d t9 = sqrt(t8);                         // sqrt((n+r)/2), sqrt((n-r)/2)
#if INSTRSET < 10
    Vec8d t10 = aa & sbithi;                     // 0, signbit of i
    Vec8d t11 = t9 ^ t10;                        // sqrt((n+r)/2), sign(i)*sqrt((n-r)/2)
#else   // AVX512VL                              // merge two instructions
    Vec8d t11 = _mm512_castsi512_pd(_mm512_ternarylogic_epi32(_mm512_castpd_si512(aa), _mm512_castpd_si512(sbithi), _mm512_castpd_si512(t9), 0x6A));
#endif
    return Complex4d(t11);
#else
    return Complex4d(csqrt(a.get_low()), csqrt(a.get_high()));
#endif
}

// function select
static inline Complex4d select (Vec8db const s, Complex4d const a, Complex4d const b) {
    return Complex4d(select(s, a.to_vector(), b.to_vector()));
}

// interleave real and imag into complex vector
static inline Complex4d interleave_c (Vec4d const re, Vec4d const im) {
    Vec8d c(re, im);
    Vec8d d = permute8<0,4,1,5,2,6,3,7>(c);
#if INSTRSET >= 9  // AVX512
    return __m512d(d);
#else
    return Complex4d(d);
#endif
}

#endif  // MAX_VECTOR_SIZE >= 512

/*****************************************************************************
*
*          Conversion functions
*
*****************************************************************************/

// function to_float: convert Complex1d to Complex1f
static inline Complex1f to_float (Complex1d const a) {
    return _mm_cvtpd_ps(a);
}

// function to_double: convert Complex1f to Complex1d
static inline Complex1d to_double (Complex1f const a) {
    return _mm_cvtps_pd(a);
}


#if MAX_VECTOR_SIZE >= 256
// function to_float: convert Complex2d to Complex2f
static inline Complex2f to_float (Complex2d const a) {
#if INSTRSET >= 7  // AVX
    return _mm256_cvtpd_ps(a);
#else
    return Complex2f(to_float(a.get_low()), to_float(a.get_high()));
#endif
}

// function to_double: convert Complex2f to Complex2d
static inline Complex2d to_double (Complex2f const a) {
#if INSTRSET >= 7  // AVX
    return _mm256_cvtps_pd(a);
#else
    return Complex2d(to_double(a.get_low()), to_double(a.get_high()));
#endif
}


#endif  // MAX_VECTOR_SIZE >= 256

#if MAX_VECTOR_SIZE >= 512
// function to_float: convert Complex2d to Complex2f
static inline Complex4f to_float (Complex4d const a) {
#if INSTRSET >= 9  // AVX512
    return _mm512_cvtpd_ps(a);
#else
    return Complex4f(to_float(a.get_low()), to_float(a.get_high()));
#endif
}

// function to_double: convert Complex2f to Complex2d
static inline Complex4d to_double (Complex4f const a) {
#if INSTRSET >= 9  // AVX512
    return _mm512_cvtps_pd(a);
#else
    return Complex4d(to_double(a.get_low()), to_double(a.get_high()));
#endif
}

#endif  // MAX_VECTOR_SIZE >= 512


// complex sum of vector elements
template <typename C>
static inline auto chorizontal_add (C const a) {
    static_assert(((C::elementtype() & 0x100) != 0), "Must be complex vector");
    // split vector into two halves and add them. repeat until only one element
    if constexpr (a.size() == 1) {
        return a;
    }
    else {
        auto b = a.get_low() + a.get_high();
        if constexpr (b.size() == 1) {
            return b;
        }
        else {
            auto c = b.get_low() + b.get_high();
            if constexpr (c.size() == 1) {
                return c;
            }
            else {
                auto d = c.get_low() + c.get_high();
                if constexpr (d.size() == 1) {
                    return d;
                }
                else {
                    auto e = d.get_low() + d.get_high();
                    static_assert(e.size() == 1, "Unknown complex vector size");
                    if constexpr (e.size() == 1) {
                        return e;
                    }
                }
            }
        }
    }
}

// flip odd and even vector elements. used internally
template <typename V>
static inline V flip_odd_even(V const a) {
    V flip = a;
    if constexpr (V::size() == 2) {
        flip = permute2<1,0>(a);
    }
    else if constexpr (V::size() == 4) {
        flip = permute4<1,0,3,2>(a);
    }
    else if constexpr (V::size() == 8) {
        flip = permute8<1,0,3,2,5,4,7,6>(a);
    }
    else if constexpr (V::size() == 16) {
        flip = permute16<1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14>(a);
    }
    else if constexpr (V::size() == 32) {
        flip = permute32<1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14,17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30>(a);
    }
    return flip;
}

// multiplication, without loss of precision
template <typename C>
static inline C mul_accurate (C const a, C const b) {
    auto aa = a.to_vector();          // a as vector of interleaved real and imaginary parts
    auto bb = b.to_vector();          // a as vector of interleaved real and imaginary parts
    typedef decltype(aa) V;           // vector type
    V b_flip = aa, a_im = aa, a_re = aa;
    if constexpr (V::size() == 2) {
        b_flip = permute2<1,0>(bb);        // Swap b.re and b.im
        a_im   = permute2<1,1>(aa);        // Imag part of a in both
        a_re   = permute2<0,0>(aa);        // Real part of a in both
        b_flip = change_sign<1,0>(b_flip); // change sign of real part
    }
    else if constexpr (V::size() == 4) {
        b_flip = permute4<1,0,3,2>(bb);        // Swap b.re and b.im
        a_im   = permute4<1,1,3,3>(aa);        // Imag part of a in both
        a_re   = permute4<0,0,2,2>(aa);        // Real part of a in both
        b_flip = change_sign<1,0,1,0>(b_flip); // change sign of real part
    }
    else if constexpr (V::size() == 8) {
        b_flip = permute8<1,0,3,2,5,4,7,6>(bb);        // Swap b.re and b.im
        a_im   = permute8<1,1,3,3,5,5,7,7>(aa);        // Imag part of a in both
        a_re   = permute8<0,0,2,2,4,4,6,6>(aa);        // Real part of a in both
        b_flip = change_sign<1,0,1,0,1,0,1,0>(b_flip); // change sign of real part
    }
    else if constexpr (V::size() == 16) {
        b_flip = permute16<1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14>(bb); // Swap b.re and b.im
        a_im   = permute16<1,1,3,3,5,5,7,7,9,9,11,11,13,13,15,15>(aa); // Imag part of a in both
        a_re   = permute16<0,0,2,2,4,4,6,6,8,8,10,10,12,12,14,14>(aa); // Real part of a in both
        b_flip = change_sign<1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>(b_flip); // change sign of real part
    }
    else if constexpr (V::size() == 32) {
        b_flip = permute32<1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14,17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30>(bb); // Swap b.re and b.im
        a_im   = permute32<1,1,3,3,5,5,7,7,9,9,11,11,13,13,15,15,17,17,19,19,21,21,23,23,25,25,27,27,29,29,31,31>(aa); // Imag part of a in both
        a_re   = permute32<0,0,2,2,4,4,6,6,8,8,10,10,12,12,14,14,16,16,18,18,20,20,22,22,24,24,26,26,28,28,30,30>(aa); // Real part of a in both
        b_flip = change_sign<1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0>(b_flip); // change sign of real part
    }
    V aib = a_im * b_flip;                  // (-a.im*b.im, a.im*b.re)
    V arb = a_re * bb;                      // (a.re*b.re,  a.re*b.im)
    V aibrem = mul_sub(a_im, b_flip, aib);  // remaining bits of aib
    V arbrem = mul_sub(a_re, bb, arb);      // remaining bits of arb
    V prod = aib + arb + (aibrem + arbrem); // add high and low bits
    return C(prod);                         // return corrected product  
}

// division, without loss of precision
template <typename C>
static inline C div_accurate (C const a, C const b) {
    C pp = mul_accurate(a, ~b);
    auto bb = b.to_vector();
    auto nn = bb * bb;
    auto nn_flip = flip_odd_even(nn);
    auto dd = pp.to_vector() / (nn + nn_flip);   // a/b = (a*~b)/norm_squared(b)
    return C(dd);
}

// compare complex numbers. abs_greater: abs(a) > abs(b)
template <typename C>
static inline auto abs_greater(C const a, C const b) {
    auto aa = a.to_vector();
    auto bb = b.to_vector();
    auto a2 = aa * aa;
    auto b2 = bb * bb;
    auto absa2 = a2 + flip_odd_even(a2);
    auto absb2 = b2 + flip_odd_even(b2);
    return absa2 > absb2;
}

// compare complex numbers. abs_less: abs(a) < abs(b)
template <typename C>
static inline auto abs_less(C const a, C const b) {
    return abs_greater(b, a);
}



/*****************************************************************************
*
*          Mathematical functions
*
*****************************************************************************/

// mathematical functions must be defined before including complexvec.h:
#if defined (VECTORMATH_EXP_H) && defined (VECTORMATH_TRIG_H) && VECTORCLASS_H >= 20000

// function cexp: complex exponential function
// cexp(re+i*im) = exp(re)*(cos(im)+i*sin(im))
// (Does not work with Intel compiler 2019)
template <typename C>
static inline C cexp (C const a) {     // complex exponential function
    auto vec = a.to_vector();          // a as vector of interleaved real and imaginary parts
    // get real and imaginary parts in separate vectors
    auto im = vec;
    auto re = vec;
    if constexpr (vec.size() == 2) {
        re = permute2<0,0>(vec);
        im = permute2<1,1>(vec);
    }
    else if constexpr (vec.size() == 4) {
        re = permute4<0,0,2,2>(vec);
        im = permute4<1,1,3,3>(vec);
    }
    else if constexpr (vec.size() == 8) {
        re = permute8<0,0,2,2,4,4,6,6>(vec);
        im = permute8<1,1,3,3,5,5,7,7>(vec);
    }
    else if constexpr (vec.size() == 16) {
        re = permute16<0,0,2,2,4,4,6,6,8,8,10,10,12,12,14,14>(vec);
        im = permute16<1,1,3,3,5,5,7,7,9,9,11,11,13,13,15,15>(vec);
    }
    else if constexpr (vec.size() == 32) {
        re = permute32<0,0,2,2,4,4,6,6,8,8,10,10,12,12,14,14,16,16,18,18,20,20,22,22,24,24,26,26,28,28,30,30>(vec);
        im = permute32<1,1,3,3,5,5,7,7,9,9,11,11,13,13,15,15,17,17,19,19,21,21,23,23,25,25,27,27,29,29,31,31>(vec);
    }
    // get sin and cos of imag part
    auto cosi = vec;
    auto sini = sincos(&cosi, im);
    // get exp of real part
    auto expr = exp(re);
    // interleave cos and sin into real and imag parts
    auto sincosi = vec; 
    if constexpr (vec.size() == 2) {
        sincosi = blend2<0,3>(cosi, sini);
    }
    else if constexpr (vec.size() == 4) {
        sincosi = blend4<0,5,2,7>(cosi, sini);
    }
    else if constexpr (vec.size() == 8) {
        sincosi = blend8<0,9,2,11,4,13,6,15>(cosi, sini);
    }
    else if constexpr (vec.size() == 16) {
        sincosi = blend16<0,17,2,19,4,21,6,23,8,25,10,27,12,29,14,31>(cosi, sini);
    }
    else if constexpr (vec.size() == 32) {
        sincosi = blend32<0,33,2,35,4,37,6,39,8,41,10,43,12,45,14,47,16,49,18,51,20,53,22,55,24,57,26,59,28,61,30,63>(cosi, sini);
    }
    // multiply (sin(im) + i*cos(im)) by exp(re)
    auto result = expr * sincosi;
    if constexpr (C::size() == 1 && vec.size() == 4) {
        result.cutoff(2);  // using only half of vector. set the rest to zero
    }
    return C(result);
}


// function clog: complex logarithm function
// clog(re+i*im) = 0.5*log(re*re + im*im) + i*atan2(im, re)
// (Does not work with Intel compiler 2019)
template <typename C>
static inline C clog (C const a) {     // complex logarithm function
    auto vec = a.to_vector();          // a as vector of interleaved real and imaginary parts
    // get sum of squares
    auto ssum = vec * vec;
    // get real and imaginary parts in separate vectors
    auto im = vec;
    auto re = vec;
    if constexpr (vec.size() == 2) {
        re = permute2<V_DC,0>(vec);
        im = permute2<V_DC,1>(vec);
        ssum += permute2<1,V_DC>(ssum);
    }
    else if constexpr (vec.size() == 4) {
        re = permute4<V_DC,0,V_DC,2>(vec);
        im = permute4<V_DC,1,V_DC,3>(vec);
        ssum += permute4<1,V_DC,3,V_DC>(ssum);
    }
    else if constexpr (vec.size() == 8) {
        re = permute8<V_DC,0,V_DC,2,V_DC,4,V_DC,6>(vec);
        im = permute8<V_DC,1,V_DC,3,V_DC,5,V_DC,7>(vec);
        ssum += permute8<1,V_DC,3,V_DC,5,V_DC,7,V_DC>(ssum);
    }
    else if constexpr (vec.size() == 16) {
        re = permute16<V_DC,0,V_DC,2,V_DC,4,V_DC,6,V_DC,8,V_DC,10,V_DC,12,V_DC,14>(vec);
        im = permute16<V_DC,1,V_DC,3,V_DC,5,V_DC,7,V_DC,9,V_DC,11,V_DC,13,V_DC,15>(vec);
        ssum += permute16<1,V_DC,3,V_DC,5,V_DC,7,V_DC,9,V_DC,11,V_DC,13,V_DC,15,V_DC>(ssum);
    }
    // get angle
    auto angle = atan2(im, re);
    // get radius
    auto radius = log(ssum) * decltype(vec)(0.5f);
    auto result = vec;
    // interleave radius and angle into real and imag parts
    if constexpr (vec.size() == 2) {
        result = blend2<0,3>(radius, angle);
    }
    else if constexpr (vec.size() == 4) {
        result = blend4<0,5,2,7>(radius, angle);
    }
    else if constexpr (vec.size() == 8) {
        result = blend8<0,9,2,11,4,13,6,15>(radius, angle);
    }
    else if constexpr (vec.size() == 16) {
        result = blend16<0,17,2,19,4,21,6,23,8,25,10,27,12,29,14,31>(radius, angle);
    }
    if constexpr (C::size() == 1 && vec.size() == 4) {
        result.cutoff(2);  // using only half of vector. set the rest to zero
    }
    return C(result);
}


#endif // VECTORMATH_EXP_H, VECTORMATH_TRIG_H

#ifdef VCL_NAMESPACE
}
#endif

#endif  // COMPLEXVEC_H
