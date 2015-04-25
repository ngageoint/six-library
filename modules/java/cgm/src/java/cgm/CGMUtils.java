package cgm;

public class CGMUtils
{

    /**
     * The name of the library to load
     */
    public static final String NATIVE_LIBRARY_NAME = "cgm.jni-c";

    static
    {
        loadLibrary();
    }

    public static void loadLibrary()
    {
        // Load the library
        System.loadLibrary(NATIVE_LIBRARY_NAME);
    }

    private CGMUtils()
    {
    }

}
