%module math_poly

%feature("autodoc", "1");

%ignore math::poly::OneD<Vector3>::truncateToNonZeros;
%ignore math::poly::OneD<Vector3>::transformInput;
%ignore math::poly::OneD<Vector3>::integrate;
%ignore math::poly::OneD<Vector3>::power;

%{
#include <string>
#include <sstream>
#include "import/math/linear.h"
typedef math::linear::VectorN<3,double> Vector3;
typedef math::linear::Vector<double> VectorDouble;
#include "math/poly/OneD.h"
#include "math/poly/TwoD.h"
#include "math/poly/Fit.h"
%}

%import "math_linear.i"
%import "except.i"

%include "std_string.i"
%include "carrays.i"
%array_functions(double, doubleArray);

%include "math/poly/OneD.h"

%template(Poly1D) math::poly::OneD<double>;


%extend math::poly::OneD<double>
{
    double __getitem__(long i)
    { 
        if (i > self->order())
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return 0.0;
        }
        return (*self)[i];
    }
    
    void __setitem__(long i, double val)
    { 
        if (i > self->order())
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return;
        }
        (*self)[i] = val;
    }
    
    std::string __str__()
    {
        std::ostringstream ostr;
        ostr << *self;
        return ostr.str();
    }
}

%include "math/poly/TwoD.h"

%template(Poly2D) math::poly::TwoD<double>;

%extend math::poly::TwoD<double>
{
    double __getitem__(PyObject* inObj)
    {
        if (!PyTuple_Check(inObj))
        {
            PyErr_SetString(PyExc_TypeError, "Expecting a tuple");
            return 0.0;
        }
        Py_ssize_t xpow, ypow;
        if (!PyArg_ParseTuple(inObj, "nn", &xpow, &ypow))
        {
            return 0.0;
        }
        if (xpow > self->orderX() || ypow > self->orderY())
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return 0.0;
        }
        return (*self)[xpow][ypow];
    }

    void __setitem__(PyObject* inObj, double val)
    {
        if (!PyTuple_Check(inObj))
        {
            PyErr_SetString(PyExc_TypeError, "Expecting a tuple");
            return;
        }
        Py_ssize_t xpow, ypow;
        if (!PyArg_ParseTuple(inObj, "nn", &xpow, &ypow))
        {
            return;
        }
        if (xpow > self->orderX() || ypow > self->orderY())
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return;
        }
        (*self)[xpow][ypow] = val;
    }

    std::string __str__()
    {
        std::ostringstream ostr;
        ostr << *self;
        return ostr.str();
    }
}

typedef math::linear::VectorN<3,double> Vector3;
%template(PolyVector3) math::poly::OneD<Vector3>;
 
%extend math::poly::OneD<Vector3 >
{
    public:
        Vector3 __getitem__(long i) 
        { 
            return (*self)[i]; 
        }

        void __setitem__(long i, Vector3 val) 
        { 
            (*self)[i] = val; 
        }
};

%include "math/poly/Fit.h"

typedef math::linear::Vector<double> VectorDouble;
%template(FitVectorDouble) math::poly::fit<VectorDouble>;
