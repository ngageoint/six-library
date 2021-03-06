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
#ifndef __SIO_LITE_FILE_HEADER_H__
#define __SIO_LITE_FILE_HEADER_H__

#include <map>
#include <string>
#include <vector>
#include <import/except.h>
#include <import/sys.h>
#include <import/io.h>
#include "sio/lite/UserDataDictionary.h"
#include "sio/lite/InvalidHeaderException.h"
#include "sio/lite/UnsupportedDataTypeException.h"

namespace sio
{
namespace lite
{

class FileHeader
{
public:
    /** Unsigned byte data */
    static const int UNSIGNED = 1;
    /** Signed byte data */
    static const int SIGNED = 2;
    /** Float data */
    static const int FLOAT = 3;
    /** Complex unsigned data */
    static const int COMPLEX_UNSIGNED = 11;
    /** Complex signed data */
    static const int COMPLEX_SIGNED = 12;
    /** Complex float data */
    static const int COMPLEX_FLOAT = 13;
    /** N-Byte unsigned */
    static const int N_BYTE_UNSIGNED = 21;
    /** N-Byte signed */
    static const int N_BYTE_SIGNED = 22;

    /*!
     * SIOs can either have a basic header which is always of length 20 or a
     * more complex header which has extra freeform key/value pairs
     */
    static const size_t BASIC_HEADER_LENGTH = 20;

    /**
     *  Constructor.
     */
    FileHeader(int numLines, int numElements, int elementSize,
               int elementType, int ver = 1)
            : nl(numLines), ne(numElements), es(elementSize), et(elementType),
            version(ver), nullTerminatedIds(true) {}

    FileHeader() : nl(0), ne(0), es(0), et(0), version(1),
                 nullTerminatedIds(true){}

    //! Destructor.
    virtual ~FileHeader() {}

    /**
     * This is the length of the header.  It is calculated on-the-spot each
     * time this method is called.
     * @return The length of the header
     */
    long getLength() const;

    /**
     *  This is the number of lines in the 2D stream
     *  @return Number of lines in SIO data
     */
    int getNumLines() const { return nl; }
    void setNumLines(int numLines) { nl = numLines; }

    /**
     *  This is the nubmer of elements in the 2D stream
     *  @return Number of elements in SIO data
     */
    int getNumElements() const { return ne; }
    void setNumElements(int numElements) { ne = numElements; }

    /**
     *   This is the element size.  It is the es byte in the header.
     *   @return The element size
     */
    int getElementSize() const { return es; }
    void setElementSize(int size) { es = size; }

    /**
     *  This is the element type.  It is the et byte in the header.
     *  @return The element type.
     */
    int getElementType() const { return et; }
    void setElementType(int type) { et = type; }

    /**
     *  As an enumerated type, the element type is not very useful
     *  (unless you make a habit of reading SIO headers and have them
     *  commited to memory).  This method is usually more helpful for
     *  users.
     *  @return A string representation of the element type
     */
    std::string getElementTypeAsString() const;

    /**
     *  This produces the file version.  Valid SIO versions appear
     *  to be
     *  1, 2, 3, & 4
     *  @return The version.
     */
    int getVersion() const { return version; }
    void setVersion(int newVersion) { version = newVersion; }

    /**
     *   This is the sate of null termination for id strings.  It
     *   indicates whether user data id strings are null terminated
     *   or not in the header data in the stream.
     *   @return The null terminated id strings flag
     */
    bool idsAreNullTerminated() const { return nullTerminatedIds; }
    void setNullTerminationFlag(bool flag) { nullTerminatedIds = flag; }

    /**
     *  In other words (for java), is our input stream little-endian
     *  @return Whether or not the input stream is little-endian
     */
    bool isDifferentByteOrdering() const { return differentByteOrdering; }
    void setIsDifferentByteOrdering(bool isDifferent)
    {
        differentByteOrdering = isDifferent;
    }

    /**
     * Does a given user data field exist?
     *
     * This is equivalent to calling: getUserDataSection().exists(key)
     *
     * @param key The key to search for in our hash table
     * @return true if it exists
     */
    bool userDataFieldExists(const std::string& key) const;

    /**
     *  Get all of the user data field names
     *  @return An iteratable structure of all keys in the hash table
     */
    void getAllUserDataFields(std::vector<std::string>& keys) const;

    /**
     *  Get the number of user data fields in our associative array
     *  @return The number of user data fields
     */
    size_t getNumUserDataFields() const { return userData.size(); }

    /**
     *  Get the raw byte user data for a given user data ID.
     *  @return An array of user data for a given key
     */
    std::vector<sys::byte>& getUserData(const std::string& key);

    /**
     *  Get back the whole hash table
     *  @return The hash table
     */
    const sio::lite::UserDataDictionary& getUserDataSection() const{ return userData; }
    sio::lite::UserDataDictionary& getUserDataSection() { return userData; }


    //! Add a std::string user data field
    void addUserData(const std::string& field, const std::string& data);
    //! Add a std::vector<sys::byte> user data field
    void addUserData(const std::string& field,
                     const std::vector<sys::byte>& data);
    //! Add an int user data field
    void addUserData(const std::string& field, int data);

    /**
     * Writes the SIO header to the given OutputStream
     * @param numBands the number of bands intended for the SIO file
     * @os    the OutputStream to write the header to
     */
    void to(size_t numBands, io::OutputStream& os);

protected:
    /** Number of lines in the image or vector */
    int nl;

    /** Number of elements in the image or vector */
    int ne;

    /** The size of each individual element in the stream */
    int es;

    /** The type of element */
    int et;

    /** The version of SIO file we have */
    int version;

    /** A map representing user data and its corresponding ID keys */
    bool nullTerminatedIds;
    sio::lite::UserDataDictionary userData;

    /** Is our input file byte ordering different from our system's */
    bool differentByteOrdering;

    /** Write the version2 user data */
    void writeUserData(io::OutputStream& os);
};

}
}


#endif


