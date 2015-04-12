Either Expat or Xerces XML parsers must be installed in order to use this module.  

To install this module, from the top source directory run:
$  ./configure --enable-xml-layer=[expat | xerces] [--with-xml-home=/path/to/xml] --prefix=/path/to/install
$  make
$  make install

*Note:
 If Expat is chosen and is not located on the system, or specified with configure then
 it is automatically installed into the drivers directory

