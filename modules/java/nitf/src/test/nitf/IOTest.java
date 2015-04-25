package nitf;

import junit.framework.TestCase;

import org.apache.commons.lang.exception.ExceptionUtils;

public class IOTest extends TestCase
{

    public void testMemoryIO()
    {
        try
        {
            IOInterface memIO = new MemoryIO(1024);
            String val = "NITF";
            memIO.write(val.getBytes());
            assertEquals(val.length(), memIO.tell());

            memIO.seek(0, IOInterface.SEEK_SET);
            byte[] buf = new byte[4];
            memIO.read(buf);

            assertEquals(val, new String(buf));
            memIO.close();
        }
        catch (NITFException e)
        {
            fail(ExceptionUtils.getStackTrace(e));
        }
    }

}
