package cgm;

import nitf.CloneableObject;
import nitf.MemoryDestructor;
import nitf.NITFException;

public class Metafile extends CloneableObject
{
    Metafile(long address)
    {
        super(address);
    }

    @Override
    public CloneableObject makeClone() throws NITFException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public native void setName(String name);

    public native String getName();

    public native short getVersion();

    public native void setVersion(short version);

    public native String getDescription();

    public native void setDescription(String description);

    public native String[] getFonts();

    public native void setFonts(String... fonts);

    public native void removeFont(String font);

    public native void addFont(String font);

    public native Picture getPicture();

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
