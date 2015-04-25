package cgm;

import nitf.DestructibleObject;
import nitf.IOInterface;
import nitf.MemoryDestructor;
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
     * Read and parse the inputHandle into a Metafile
     * 
     * @param inputHandle
     *            the input handle used to read from
     * @return a Metafile containing the parsed data
     * @throws NITFException
     */
    public native synchronized Metafile read(IOInterface inputHandle)
            throws NITFException;

    @Override
    protected MemoryDestructor getDestructor()
    {
        return new Destructor();
    }

    private static class Destructor implements MemoryDestructor
    {
        public native boolean destructMemory(long nativeAddress);
    }
}
