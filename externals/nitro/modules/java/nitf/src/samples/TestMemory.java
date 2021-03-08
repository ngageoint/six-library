import java.io.File;
import java.io.FilenameFilter;
import java.util.Arrays;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import nitf.IOHandle;
import nitf.Reader;
import nitf.Record;
import nitf.SegmentReader;
import nitf.TextSegment;

import org.apache.commons.io.FilenameUtils;
import org.apache.commons.lang.exception.ExceptionUtils;

public class TestMemory
{

    public static void main(String[] args) throws Exception
    {
        int n = 1000;
        List<File> files = new LinkedList<File>();
        for (int i = 0; i < args.length; ++i)
        {
            String arg = args[i];
            if (arg.equals("-n") && (i < args.length - 1))
                n = Integer.parseInt(args[++i]);
            else
            {
                File f = new File(arg);
                if (f.isDirectory())
                {
                    File[] dirFiles = f.listFiles(new FilenameFilter()
                    {
                        public boolean accept(File dir, String name)
                        {
                            String ext = FilenameUtils.getExtension(name)
                                    .toLowerCase();
                            return ext.matches("nitf|nsf|ntf");
                        }
                    });
                    files.addAll(Arrays.asList(dirFiles));
                }
                else
                    files.add(f);
            }
        }

        Iterator<File> it = null;
        for (int c = 0; c < n; c++)
        {
            if (it == null)
                it = files.iterator();
            if (!it.hasNext())
            {
                it = null;
                continue;
            }

            File file = it.next();
            System.out.println("Reading file " + (c + 1) + " : "
                    + file.getName());
            IOHandle handle = new IOHandle(file.getAbsolutePath(),
                    IOHandle.NITF_ACCESS_READONLY, IOHandle.NITF_OPEN_EXISTING);

            try
            {
                Reader reader = new Reader();
                Record record = reader.read(handle);
                TextSegment[] texts = record.getTexts();
                for (int i = 0; i < texts.length; i++)
                {
                    SegmentReader textReader = reader.getNewTextReader(i);
                    byte[] data = new byte[(int) textReader.getSize()];
                    textReader.read(data);
                    System.out.println("Read text data!");
                }
            }
            catch (Exception e)
            {
                System.out.println(ExceptionUtils.getStackTrace(e));
            }
            finally
            {
                handle.close();
            }
        }
    }

}
