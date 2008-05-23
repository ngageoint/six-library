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

package nitf;

/**
 * Class that has the functionality of reading an image
 */
public final class DEReader extends DestructibleObject
{

    /**
     * @see DestructibleObject#DestructibleObject(long)
     */
    DEReader(long address)
    {
        super(address);
    }

    /**
     * Destroys the underlying object
     */
    protected native synchronized void destructMemory();

    /**
     * Returns the UserSegment object associated with this DEReader
     *
     * @return UserSegment object
     * @throws NITFException
     */
    public native UserSegment getUserSegment() throws NITFException;

    /**
     * The read function reads data from the associated DE segment.
     * The reading of the data is serial as if a flat file were being read by an
     * input stream. Except for the first argument, this function has the same
     * calling sequence and behavior as IOHandle.read().
     *
     * @param buffer Buffer to hold data
     * @param count  Amount of data to return
     * @return true on success
     * @throws NITFException
     * @see IOHandle#read(byte[], int)
     */
    public native boolean read(byte[] buffer, int count) throws NITFException;


    /**
     * The seek function allows the user to seek within the extension
     * data. Except for the first argument, this function has the same calling
     * sequence and behaivior as IOHandle.seek(). The offset is relative to the top
     * of the DES data.
     *
     * @param offset The seek offset
     * @param whence Starting at (IOHandle.SEEK_SET, IOHandle.SEEK_CUR, IOHandle.SEEK_END)
     * @return The offset from the beginning to the current position
     * @throws NITFException
     * @see IOHandle.SEEK_SET
     * @see IOHandle.SEEK_CUR
     * @see IOHandle.SEEK_END
     * @see IOHandle#seek(long, int)
     */
    public native long seek(long offset, int whence) throws NITFException;


    /**
     * The tell function allows the user to determine the current
     * offset within the extension data. Except for the first argument, this
     * function has the same calling sequence and behaivior as IOHandle.tell(). The
     * offset is relative to the top of the DES data.
     *
     * @return The offset from the beginning to the current position
     * @throws NITFException
     * @see IOHandle#tell()
     */
    public native long tell() throws NITFException;


    /**
     * The seek function allows the user to determine the size of
     * the data. Except for the first argument, this function has the same calling
     * sequence and behavior as IOHandle.getSize().
     *
     * @return The offset from the beginning to the current position
     * @throws NITFException
     * @see IOHandle#getSize()
     */
    public native long getSize() throws NITFException;

}

