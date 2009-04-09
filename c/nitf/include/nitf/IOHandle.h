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
 * License along with this program; if not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_IO_HANDLE_H__
#define __NITF_IO_HANDLE_H__

#include "nitf/System.h"
#include "nitf/Error.h"

#   define NITF_IO_SUCCESS(I) ((I) >= 0)

/* How many times should we try to read until we declare the IOHandle dead.  Use a timeout later. */
#define NITF_MAX_READ_ATTEMPTS 100

NITF_CXX_GUARD

/*!
 *  Create an IO handle.  If the file is set to create,
 *  the permissions will be built into the create:
 *  \see System.h
 *
 *  \param fname  The file name
 *  \param access The access: valid values include the or'ing of:
 *                NITF_READONLY
 *                NITF_WRITEONLY
 *                NITF_READWRITE
 *  \param
 *  \param error  The populated error, only if NITF_INVALID_HANDLE()
 *  \return The fresh handle.  Test this with NITF_INVALID_HANDLE()
 */
NITFAPI(nitf_IOHandle) nitf_IOHandle_create(const char *fname,
        nitf_AccessFlags access,
        nitf_CreationFlags creation,
        nitf_Error * error);


/*!
 *  Read from the IO handle.  This function is guaranteed to return
 *  after having read the requisite number of bytes or fail out.
 *  Check its return value against zero or NITF_FAILURE.
 *
 *  \param handle The handle to read from
 *  \param buf    The buffer to read into
 *  \param size   The number of bytes to read
 *  \param error  Populated if function returns 0
 *  \return       1 on success and 0 otherwise
 */
NITFAPI(NITF_BOOL) nitf_IOHandle_read(nitf_IOHandle handle,
                                      char *buf, size_t size,
                                      nitf_Error * error);


/*!
 *  Write to the IO handle.  This function attempts to write to the IO handle
 *  until it has written the requisite number of bytes (specified as the size
 *  parameter).  Upon failure, it returns NITF_FAILURE and the error is set.
 *
 *  \param handle The handle to write from
 *  \param buf    The buffer to write from
 *  \param size   The number of bytes to write
 *  \param error  The error, only if !NITF_IO_SUCCESS()
 *  \return NITF_SUCCESS if the method succeeds, NITF_FAILURE on failure.
 */
NITFAPI(NITF_BOOL) nitf_IOHandle_write(nitf_IOHandle handle,
                                       const char *buf, size_t size,
                                       nitf_Error * error);

/*!
 *  Seek into the handle at this point.  Basically
 *  has the same usage as lseek().  If whence is SEEK_SET, the seek
 *  pointer is to offset bytes.  If SEEK_CUR, start at the
 *  current offset and add offset argument.  If SEEK_END, the seek
 *  is added to the size of the file.  On failure, should return a value
 *  that may be checked with NITF_IO_SUCCESS()
 *
 *  \param handle  The handle to use
 *  \param offset  The seek offset
 *  \param whence  Starting at (SEEK_SET, SEEK_CUR, SEEK_END)
 *  \param error   The error, if !NITF_IO_SUCCESS()
 *  \return The offset from the beginning to the current position
 */
NITFAPI(nitf_Off) nitf_IOHandle_seek(nitf_IOHandle handle,
                                  nitf_Off offset, int whence,
                                  nitf_Error * error);

/*!
 *  Tell the location that the handle is pointing to.  On failure,
 *  this returns a value that may be checked with the NITF_IO_SUCCESS()
 *  macro.
 *
 *  \param handle The io handle
 *  \param error The error to populate if there is a problem
 *  \return The offset
 */
NITFAPI(nitf_Off) nitf_IOHandle_tell(nitf_IOHandle handle,
                                  nitf_Error * error);


/*!
 *  Get the size of the handle (how big is the file).
 *  As with the other methods, this function should return a value
 *  that is testable with NITF_IO_SUCCESS().
 *
 *
 *  \param handle The handle to determine the size of
 *  \param error  A populated error if something goes wrong
 *  \return The size of the file
 */
NITFAPI(nitf_Off) nitf_IOHandle_getSize(nitf_IOHandle handle,
                                     nitf_Error * error);

/*!
 *  Close the IO handle.
 *
 *  \param handle  The handle to close
 *  \return void
 */
NITFAPI(void) nitf_IOHandle_close(nitf_IOHandle handle);

NITF_CXX_ENDGUARD

#endif
