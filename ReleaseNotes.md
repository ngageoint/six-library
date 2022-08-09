![alt tag](https://raw.github.com/ngageoint/six-library/master/docs/six_logo.png?raw=true)

# SIX [Release](https://github.com/ngageoint/six-library/releases) Notes

## [Version 3.?.??](https://github.com/ngageoint/six-library/releases/tag/SIX-3.?.??); ??? ?, 202?
* [coda-oss](https://github.com/mdaus/coda-oss) version [202?-??-??](https://github.com/mdaus/coda-oss/releases/tag/202?-??-??)
* [nitro](https://github.com/mdaus/nitro) version [2.??.??](https://github.com/mdaus/nitro/releases/tag/NITRO-2.??.??)
* Routines that "traffic" in XML strings (e.g., `parseDataFromString()` or `toXMLString()`) now use
  `std::u8string` (actually `coda_oss::u8string`) instead of `std::string`.

## [Version 3.1.13](https://github.com/ngageoint/six-library/releases/tag/SIX-3.1.13); August 2, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) version [2022-08-02](https://github.com/mdaus/coda-oss/releases/tag/2022-08-02)
* [nitro](https://github.com/mdaus/nitro) version [2.10.11](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.11)
* Better handling of optional error fields
* `vtsSetEnv` command

## [Version 3.1.12](https://github.com/ngageoint/six-library/releases/tag/SIX-3.1.12); June 29, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) version [2022-06-29](https://github.com/mdaus/coda-oss/releases/tag/2022-06-29)
* [nitro](https://github.com/mdaus/nitro) version [2.10.10](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.10)
* Support `OTHER.*` polarization values in XML from SIDD 3.0/SICD 1.3
* Updated to [CSM 3.0.4](https://github.com/ngageoint/csm/releases/tag/v3.0.4).
* Implements the back-end C++ changes from #351.

   * Support writing multiple CPHD channels to file from a single wideband buffer.
   * Support 1-, 2-, and 4-byte datatypes within the `PVPBlock`. All user-defined PVP were hard-coded to 8-byte types before.
   * Make `cphd::PVPBlock` used `std::map` to store added PVP parameters instead of `std::unordered_map`;
    this was needed to support older versions of SWIG.
   * Fix signal datatype in PVP to be 8-byte integer according to the CPHD v1.x spec. Was set to double previously.

* Fix `getARPVector()` and `getARPVelocityVector()` methods to return vectors filled with constant values;
  these values are used when performing `imageToSceneAdjustment`.

## [Version 3.1.11](https://github.com/ngageoint/six-library/releases/tag/SIX-3.1.11); May 13, 2022
* Lastest [coda-oss](https://github.com/mdaus/coda-oss) and [nitro](https://github.com/mdaus/nitro) (updates from **master**, no new releases)
* Fix bug in XML serializaton where `double`s were given a class of `xs::double` (two `:`s) instead of `xs:double`.

## [Version 3.1.10](https://github.com/ngageoint/six-library/releases/tag/SIX-3.1.10); May 3, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) version [2022-05-03](https://github.com/mdaus/coda-oss/releases/tag/2022-05-03)
* [nitro](https://github.com/mdaus/nitro) version [2.10.9](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.9)
* improve performance of *AMP8I_PHS8I* reading.

## [Version 3.1.9](https://github.com/ngageoint/six-library/releases/tag/SIX-3.1.9); February 22, 2022 (aka 2/22/22)
* [coda-oss](https://github.com/mdaus/coda-oss) version [2022-02-22](https://github.com/mdaus/coda-oss/releases/tag/2022-02-22)
* [nitro](https://github.com/mdaus/nitro) version [2.10.8](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.8)
* replace KDTree *`std::complex<float>` -> AMP8I_PHS8I conversion* with a
  ["math based" approach](https://github.com/ngageoint/six-library/pull/537#issuecomment-1026453353).
* restore SIDD 2.0 `AngleMagnitudeType`, SIDD 3.0 is `AngleZeroToExclusive360MagnitudeType`
* bugfix: "We found a bug/mistake/error in **six.sicd/source/RadarCollection.cpp**, specifically the function to rotate the Area\Plane\SegmentList block."

## [Version 3.1.8](https://github.com/ngageoint/six-library/releases/tag/SIX-3.1.8); December 13, 2021
* [coda-oss](https://github.com/mdaus/coda-oss) version [2021-12-13](https://github.com/mdaus/coda-oss/releases/tag/2021-12-13)
* [nitro](https://github.com/mdaus/nitro) version [2.10.7](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.7)
* write [8AMPI_PHSI](https://github.com/ngageoint/six-library/tree/feature/8AMPI_PHSI) files
* update schema for [SIDD 3.0](https://github.com/ngageoint/six-library/tree/feature/SIDD-3.0)

-----

## Contact
February 2022, Dan <dot> Smith <at> Maxar <dot> <see><oh><em>
