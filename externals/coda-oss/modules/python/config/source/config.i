%module(package="coda") coda_config

#define static_assert(...) // SWIG <= 3.0.12 cannot handle static_assert
#define final // SWIG (as of 4.0.2) does not understand "final"
