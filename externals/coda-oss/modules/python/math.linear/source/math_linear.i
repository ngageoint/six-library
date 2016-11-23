%module(package="coda") math_linear

%feature("autodoc", "1");

%{
#include "math/linear/VectorN.h"
#include "math/linear/Vector.h"
#include "math/linear/MatrixMxN.h"
#include "math/linear/Matrix2D.h"
%}

%import "except.i"

%include "std_vector.i"
%include "std_string.i"

%ignore *::operator=;

%include "math/linear/MatrixMxN.h"
%include "math/linear/Matrix2D.h"
%include "math/linear/VectorN.h"
%include "math/linear/Vector.h"

// Pickle utilities
%pythoncode
%{
from coda.coda_types import pickle
%}
%import <types.i>

%extend math::linear::VectorN
{
%pythoncode
%{
    def __getstate__(self):
        return pickle.dumps(self.vals())
    def __setstate__(self, state):
        self.__init__(pickle.loads(state))
%}
}


%template(std_vector_double) std::vector<double>;
%template(std_vector_vector_double) std::vector<std::vector<double> >;

// define template variants we want in python
%template(Matrix1x1) math::linear::MatrixMxN<1,1,double>;
%template(Matrix1x2) math::linear::MatrixMxN<1,2,double>;
%template(Matrix1x3) math::linear::MatrixMxN<1,3,double>;
%template(Matrix2x1) math::linear::MatrixMxN<2,1,double>;
%template(Matrix2x2) math::linear::MatrixMxN<2,2,double>;
%template(Matrix2x3) math::linear::MatrixMxN<2,3,double>;
%template(Matrix2x7) math::linear::MatrixMxN<2,7,double>;
%template(Matrix3x1) math::linear::MatrixMxN<3,1,double>;
%template(Matrix3x2) math::linear::MatrixMxN<3,2,double>;
%template(Matrix3x3) math::linear::MatrixMxN<3,3,double>;
%template(Matrix3x7) math::linear::MatrixMxN<3,7,double>;
%template(Matrix7x2) math::linear::MatrixMxN<7,2,double>;
%template(Matrix7x3) math::linear::MatrixMxN<7,3,double>;
%template(Matrix7x7) math::linear::MatrixMxN<7,7,double>;
%template(Vector2) math::linear::VectorN<2, double>;
%template(Vector3) math::linear::VectorN<3, double>;
%template(cross) math::linear::cross<double>;
%template(VectorDouble) math::linear::Vector<double>;
%template(MatrixDouble) math::linear::Matrix2D<double>;

%extend math::linear::VectorN<2,double> {
    // SWIG doesn't automatically generate [] operator
    double __getitem__(long i) { return (*self)[i]; };
    void __setitem__(long i, double val) { (*self)[i] = val; };

    // string representation in python
    std::string __str__() {
        std::ostringstream strStream;
        strStream << *self;
        return strStream.str();
    }

    math::linear::VectorN<2, double> __deepcopy__(PyObject* memo)
    {
        return math::linear::VectorN<2, double>(*$self);
    }

    // helper method to facilitate creating a numpy array from Vector2
    // v = ml.Vector2([1.0, 1.0])
    // arr = np.asarray(v.vals())
    //
    // ideally we should implement __array__() instead, which will allow it
    // to be used by most numpy functions
    std::vector<double> vals() {
        return self->matrix().col(0);
    }
}

%extend math::linear::VectorN<3,double> {
    // SWIG doesn't automatically generate [] operator
    double __getitem__(long i) { return (*self)[i]; };
    void __setitem__(long i, double val) { (*self)[i] = val; };

    // string representation in python
    std::string __str__() {
        std::ostringstream strStream;
        strStream << *self;
        return strStream.str();
    }

    math::linear::VectorN<3, double> __deepcopy__(PyObject* memo)
    {
        return math::linear::VectorN<3, double>(*$self);
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

%extend math::linear::Vector<double>
{
    // SWIG doesn't automatically generate [] operator
    double __getitem__(long i)
    {
        if(i < 0 || i > self->size() - 1)
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return 0.0;
        }
        return (*self)[i];
    }
    void __setitem__(long i, double val)
    {

        if(i < 0 || i > self->size() - 1)
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return;
        }
        (*self)[i] = val;
    }

    // string representation in python
    std::string __str__()
    {
        std::ostringstream strStream;
        strStream << *self;
        return strStream.str();
    }

    // helper method to facilitate creating a numpy array from VectorDouble
    // v= ml.VectorDouble([1.0, 1.0, 1.0])
    // arr = np.asarray(v.vals())
    //
    // ideally we should implement __array__() instead, which will allow it
    // to be used by most numpy functions
    std::vector<double> vals()
    {
        return self->matrix().col(0);
    }
}

%extend math::linear::Matrix2D<double>
{
    // SWIG doesn't automatically generate [] operator
    double __getitem__(PyObject* inObj)
    {
        if (!PyTuple_Check(inObj))
        {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple");
            return 0.0;
        }
        Py_ssize_t m, n;
        if (!PyArg_ParseTuple(inObj, "nn", &m, &n))
        {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple of the form (size_t, size_t)");
            return 0.0;
        }
        if (m < 0 || m > self->rows() - 1 ||
            n < 0 || n > self->cols() - 1)
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return 0.0;
        }
        return (*self)(m,n);
    }
    void __setitem__(PyObject* inObj, double val)
    {
        if (!PyTuple_Check(inObj))
        {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple");
            return;
        }
        Py_ssize_t m, n;
        if (!PyArg_ParseTuple(inObj, "nn", &m, &n))
        {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple of the form (size_t, size_t)");
            return;
        }
        if (m < 0 || m > self->rows() - 1 ||
            n < 0 || n > self->cols() - 1)
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return;
        }
        (*self)(m,n) = val;
    }

    // string representation in python
    std::string __str__()
    {
        std::ostringstream strStream;
        strStream << *self;
        return strStream.str();
    }

    // helper method to facilitate creating a numpy array from Matrix2D
    // v= ml.VectorDouble([1.0, 1.0, 1.0])
    // arr = np.asarray(v.vals())
    //
    // ideally we should implement __array__() instead, which will allow it
    // to be used by most numpy functions
    std::vector<std::vector<double> > vals()
    {
        std::vector<std::vector<double> > returnVals(self->rows());
        for (size_t m=0; m< self->rows(); m++)
        {
            returnVals[m] = std::vector<double>(self->cols());
            for (size_t n = 0; n < self->cols(); n++)
            {
                returnVals[m][n] = (*self)(m,n);
            }
        }
        return returnVals;
    }
}

%define ExtendMatrix(M, N)
%extend math::linear::MatrixMxN<M, N, double>
{
    // SWIG doesn't automatically generate [] operator
    double __getitem__(PyObject* inObj)
    {
        if (!PyTuple_Check(inObj))
        {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple");
            return 0.0;
        }
        Py_ssize_t m, n;
        if (!PyArg_ParseTuple(inObj, "nn", &m, &n))
        {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple of the form (size_t, size_t)");
            return 0.0;
        }
        if (m < 0 || m > self->rows() - 1 ||
            n < 0 || n > self->cols() - 1)
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return 0.0;
        }
        return (*self)(m,n);
    }
    void __setitem__(PyObject* inObj, double val)
    {
        if (!PyTuple_Check(inObj))
        {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple");
            return;
        }
        Py_ssize_t m, n;
        if (!PyArg_ParseTuple(inObj, "nn", &m, &n))
        {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple of the form (size_t, size_t)");
            return;
        }
        if (m < 0 || m > self->rows() - 1 ||
            n < 0 || n > self->cols() - 1)
        {
            PyErr_SetString(PyExc_ValueError, "Index out of range");
            return;
        }
        (*self)(m,n) = val;
    }

    // string representation in python
    std::string __str__()
    {
        std::ostringstream strStream;
        strStream << *self;
        return strStream.str();
    }

    // helper method to facilitate creating a numpy array from Matrix2D
    // v= ml.VectorDouble([1.0, 1.0, 1.0])
    // arr = np.asarray(v.vals())
    //
    // ideally we should implement __array__() instead, which will allow it
    // to be used by most numpy functions
    std::vector<std::vector<double> > vals()
    {
        std::vector<std::vector<double> > returnVals(self->rows());
        for (size_t m=0; m< self->rows(); m++)
        {
            returnVals[m] = std::vector<double>(self->cols());
            for (size_t n = 0; n < self->cols(); n++)
            {
                returnVals[m][n] = (*self)(m,n);
            }
        }
        return returnVals;
    }
}
%enddef


ExtendMatrix(1, 1);
ExtendMatrix(1, 2);
ExtendMatrix(1, 3);
ExtendMatrix(2, 1);
ExtendMatrix(2, 2);
ExtendMatrix(2, 3);
ExtendMatrix(2, 7);
ExtendMatrix(3, 1);
ExtendMatrix(3, 2);
ExtendMatrix(3, 3);
ExtendMatrix(3, 7);
ExtendMatrix(7, 2);
ExtendMatrix(7, 3);
ExtendMatrix(7, 7);

