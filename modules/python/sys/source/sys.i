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
