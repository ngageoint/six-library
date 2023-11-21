HDF5 version 1.14.2 from https://github.com/HDFGroup/hdf5/releases/tag/hdf5-1_14_2

==============================================

Three files need to be copied and renamed.  On each platform

1. Build using CMake
2. Copy/rename H5Tinit.c to H5Tinit_<compiler>.c_
2. Copy/rename H5lib_settings.c to H5lib_settings_<compile>.c_
2. Copy/rename H5pubconf.h to H5pubconf_<OS>.h

