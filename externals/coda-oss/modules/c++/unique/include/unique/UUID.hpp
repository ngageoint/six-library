#ifndef __UNIQUE_UUID_HPP__
#define __UNIQUE_UUID_HPP__

#include <import/except.h>

namespace unique
{

typedef except::InvalidFormatException UUIDException;

/*!
 * Create a 36-character UUID and return as a std::string
 */ 
std::string generateUUID();

}
#endif

