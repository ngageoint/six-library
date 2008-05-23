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
 * <b>The User segment object and associated functions provide support
 * for accessing the information in user defined DE segments</b>
 * <p/>
 * The  UserSegment object provides an interface to user application specific
 * header and data in a DE Segment. It provides a framework for the Reader
 * and Writer to manage the access to user defined data in a DES. The object
 * acts in the background and is not explicitly created by the user. Because
 * this object does not contain header information and is used as part of
 * an action (read or write) is does not need a clone method.
 * <p/>
 * Most of the functionality is provided by the type specific functions
 * provided as pointers in the interface object nitf_IUserSegment. The
 * UserSegment object an support is primarily for internal support of the
 * user header and data read/write in the Reader and Writer
 * <p/>
 * The user segment header (not the DES header) is modeled as a TRE and uses
 * the TRE object support functions for creation. A provision is made in the
 * interface for DES that have user headers that cannot be modeled via a TRE
 * description but the result is always represented as a TRE. The header is
 * created by the Reader and stored in the associated DESubheader in the
 * record. There is a provision for direct creation of the user header when
 * the user is creating a new record via assignment.
 * <p/>
 * The tag field in the TRE representing the DE header will be set to "DES".
 * This field is normally the name (tag) of the TRE.
 * <p/>
 * The read mode presents the user data as a flat virtual file with an
 * interface almost identical to the IOHandle interface. Compressed DES data
 * is supported and the virtual file is the uncompressed data. Seeking is
 * allowed in the virtual file, but there could be significant performance
 * penalties for seeking in compressed data.
 * <p/>
 * Writing of user data is controlled via a BandSource. The user creates a
 * BandSource object that is used by the writer to get the user data. The user
 * data I/O model is a single band stream of data.
 * <p/>
 * The reading of DES data is via the nitf_DESReader object which follows the
 * same model as other segment data readers such as nitf_ImageReader.
 * <p/>
 * DES writing follows a model similar to the writing of an image segment. The
 * user creates a Writer and and a DES source. The functions
 * nitf_Writer_newDEWriter and nitf_Writer_attachDESource followed by
 * nitf_Writer_write as with writing an image segment
 * <p/>
 * Note: After careful reading of the NITF specification is was concluded that
 * the user header is part of the DE subheader and its length is part of the
 * subheader length, not part of the data length.
 * <p/>
 * DES segments are managed via handlers that are loaded via the plug-in
 * interface. This interface follows a model similar to decompression plug-ins
 * An init function is called when the plugin is loaded and a clean-up function
 * when the plugin is unloaded. The interface table (defined below) is returned
 * by the getInterface function.
 * <p/>
 * <p/>
 * The object contains fields to facilitate reading and writing. The type data
 * field is provided for support of encoded or compressed types during reads
 * and BandSource instances during write. When the user data can be read
 * directly, the virtual length and offset are actual file values (the most
 * common case). When the type does not read the data directly, the virtual
 * length and pointers refer to the virtual file and are maintained by the
 * type support functions.
 * <p/>
 * The baseOffset is the offset to the user data, just past the header.
 */
public final class UserSegment extends NITFObject
{

    /**
     * @see NITFObject#NITFObject(long)
     */
    UserSegment(long address)
    {
        super(address);
    }

    /**
     * Length of user data
     *
     * @return int
     * @throws NITFException
     */
    public native int getDataLength() throws NITFException;

    /**
     * Offset to start of DES user data
     *
     * @return long
     * @throws NITFException
     */
    public native long getBaseOffset() throws NITFException;

    /**
     * Length of virtual file
     *
     * @return int
     * @throws NITFException
     */
    public native int getVirtualLength() throws NITFException;

    /**
     * Current offset
     *
     * @return long
     * @throws NITFException
     */
    public native long getVirtualOffset() throws NITFException;


    //TODO add access to the interface????

}

