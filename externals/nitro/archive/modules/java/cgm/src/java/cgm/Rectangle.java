package cgm;

import nitf.NITFObject;

public class Rectangle extends NITFObject
{

    Rectangle(long address)
    {
        super(address);
    }

    public native short getX1();

    public native short getX2();

    public native short getY1();

    public native short getY2();

    public native void setX1(short val);

    public native void setX2(short val);

    public native void setY1(short val);

    public native void setY2(short val);

}
