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
#include "sio/lite/FileHeader.h"

// magic + nl + ne + et + es
const int SIO_HEADER_LENGTH = 20;

std::string sio::lite::FileHeader::getElementTypeAsString() const
{
    std::string type;
    //  Switch on the enumerated type and produce something useful
    switch ( et )
    {
        case UNSIGNED:
            type = "Unsigned byte";
            break;
        case SIGNED:
            type = "Signed byte";
            break;
        case FLOAT:
            type = "Float";
            break;
        case COMPLEX_UNSIGNED:
            type = "Complex unsigned";
            break;
        case COMPLEX_SIGNED:
            type = "Complex signed";
            break;
        case COMPLEX_FLOAT:
            type = "Complex float";
            break;
        case N_BYTE_UNSIGNED:
            type = "N-Byte unsigned";
            break;
        case N_BYTE_SIGNED:
            type = "N-Byte signed";
            break;
        default:
            type = "Unknown type";

    }
    return type;
}

long sio::lite::FileHeader::getLength() const
{
    size_t length = SIO_HEADER_LENGTH;

    if (userData.size() > 0)
        length += 4; //num fields int
    for (sio::lite::UserDataDictionary::ConstIterator it = userData.begin();
        it != userData.end(); ++it)
    {
        length += 4; //key size
        length += it->first.length(); //key data
        if (idsAreNullTerminated())
            length += 1; //1 (null-byte)
        length += 4; //data size
        length += it->second.size(); //num bytes of data
    }
    return static_cast<long>(length);
}


bool sio::lite::FileHeader::userDataFieldExists(const std::string& key) const
{
    return userData.exists(key);
}

void sio::lite::FileHeader::getAllUserDataFields(
    std::vector<std::string>& keys) const
{
    for (sio::lite::UserDataDictionary::ConstIterator p = userData.begin();
        p != userData.end(); ++p)
        keys.push_back(p->first);
}

std::vector<sys::byte>& sio::lite::FileHeader::getUserData(const std::string& key)
{
    if (!userData.exists(key))
        throw except::NoSuchKeyException(key);
    return userData[key];
}


void sio::lite::FileHeader::to(size_t numBands, io::OutputStream& os)
{
    if (numBands <= 0) numBands = 1;
    //compare the input numBands to the elementType

    int elementType = et;
    int elementSize = es;

    if (numBands > 2)
    {
        if (elementType == sio::lite::FileHeader::UNSIGNED)
        {
            elementType = sio::lite::FileHeader::N_BYTE_UNSIGNED;
            elementSize *= static_cast<int>(numBands);
        }
        else if (elementType == sio::lite::FileHeader::SIGNED)
        {
            elementType = sio::lite::FileHeader::N_BYTE_SIGNED;
            elementSize *= static_cast<int>(numBands);
        }
        /*!
         *  To really handle this properly, we need to know
         *  how to lay out the bands for them.  For instance,
         *  If the bands were 'interleaved by pixel' we would
         *  expect that numElems would be multiplied by numBands
         *
         *  For now, we punt...
         */
        else
            nl *= static_cast<int>(numBands);
    }

    if (elementType != sio::lite::FileHeader::UNSIGNED && elementType != sio::lite::FileHeader::SIGNED &&
            elementType != sio::lite::FileHeader::FLOAT && elementType != sio::lite::FileHeader::COMPLEX_UNSIGNED &&
            elementType != sio::lite::FileHeader::COMPLEX_SIGNED && elementType != sio::lite::FileHeader::COMPLEX_FLOAT &&
            elementType != sio::lite::FileHeader::N_BYTE_UNSIGNED && elementType != sio::lite::FileHeader::N_BYTE_SIGNED)
        throw except::Exception(Ctxt("Unknown element type"));

    //update the version based on the user data fields
    version = getNumUserDataFields() > 0 ? 2 : 1;

    //construct the magic byte
    int magic = (255 - version) | 127 << 8 | version << 16 | 255 << 24;

    os.write((const sys::byte*)&magic, 4);
    os.write((const sys::byte*)&nl, 4);
    os.write((const sys::byte*)&ne, 4);
    os.write((const sys::byte*)&elementType, 4);
    os.write((const sys::byte*)&elementSize, 4);

    if (version > 1)
        writeUserData(os);
}

void sio::lite::FileHeader::writeUserData(io::OutputStream& os)
{
    const auto numFields = static_cast<int32_t>(userData.size());
    os.write((const sys::byte*)&numFields, 4);

    for(sio::lite::UserDataDictionary::Iterator it = userData.begin();
            it != userData.end(); ++it)
    {
        std::string key = it->first;
        //add 1 for null-byte termination
        const auto keySize = static_cast<int32_t>(key.length() + 1);
        os.write((const sys::byte*)&keySize, 4);
        os.write((const sys::byte*)key.c_str(), keySize);

        std::vector<sys::byte>& uData = it->second;
        const auto udSize =  static_cast<int32_t>(uData.size());
        os.write((const sys::byte*)&udSize, 4);

        //Do we need to check for endian-ness and possibly byteswap???
        for (std::vector<sys::byte>::iterator iter = uData.begin();
                iter != uData.end(); ++iter)
        {
            sys::byte x = *iter;
            os.write(&x, 1);
        }
    }
}

void sio::lite::FileHeader::addUserData(const std::string& field,
                                        const std::string& data)
{
    const sys::byte* const begin =
        reinterpret_cast<const sys::byte*>(data.c_str());

    const std::vector<sys::byte> vec(begin, begin + data.length());
    userData.add(field, vec);
}


void sio::lite::FileHeader::addUserData(const std::string& field,
                                        const std::vector<sys::byte>& data)
{
    userData.add(field, data);
}

void sio::lite::FileHeader::addUserData(const std::string& field, int data)
{
    std::vector<sys::byte> vec;
    vec.reserve(sizeof(int));
    char* cData = (char*)&data;
    for (int i = 0, size = sizeof(int); i < size; ++i)
        vec.push_back((sys::byte)cData[i]);
    userData.add(field, vec);
}

