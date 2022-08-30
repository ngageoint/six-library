```
     _________________________
    |   ____  _||_  ___  __   |
    |  /___ \/_||_\| __\/  \  |
    | //   \// || \||  \\ _ \ |
    | ||   [===||===]  ||(_)| |
    | ||   _|| || |||  ||__ | |
    | \\ _/ |\_||_/||__/|| || |
    |  \___/ \_||_/|___/|| || |
    |__________||_____________|
 ```
# coda-oss Release Notes

## [Release 2022-08-30](https://github.com/mdaus/coda-oss/releases/tag/2022-08-30)
* XML is now always written as UTF-8; the code will still try to read Windows-1252.
* `Uri`s are no longer validated by default.
* Minor tweaks from other projects.
* Final C++11 release 🤞🏻; future releases will be C++14 from [main](https://github.com/mdaus/coda-oss/tree/main).

## [Release 2022-08-02](https://github.com/mdaus/coda-oss/releases/tag/2022-08-02)
* remove *Expat* and *libXML* modules and support in **xml.lite**; only *Xerces* was actively used.
* fix `waf` to work-around FIPS warning because of insecure *md5* hashing.
* tweak `str::EncodedStringView` and `str::EncodedString` for
  [future XML changes](https://github.com/mdaus/coda-oss/tree/feature/always-write-xml-as-utf8).

## [Release 2022-06-29](https://github.com/mdaus/coda-oss/releases/tag/2022-06-29)
* remove **modules/drivers/boost** as it was empty (and unused);
  **modules/c++/serialize** depended on boost, so it has also been removed.
* Update to [zlib 1.2.12](https://www.zlib.net/zlib-1.2.12.tar.gz),
  ["Due to the bug fixes, any installations of 1.2.11 should be replaced with 1.2.12."](https://www.zlib.net/)
* Build most code in Visual Studio 2022
* Removed more compiler warnings
* Begin work on `CODA_OSS_API` (needed for building a shared-library/DLL)
* Add `run1D()` method to `mt::GenerationThreadPool` 

## [Release 2022-05-03](https://github.com/mdaus/coda-oss/releases/tag/2022-05-03)
* Fixed a bug in `Poly2D::atY()`; improved `flipXY()` behavior.
* Implement [std::filesystem::file_size()](https://en.cppreference.com/w/cpp/filesystem/file_size).
* use `inline` functions for `TEST_` macros
* force use of [64-bit `time_t`](https://en.wikipedia.org/wiki/Year_2038_problem)
* more routines now support a `std::span` overload; e.g., `io::InputStream::read()`.

## [Release 2022-02-22](https://github.com/mdaus/coda-oss/releases/tag/2022-02-22)
* new `EnocdedString` and `EncodedStringView` to manage strings in different encodings
* XML containing UTF-8 characters can now be validated
* Update to [GSL 4.0.0](https://github.com/microsoft/GSL/releases/tag/v4.0.0)
* our implementation of `std` stuff is all in the `coda_oss` namespace
* old, unused **log4j.jar** deleted to remove any questions about the security vulnerability

## [Release 2021-12-13](https://github.com/mdaus/coda-oss/releases/tag/2021-12-13)
* Try hard to parse XML encoded in various ways
* Simplify XML element creation for common idioms
