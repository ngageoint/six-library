package cgm;

import nitf.DestructibleObject;
import nitf.MemoryDestructor;

public class Element extends DestructibleObject
{

    Element(long address)
    {
        super(address);
    }

    public native String getName();

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
