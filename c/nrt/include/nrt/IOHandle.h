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
 * License along with this program; if not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NRT_IO_HANDLE_H__
#define __NRT_IO_HANDLE_H__

#include "nrt/System.h"

#   define NRT_IO_SUCCESS(I) ((I) >= 0)

/* How many times should we try to read until we declare the IOHandle dead.  Use a timeout later. */
#ifndef NITF_MAX_READ_ATTEMPTS
#define NRT_MAX_READ_ATTEMPTS 100
#endif

NRT_CXX_GUARD
/*!
 *  Create an IO handle.  If the file is set to create,
 *  the permissions will be built into the create:
 *  \see System.h
 *
 *  \param fname  The file name
 *  \param access The access: valid values include the or'ing of:
 *                NRT_READONLY
 *                NRT_WRITEONLY
 *                NRT_READWRITE
 *  \param
 *  \param error  The populated error, only if NRT_INVALID_HANDLE()
 *  \return The fresh handle.  Test this with NRT_INVALID_HANDLE()
 */
NRTAPI(nrt_IOHandle) nrt_IOHandle_create(const char *fname,
                                         nrt_AccessFlags access,
                                         nrt_CreationFlags creation,
                                         nrt_Error * error);

/*!
 *  Read from the IO handle.  This function is guaranteed to return
 *  after having read the requisite number of bytes or fail out.
 *  Check its return value against zero or NRT_FAILURE.
 *
 *  \param handle The handle to read from
 *  \param buf    The buffer to read into
 *  \param size   The number of bytes to read
 *  \param error  Populated if function returns 0
 *  \return       1 on success and 0 otherwise
 */
NRTAPI(NRT_BOOL) nrt_IOHandle_read(nrt_IOHandle handle, char *buf, size_t size,
                                   nrt_Error * error);

/*!
 *  Write to the IO handle.  This function attempts to write to the IO handle
 *  until it has written the requisite number of bytes (specified as the size
 *  parameter).  Upon failure, it returns NRT_FAILURE and the error is set.
 *
 *  \param handle The handle to write from
 *  \param buf    The buffer to write from
 *  \param size   The number of bytes to write
 *  \param error  The error, only if !NRT_IO_SUCCESS()
 *  \return NRT_SUCCESS if the method succeeds, NRT_FAILURE on failure.
 */
NRTAPI(NRT_BOOL) nrt_IOHandle_write(nrt_IOHandle handle, const char *buf,
                                    size_t size, nrt_Error * error);

/*!
 *  Seek into the handle at this point.  Basically
 *  has the same usage as lseek().  If whence is SEEK_SET, the seek
 *  pointer is to offset bytes.  If SEEK_CUR, start at the
 *  current offset and add offset argument.  If SEEK_END, the seek
 *  is added to the size of the file.  On failure, should return a value
 *  that may be checked with NRT_IO_SUCCESS()
 *
 *  \param handle  The handle to use
 *  \param offset  The seek offset
 *  \param whence  Starting at (SEEK_SET, SEEK_CUR, SEEK_END)
 *  \param error   The error, if !NRT_IO_SUCCESS()
 *  \return The offset from the beginning to the current position
 */
NRTAPI(nrt_Off) nrt_IOHandle_seek(nrt_IOHandle handle, nrt_Off offset,
                                  int whence, nrt_Error * error);

/*!
 *  Tell the location that the handle is pointing to.  On failure,
 *  this returns a value that may be checked with the NRT_IO_SUCCESS()
 *  macro.
 *
 *  \param handle The io handle
 *  \param error The error to populate if there is a problem
 *  \return The offset
 */
NRTAPI(nrt_Off) nrt_IOHandle_tell(nrt_IOHandle handle, nrt_Error * error);

/*!
 *  Get the size of the handle (how big is the file).
 *  As with the other methods, this function should return a value
 *  that is testable with NRT_IO_SUCCESS().
 *
 *
 *  \param handle The handle to determine the size of
 *  \param error  A populated error if something goes wrong
 *  \return The size of the file
 */
NRTAPI(nrt_Off) nrt_IOHandle_getSize(nrt_IOHandle handle, nrt_Error * error);

/*!
 *  Close the IO handle.
 *
 *  \param handle  The handle to close
 *  \return void
 */
NRTAPI(void) nrt_IOHandle_close(nrt_IOHandle handle);

NRT_CXX_ENDGUARD
#endif
