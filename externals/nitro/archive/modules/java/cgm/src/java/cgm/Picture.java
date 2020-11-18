package cgm;

import nitf.NITFObject;

public class Picture extends NITFObject
{

    Picture(long address)
    {
        super(address);
    }

    public native String getName();

    public native void setName(String name);

    // public native ColorSelectionMode getColorSelectionMode();
    //
    // public native WidthSpecificationMode getEdgeWidthSpec();
    //
    // public native WidthSpecificationMode getLineWidthSpec();

    public native Rectangle getVDCExtent();

    public native PictureBody getPictureBody();
}
