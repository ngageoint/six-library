package nitf;

import junit.framework.TestCase;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class RecordTest extends TestCase
{
    private static final Log log = LogFactory.getLog(RecordTest.class);

    public void testCreate()
    {
        try
        {
            Record record = new Record();
            assertTrue(record.getHeader().getFileHeader().setData("NITF"));
            assertEquals(record.getHeader().getFileHeader().toString(), "NITF");
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }

}
