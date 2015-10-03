/* =========================================================================
 * This file is part of sio.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-c++ is free software; you can redistribute it and/or modify
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
