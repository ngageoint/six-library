![alt tag](https://raw.github.com/ngageoint/six-library/master/docs/six_logo.png?raw=true)

## What SIX does

NGA is working to standardize Synthetic Aperture RADAR (SAR) systems in use throughout the community on a common format for complex SAR data (both wideband and narrowband metadata) as well as data derived from these data sets.  By providing access through GitHub to the SIX library, the community will have access not only to the sensor-independent complex data (SICD) specification and the sensor independent derived data (SIDD) specification documents, but also a well-vetted, mature implementation of software code providing the basic functions necessary to use these formats.  Additionally, the SIX library includes robust methods to validate the XML metadata in these files to provide developers and researchers with a means of testing that their files are compliant with the metadata and file format schema.  By providing this support to the community, the US Government may reduce the costs incurred in reformatting SAR data to our standards and more easily benefit from a wider array of SAR data from a variety of platforms and sensors. Additionally, the SICD and SIDD standards represent value to the community because they are the result of significant research and engineering into the mechanisms necessary (and found optimal) in representing a diverse array of SAR collection and processing metadata so that processing and visualization tools can work with them appropriately.

### Origin

The SIX (Sensor Independent XML) library was developed at the National Geospatial-Intelligence Agency (NGA) in collaboration with [Radiant Solutions](http://www.radiantsolutions.com/). The government has "unlimited rights" and is releasing this software to increase the impact of government investments by providing developers with the opportunity to take things in new directions. The software use, modification, and distribution rights are stipulated within the LGPL 3.0 license.

The SIX library is a cross-platform C++ API for reading and writing NGA's complex phase history (CPHD), complex (SICD), and derived (SIDD) sensor independent radar formats; Python bindings are gradually being added as well.  Additionally it is the official reference implementation library for the [SIDD](https://github.com/ngageoint/six-library/wiki/Sensor-Independent-Derived-Data-(SIDD)-Standard) format. The library also provides a [sensor model implementation](https://github.com/ngageoint/six-library/wiki) of many equations in the SICD and SIDD document.  This sensor model implementation provides the foundations for the SICD and SIDD [CSM](https://github.com/sminster/csm) (Community Sensor Model) implementation.

It is available as open-source software under the Lesser GNU Public License (LGPL). This license is commonly used in the open-source community, and allows applications that are not open source to make use of the library without penalty. As with other open source projects, the library is available as-is, with no warranty. 

See the [manual](https://github.com/ngageoint/six-library/blob/master/docs/six-manual.pdf) for detailed information including build instructions and API documentation.

The latest version of the library is available at https://github.com/ngageoint/six-library.git.

### Releases
The master is considered stable, but official [releases](https://github.com/ngageoint/six-library/releases) also occur as major features are added.

### Pull Requests

All pull request contributions to this project will be released under the LGPL 3.0 license.

Software source code previously released under an open source license and then modified by NGA staff is considered a "joint work" (see 17 USC 101); it is partially copyrighted, partially public domain, and as a whole is protected by the copyrights of the non-government authors and must be released according to the terms of the original open source license.
