/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_USER_SEGMENT_H__
#define __NITF_USER_SEGMENT_H__

#include "nitf/System.h"
#include "nitf/Error.h"
#include "nitf/Record.h"
#include "nitf/BandSource.h"
#include "nitf/DefaultUserSegment.h"


NITF_CXX_GUARD

/*    Forward references */

struct _nitf_IUserSegment;
struct _nitf_UserSegment;

/*!
   \file UserSegment - User data access object for DE Segment

  \brief The User segment object and associated functions provide support
  for accessing the information in user defined DE segments

  The  UserSegment object provides an interface to user application specific
  header and data in a DE Segment. It provides a framework for the Reader
  and Writer to manage the access to user defined data in a DES. The object
  acts in the background and is not explicitly created by the user. Because
  this object does not contain header information and is used as part of
  an action (read or write) is does not need a clone method.

  Most of the functionality is provided by the type specific functions
  provided as pointers in the interface object nitf_IUserSegment. The
  UserSegment object an support is primarily for internal support of the
  user header and data read/write in the Reader and Writer

  The user segment header (not the DES header) is modeled as a TRE and uses
  the TRE object support functions for creation. A provision is made in the
  interface for DES that have user headers that cannot be modeled via a TRE
  description but the result is always represented as a TRE. The header is
  created by the Reader and stored in the associated DESubheader in the
  record. There is a provision for direct creation of the user header when
  the user is creating a new record via assignment.

  The tag field in the TRE representing the DE header will be set to "DES".
  This field is normally the name (tag) of the TRE.

  The read mode presents the user data as a flat virtual file with an
  interface almost identical to the IOHandle interface. Compressed DES data
  is supported and the virtual file is the uncompressed data. Seeking is
  allowed in the virtual file, but there could be significant performance
  penalties for seeking in compressed data.

  Writing of user data is controlled via a BandSource. The user creates a
  BandSource object that is used by the writer to get the user data. The user
  data I/O model is a single band stream of data.

  The reading of DES data is via the nitf_DESReader object which follows the
  same model as other segment data readers such as nitf_ImageReader.

  DES writing follows a model similar to the writing of an image segment. The
  user creates a Writer and and a DES source. The functions
  nitf_Writer_newDEWriter and nitf_Writer_attachDESource followed by
  nitf_Writer_write as with writing an image segment

  Note: After careful reading of the NITF specification is was concluded that
  the user header is part of the DE subheader and its length is part of the
  subheader length, not part of the data length.

  DES segments are managed via handlers that are loaded via the plug-in
  interface. This interface follows a model similar to decompression plug-ins
  An init function is called when the plugin is loaded and a clean-up function
  when the plugin is unloaded. The interface table (defined below) is returned
  by the getInterface function.
*/

/*!
  \brief NITF_IUSER_SEGMENT_CREATE_HEADER_FUNCTION - Create user header
  function prototype

  The create header function is called by the Reader when reading the Record
  object.

  \param iface     The interface for the associated DE type
  \param record    The Record object associated with the segment
  \param subhdr    The associated DESubheader
  \param buffer    The header as raw data
  \param error     The error object for error reports

  \return The header TRE or NULL is returned. On error, the error object is
  set
*/

typedef nitf_TRE * (*NITF_IUSER_SEGMENT_CREATE_HEADER_FUNCTION)(
    struct _nitf_IUserSegment *iface, nitf_Record *record,
    nitf_DESubheader *subhdr, NITF_DATA *data, nitf_Error *error);

/*!
  \brief NITF_IUSER_SEGMENT_INSTANCE_SETUP_FUNCTION - User type specific
  instance data setup function prototype

  The instance setup function is called by the UserSegment object constructor.
  to allocate and initialize the instance data (data field). The object can
  be setup for read or write.

  \param user      The associated UserSegment object
  \param record    The Record object associated with the segment
  \param des       The associated DESegment
  \param forWrite  The setup for write if TRUE
  \param error     The error object for error reports

  \return TRUE is returned on success. On error, the error object is set
*/

typedef NITF_BOOL  (*NITF_IUSER_SEGMENT_INSTANCE_SETUP_FUNCTION)(
    struct _nitf_UserSegment *user, nitf_Record *record,
    nitf_DESegment *des, NITF_BOOL forWrite, nitf_Error *error);

/*!
  \brief NITF_IUSER_SEGMENT_READ_FUNCTION - Type specific user data read
  function prototype

  The nitf_DEReader_read function reads data from the associated DE segment.
  This function is implemented when the DE segment user data cannot be read
  directly. For example, if it is compressed

  Except for the first argument, this function has the same calling sequence
  as IOHandle_read.

  \param user,                 - Associated UserSegment
  \param buffer                - Buffer to hold data
  \param count                 - Amount of error to return
  \param error                 - For error returns

  \return The number of bytes read or -1 on error. On error, the error object
  is set.
*/

typedef size_t (*NITF_IUSER_SEGMENT_READ_FUNCTION)(
    struct _nitf_UserSegment *user, NITF_DATA *buffer,
    size_t count, nitf_Error *error);

/*!
  \brief NITF_IUSER_SEGMENT_SEEK_FUNCTION - Type specific user data seek
  function prototype

  The nitf_DEReader_read function seeks the virtual file in the associated
  DE segment. This function is implemented when the DE segment user data
  cannot be accessed read directly. For example, if it is compressed.

  Except for the first argument, this function has the same calling sequence
  as IOHandle_seek.

  The returned value maybe tested with NITF_IO_SUCCESS()

  \param user,                 - Associated UserSegment
  \param offset           - The seek offset
  \param whence           - Starting at (SEEK_SET, SEEK_CUR, SEEK_END)
  \param error            - For error returns

   \return The offset from the beginning to the current position is set.
*/

typedef off_t (*NITF_IUSER_SEGMENT_SEEK_FUNCTION)(
    struct _nitf_UserSegment *user, off_t offset,
    int whence, nitf_Error *error);


/*!
  \brief NITF_IUSER_SEGMENT_SOURCE_CONSTRUCT_FUNCTION - User source construct
  function prototype

  The source construct function is called by the UserSegment source construct
  function which should be called by the user as part of the preparation for
  writing the DES. The associated Record object is required in case there are
  any dependencies to fields in other segments.

  This function operates on the UserSegment object setting up whatever
  is required for supplying the data. The actual BandSource object is
  not required.

  \param user      The associated UserSegment object
  \param record    The Record object associated with the segment
  \param des       The associated DESegment
  \param error     The error object for error reports

  \return True is returned on success. On error, the error object is set
*/

typedef NITF_BOOL (*NITF_IUSER_SEGMENT_SOURCE_CONSTRUCT_FUNCTION)(
    struct _nitf_UserSegment *user, nitf_Record *record,
    nitf_DESegment *des, nitf_Error *error);

/*!
  \brief NITF_IUSER_SEGMENT_WRITE_HEADER_FUNCTION - Write user header
  function prototype

  The write header function is called by the Writer to write the user
  header for the DES. The associated Record object is required in case
  there are any dependencies to fields in other segments.

  The supplied IOHandle can be positioned anywhere in the file. On completion
  it may be positioned anywhere.

  \param user      The associated UserSegment object
  \param record    The Record object associated with the segment
  \param out       The output IOHandle
  \param error     The error object for error reports

  \return TRUE is returned on success. On error, the error object is set
*/

typedef NITF_BOOL  (*NITF_IUSER_SEGMENT_WRITE_HEADER_FUNCTION)(
    struct _nitf_UserSegment *user, nitf_Record *record,
    nitf_IOHandle *out, nitf_Error *error);

/*!
  \brief NITF_IUSER_SEGMENT_DESTRUCT_FUNCTION - DES type specific destructor

  The DES type specific destructor destroys the user instance data. This
  function is only called if the setup creates user instance data (if the
  typeData field is not NULL)

  \param user      The associated UserSegment object

  \return None
*/

typedef void (*NITF_IUSER_SEGMENT_DESTRUCT_FUNCTION)(
    struct _nitf_UserSegment *user);

/*!
  \brief nitf_IUserSegment - User type implementation interface

  nitf_IUserSegment user DES type implementation interface is a structure of
  function pointers and other data that provide user specific functionality
  need to implement the generic user segment operations.

  The id and description string are provided to facilitate error messages
  and prints.

  The createHeader function is called to create the user header. For normal
  DES this pointer is NULL and the TRE description and TRE object functions
  are used. The pointer and the TRE description can both be non-NULL but
  normally the createHeader is NULL and the description is non-NULL.

  The instanceSetup function is called to create and initialize the type
  instance data (typeData field). Type data is normally only needed when
  the virtual file is not the actual file as in a compressed type. This data
  is freed by the user type destructor.

  The sourceConstruct function creates a BandSource object which is used for
  writing the user data.

  The read and seek function support user data read for DES that cannot be
  read direct reads, when the virtual file is not the real file. An example
  would be a DES with compressed data.

  The writeHeader function writes the header. It is for non-standard DESs
  and is normally NULL.

  The destuct function destroys the user instance data. The destructor should
  free all allocated instance data and set the typeData field to NULL

  The interface structure should be static. It is not destroyed with the
  object. The embedded band source interface should also be static and is
  used to create the source object for writing. The source (BandSource)
  object's data is the associate UserSegment object.

  The internal data pointer is provided for the handle to use to store a
  pointer to global data generated at at handler init. Examples might be
  look-up tables used to support the processing of a decompressor.
*/

typedef struct _nitf_IUserSegment
{
    char *description;  /*!< DES descriptive name */
    nitf_TREDescription *
    headerDesc;       /*!< Header TRE description */
    NITF_IUSER_SEGMENT_CREATE_HEADER_FUNCTION
    createHeader;     /*!< Create user header */
    NITF_IUSER_SEGMENT_INSTANCE_SETUP_FUNCTION
    instanceSetup;    /*!< Allocate and initialize instance data */
    NITF_IUSER_SEGMENT_READ_FUNCTION
    read;             /*!< Read virtual file */
    NITF_IUSER_SEGMENT_SEEK_FUNCTION
    seek;             /*!< Seek in virtual file */
    NITF_IUSER_SEGMENT_SOURCE_CONSTRUCT_FUNCTION
    sourceConstruct;  /*!< Create user data source */
    NITF_IUSER_SEGMENT_WRITE_HEADER_FUNCTION
    writeHeader;      /*!< Write user header */
    NITF_IUSER_SEGMENT_DESTRUCT_FUNCTION
    destruct;         /*!< User specific destructor */
    nitf_IDataSource *
    bandImpl;         /*!< BandSource implementation */
    void *internal;     /*!< Handler internal data */
}
nitf_IUserSegment;

/*!
  \brief nitf_UserSegment - User segment object

  nitf_UserSegment is the instance data for the UserSegment object.

  The object contains fields to facilitate reading and writing. The type data
  field is provided for support of encoded or compressed types during reads
  and BandSource instances during write. When the user data can be read
  directly, the virtual length and offset are actual file values (the most
  common case). When the type does not read the data directly, the virtual
  length and pointers refer to the virtual file and are maintained by the
  type support functions.

  The baseOffset is the offset to the user data, just past the header.
*/

typedef struct _nitf_UserSegment
{
    nitf_IUserSegment *iface;  /*!< User type implementation interface */
    void *typeData;            /*!< Type instance data */
    nitf_Uint32 dataLength;       /*!< Length of user data */
    nitf_Uint64 baseOffset;    /*!< Offset to start of DES user data */
    nitf_Uint32 virtualLength;    /*!< Length of virtual file */
    nitf_Uint64 virtualOffset; /*!< Current offset */
}
nitf_UserSegment;

/*!
  \brief nitf_UserSegment_getInterface - Get type interface

  nitf_UserSegment_getInterface gets the type interface given the DE type
  ID. This function isolates the details of how a DE ID string is converted
  to an interface.

  Note: This is not a constructor the returned interface pointer is to a
  static thing.

  The return can be NULL for two reasons, there is no plug-in or there was
  an error. If there was an error, then bad returns true.

  \return Returns a pointer to the interface or NULL.
*/

NITFAPI(nitf_IUserSegment *) nitf_UserSegment_getInterface
(
    char *desID,                  /*!< DE ID string */
    int *bad,                     /*!< Failure flag */
    nitf_Error *error             /*!< For error returns */
);

/*!
  \brief nitf_UserSegment_decodeUserHeader - Decode DE segment user header from
  record

  nitf_UserSegment_decodeUserHeader is called by the Reader during the read
  of the record. It can also be called directly by the user if a new record
  is being created and intitialized directly.

  The record object is required in case the DES definition makes the header
  dependent, in some way, on another part of the file. For example, if there
  is a variable length field whose length is dependent on the number of bands
  in the first image segment but the band count is not included in the DES
  header.

  The supplied DESegment object should be initialized along with its subheader.
  All fields should be filled except the subheaderFields field which will be
  set.

  If the buffer argument is NULL, the TRE will have the correct description
  but no values

  \return The new TRE Or NULL. On error, the error object is set.
*/

NITFAPI(nitf_TRE *) nitf_UserSegment_decodeUserHeader
(
    nitf_Record *record,      /*!< Associated Record object */
    nitf_DESubheader *subhdr, /*!< Associated DES subheader */
    NITF_DATA *buffer,        /*!< The header as raw data */
    nitf_Error *error         /*!< For error returns */
);

/*!
  \brief nitf_UserSegment_construct - Constructor for UserSegment object

  The UserSegment constructor is called internally as part of the read or
  write setup process. It will be called after record has been constructed
  and populated (by reading, cloning, or direct user set-up).

  \return The new UserSegment object or Error. On error, the supplied
  error object is set
*/

NITFAPI(nitf_UserSegment *) nitf_UserSegment_construct
(
    nitf_Record *record,      /*!< Associated Record object */
    nitf_DESegment *des,      /*!< Associated DES */
    NITF_BOOL forWrite,       /*!< Construct for write if TRUE */
    nitf_Error *error         /*!< For error returns */
);

/*!
  \brief nitf_UserSegment_Source_construct - Create data source for write

  nitf_UserSegment_Source_construct creates a data source for write. The
  source is a BandSource object that is used to write the DES data to disk

  \return Returns a BandSource object or NULL. On error, the error object
  is set
*/

NITFPROT(nitf_DataSource *)  nitf_UserSegment_constructSource
(
    nitf_UserSegment *user,   /*!< Associated user segment */
    nitf_Record *record,      /*!< Associated Record object */
    nitf_DESegment *des,      /*!< Associated DES */
    nitf_Error *error         /*!< For error reports */
);

/*!
  nitf_UserSegment_destruct - Destructor for user segment object

  The destructor does not free the interface structure which should be static

  \return None
*/

NITFAPI(void) nitf_UserSegment_destruct
(
    nitf_UserSegment **user      /*!< Associated user segment */
);

NITF_CXX_ENDGUARD

#endif
