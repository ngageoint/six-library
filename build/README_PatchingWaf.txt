coda-oss contains a custom-patched version of waf forked from a very old
version of that build tool. It is distributed as a self-extracting 'waf'
executable.

For future reference, the waf self-extractor both extracts itself,
and if it is extracted using Python2, also patches the extracted code.

For those needing to repack it in the future, the steps are:

* Check out an old version of waf from:
    https://gitlab.com/ita1024/waf-old/-/tree/waf-1.7.14
* Check out coda-oss.
* Make sure that your python interpreter is python 3.
* Go to the build subdirectory, and run 'waf --help' to extract the .waf3-xxx
  subdirectory.  IF DONE WITH PYTHON2, THIS WILL SILENTLY PATCH THE CODE TO
  ONLY WORK WITH PYTHON2.
* Apply your changes within .waf3-xxx/waflib.
* Replace the waflib directory in the waf checkout with the patched waflib 
  from coda-oss.
* In the waf checkout, run 'waf-light --make-waf'.
* This should generate a 'waf' package.  Make sure to test extracting it under
  both python2 and python3, and make sure it extracts to the same thing that
  the original coda-oss waf package did (plus your new changes).
