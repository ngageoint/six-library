Since 2007, matlab ships with its own nitf reader.  The reader is 
adequate for some purposes, but not all.  The intent here is to externalize
all major nitf capabilities for read using NITRO to read the file.

The intent is to optionally support TRE extensions
(using the same TRE handlers that the C uses), as well as to support
compressed image reading (via any available NITRO decompression plugins)

Disclaimer: This interface to matlab is under development and has not been
heavily tested yet.  The build system is limited and testing was done using
matlab v2007a.  You may have to tweak the configuration to get this working on
your system.  Use at your own risk.

Example usage:

nitf = '/path/to/v_3005a.ntf'
handlers = '/path/to/nitf/plugins/'
% First usage either requires NITF_PLUGIN_PATH env. var. to be set
% or doesnt bother to parse the TREs
record = nitf_metadata(nitf)

% Second usage takes path to plugins.  
record = nitf_metadata(nitf, handlers)

