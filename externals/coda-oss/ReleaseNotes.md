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

## Release 2022-??-??
* Build most code in Visual Studio 2022
* Removed more compiler warnings
* Begin work on `CODA_OSS_API` (needed for building a shared-library/DLL)
* Add `run1D()` method to `mt::GenerationThreadPool` 

## Release 2022-05-03
* Fixed a bug in `Poly2D::atY()`; improved `flipXY()` behavior.
* Implement [std::filesystem::file_size()](https://en.cppreference.com/w/cpp/filesystem/file_size).
* use `inline` functions for `TEST_` macros
* force use of [64-bit `time_t`](https://en.wikipedia.org/wiki/Year_2038_problem)
* more routines now support a `std::span` overload; e.g., `io::InputStream::read()`.

## (Release 2022-02-22)
* new `EnocdedString` and `EncodedStringView` to manage strings in different encodings
* XML containing UTF-8 characters can now be validated
* Update to [GSL 4.0.0](https://github.com/microsoft/GSL/releases/tag/v4.0.0)
* our implementation of `std` stuff is all in the `coda_oss` namespace
* old, unused **log4j.jar** deleted to remove any questions about the security vulnerability

## [Release 2021-12-13](https://github.com/mdaus/coda-oss/releases/tag/2021-12-13)
* Try hard to parse XML encoded in various ways
* Simplify XML element creation for common idioms
