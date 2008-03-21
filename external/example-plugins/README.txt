To build this plugin, you want to include libxml2 in your path.  Because
of how its installed on linux, you do need to add the include path to
it from the configure line:

./configure --with-includes=/usr/include/libxml2/

You dont need to add any references to the nitf library, since they are
already listed in the build/Makefile(.in)

