#pragma once

#include <cstddef>

#if __cplusplus < 201703L // no C++17, see https://en.cppreference.com/w/cpp/preprocessor/replace
// https://en.cppreference.com/w/cpp/types/byte
namespace std
{
	enum class byte : unsigned char {};
}
#endif