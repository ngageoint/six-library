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


#ifndef __TIFF_IFD_ENTRY_H__
#define __TIFF_IFD_ENTRY_H__

#include <memory>
#include <string>
#include <vector>
#include <import/io.h>
#include "tiff/GenericType.h"

namespace tiff
{

/**
 *********************************************************************
 * @class IFDEntry
 * @brief Contains all the information for an IFD entry
 *
 * A TIFF IFD entry is basically a footer entry.  It keeps track of
 * information about the image such as image width, or element size.
 * Provides a container for storing TIFF IFD entry information.  This
 * includes all of the data associated with an IFD entry.  There are
 * functions for printing out the entry, reading and writing it to a
 * file, and accessing the data.
 *********************************************************************/
class IFDEntry : public io::Serializable
{
public:
    //! Constructor
    IFDEntry() :
        mTag(0), mType(0), mCount(0), mOffset(0)
    {
    }

    /**
     *****************************************************************
     * Constructor.  Allows the user to specify the tag id, type,
     * name and the number of values it has.
     *
     * @param tag
     *   the numeric identifier for this entry
     * @param type
     *   the data type for this entry (see TIFFConst::Type)
     * @param name
     *   the name of this entry (i.e. "ImageWidth")
     * @param count
     *   the number of values for this entry
     *****************************************************************/
    IFDEntry(const unsigned short tag, const unsigned short type,
            const std::string& name, const sys::Uint32_T count = 0) :
        mTag(tag), mType(type), mCount(count), mOffset(0), mName(name)
    {
    }

    /**
     *****************************************************************
     * Constructor.  Allows the user to specify the tag id, type,
     * and the number of values it has.
     *
     * @param tag
     *   the numeric identifier for this entry
     * @param type
     *   the data type for this entry (see TIFFConst::Type)
     * @param count
     *   the number of values for this entry
     *****************************************************************/
    IFDEntry(const unsigned short tag, const unsigned short type,
            const sys::Uint32_T count = 0) :
        mTag(tag), mType(type), mCount(count), mOffset(0)
    {
    }

    //! Deconstructor
    ~IFDEntry()
    {
        for (sys::Uint32_T i = 0; i < mValues.size(); ++i)
            delete mValues[i];
    }

    /**
     *****************************************************************
     * Returns the value at the specified index.
     *
     * @param index
     *   the index that indicates which value to retrieve
     * @return
     *   the value at the specified index, or NULL
     *****************************************************************/
    tiff::TypeInterface *operator[](const sys::Uint32_T index) const
    {
        return mValues[index];
    }

    /**
     *****************************************************************
     * Writes the IFD entry to the specified output stream.
     *
     * @param output
     *   the output stream to write the entry to
     *****************************************************************/
    void serialize(io::OutputStream& output);

    /**
     *****************************************************************
     * Reads the IFD entry from the specified input stream.
     *
     * @param input
     *   the input stream to read the entry from
     *****************************************************************/
    void deserialize(io::InputStream& input);
    void deserialize(io::InputStream& input, const bool reverseBytes);

    /**
     *****************************************************************
     * Prints the IFD entry in a readable format to the specified 
     * output stream.
     *
     * @param output
     *   the output stream to write the entry to
     *****************************************************************/
    void print(io::OutputStream& output) const;

    /**
     *****************************************************************
     * Returns the name of the IFD entry.
     *
     * @return
     *  the name of the IFD entry.
     *****************************************************************/
    const std::string& getName() const
    {
        return mName;
    }

    /**
     *****************************************************************
     * Returns the name of the IFD entry.
     *
     * @return
     *  the name of the IFD entry.
     *****************************************************************/
    std::string getName()
    {
        return mName;
    }

    /**
     *****************************************************************
     * Returns the type of the IFD entry.
     *
     * @return
     *  the type of the IFD entry.
     *****************************************************************/
    unsigned short getType() const
    {
        return mType;
    }

    /**
     *****************************************************************
     * Returns the number of values in the IFD entry
     *
     * @return
     *  the number of values in the IFD entry.
     *****************************************************************/
    sys::Uint32_T getCount() const
    {
        return mCount;
    }

    /**
     *****************************************************************
     * Returns the value offset.
     *
     * @return
     *  the value offset
     *****************************************************************/
    sys::Uint32_T getOffset() const
    {
        return mOffset;
    }

    /**
     *****************************************************************
     * Returns the tag ID of the IFD entry.
     *
     * @return
     *  the tag ID of the IFD entry.
     *****************************************************************/
    unsigned short getTagID() const
    {
        return mTag;
    }

    /**
     *****************************************************************
     * Returns the vector of values that are in the IFD entry.
     *
     * @return
     *  the vector of values in the IFD entry.
     *****************************************************************/
    const std::vector<tiff::TypeInterface *>& getValues() const
    {
        return mValues;
    }

    /**
     *****************************************************************
     * Returns the vector of values that are in the IFD entry.
     *
     * @return
     *  the vector of values in the IFD entry.
     *****************************************************************/
    std::vector<tiff::TypeInterface *> getValues()
    {
        return mValues;
    }

    /**
     *****************************************************************
     * Adds a value to the IFD entry, taking ownership.  IFD Entries
     * can have multiple values within them.
     *
     * @param value
     *   the tiff::GenericType to add as a value
     *****************************************************************/
    void addValue(tiff::TypeInterface *value)
    {
        mValues.push_back(value);
        ++mCount;
    }

    /**
     *****************************************************************
     * Adds a value to the IFD entry.  IFD Entries can have multiple
     * values within them.
     *
     * @param value
     *   the tiff::GenericType to add as a value
     *****************************************************************/
    void addValue(std::unique_ptr<tiff::TypeInterface>&& value)
    {
        mValues.push_back(value.get());
        ++mCount;
        value.release();
    }

    /**
     *****************************************************************
     * Adds a double value to the IFD entry.
     *
     * @param value
     *   the double to add as a value
     *****************************************************************/
    void addValue(double value);

    /**
     *****************************************************************
     * Adds the same double value to the IFD entry multiple times.
     *
     * @param value
     *   the double to add as a value
     * @param numValues
     *   the number of times to add the value
     *****************************************************************/
    void addValue(double value, size_t numValues)
    {
        for (size_t ii = 0; ii < numValues; ++ii)
        {
            addValue(value);
        }
    }

    /**
     *****************************************************************
     * Adds each character of the string as a value to the IFD entry.
     *
     * @param value
     *   the string to add as a series of values
     * @param tiffType
     *   the type of the value to use.  defaults to ascii.
     *****************************************************************/
    void addValues(const char* str, int tiffType = Const::Type::ASCII);

    void addValues(const std::string& str, int tiffType = Const::Type::ASCII)
    {
        addValues(str.c_str(), tiffType);
    }

    /**
     *****************************************************************
     * Parses the specified buffer for values to store into the
     * IFD entry.  Sets the value count of the IFD entry to the
     * specified count and uses it to determine how many values are in
     * the buffer, and the type of the IFD entry to determine the size
     * of each value.
     *
     * @param buffer
     *   the buffer to parse for values
     * @param count
     *   the number of values in the buffer
     *****************************************************************/
    void parseValues(const unsigned char *buffer, const sys::Uint32_T count);

    /**
     *****************************************************************
     * Used for outputting the IFD entry to a file.  Calculates
     * a file offset to put data that overflows the size allowed for
     * an IFD entry value (4 bytes) and sets the value count to be
     * the number of values that were added to the IFD entry.
     *
     * @param offset
     *   the next free file offset that the values will can be
     *   written to
     * @return
     *   the next free file offset, compensating for the IFD entry's
     *   values being written at the specified input offset
     *****************************************************************/
    sys::Uint32_T finalize(const sys::Uint32_T offset);

    /**
     *****************************************************************
     * According to the TIFF 6.0 spec, the size of an IFD entry is 12
     * bytes.  The sizeof operator is thrown off by the extra members 
     * mName of string type, and mValues of vector type (both of which
     * are not in the specification but exist to make life simpler),
     * hence the adjustment.  Returns the size of the IFD entry.
     *
     * @return
     *   the size of an IFD entry (12 bytes).
     *****************************************************************/
    static unsigned short sizeOf()
    {
        return 12;
    }

private:

    /**
     *****************************************************************
     * Parses the specified buffer for values to store into the
     * IFD entry.  Uses the value count of the IFD entry to determine
     * how many values are in the buffer, and the type of the IFD
     * entry to determine the size of each value.
     *
     * @param buffer
     *   the buffer to parse for values
     *****************************************************************/
    void parseValues(const unsigned char *buffer);

    //! The TIFF tag identifier
    unsigned short mTag;

    //! The TIFF type identifier
    unsigned short mType;

    //! The number of values in the IFD entry
    sys::Uint32_T mCount;

    //! The file offset to values for the IFD entry
    sys::Uint32_T mOffset;

    //! The name of the IFD entry (i.e. "ImageWidth")
    std::string mName;

    //! The values in this IFD entry
    std::vector<tiff::TypeInterface *> mValues;
};

} // End namespace.

#endif // __TIFF_IFD_ENTRY_H__
