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

import java.io.PrintStream;

/**
 * A representation of the NITF Data Extension subheader
 */
public final class DESubheader extends NITFObject
{

    /**
     * @see NITFObject#NITFObject(long)
     */
    DESubheader(long address)
    {
        super(address);
    }

    /**
     * Returns the File Part Type (DE) Field
     *
     * @return the File Part Type (DE) Field
     */
    public native synchronized Field getFilePartType();

    /**
     * Returns the Unique DES Type Identifer (DESID) Field
     *
     * @return the Unique DES Type Identifer (DESID) Field
     */
    public native synchronized Field getTypeID();

    /**
     * Returns the DES File Security Classification (DECLAS) Field
     *
     * @return the DES File Security Classification (DECLAS) Field
     */
    public native synchronized Field getSecurityClass();

    /**
     * Returns the FileSecurity information for this DES
     *
     * @return the FileSecurity information for this DES
     */
    public native synchronized FileSecurity getSecurityGroup();

    /**
     * Returns the Version of the Data Definition (DESVER) Field
     *
     * @return the Version of the Data Definition (DESVER) Field
     */
    public native synchronized Field getVersion();

    /**
     * Returns the DES Overflowed Header Type (DESOFLW) Field
     *
     * @return the DES Overflowed Header Type (DESOFLW) Field
     */
    public native synchronized Field getOverflowedHeaderType();

    /**
     * Returns the DES Data Item Overflowed (DESITEM) Field
     *
     * @return the DES Data Item Overflowed (DESITEM) Field
     */
    public native synchronized Field getDataItemOverflowed();

    /**
     * Returns the DES User Defined Subheader Length (DESSHL) Field
     *
     * @return the DES User Defined Subheader Length (DESSHL) Field
     */
    public native synchronized Field getSubheaderFieldsLength();

    /**
     * Returns the subheaderFields TRE
     *
     * @return the subheaderFields TRE
     */
    public native synchronized TRE getSubheaderFields()
            throws NITFException;

    /**
     * Returns the length of the DES User Defined Data (DESDATA) segment
     *
     * @return the length of the DES User Defined Data (DESDATA) segment
     */
    public native synchronized long getDataLength();

    /**
     * Sets the length of the data
     *
     * @param dataLength the length of the data
     */
    public native synchronized void setDataLength(long dataLength);

    /**
     * Returns the DES Extended Subheader Data Extensions segment
     *
     * @return the DES Extended Subheader Data Extensions segment
     */
    public native synchronized Extensions getUserDefinedSection();

    /**
     * Prints the data associated with the DESubheader to a PrintStream
     *
     * @param out
     */
    public void print(PrintStream out) throws NITFException
    {
        out.println("FilePartType = [" + getFilePartType() + "]");
        out.println("TypeID = [" + getTypeID() + "]");
        out.println("TypeID = [" + getSecurityClass() + "]");

        getSecurityGroup().print(out);

        out.println("Version = [" + getVersion() + "]");
        out.println(
                "OverflowedHeaderType = [" + getOverflowedHeaderType() + "]");
        out.println("DataItemOverflowed = [" + getDataItemOverflowed() + "]");
        out.println(
                "SubheaderFieldsLength = [" + getSubheaderFieldsLength() + "]");
        out.println("DataLength = [" + getDataLength() + "]");

        //print the TREs, if any
        final Extensions extendedSection = getUserDefinedSection();
        if (extendedSection != null)
        {
            extendedSection.print(out);
        }
    }
}

