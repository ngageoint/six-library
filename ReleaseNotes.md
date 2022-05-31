![alt tag](https://raw.github.com/ngageoint/six-library/master/docs/six_logo.png?raw=true)

# SIX Release Notes

## Version 3.1.11; May 13, 2022
* Lastest [coda-oss](https://github.com/mdaus/coda-oss) and [nitro](https://github.com/mdaus/nitro) (updates from **master**, no new releases)
* Fix bug in XML serializaton where `double`s were given a class of `xs::double` (two `:`s) instead of `xs:double`.

## Version 3.1.10; May 3, 2022
* [coda-oss](https://github.com/mdaus/coda-oss) version [2022-05-03](https://github.com/mdaus/coda-oss/releases/tag/2022-05-03)
* [nitro](https://github.com/mdaus/nitro) version [2.10.9](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.9)
* improve performance of *AMP8I_PHS8I* reading.

## Version 3.1.9; February 22, 2022 (aka 2/22/22)
* [coda-oss](https://github.com/mdaus/coda-oss) version [2022-02-22](https://github.com/mdaus/coda-oss/releases/tag/2022-02-22)
* [nitro](https://github.com/mdaus/nitro) version [2.10.8](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.8)
* replace KDTree *`std::complex<float>` -> AMP8I_PHS8I conversion* with a
  ["math based" approach](https://github.com/ngageoint/six-library/pull/537#issuecomment-1026453353).
* restore SIDD 2.0 `AngleMagnitudeType`, SIDD 3.0 is `AngleZeroToExclusive360MagnitudeType`
* bugfix: "We found a bug/mistake/error in **six.sicd/source/RadarCollection.cpp**, specifically the function to rotate the Area\Plane\SegmentList block."

## Version 3.1.8; December 13, 2021
* [coda-oss](https://github.com/mdaus/coda-oss) version [2021-12-13](https://github.com/mdaus/coda-oss/releases/tag/2021-12-13)
* [nitro](https://github.com/mdaus/nitro) version [2.10.7](https://github.com/mdaus/nitro/releases/tag/NITRO-2.10.7)
* write [8AMPI_PHSI](https://github.com/ngageoint/six-library/tree/feature/8AMPI_PHSI) files
* update schema for [SIDD 3.0](https://github.com/ngageoint/six-library/tree/feature/SIDD-3.0)

-----

## Contact
February 2022, Dan <dot> Smith <at> Maxar <dot> <see><oh><em>
