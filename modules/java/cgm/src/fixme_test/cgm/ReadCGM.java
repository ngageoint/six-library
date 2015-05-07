import cgm.MetafileReader;
import cgm.Metafile;
import nitf.IOHandle;
import nitf.NITFException;

public class ReadCGM
{

    public static void main(String[] args) throws NITFException
    {
        MetafileReader reader = new MetafileReader();
        IOHandle handle = new IOHandle(args[0]);
        final Metafile metafile = reader.read(handle);
        
        System.out.println(metafile.getName());
        System.out.println(metafile.getDescription());
        
        System.out.println(metafile.getPicture().getName());
        
        System.out.println(metafile.getPicture().getVDCExtent().getX1());
        System.out.println(metafile.getPicture().getVDCExtent().getY1());
        System.out.println(metafile.getPicture().getVDCExtent().getX2());
        System.out.println(metafile.getPicture().getVDCExtent().getY2());
    }
}
