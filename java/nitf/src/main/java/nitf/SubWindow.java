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
 * An object that contains information for reading image data.
 */
public final class SubWindow extends DestructibleObject
{

    /**
     * Creates a new SubWindow object
     * 
     * @throws NITFException
     */
    public SubWindow() throws NITFException
    {
        construct();
    }

    /**
     * @see DestructibleObject#DestructibleObject(long)
     */
    SubWindow(long address)
    {
        super(address);
    }

    /**
     * Destructs the underlying memory
     */
    protected native synchronized void destructMemory();

    /**
     * Returns the BandList array
     * 
     * @return
     */
    public native synchronized int[] getBandList();

    /**
     * Constructs the underlying memory
     * 
     * @throws NITFException
     */
    private native synchronized void construct() throws NITFException;

    /**
     * Sets the number of columns
     * 
     * @param numCols
     */
    public native synchronized void setNumCols(int numCols);

    /**
     * Sets the BandList array
     * 
     * @param bandList
     */
    public native synchronized void setBandList(int[] bandList);

    /**
     * Returns the start row
     * 
     * @return
     */
    public native synchronized int getStartRow();

    /**
     * Sets the start row
     * 
     * @param startRow
     */
    public native synchronized void setStartRow(int startRow);

    /**
     * Returns the number of rows
     * 
     * @return
     */
    public native synchronized int getNumRows();

    /**
     * Sets the number of rows
     * 
     * @param numRows
     */
    public native synchronized void setNumRows(int numRows);

    /**
     * Returns the start column
     * 
     * @return
     */
    public native synchronized int getStartCol();

    /**
     * Sets the start column
     * 
     * @param startCol
     */
    public native synchronized void setStartCol(int startCol);

    /**
     * Returns the number of columns
     * 
     * @return
     */
    public native synchronized int getNumCols();

    /**
     * Returns the number of bands
     * 
     * @return
     */
    public native synchronized int getNumBands();

    /**
     * Sets the number of bands
     * 
     * @param numBands
     */
    public native synchronized void setNumBands(int numBands);

    /**
     * Sets the DownSampler for this SubWindow If none is specified, or if
     * downSampler is null, the data will not be downsampled
     * 
     * @param downSampler
     */
    public native synchronized void setDownSampler(DownSampler downSampler);

    /**
     * Returns the DownSampler for this window, or null if none is specified
     * 
     * @return
     */
    public native synchronized DownSampler getDownSampler();

    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append(SubWindow.class.getName() + ":[");
        buf.append("startCol=" + getStartCol());
        buf.append(",startRow=" + getStartRow());
        buf.append(",numCols=" + getNumCols());
        buf.append(",numRows=" + getNumRows());
        buf.append(",numBands=" + getNumBands());
        buf.append("]");
        return buf.toString();
    }

}

