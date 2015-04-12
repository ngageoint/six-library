%module math_linear

%feature("autodoc", "1");

%{
#include "math/linear/VectorN.h"
#include "math/linear/MatrixMxN.h"
%}

%include "std_vector.i"
%include "std_string.i"

%ignore *::operator=;

%include "math/linear/MatrixMxN.h"
%include "math/linear/VectorN.h"

%template(std_vector_double) std::vector<double>;

// define template variants we want in python
%template(Matrix3x1) math::linear::MatrixMxN<3,1,double>;
%template(Vector3) math::linear::VectorN<3, double>;
%template(cross) math::linear::cross<double>;

%extend math::linear::VectorN<3,double> {
    // SWIG doesn't automatically generate [] operator
    double __getitem__(long i) { return (*self)[i]; };
    void __setitem__(long i, double val) { (*self)[i] = val; };

    // string representation in python
    char* __str__() {
        static char temp[256];
        sprintf( temp, "[ %g, %g, %g ]", (*self)[0], (*self)[1], (*self)[2] );
        return &temp[0];
    }

    // helper method to facilitate creating a numpy array from Vector3
    // v = ml.Vector3([1.0, 1.0, 1.0])
    // arr = np.asarray(v.vals())
    //
    // ideally we should implement __array__() instead, which will allow it 
    // to be used by most numpy functions
    std::vector<double> vals() {
        return self->matrix().col(0);
    }
}
