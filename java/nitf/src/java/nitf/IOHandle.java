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
 * A representation of a handle to a file object. <p/> This class provides
 * access to a native file object, so it does not use the Java io library.
 */
public final class IOHandle extends NITFObject
{
    /**
     * Creates a new file
     */
    public static final int NITF_CREATE = 10;

    /**
     * Truncates the file
     */
    public static final int NITF_TRUNCATE = 11;

    /**
     * Opens an existing file
     */
    public static final int NITF_OPEN_EXISTING = 12;

    /**
     * Opens the file with read-only privileges
     */
    public static final int NITF_ACCESS_READONLY = 0x01;

    /**
     * Opens the file with write-only privileges
     */
    public static final int NITF_ACCESS_WRITEONLY = 0x02;

    /**
     * Opens the file for reading and/or writing
     */
    public static final int NITF_ACCESS_READWRITE = 0x03;

    /**
     * Seek offset is relative to current position
     */
    public static final int SEEK_CUR = 10;

    /**
     * Seek offset is relative to start of file
     */
    public static final int SEEK_SET = 20;

    /**
     * Seek offset is relative to end of file
     */
    public static final int SEEK_END = 30;

    private String fileName;

    private int accessFlag;

    private int creationFlag;

    IOHandle(long address)
    {
        super(address);
    }

    synchronized long getIOHandle()
    {
        return getAddress();
    }

    synchronized void setIOHandle(long ioHandle)
    {
        setAddress(ioHandle);
    }

    /**
     * Creates a new IOHandle object for READING, using the file referenced from
     * a parent pathname string; This sets the access flag to
     * NITF_ACCESS_READONLY and the creation flag to NITF_OPEN_EXISTING
     * 
     * @param fileName
     *            the path of the file associated with this handle
     * @throws NITFException
     */
    public IOHandle(String fileName) throws NITFException
    {
        this(fileName, NITF_ACCESS_READONLY, NITF_OPEN_EXISTING);
    }

    /**
     * Creates a new IOHandle object, using the file referenced from a parent
     * pathname string and an access modifier.
     * 
     * @param fileName
     *            the path of the file associated with this handle
     * @param accessFlag
     *            options are <code>NITF_ACCESS_READONLY<code>,
     *                     <code>NITF_ACCESS_WRITEONLY</code>, or <code>NITF_ACCESS_READWRITE</code>
     * @param creationFlag options are <code>NITF_CREATE<code>,
     *                     <code>NITF_TRUNCATE</code>, or <code>NITF_OPEN_EXISTING</code>
     * @throws NITFException
     */
    public IOHandle(String fileName, int accessFlag, int creationFlag)
            throws NITFException
    {
        this.fileName = fileName;

        if (accessFlag != NITF_ACCESS_READONLY
                && accessFlag != NITF_ACCESS_READWRITE
                && accessFlag != NITF_ACCESS_WRITEONLY)
        {
            throw new NITFException(
                    "Access flag must be a valid NITF_ACCESS flag");
        }
        if (creationFlag != NITF_CREATE && creationFlag != NITF_TRUNCATE
                && creationFlag != NITF_OPEN_EXISTING)
        {
            throw new NITFException(
                    "Creation flag must be a valid NITF Creation flag");
        }

        this.accessFlag = accessFlag;
        this.creationFlag = creationFlag;
        long ioHandle = createHandle(fileName, accessFlag, creationFlag);
        setAddress(ioHandle);
        NITFResourceManager.getInstance().trackIOHandle(this);
    }

    /**
     * Reads size bytes into the specified byte buffer
     * 
     * @param buf
     *            the byte buffer to store the data
     * @param size
     *            the amount to read
     * @throws NITFException
     */
    public native synchronized void read(byte[] buf, int size)
            throws NITFException;

    /**
     * Writes size bytes to the file at the current position
     * 
     * @param buf
     *            the byte buffer containing the data
     * @param size
     *            the amount to write
     * @throws NITFException
     */
    public native synchronized void write(byte[] buf, int size)
            throws NITFException;

    /**
     * Seeks to the specified offset relative to the position specified by
     * whence
     * 
     * @param offset
     *            the offset in the file
     * @param whence
     *            the type of seek, either SEEK_CUR, SEEK_SET, or SEEK_END
     * @return the position in the file after the seek
     * @throws NITFException
     */
    public native synchronized long seek(long offset, int whence)
            throws NITFException;

    /**
     * Returns the current file pointer position
     * 
     * @return the current file pointer position
     * @throws NITFException
     */
    public native synchronized long tell() throws NITFException;

    /**
     * Returns the size of the file descriptor
     * 
     * @return the size of the file descriptor
     * @throws NITFException
     */
    public native synchronized long getSize() throws NITFException;

    /**
     * Closes the file descriptor. Good implementation should always end by
     * calling this.
     */
    public native synchronized void close();

    /**
     * Return the name of the file associated with this handle
     * 
     * @return the name of the file associated with this handle
     */
    public String getFileName()
    {
        return fileName;
    }

    /**
     * Native function used internally to create an file handle
     * 
     * @param fileName
     * @param accessFlag
     * @param creationFlag
     * @return
     * @throws NITFException
     */
    private native synchronized long createHandle(String fileName,
            int accessFlag, int creationFlag) throws NITFException;
}
