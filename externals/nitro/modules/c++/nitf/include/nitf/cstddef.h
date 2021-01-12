#pragma once

#include <cstddef>

#include "sys/Conf.h"

#ifndef NITRO_DEFINE_std_byte_
#if CODA_OSS_cplusplus < 201703L // no C++17
#define NITRO_DEFINE_std_byte_ 1
#else
#define NITRO_DEFINE_std_byte_ 0 
#endif  // CODA_OSS_cplusplus
#endif  // NITRO_DEFINE_std_byte_

#if NITRO_DEFINE_std_byte_
// This is ever-so-slightly uncouth: we're not supposed to augment "std".
// https://en.cppreference.com/w/cpp/types/byte
namespace std
{
	enum class byte : unsigned char {};
}
#endif