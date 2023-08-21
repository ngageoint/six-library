# NITRO (NITF i/o) Release Notes

## [Version 2.11.4](https://github.com/mdaus/nitro/releases/tag/NITRO-2.11.4); August 18 5, 2023
* [coda-oss](https://github.com/mdaus/coda-oss) release [2023-08-18](https://github.com/mdaus/coda-oss/releases/tag/2023-08-18)
* added **.gitattributes** and normalized line-endings (`\n` for most text files).

## [Version 2.11.3](https://github.com/mdaus/nitro/releases/tag/NITRO-2.11.3); June 5, 2023
* [coda-oss](https://github.com/mdaus/coda-oss) release [2023-06-05](https://github.com/mdaus/coda-oss/releases/tag/2023-06-05)
* Fix display of [certain TREs](https://github.com/mdaus/nitro/issues/529).
* Adjust compiler flags for clean *CMake* builds.
* Implement `DecompressionInterface::adapterFreeBlock()` to fix decompression.

## [Version 2.11.2](https://github.com/mdaus/nitro/releases/tag/NITRO-2.11.2); December 14, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) release [2022-12-14](https://github.com/mdaus/coda-oss/releases/tag/2022-12-14)
* Further work at getting unittests to work better when included elsewhere (e.g., SIX) as **externals**.

## [Version 2.11.1](https://github.com/mdaus/nitro/releases/tag/NITRO-2.11.1); November 4, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) release [2022-11-04](https://github.com/mdaus/coda-oss/releases/tag/2022-11-04)
* fixed incorrect field sizes in the CSEXRB TRE.
* make unittests work better when included elsewhere (e.g., SIX) as **externals**.
* remove exception specifications
* `tarfile.extractall()` sanitization

## [Version 2.11.0](https://github.com/mdaus/nitro/releases/tag/NITRO-2.11.0); August 30, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) release [2022-08-30_cpp14](https://github.com/mdaus/coda-oss/releases/tag/2022-08-30_cpp14)
* Cut from [main](https://github.com/mdaus/nitro/tree/main); identical to [Version 2.10.12](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.12)
except that C++14 is now required.  [master](https://github.com/mdaus/nitro/tree/master) remains at C++11.

## [Version 2.10.12](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.12); August 30, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) release [2022-08-30](https://github.com/mdaus/coda-oss/releases/tag/2022-08-30)
* Build JPEG decompression as a plug-in.
* tweak unittests so they run in SIX.
* ~~Final C++11 release 🤞🏻; future releases will be C++14 from [main](https://github.com/mdaus/nitro/tree/main).~~

## [Version 2.10.11](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.11); August 2, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) release [2022-08-02](https://github.com/mdaus/coda-oss/releases/tag/2022-08-02)
* remove more compiler warnings

## [Version 2.10.10](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.10); June 29, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) release [2022-06-29](https://github.com/mdaus/coda-oss/releases/tag/2022-06-29)

## [Version 2.10.9](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.9); May 3, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) release [2022-05-03](https://github.com/mdaus/coda-oss/releases/tag/2022-05-03)
* the large unused **archive** directory has been removed; contents are in [archive/java](https://github.com/mdaus/nitro/tree/archive/java)
and [archive/mex](https://github.com/mdaus/nitro/tree/archive/mex).
* the large unused **docs** directory has been removed; contents are in [archive/docs](https://github.com/mdaus/nitro/tree/archive/docs).
* Copy the J2K wrappers from **coda**.

## [Version 2.10.8](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.8); February 22, 2022 (aka 2/22/22)
* [coda-oss](https://github.com/mdaus/coda-oss) release [2022-02-22](https://github.com/mdaus/coda-oss/releases/tag/2022-02-22)
* Show how to use [strongly typed TREs](https://github.com/mdaus/nitro/tree/feature/strongly-typed-TREs).
* Infrastucture to support "[reflection](https://github.com/mdaus/nitro/tree/feature/reflection)" over C `struct`s.
* Begin C++ wrappers for the C *j2k* routines; work-in-progress.

## [Version 2.10.7](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.7); December 13, 2021
* [coda-oss](https://github.com/mdaus/coda-oss) release [2021-12-13](https://github.com/mdaus/coda-oss/releases/tag/2021-12-13)
* allow `NITF_PLUGIN_PATH` warning to be turned off
* tweaks to compile with older C++11 compilers
* build with Visual Studio 2022

-----

## Contact
February 2022, Dan <dot> Smith <at> Maxar <dot> <see><oh><em>