/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_READER_H__
#define __NITF_READER_H__

#include "nitf/System.h"
#include "nitf/PluginRegistry.h"
#include "nitf/DefaultTRE.h"
#include "nitf/Record.h"
#include "nitf/FieldWarning.h"
#include "nitf/ImageReader.h"
#include "nitf/SegmentReader.h"

NITF_CXX_GUARD

/*!
 *  \struct nitf_Reader
 *  \brief  This object represents the 2.1 file reader
 *
 *  This reader class contains the
 *
 */
typedef struct _nitf_Reader
{
    nitf_List *warningList;
    nitf_IOInterface* input;
    nitf_Record *record;
    NITF_BOOL ownInput;

}
nitf_Reader;


/*!
 *  Construct a new reader
 *  \param error A populated structure upon failure
 *  \return A reader, or NULL on failure
 */
NITFAPI(nitf_Reader *) nitf_Reader_construct(nitf_Error * error);

/*!
 *  Destroy a reader, set *reader to NULL
 *  \param reader The reader to NULL-set
 */
NITFAPI(void) nitf_Reader_destruct(nitf_Reader ** reader);

/*!
 *  This is the method for reading information from a NITF (or NSIF).  It
 *  reads all of the support data, including TREs, which it parses
 *  if it knows of the appropriate location.  The output is a record
 *  of the information in the NITF.
 *
 *  After this operation is completed, a user will typically create
 *  a new ImageReader using the method in this class in order to
 *  read the imagery from an appropriate sub-window.
 *
 *  \param reader The reader object
 *  \param io The file io
 *  \param error A populated error if return value is zero
 *  \return A dynamically allocated record pointing to the internal
 *  support data in the NITF file
 */
NITFAPI(nitf_Record *) nitf_Reader_read(nitf_Reader * reader,
                                        nitf_IOHandle inputHandle,
                                        nitf_Error * error);

/*!
 *  Same as the read function, except this method allows you to change
 *  the underlying interface.  The read method calls this one using an
 *  IOInterface adapter.
 */
NITFAPI(nitf_Record *) nitf_Reader_readIO(nitf_Reader* reader,
                                          nitf_IOInterface* io,
                                          nitf_Error* error);


/*!
 * This creates a new ImageReader object that can be used to access the
 * data in the image segment.  This should be done after the read()
 * function has been called and a record loaded.
 * 
 * The ImageReader object is a specialized reader for dealing with images. It is
 * provided by the library, and should almost always be used when reading
 * images.
 *
 *  \param reader The reader object
 *  \param imageSegmentNumber The index
 *  \param error A populated error if return value is zero
 *  \return new nitf_ImageReader* for the image in question
 */
NITFAPI(nitf_ImageReader *) nitf_Reader_newImageReader
(
    nitf_Reader *reader,
    int imageSegmentNumber,
    nitf_Error * error
);


/*!
 *  This creates a new SegmentReader object that can be used to access the
 *  data in the text segment.
 *
 *  \param reader The reader object
 *  \param textSegmentNumber The index
 *  \param error A populated error if return value is zero
 *  \return new nitf_SegmentReader* for the segment in question
 */
NITFAPI(nitf_SegmentReader*) nitf_Reader_newTextReader
(
    nitf_Reader *reader,
    int textSegmentNumber,
    nitf_Error * error
);


/*!
 *  This creates a new SegmentReader object that can be used to access the
 *  data in the graphic segment.
 *
 *  \param reader The reader object
 *  \param graphicSegmentNumber The index
 *  \param error A populated error if return value is zero
 *  \return new nitf_SegmentReader* for the segment in question
 */
NITFAPI(nitf_SegmentReader*) nitf_Reader_newGraphicReader
(
    nitf_Reader *reader,
    int graphicSegmentNumber,
    nitf_Error * error
);

/*!
 *  This creates a new DEReader object that can be used to access the
 *  data in the DE segment.
 *
 *  \param reader The reader object
 *  \param DESegmentNumber The index
 *  \param error A populated error if return value is zero
 *  \return new nitf_DEReader* for the segment in question
 */
NITFAPI(nitf_SegmentReader *) nitf_Reader_newDEReader
(
    nitf_Reader *reader,
    int DESegmentNumber,
    nitf_Error * error
);


/*!
 * Return the NITFVersion of the file passed in by its file name.
 * This is a static method (not associated with a specific Reader).
 * \param fileName The name of the file to check
 * \return NITFVersion of the file
 */
NITFAPI(nitf_Version) nitf_Reader_getNITFVersion(const char* fileName);


NITF_CXX_ENDGUARD

#endif
