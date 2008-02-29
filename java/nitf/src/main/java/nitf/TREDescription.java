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

import java.util.HashMap;
import java.util.Map;

/**
 * Provides access to statically defined descriptions of TREs. It also allows
 * users to create their own descriptions.
 */
public final class TREDescription extends DestructibleObject
{

    /**
     * Specifies that the length of the TRE Description field should be computed
     * by the library, and its length is based on a previous field value.
     */
    public static final int CONDITIONAL_LENGTH = -100;

    /**
     * Map that keeps track of TREDescriptions that are NOT static
     */
    protected static final Map userConstructed = new HashMap();

    /**
     * Creates a new TREDescription
     * 
     * @param dataType
     *            the type the data is Valid values: Field.NITF_BCS_A,
     *            Field.NITF_BCS_N, Field.BINARY, NITF_LOOP, NITF_ENDLOOP,
     *            NITF_IF, NITF_ENDIF, NITF_END
     * @param dataCount
     *            the size of the data that this description represents
     * @param label
     *            a legible descriptive label
     * @param tag
     *            a unique tag representing the field
     * @throws NITFException
     */
    public TREDescription(TREDescriptionDataType dataType, int dataCount,
            String label, String tag) throws NITFException
    {
        construct(dataType, dataCount, label, tag);

        // NOTE: the underlying code adds the object to the userConstructed map
    }

    /**
     * Constructs the underlying data object <p/>
     * 
     * @param dataType
     *            the type the data is Valid values: Field.NITF_BCS_A,
     *            Field.NITF_BCS_N, Field.BINARY, NITF_LOOP, NITF_ENDLOOP,
     *            NITF_IF, NITF_ENDIF, NITF_END
     * @param dataCount
     *            the size of the data that this description represents
     * @param label
     *            a legible descriptive label
     * @param tag
     *            a unique tag representing the field
     * @throws NITFException
     */
    private native void construct(TREDescriptionDataType dataType,
            int dataCount, String label, String tag) throws NITFException;

    /**
     * @see NITFObject#NITFObject(long)
     */
    TREDescription(long address)
    {
        setAddress(address);
    }

    // override, so we can check the map first
    synchronized void setAddress(long address)
    {
        synchronized (userConstructed)
        {
            if (userConstructed.containsKey(String.valueOf(address)))
            {
                super.setAddress(address);
            }
            else
            {
                this.address = address;
            }
        }
    }

    /**
     * Destructs the underlying memory
     */
    protected native void destructMemory();

    /**
     * Returns the data type
     * 
     * @return
     */
    public native synchronized TREDescriptionDataType getDataType();

    /**
     * Returns the data count
     * 
     * @return
     */
    public native synchronized int getDataCount();

    /**
     * Returns the label
     * 
     * @return
     */
    public native synchronized String getLabel();

    /**
     * Returns the tag
     * 
     * @return
     */
    public native synchronized String getTag();

    /**
     * Returns the TREDescription used as the end marker for an array of
     * TREDescriptions. <p/> Users should always call this to get an end marker
     * before ever setting an array of TREDescriptions to a TRE
     * 
     * @return TREDescription end marker
     */
    public static TREDescription getEndMarker() throws NITFException
    {
        // this cannot be one static instance, since it will be flagged to
        // be nonstatic
        return new TREDescription(TREDescriptionDataType.NITF_END, 0, "", "");
    }

    /**
     * Returns the TREDescription used as the marker for a LOOP
     * 
     * @param evaluator
     *            this is the evaluation command: it is OPTIONAL Examples: "+ 1"
     *            --> add 1 to the value of the loopTag field's Field "* 4" -->
     *            multiply the value of the loopTag field's Field by 4
     * @param loopTag
     *            the name of the tag whose value will be used as the loop count
     * @return TREDescription end marker
     */
    public static TREDescription getLoopMarker(String evaluator, String loopTag)
            throws NITFException
    {
        // nonstatic
        return new TREDescription(TREDescriptionDataType.NITF_LOOP, 0,
                evaluator, loopTag);
    }

    /**
     * Returns the TREDescription used as the marker for an ENDLOOP
     * 
     * @return TREDescription marker
     */
    public static TREDescription getEndLoopMarker() throws NITFException
    {
        // nonstatic
        return new TREDescription(TREDescriptionDataType.NITF_ENDLOOP, 0, "",
                "");
    }

    /**
     * Returns the TREDescription used as the marker for an IF
     * 
     * @param evaluator
     *            this is the evaluation command Examples: "eq GEOD" --> we want
     *            to see if it equals 'GEOD' "> 0" --> we want to see if the
     *            value is greater than zero
     * @param ifTag
     *            the name of the tag whose value will be used in the control
     *            statement
     * @return TREDescription marker
     */
    public static TREDescription getIfMarker(String evaluator, String ifTag)
            throws NITFException
    {
        // nonstatic
        return new TREDescription(TREDescriptionDataType.NITF_IF, 0, evaluator,
                ifTag);
    }

    /**
     * Returns the TREDescription used as the marker for an ENDIF
     * 
     * @return TREDescription marker
     */
    public static TREDescription getEndIfMarker(String lengthTag)
            throws NITFException
    {
        // nonstatic
        return new TREDescription(TREDescriptionDataType.NITF_ENDIF, 0, "",
                lengthTag);
    }

    /**
     * Returns the TREDescription used as the marker for a COMP_LEN
     * 
     * @return TREDescription marker
     */
    public static TREDescription getComputeLengthMarker() throws NITFException
    {
        // nonstatic
        return new TREDescription(TREDescriptionDataType.NITF_COMP_LEN, 0, "",
                "");
    }

    public String toString()
    {
        return super.toString() + "[dataType=" + getDataType() + ",dataCount="
                + getDataCount() + ",label=" + getLabel() + ",tag=" + getTag()
                + "]";
    }

}

