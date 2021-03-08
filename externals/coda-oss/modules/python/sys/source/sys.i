%module(package="coda") coda_sys

%feature("autodoc", "1");

%{
    #include "sys/Conf.h"
%}

%include "sys/Conf.h"
%include "sys/UTCDateTime.h"

%typemap(in) ssize_t {
%#if PY_VERSION_HEX >= 0x03000000
  $1 = PyLong_AsSsize_t($input);
%#else
  $1 = PyInt_AsSsize_t($input);
%#endif
}

%typemap(out) ssize_t {
%#if PY_VERSION_HEX >= 0x03000000
  $result = PyLong_FromSsize_t($1);
%#else
  $result = PyInt_FromSsize_t($1);
%#endif
}

%typemap(typecheck) ssize_t {
%#if PY_VERSION_HEX >= 0x03000000
  $1 = PyLong_Check($input) ? 1 : 0;
%#else
  $1 = PyInt_Check($input) ? 1 : 0;
%#endif
}

%typemap(in) off_t {
%#if PY_VERSION_HEX >= 0x03000000
  $1 = PyLong_AsSsize_t($input);
%#else
  $1 = PyInt_AsSsize_t($input);
%#endif
}

%typemap(out) off_t {
%#if PY_VERSION_HEX >= 0x03000000
  $result = PyLong_FromSsize_t($1);
%#else
  $result = PyInt_FromSsize_t($1);
%#endif
}

%typemap(typecheck) off_t {
%#if PY_VERSION_HEX >= 0x03000000
  $1 = PyLong_Check($input) ? 1 : 0;
%#else
  $1 = PyInt_Check($input) ? 1 : 0;
%#endif
}

%typemap(in) __int64 {
%#if PY_VERSION_HEX >= 0x03000000
  $1 = PyLong_AsSsize_t($input);
%#else
  $1 = PyInt_AsSsize_t($input);
%#endif
}

%typemap(out) __int64 {
%#if PY_VERSION_HEX >= 0x03000000
  $result = PyLong_FromSsize_t($1);
%#else
  $result = PyInt_FromSsize_t($1);
%#endif
}

%typemap(typecheck) __int64 {
%#if PY_VERSION_HEX >= 0x03000000
  $1 = PyLong_Check($input) ? 1 : 0;
%#else
  $1 = PyInt_Check($input) ? 1 : 0;
%#endif
}

%typemap(in) sys::Int64_T {
%#if PY_VERSION_HEX >= 0x03000000
  $1 = PyLong_AsSsize_t($input);
%#else
  $1 = PyInt_AsSsize_t($input);
%#endif
}

%typemap(out) sys::Int64_T {
%#if PY_VERSION_HEX >= 0x03000000
  $result = PyLong_FromSsize_t($1);
%#else
  $result = PyInt_FromSsize_t($1);
%#endif
}

%typemap(typecheck) sys::Int64_T {
%#if PY_VERSION_HEX >= 0x03000000
  $1 = PyLong_Check($input) ? 1 : 0;
%#else
  $1 = PyInt_Check($input) ? 1 : 0;
%#endif
}
