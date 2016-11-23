%module(package="coda") math_poly

%feature("autodoc", "1");

%ignore math::poly::OneD<Vector3>::truncateToNonZeros;
%ignore math::poly::OneD<Vector3>::transformInput;
%ignore math::poly::OneD<Vector3>::integrate;
%ignore math::poly::OneD<Vector3>::power;


%{
#ifndef SWIGPY_SLICE_ARG
# if PY_VERSION_HEX >= 0x03000000
#  define SWIGPY_SLICE_ARG(obj) ((PyObject*) (obj))
# else
#  define SWIGPY_SLICE_ARG(obj) ((PySliceObject*) (obj))
# endif
#endif

#include <string>
#include <sstream>
#include "import/math/linear.h"
typedef math::linear::VectorN<3,double> Vector3;
typedef math::linear::Vector<double> VectorDouble;
#include "math/poly/OneD.h"
#include "math/poly/TwoD.h"
#include "math/poly/Fit.h"
#include "numpyutils/numpyutils.h"
#include "Python.h"
#include "numpy/arrayobject.h"
%}

typedef math::linear::VectorN<3,double> Vector3;
typedef math::linear::Vector<double> VectorDouble;

%import "math_linear.i"
%import "except.i"

%include "std_string.i"
%include "std_vector.i"
%include "carrays.i"
%array_functions(double, doubleArray);

// Pickle utilities
%import <types.i> // for std::vector pickling
%pythoncode
%{
    from coda.coda_types import pickle
%}

%include "math/poly/OneD.h"
%extend math::poly::OneD
{
%pythoncode
%{
    def __setstate__(self, state):
        """Recursive unpickling method for SWIG-wrapped Poly1D."""
        self.__init__(pickle.loads(state.pop('coeffs')))

    def __getstate__(self):
        """Recursive pickling method for SWIG-wrapped Poly1D."""
        # Create a dictionary of parameters and values
        state = {}

        # Use swig_setmethods to get only data we can set later
        state['coeffs'] = pickle.dumps(self.coeffs())
        return state

%}
}

%template(Poly1D) math::poly::OneD<double>;
%template(Vector3Coefficients) std::vector<math::linear::VectorN<3, double> >;

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

    math::poly::OneD<double> __deepcopy__(PyObject* memo)
    {
        return math::poly::OneD<double>(*$self);
    }

    PyObject* __call__(PyObject* input)
    {
        if (!PySequence_Check(input))
        {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        Py_ssize_t N = PyObject_Length(input);
        PyObject* pyresult = PyList_New(N);
        for (Py_ssize_t i = 0; i < N; ++i)
        {
            PyObject* o = PySequence_GetItem(input, i);
            double val = (*self)(PyFloat_AsDouble(o));
            PyList_SetItem(pyresult, i, PyFloat_FromDouble(val));
        }
        return pyresult;
    }

    PyObject* asArray()
    {
        return numpyutils::toNumpyArray(1, self->size(), NPY_DOUBLE,
                &((*self)[0]));
    }

    %pythoncode
    %{
        @staticmethod
        def fromArray(array):
            return Poly1D(array.tolist())
    %}

}

%include "math/poly/TwoD.h"
%extend math::poly::TwoD
{

%pythoncode
%{
    def __setstate__(self, state):
        """Recursive unpickling method for SWIG-wrapped Poly2D."""
        self.__init__(pickle.loads(state.pop('coeffs')))

    def __getstate__(self):
        """Recursive pickling method for SWIG-wrapped Poly2D."""
        # Create a dictionary of parameters and values
        state = {}

        # Use swig_setmethods to get only data we can set later
        state['coeffs'] = pickle.dumps(self.coeffs())
        return state
%}
}


%template(Poly2D) math::poly::TwoD<double>;
%template(Poly1DVector) std::vector<math::poly::OneD<double>>;

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

    math::poly::TwoD<double> __deepcopy__(PyObject* memo)
    {
        return math::poly::TwoD<double>(*$self);
    }

    PyObject* __call__(PyObject* x_input, PyObject* y_input)
    {
        if (!PySequence_Check(x_input))
        {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        if (!PySequence_Check(y_input))
        {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        Py_ssize_t N = PyObject_Length(x_input);
        if (N != PyObject_Length(y_input))
        {
            PyErr_SetString(PyExc_ValueError,"Input sequences must have same length");
            return NULL;
        }
        PyObject* pyresult = PyList_New(N);
        for (Py_ssize_t i = 0; i < N; ++i)
        {
            PyObject* ox = PySequence_GetItem(x_input, i);
            PyObject* oy = PySequence_GetItem(y_input, i);
            double val = (*self)(PyFloat_AsDouble(ox), PyFloat_AsDouble(oy));
            PyList_SetItem(pyresult, i, PyFloat_FromDouble(val));
        }
        return pyresult;
    }

    PyObject* asArray()
    {
        size_t numRows = self->orderX() + 1;
        size_t numColumns = self->orderY() + 1;
        std::vector<void*> rows(numRows);
        for (size_t ii = 0; ii < rows.size(); ++ii)
        {
            rows[ii] = &((*self)[ii][0]);
        }
        return numpyutils::toNumpyArray(numColumns, NPY_DOUBLE, rows);
    }
    %pythoncode
    %{
        @staticmethod
        def fromArray(array):
            twoD = Poly2D(array.shape[0] - 1, array.shape[1] - 1)
            for i in range(len(array)):
                for j in range(len(array[0])):
                    twoD[(i,j)] = array[i][j]
            return twoD
    %}

}

%include "math/poly/Fit.h"
%template(FitVectorDouble) math::poly::fit<VectorDouble>;

// Define Python bindings for math::poly::OneD<Vector3> to be used by tests
%template(PolyVector3) math::poly::OneD<Vector3>;
%extend math::poly::OneD<Vector3>
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

        std::string __str__()
        {
            std::ostringstream ostr;
            ostr << *self;
            return ostr.str();
        }

        math::poly::OneD<Vector3> __deepcopy__(PyObject* memo)
        {
            return math::poly::OneD<Vector3>(*$self);
        }

	PyObject* __call__(PyObject* input)
        {
            if (!PySequence_Check(input))
            {
                PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
                return NULL;
            }
            Vector3* vec_ptr;
            PyObject* pytmp = 0;
            Py_ssize_t N = PyObject_Length(input);
            PyObject* pyresult = PyList_New(N);
            for (Py_ssize_t i = 0; i < N; ++i)
            {
                PyObject* o = PySequence_GetItem(input, i);
                vec_ptr = (Vector3*)new Vector3( (*self)(PyFloat_AsDouble(o)) );
                pytmp = SWIG_NewPointerObj(SWIG_as_voidptr(vec_ptr),
                                           SWIGTYPE_p_math__linear__VectorNT_3_double_t,
                                           0 | 0);
                PyList_SetItem(pyresult, i, pytmp);
            }
            return pyresult;
        }
};

// Define Python bindings for std::vector<double> to be used by tests
%template(StdVectorDouble) std::vector<double>;
%extend std::vector<double>
{
    public:
        double __getitem__(long i)
        {
            return (*self)[i];
        }

        void __setitem__(long i, double val)
        {
            (*self)[i] = val;
        }

        std::string __str__()
        {
            std::ostringstream ostr;

            ostr << "std::vector<double>[ ";
            for (int i = 0; i < self->size(); i++)
            {
                ostr << (*self)[i] << " ";
            }
            ostr << "]";

            return ostr.str();
        }
};
