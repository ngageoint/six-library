import inspect
import numpy
import types
from coda.math_poly import Poly1D, Poly2D
from pysix.six_sicd import *

def getMemberVariables(obj):
    return [(attribute, getattr(obj, attribute)) for attribute in dir(obj)
            if not inspect.ismethod(getattr(obj, attribute))
            and not attribute.startswith('_')
            and not isinstance(getattr(obj, attribute), types.BuiltinFunctionType)
            and not isinstance(getattr(obj, attribute), types.BuiltinMethodType)]

def getMethods(obj):
    return inspect.getmembers(obj, inspect.ismethod)

def isPoly(value):
    polyTypes = ['Poly1D', 'Poly2D']
    return any(typename in type(value).__name__ for typename in polyTypes)

def isSmartPointer(value):
    return ('ScopedCloneable' in type(value).__name__ or
            'ScopedCopyable' in type(value).__name__)

def isVector(value):
    return 'Vector' in type(value).__name__

def isLikeAPrimitive(value):
    return isinstance(value, (types.IntType, types.FloatType, types.StringType,
            types.NoneType, types.BooleanType, types.TupleType, types.LongType))

def wrapVector(vector):
    wrappedVector = []
    for ii in range(vector.size()):
        wrappedVector.append(wrap(vector[ii]))
    return wrappedVector

def wrap(obj):
    if isLikeAPrimitive(obj):
        return obj
    # Not wrapping this for now, because not sure what the array
    # should look like
    elif 'PolyVector3' in type(obj).__name__:
        return obj
    elif isVector(obj):
        return wrapVector(obj)
    elif isSmartPointer(obj):
        return wrap(obj.get())
    elif isPoly(obj):
        return obj.asArray()
    wrapper = type(type(obj).__name__, (), {})
    for (name, value) in getMemberVariables(obj):
        setattr(wrapper, name, wrap(value))
    return wrapper

