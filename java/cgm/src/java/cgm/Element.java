package cgm;

import nitf.DestructibleObject;
import nitf.NITFObject;

public class Element extends DestructibleObject
{

    Element(long address)
    {
        super(address);
    }

    @Override
    protected native void destructMemory();

    public native String getName();

}
