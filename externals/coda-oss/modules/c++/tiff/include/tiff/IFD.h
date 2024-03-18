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
#pragma once
#ifndef __TIFF_IFD_H__
#define __TIFF_IFD_H__

#include <map>
#include <string>

#include <import/io.h>
#include <import/except.h>
#include <config/Exports.h>

#include "tiff/IFDEntry.h"
#include "tiff/KnownTags.h"
#include "tiff/TypeFactory.h"

namespace tiff
{

/**
 *********************************************************************
 * @class IFD
 * @brief The TIFF IFD class.  Contains all IFD Entries for an image.
 *
 * This is basically the TIFF footer for every TIFF image.  Each 
 * entry in it further defines the image it is associated with.
 * Contains functions for adding new entries to the IFD or adding
 * values to a specific IFD entry.
 *********************************************************************/
class CODA_OSS_API IFD : public io::Serializable
{
public:
    //! The IFDType
    typedef std::map<unsigned short, tiff::IFDEntry *> IFDType;

    //! Constructor
    IFD() = default;

    //! Deconstructor
    ~IFD()
    {
        for (auto& kv : mIFD)
            delete kv.second;
    }

    /**
     *****************************************************************
     * Retrieves a pointer to the IFDEntry associated with the
     * specified name.
     *
     * @param name
     *   the name to look for in the IFD.
     * @return
     *   the TIFFIFDEntry associated with the specified name, or 
     *   NULL if the entry doesn't exist in the IFD
     *****************************************************************/
    tiff::IFDEntry *operator[](const char *name);
    const tiff::IFDEntry* operator[](const char* name) const;

    tiff::IFDEntry *operator[](const std::string& name)
    {
        return this->operator[](name.c_str());
    }
    const tiff::IFDEntry* operator[](const std::string& name) const
    {
        return this->operator[](name.c_str());
    }

    /**
     *****************************************************************
     * Retrieves a pointer to the IFDEntry associated with the
     * specified tag.
     *
     * @param name
     *   the tag to look for in the IFD.
     * @return
     *   the TIFFIFDEntry associated with the specified tag, or 
     *   NULL if the entry doesn't exist in the IFD
     *****************************************************************/
    tiff::IFDEntry *operator[](unsigned short tag);
    const tiff::IFDEntry* operator[](unsigned short tag) const;

    /**
     * Returns true if an IFDEntry for the given tag exists in the IFD.
     * \return true if the entry exists, otherwise false.
     */
    bool exists(unsigned short tag) const;

    /**
     * Returns true if an IFDEntry with the given name exists in the IFD.
     * \return true if the entry exists, otherwise false.
     */
    bool exists(const char *name) const;

    /**
     * Returns true if an IFDEntry with the given name exists in the IFD.
     * \return true if the entry exists, otherwise false.
     */
    bool exists(const std::string& name) const
    {
        return exists(name.c_str());
    }

    /**
     *****************************************************************
     * Adds the specified IFDEntry to the IFD.  Makes a copy of
     * the IFD entry passed in.
     *
     * @param entry
     *   the IFDEntry to copy into the IFD
     *****************************************************************/
    void addEntry(const tiff::IFDEntry *entry);

    /**
     *****************************************************************
     * Adds an IFDEntry with the specified name to the IFD.  Looks 
     * up the specified name in the KnownTagsSingleton.
     *
     * @param name
     *   the name of the IFDEntry to copy into the IFD
     *****************************************************************/
    void addEntry(const std::string& name);

    /**
     *****************************************************************
     * Adds a IFDEntry with the specified name to the IFD.  Looks 
     * up the specified name in the KnownTags Singleton.  Adds the 
     * specified value to the added IFDEntry.
     *
     * @param name
     *   the name of the IFDEntry to copy into the IFD
     * @param value
     *   the value to add to the IFDEntry
     *****************************************************************/
    template <typename T> void addEntry(const std::string& name, const T& value)
    {
        const tiff::IFDEntry *mapEntry = tiff::KnownTagsRegistry::getInstance()[name];
        //we can't add it if we don't know about it
        if (!mapEntry)
            throw except::Exception(Ctxt(str::Format("Unable to add IFD Entry: unknown tag [%s]", name)));

        const auto id = mapEntry->getTagID();
        const auto type = mapEntry->getType();

        mIFD[id] = new tiff::IFDEntry;
        *(mIFD[id]) = *mapEntry;
        mIFD[id]->addValue(tiff::TypeFactory::create( (unsigned char *)&value,
                type));
    }

    /**
     *****************************************************************
     * Looks up the specified name in the IFD.  If it exists, will
     * add the specified value to the associated IFD entry.
     *
     * @param name
     *   the name of the IFDEntry to copy into the IFD
     * @param value
     *   the value to add to the IFDEntry
     *****************************************************************/
    template <typename T> void addEntryValue(const std::string& name,
            const T value)
    {
        tiff::IFDEntry *entry = (*this)[name.c_str()];
        if (!entry)
            throw except::Exception(Ctxt("IFD entry must exist before adding values"));

        entry->addValue(tiff::TypeFactory::create((unsigned char *)&value,
                entry->getType()));
    }

    /**
     *****************************************************************
     * Writes the complete IFD to the specified output stream.
     *
     * @param output
     *   the output stream to write the IFD to
     *****************************************************************/
    void serialize(io::OutputStream& output) override;

    /**
     *****************************************************************
     * Reads the complete IFD from the specified input stream.
     *
     * @param input
     *   the input stream to read the IFD from
     *****************************************************************/
    void deserialize(io::InputStream& input) override;
    void deserialize(io::InputStream& input, const bool reverseBytes);

    /**
     *****************************************************************
     * Prints the complete IFD out to the specified output stream in
     * a readable format.
     *
     * @param output
     *   the output stream to print the IFD to
     *****************************************************************/
    void print(io::OutputStream& output) const;

    /**
     *****************************************************************
     * Returns the number of IFD entries in the IFD.
     *
     * @return
     *   the number of IFD entries in the IFD
     *****************************************************************/
    sys::Uint32_T size() const
    {
        return static_cast<sys::Uint32_T>(mIFD.size());
    }

    /**
     *****************************************************************
     * Calculates the image size in bytes from entries in the IFD and
     * returns the value.
     *
     * @return 
     *   the calculated image size in bytes
     *****************************************************************/
    sys::Uint32_T getImageSize() const;

    /**
     *****************************************************************
     * Calculates the image width in elements from entries in the 
     * IFD and returns the value.
     *
     * @return 
     *   the calculated image width in elements
     *****************************************************************/
    sys::Uint32_T getImageWidth() const;

    /**
     *****************************************************************
     * Calculates the image length in lines from entries in the 
     * IFD and returns the value.
     *
     * @return 
     *   the calculated image length in lines
     *****************************************************************/
    sys::Uint32_T getImageLength() const;

    /**
     *****************************************************************
     * Calculates the element size in bytes from entries in the 
     * IFD and returns the value.
     *
     * @return 
     *   the calculated element size in bytes
     *****************************************************************/
    unsigned short getElementSize() const;
    
    
    unsigned short getNumBands() const;

    /**
     *****************************************************************
     * Returns the file offset where the offset to the next IFD can 
     * be written.
     *
     * @return
     *   the offset to write the next IFD offset to
     *****************************************************************/
    sys::Uint32_T getNextIFDOffsetPosition()
    {
        return mNextIFDOffsetPosition;
    }

private:

    /**
     *****************************************************************
     * Finalizes all of the IFD entries.  Calculates the file offsets
     * of every IFD entry that has overflow data and sets that offset
     * into each entry.
     *
     * @param offset
     *   the file offset that indicates the beginning position of 
     *   the IFD.
     * @return
     *   the highest overflow offset calculated, this marks the
     *   potential beginning of the next image.
     *****************************************************************/
    sys::Uint32_T finalize(const sys::Uint32_T offset);

    //! The IFD entries
    IFDType mIFD;
    IFDType ifd() const
    {
        return mIFD;
    }

    //! Offset where the next IFD offset can be written to
    sys::Uint32_T mNextIFDOffsetPosition = 0;
};

} // End namespace.

#endif // __TIFF_IFD_H__
