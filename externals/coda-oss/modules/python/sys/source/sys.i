%module(package="coda") coda_sys

%feature("autodoc", "1");

%{
    #include "sys/Conf.h"
%}

%include "sys/Conf.h"

%typemap(in) ssize_t {
  $1 = PyInt_AsSsize_t($input);
}

%typemap(out) ssize_t {
  $result = PyInt_FromSsize_t($1);
}

%typemap(typecheck) ssize_t {
  $1 = PyInt_Check($input) ? 1 : 0;
}

%typemap(in) off_t {
  $1 = PyInt_AsSsize_t($input);
}

%typemap(out) off_t {
  $result = PyInt_FromSsize_t($1);
}

%typemap(typecheck) off_t {
  $1 = PyInt_Check($input) ? 1 : 0;
}

%typemap(in) __int64 {
  $1 = PyInt_AsSsize_t($input);
}

%typemap(out) __int64 {
  $result = PyInt_FromSsize_t($1);
}

%typemap(typecheck) __int64 {
  $1 = PyInt_Check($input) ? 1 : 0;
}

%typemap(in) Int64_T {
  $1 = PyInt_AsSsize_t($input);
}

%typemap(out) Int64_T {
  $result = PyInt_FromSsize_t($1);
}

%typemap(typecheck) Int64_T {
  $1 = PyInt_Check($input) ? 1 : 0;
}
