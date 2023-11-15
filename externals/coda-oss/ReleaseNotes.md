# coda-oss Release Notes

## [Release 202?-??-??](https://github.com/mdaus/coda-oss/releases/tag/202?-??-??)
* Update to [HighFive 2.8.0](https://github.com/BlueBrain/HighFive/releases/tag/v2.8.0).

## [Release 2023-10-23](https://github.com/mdaus/coda-oss/releases/tag/2023-10-23)
* Tweaked **.gitattributes**.
* Removed *str::EncodedStringView*; made *str* conversion routines more consistent.
* *FmtX()* uses overloads rather than C-style varargs.
* Add [`std::ssize()`](https://en.cppreference.com/w/cpp/iterator/size).
* `ENABLE_AVX2` and `ENABLE_AVX512F` flags for CMake builds.
* Build cleanly with ASAN.

## [Release 2023-08-18](https://github.com/mdaus/coda-oss/releases/tag/2023-08-18)
* New `sys::OS::getSIMDInstructionSet()` utility routine; SSE2 is required (default with 64-bit builds).
* `types::ComplexInteger` to work-around `std::complex<short>` no longer being [valid C++](https://en.cppreference.com/w/cpp/numeric/complex).
* Another round of reducing various compiler warnings (of note: `NULL` -> `nullptr`).
* Some support for [`std::numbers`](https://en.cppreference.com/w/cpp/header/numbers) from C++20.
* Update to [HighFive 2.7.1](https://github.com/BlueBrain/HighFive/releases/tag/v2.7.1).
* Update to [e2fsprogs 1.47.0](https://e2fsprogs.sourceforge.net/e2fsprogs-release.html#1.47.0).
* Update to [xerces-c 3.2.4](https://issues.apache.org/jira/secure/ReleaseNote.jspa?version=12350542&styleName=Text&projectId=10510).
* Update to [HDF5](https://www.hdfgroup.org/) [1.14.2](https://github.com/HDFGroup/hdf5/releases/tag/hdf5-1_14_2).
* `mem::ComplexView`s renamed to be more descriptive.
* **hdf5.lite** removed, use [HighFive](https://github.com/BlueBrain/HighFive/) (included).
* added **.gitattributes** and normalized line-endings (`\n` for most text files).

## [Release 2023-06-05](https://github.com/mdaus/coda-oss/releases/tag/2023-06-05)
* *zlib* updated to [1.2.13](https://github.com/madler/zlib/releases/tag/v1.2.13).
* new `mem::ComplexView` class to make it easier to process complex data stored in parallel. 
* adjust compiler flags for clean *CMake* builds.
* wrap common "file open" routines (e.g., `fopen()`) to support `sys::expandEnvironmentVariables()`.
* add header-only [HighFive](https://github.com/BlueBrain/HighFive) HDF5-wrapper library.
* Added a handful of [HighFive](https://github.com/BlueBrain/HighFive) utility routines.
* `mt::transform_async()` removed, it doesn't match C++17 techniques.
* Revamp `sys::byteSwap()` for improved type-safety and
  [better performance](https://devblogs.microsoft.com/cppblog/a-tour-of-4-msvc-backend-improvements/).
 * Added case-insensitive string comparison utilities: `str::eq()` and `str::ne()`;
   `xml::lite::Uri`s compare case-insensitive.

## [Release 2022-12-14](https://github.com/mdaus/coda-oss/releases/tag/2022-12-14)
* removed remaining vestiges of `std::auto_ptr`, provide `mem::AutoPtr` for the tiny handful of
  places (e.g., SWIG bindings) that still need copying.
* `xml::lite::Element` overloads to make creating new XML documents easier; see unittests for examples.
* try even harder to find unittest files in various scenarios.
* build *hdf5.lite* with **waf**.
* New `--optz=fastest-possible` (**waf** only) which adds
  [`-march=native`](https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/x86-Options.html#x86-Options)
  to *fastest* for **waf** builds.  There is a new *faster* option (the default) which is currently
  the same as *fastest* (i.e., no `-march=native`).

## [Release 2022-11-04](https://github.com/mdaus/coda-oss/releases/tag/2022-11-04)
* "Cut" from [main](https://github.com/mdaus/coda-oss/tree/main), not *master*.
* The [HDFGroup](https://hdfgroup.org/)s [HDF5 library](https://github.com/HDFGroup/hdf5) is built
   and installed; a simple (and very incomplete!) wrapper is provided, this is at the "hello world!" stage.
* A few **cli** routines have been tweaked to make unit-testing easier.
* Utility routines for finding various files when unit-testing.
* Removed C++14 work-arounds needed in C++11. Legacy C++ exception specifications removed.
* Rebuild `waf` for FIPS error; added more debug options.
 
## [Release 2022-08-30](https://github.com/mdaus/coda-oss/releases/tag/2022-08-30)
* XML is now always written as UTF-8; the code will still try to read Windows-1252.
* `Uri`s are no longer validated by default.
* Minor tweaks from other projects.
* ~~Final C++11 release 🤞🏻; future releases will be C++14 from [main](https://github.com/mdaus/coda-oss/tree/main).~~

## [Release 2022-08-02](https://github.com/mdaus/coda-oss/releases/tag/2022-08-02)
* remove *Expat* and *libXML* modules and support in **xml.lite**; only *Xerces* was actively used.
* ~~fix `waf` to work-around FIPS warning because of insecure *md5* hashing.~~
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
