import java.io.File;
import java.io.FilenameFilter;
import java.util.LinkedList;
import java.util.List;

import nitf.IOHandle;
import nitf.ImageSegment;
import nitf.NITFException;
import nitf.Reader;
import nitf.Record;

import org.apache.commons.io.FileUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.commons.lang.exception.ExceptionUtils;

public class StealRawImage
{

    static class ImageRunner implements Runnable
    {
        private File file;

        public ImageRunner(File file)
        {
            this.file = file;
        }

        public void run()
        {
            try
            {
                long t = System.currentTimeMillis();
                System.out.println("== Reading " + file.getAbsolutePath()
                        + " ==");

                Reader reader = new Reader();

                IOHandle handle = new IOHandle(file.getAbsolutePath(),
                        IOHandle.NITF_ACCESS_READONLY,
                        IOHandle.NITF_OPEN_EXISTING);

                Record record = reader.read(handle);

                // get the image segments
                ImageSegment[] segments = record.getImages();
                for (int i = 0; i < segments.length; ++i)
                {
                    ImageSegment segment = segments[i];
                    long offset = segment.getImageOffset();
                    long imageEnd = segment.getImageEnd();

                    handle.seek(offset);

                    int sz = (int) (imageEnd - offset);
                    byte[] buf = new byte[sz];
                    handle.read(buf);

                    File f = new File(System.getProperty("user.dir"), file
                            .getName()
                            + "_im" + i + ".raw");
                    System.out.println("Writing to: " + f.getAbsolutePath());
                    FileUtils.writeByteArrayToFile(f, buf);
                }

                handle.close();
                System.out.println("Time elapsed: "
                        + ((System.currentTimeMillis() - t) / 1000.0)
                        + " seconds : " + file.getAbsolutePath());
            }
            catch (Exception e)
            {
                System.err.println(ExceptionUtils.getStackTrace(e));
            }

        }
    }

    public static void main(String[] args) throws NITFException
    {
        if (args.length < 1)
        {
            System.err.println("args: <filenames>");
            System.exit(0);
        }

        List<File> files = new LinkedList<File>();

        for (int arg = 0; arg < args.length; ++arg)
        {
            File f = new File(args[arg]);
            if (f.exists() && f.isFile())
                files.add(f);
            else if (f.exists() && f.isDirectory())
            {
                for (File nitf : f.listFiles(new FilenameFilter()
                {
                    public boolean accept(File dir, String name)
                    {
                        return StringUtils.endsWithIgnoreCase(name, "ntf")
                                || StringUtils.endsWithIgnoreCase(name, "nitf");
                    }
                }))
                {
                    files.add(nitf);
                }
            }
        }

        for (File file : files)
        {
            // new Thread(new ImageRunner(file)).start();
            // just run and wait for now
            new Thread(new ImageRunner(file)).run();
        }
    }
}
