OpenSSL must be installed in order to use this module.  

To install this module, from the top source directory run:
$  ./configure --enable-ssl-layer=openssl [--with-ssl-home=/path/to/openssl] --prefix=/path/to/install
$  make
$  make install

*Note:
 If OpenSSL is not located on the system, and ssl-home is not specified
 then it is automatically installed into the drivers directory.
