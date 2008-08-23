import nitf.DESegment;
import nitf.IOHandle;
import nitf.Record;
import nitf.SegmentSource;
import nitf.SegmentWriter;
import nitf.TRE;
import nitf.Writer;

public class TestDESCreate
{

    final static String DATA = "123456789ABCDEF0";

    public static void main(String[] args) throws Exception
    {

        Record rec = new Record();

        rec.getHeader().getFileHeader().setData("NITF");
        rec.getHeader().getFileVersion().setData("02.10");
        rec.getHeader().getClassification().setData("U");
        rec.getHeader().getFileTitle().setData("Test DES");

        // add a new DESegment
        final DESegment des = rec.newDESegment();

        // create and get a new subheaderFields TRE
        // you want to use the returned one since the TRE passed in gets cloned
        TRE newTRE = new TRE("JITCID");
        TRE tre = des.getSubheader().setSubheaderFields(newTRE);
        tre.setField("FILCMT", "A comment");

        tre.print(System.out);

        des.getSubheader().getFilePartType().setData("DE");
        // set the typeId to the TRE tag
        des.getSubheader().getTypeID().setData("JITCID");
        des.getSubheader().getVersion().setData("01");
        des.getSubheader().getSecurityClass().setData("U");

        IOHandle handle = new IOHandle("test.ntf",
                IOHandle.NITF_ACCESS_WRITEONLY, IOHandle.NITF_CREATE);

        // setup a Writer
        Writer writer = new Writer();
        writer.prepare(rec, handle);

        final SegmentWriter deWriter = writer.getNewDEWriter(0);

        final SegmentSource source = SegmentSource.makeSegmentMemorySource(DATA
                .getBytes(), DATA.length(), 0, 0);

        deWriter.attachSource(source);

        writer.write();
    }

}
