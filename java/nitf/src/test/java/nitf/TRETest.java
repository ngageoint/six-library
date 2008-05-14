package nitf;

import java.util.Iterator;
import java.util.List;

import junit.framework.TestCase;
import nitf.TRE.FieldPair;

import org.apache.commons.lang.exception.ExceptionUtils;

/**
 * This TestCase assumes your NITF_PLUGIN_PATH environment variable is set and
 * that the JITCID TRE plug-in exists.
 * 
 */
public class TRETest extends TestCase
{

    private TRE makeNewTRE(String tag) throws NITFException
    {
        return new TRE(tag, tag);
    }

    public void testCreate()
    {
        try
        {
            TRE tre = makeNewTRE("JITCID");
            assertEquals("JITCID", tre.getTag());
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }

    public void testGetSetFields()
    {
        try
        {
            TRE tre = makeNewTRE("JITCID");
            String comment = "A comment";
            assertTrue(tre.setField("FILCMT1", comment.getBytes()));
            Field field = tre.getField("FILCMT1");
            assertNotNull(field);
            assertEquals(comment, field.getStringData().trim());
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }

    public void testFind()
    {
        try
        {
            TRE tre = makeNewTRE("JITCID");
            List<FieldPair> fields = tre.find("FILCMT");
            assertNotNull(fields);
            assertEquals(2, fields.size());
            for (FieldPair fieldPair : fields)
            {
                System.out.println(fieldPair.getName());
            }
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }

    public void testIterator()
    {
        try
        {
            TRE tre = makeNewTRE("JITCID");
            for (Iterator<FieldPair> it = tre.iterator(); it.hasNext();)
            {
                FieldPair pair = (FieldPair) it.next();
                System.out.println(pair.getName());
            }
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }

    public void testPrint()
    {
        try
        {
            TRE tre = makeNewTRE("JITCID");
            tre.setField("FILCMT1", "comment1".getBytes());
            tre.setField("FILCMT2", "comment2".getBytes());
            tre.print(System.out);
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }
}
