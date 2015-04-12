/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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

/**
 *********************************************************************
 * @file
 * @brief Contains classes for handling TIFF files.
 *********************************************************************/
#ifndef __TIFF_COMMON_H__
#define __TIFF_COMMON_H__

#include <import/sys.h>
#include <string>
#include <sstream>

namespace tiff
{

/**
 *********************************************************************
 * @class Const
 * @brief Contains constants used by TIFF files.
 *
 * Constants such as TIFF types, type sizes, and others have been
 * pooled together into this class.  Every member can be accessed
 * statically so there is no need to create in instance of this class
 * to use the constants.
 *********************************************************************/
class Const
{
public:
    //! Default constructor
    Const()
    {
    }

    //! Deconstructor
    ~Const()
    {
    }

    /**
     *****************************************************************
     * @class Type
     * @brief Contains an enumeration of every element type that TIFF
     * supports.
     *****************************************************************/
    class Type
    {
    public:
        enum
        {
            NOTYPE = 0,
            BYTE,
            ASCII,
            SHORT,
            LONG,
            RATIONAL,
            SBYTE,
            UNDEFINED,
            SSHORT,
            SLONG,
            SRATIONAL,
            FLOAT,
            DOUBLE,
            MAX
        };
    };

    /**
     *****************************************************************
     * @class Type
     * @brief Contains an enumeration of every sample format type.
     *****************************************************************/
    class SampleFormatType
    {
    public:
        enum
        {
            UNSIGNED_INT = 1,
            SIGNED_INT,
            IEEE_FLOAT,
            UNDEFINED
        };
    };

    /**
     *****************************************************************
     * @class Type
     * @brief Contains an enumeration of every photometric
     * interpretation type.
     *****************************************************************/
    class PhotoInterpType
    {
    public:
        enum
        {
            WHITE_IS_ZERO,
            BLACK_IS_ZERO,
            RGB,
            COLORMAP,
            TRANSPARENCY_MASK
        };
    };


    /*
     * Compression
     * http://www.awaresystems.be/imaging/tiff/tifftags/compression.html
     */

    class CompressionType
    {
    public:
        enum
        {
            NO_COMPRESSION = 1,
            CCITT_HUFFMAN,
            CCITT_G3_FAX,
            CCITT_G4_FAX,
            LZW,
            JPEG_OLD,
            JPEG,
            DEFLATE,
            JBIG_BW,
            JBIG_COLOR,
            PACK_BITS = 32773
        };
    };


    /**
     *****************************************************************
     * Returns the size of the specified TIFF type.
     *
     * @param type
     *   The TIFF type to return the size of
     * @return
     *   The size of the specified TIFF type
     *****************************************************************/
    static short sizeOf(unsigned short type)
    {
        return mTypeSizes[type];
    }

private:
    //! The array of sizes for each TIFF type.
    static short mTypeSizes[Type::MAX];
};

/**
 *********************************************************************
 * @class PrintStrategy
 * @brief A strategy that defines how to print data.
 *
 * Most TIFF types will use this print strategy that simply converts
 * the data to a string.
 *********************************************************************/
class PrintStrategy
{
public:
    //! Default constructor
    PrintStrategy()
    {
    }

    //! Deconstructor
    ~PrintStrategy()
    {
    }

    /**
     *****************************************************************
     * Returns the string form of the data passed in.  Used to make
     * numbers printable without using printf().
     *
     * @param data
     *   the data to convert to a string
     * @return
     *   the data in string format
     *****************************************************************/
    template <typename T> static std::string toString(const T& data)
    {
        std::stringstream tempStream;
        tempStream << data;
        std::string output(tempStream.str());
        return output;
    }
};

/**
 *******************************************************************
 * @class RationalPrintStrategy
 * @brief A strategy that defines how to print data for rational
 * values.
 *
 * This print strategy is used to convert rational values into a
 * printable string.  It divides the numerator by the denominator
 * and returns the string form of the result.
 *******************************************************************/
class RationalPrintStrategy
{
public:
    //! Default Constructor
    RationalPrintStrategy() {}

    //! Constructor
    ~RationalPrintStrategy()
    {
    }

    /**
     *************************************************************
     * Converts the specified data to a string.  Both numerator
     * and denominator are signed longs.
     *
     * @param data
     *   the data to convert to a string.
     * @return
     *   the string form of the rational number
     *************************************************************/
    static std::string toString(const sys::Uint32_T data);

};

/**
 *****************************************************************
 * Combines a numerator and a denominator into a single data
 * type.  The numerator is placed in the top 4 bytes, and the
 * denominator in the bottom 4 bytes of the returned sys::Uint64_T.
 *
 * @param numerator
 *   the numerator
 * @param denominator
 *   the denominator
 * @return
 *   the combined rational number
 *****************************************************************/
sys::Uint64_T combine(sys::Uint32_T numerator, sys::Uint32_T denominator);

/**
 *****************************************************************
 * Combines a numerator and a denominator into a single data
 * type.  The numerator is placed in the top 4 bytes, and the
 * denominator in the bottom 4 bytes of the returned unsigned
 * sys::Uint32_T.
 *
 * @param numerator
 *   the numerator
 * @param denominator
 *   the denominator
 * @return
 *   the combined rational number
 *****************************************************************/
sys::Uint64_T combine(sys::Uint32_T numerator, sys::Uint32_T denominator);

/**
 *****************************************************************
 * Splits the 'value' parameter into a numerator and a
 * denominator.
 *
 * @param value
 *   the rational number to split into parts
 * @param numerator
 *   the returned numerator
 * @param denominator
 *   the returned denominator
 *****************************************************************/
//void split(sys::Uint32_T value, sys::Uint32_T &numerator, sys::Uint32_T &denominator);

/**
 *****************************************************************
 * Splits the 'value' parameter into a numerator and a
 * denominator.
 *
 * @param value
 *   the rational number to split into parts
 * @param numerator
 *   the returned numerator
 * @param denominator
 *   the returned denominator
 *****************************************************************/
void split(sys::Uint64_T value, sys::Uint32_T &numerator,
           sys::Uint32_T &denominator);

} // End namespace tiff.

#endif // __TIFF_COMMON_H__
