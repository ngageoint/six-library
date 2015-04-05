%module math_poly

%feature("autodoc", "1");

%{
#include "math/poly/OneD.h"
#include "math/poly/TwoD.h"
%}

%include "math/poly/OneD.h"

%template(Poly1D) math::poly::OneD<double>;

%extend math::poly::OneD<double> {
    double __getitem__(long i) { return (*self)[i]; };
    void __setitem__(long i, double val) { (*self)[i] = val; };
}

%include "math/poly/TwoD.h"

%template(Poly2D) math::poly::TwoD<double>;

#if 0
// look into using carray.i
%extend math::poly::TwoD<double> {
    double* __getitem__(long i) { return (*self)[i]; };
}
#endif
