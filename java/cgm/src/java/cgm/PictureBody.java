package cgm;

import nitf.NITFObject;

public class PictureBody extends NITFObject
{

    PictureBody(long address)
    {
        super(address);
    }

    public native boolean isTransparent();

    public native short[] getAuxColor();

    public native Element[] getElements();

}
