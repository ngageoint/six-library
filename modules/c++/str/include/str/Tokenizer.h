/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * str-c++ is free software; you can redistribute it and/or modify
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


#ifndef __STR_TOKENIZER_H__
#define __STR_TOKENIZER_H__

/*!
 * \file Tokenizer.h
 * \brief A class functor for string tokenization
 *
 * Provides the implemenation of the Tokenizer class, which is
 * a function object that can do tokenization base on a delimiter
 *
 */

#include <string>
#include <vector>

namespace str
{

/*!
 * \class Tokenizer
 * \brief A class functor for string tokenization
 *
 * By specifying a set of delimiters and an input string,
 * the user can tokenize using this method.
 * example: vector<string> v = Tokenizer(str, ";");
 *
 *
 */

class Tokenizer
{
public:
    typedef std::vector<std::string> Tokens;
    /*!
     * Constructor.  Take a string to parse, and a delimiter set
     * \param str String to parse
     * \param delim string to divide str up using
     */
    Tokenizer(const std::string& str, const std::string& delim);

    /*!
     * Method to return the resultant vector 
     * \return The vector that was created by the tokenizer
     */
    operator Tokenizer::Tokens& ()
    {
        return vec;
    }
protected:
    Tokenizer::Tokens vec;
};

}
#endif
