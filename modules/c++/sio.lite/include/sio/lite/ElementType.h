/******************************************************************************
 * (C) Copyright 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#ifndef __SIO_LITE_ELEMENT_TYPE_H__
#define __SIO_LITE_ELEMENT_TYPE_H__

#include <sys/Conf.h>
#include <sio/lite/FileHeader.h>

namespace sio
{
namespace lite
{
template <typename T>
struct ElementType
{
    static const size_t Type = sio::lite::FileHeader::UNSIGNED;
};
template <>
struct ElementType<sys::Int8_T>
{
    static const size_t Type = sio::lite::FileHeader::SIGNED;
};
template <>
struct ElementType<sys::Int16_T>
{
    static const size_t Type = sio::lite::FileHeader::SIGNED;
};
template <>
struct ElementType<sys::Int32_T>
{
    static const size_t Type = sio::lite::FileHeader::SIGNED;
};
template <>
struct ElementType<sys::Int64_T>
{
    static const size_t Type = sio::lite::FileHeader::SIGNED;
};
template <>
struct ElementType<float>
{
    static const size_t Type = sio::lite::FileHeader::FLOAT;
};
template <>
struct ElementType<double>
{
    static const size_t Type = sio::lite::FileHeader::FLOAT;
};
template <>
struct ElementType<std::complex<float> >
{
    static const size_t Type = sio::lite::FileHeader::COMPLEX_FLOAT;
};
}
}

#endif
