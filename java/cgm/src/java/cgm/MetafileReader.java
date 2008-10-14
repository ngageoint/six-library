package cgm;

import nitf.DestructibleObject;
import nitf.IOHandle;
import nitf.NITFException;

public class MetafileReader extends DestructibleObject
{

    static
    {
        CGMUtils.loadLibrary();
    }

    /**
     * Reader Constructor
     * 
     * @throws NITFException
     *             if a problem occurs in the underlying library
     */
    public MetafileReader() throws NITFException
    {
        construct();
    }

    /**
     * @see DestructibleObject#DestructibleObject(long)
     */
    MetafileReader(long address)
    {
        super(address);
    }

    /**
     * Constructs a new MetafileReader
     * 
     * @throws NITFException
     */
    private native synchronized void construct() throws NITFException;

    /**
     * Destructs the memory for the underlying object
     */
    protected native synchronized void destructMemory();

    /**
     * Read and parse the inputHandle into a Metafile
     * 
     * @param inputHandle
     *            the IOHandle used to read from
     * @return a Metafile containing the parsed data
     * @throws NITFException
     */
    public native synchronized Metafile read(IOHandle inputHandle)
            throws NITFException;
}
