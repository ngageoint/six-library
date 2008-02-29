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
 * A representation of the NITF Tagged Record Extensions (TRE)
 */
public final class TRE extends DestructibleObject
{

    /**
     * Specifies that the length of the TRE should be computed by the library, and not specified by the user at
     * construction time
     */
    public static final int DEFAULT_LENGTH = 0;

    /**
     * Creates a new TRE of the given type
     * 
     * @param tag
     *            the type of TRE to create
     * @throws NITFException
     *             if tag is an unkown type
     */
    public TRE(String tag) throws NITFException
    {
        this(tag, DEFAULT_LENGTH);
    }

    /**
     * Creates a new TRE of the given type, with the given length
     * 
     * @param tag
     *            the type of TRE to create
     * @param size
     *            the size of the TRE, or DEFAULT_LENGTH
     * @throws NITFException
     *             if tag is an unkown type
     */
    public TRE(String tag, int size) throws NITFException
    {
        this(tag, size, null);
    }

    /**
     * Creates a new TRE of the given type, and uses the given TREDescription[] for the description of the TRE. This
     * should only be used if you have created a valid description. Passing in null for the description is the same as
     * calling the TRE(String tag) constructor.
     * 
     * @param tag
     *            the type of TRE to create
     * @param description
     *            TREDescription
     * @throws NITFException
     */
    public TRE(String tag, int size, TREDescription[] description)
            throws NITFException
    {
        if (!PluginRegistry.canHandleTRE(tag))
            throw new NITFException(
                    "TRE Handler cannot be found for this TRE: " + tag);

        if (!construct(tag, size, description))
        {
            throw new NITFException("Could not create TRE with tag: [" + tag
                    + "]");
        }
    }

    /**
     * @see NITFObject#NITFObject(long)
     */
    TRE(long address)
    {
        super(address);
    }

    protected native synchronized void destructMemory();

    private native boolean construct(String tag, int size,
            TREDescription[] description) throws NITFException;

    /**
     * Returns the length of this TRE
     * 
     * @return
     */
    public native synchronized int getLength();

    /**
     * Sets the length of the TRE
     * 
     * @param length
     * @throws NITFException
     */
    public native void setLength(int length) throws NITFException;

    /**
     * Computes the current length of the TRE by iterating over it
     * 
     * @return
     */
    public native synchronized int computeLength();

    /**
     * Returns the TREDescription array for this TRE
     * 
     * @return
     */
    public native synchronized TREDescription[] getTREDescription();

    /**
     * Returns the TRE identifier tag
     * 
     * @return the TRE identifier tag
     */
    public native synchronized String getTag();

    /**
     * Returns true if the given field tag exists within the TRE The tag is the identifier for the field. Check out the
     * TRE Descriptions for a listing of the field tags for each TRE. Note - Fields that loop will have a tag TAG[i],
     * such that i is the number in the loop, starting with 0
     * 
     * @param tag
     *            the identifier for the field
     * @return true if the field exists, false otherwise
     */
    public native synchronized boolean exists(String tag);

    /**
     * Returns the Field associated with the given tag
     * 
     * @param tag
     *            the identifier for the field
     * @throws NITFException
     *             if an error occurs or no field with with the given tag exists
     */
    public native synchronized Field getField(String tag) throws NITFException,
            NoSuchFieldException;

    /**
     * Attempts to set the value of the field referenced by tag to the data given. Throws an NITFException if an error
     * occurs.
     * 
     * @param tag
     *            the identifier for the field
     * @param data
     *            the data to use
     * @throws NITFException
     *             if an error occurs
     */
    public native synchronized boolean setField(String tag, byte[] data)
            throws NITFException;

    /**
     * Returns true if the TRE is sane, i.e. it contains all of the values that should be there, based on the
     * description of the TRE. If it is not sane, it returns false
     * 
     * @return true if the TRE is sane, false otherwise
     */
    public native synchronized boolean isSane();

    /**
     * Prints the contents of the TRE to the given PrintStream
     * 
     * @param stream
     *            PrintStream to print to
     * @throws NITFException
     */
    public synchronized void print(PrintStream stream) throws NITFException
    {
        stream.println("\n---------------" + getTag() + "---------------");
        final TREField[] fields = getFields();
        for (int i = 0; i < fields.length; i++)
        {
            TREField field = fields[i];

            stream
                    .println(field.getDescription().getTag()
                            + " ("
                            + field.getDescription().getLabel()
                            + ") = ["
                            + (field.getField().getType() == FieldType.NITF_BINARY ? ("<binary stuff, length = "
                                    + field.getField().getLength() + ">")
                                    : field.getField().toString()) + "]");
        }
        stream.println("------------------------------------");
    }

    /**
     * Attempts to re-set the description to the one provided by the plugin for the given tag This function also sets
     * the tag for the tre to the parameter
     * 
     * @param tag
     *            the main TRE identifier
     * @param description
     *            OPTIONAL description to set it to
     * @return true if success, false otherwise
     */
    // private native synchronized boolean setDescription(String tag,
    // TREDescription[] description);
    /**
     * Return all of the Fields that are a part of the TRE, in order
     * 
     * @return Field array
     * @throws NITFException
     */
    public native synchronized TREField[] getFields() throws NITFException;

    /**
     * Contains an association between a TRE Field and Description
     */
    public static class TREField
    {
        protected TREDescription description = null;

        protected Field field = null;

        protected String qualifiedTag = null;

        protected TREField(Field field, TREDescription description)
        {
            this.description = description;
            this.field = field;
        }

        /**
         * Returns the TREDescription
         * 
         * @return TREDescription
         */
        public TREDescription getDescription()
        {
            return description;
        }

        /**
         * Returns the Field
         * 
         * @return Field
         */
        public Field getField()
        {
            return field;
        }

        /**
         * @return Returns the qualifiedTag.
         */
        public String getQualifiedTag()
        {
            return qualifiedTag;
        }

        /**
         * @param qualifiedTag
         *            The qualifiedTag to set.
         */
        protected void setQualifiedTag(String qualifiedTag)
        {
            this.qualifiedTag = qualifiedTag;
        }

    }

}

