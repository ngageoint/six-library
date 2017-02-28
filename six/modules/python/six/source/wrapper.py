import inspect
import numpy
import types
from coda.math_poly import Poly1D, Poly2D
from pysix.six_sicd import *

def getMemberVariables(obj):
    members = []
    for attribute in dir(obj):
        value = getattr(obj, attribute)
        if (not inspect.ismethod(value) and
            not inspect.isfunction(value) and
            not attribute.startswith('_') and
            not isinstance(value, types.BuiltinFunctionType) and
            not isinstance(value, types.BuiltinMethodType)):
            members.append((attribute, value))
    return members

def getMethods(obj):
    return inspect.getmembers(obj, inspect.ismethod)

def isPoly(value):
    polyTypes = ['Poly1D', 'Poly2D']
    return any(typename in type(value).__name__ for typename in polyTypes)

def isSmartPointer(value):
    return ('ScopedCloneable' in type(value).__name__ or
            'ScopedCopyable' in type(value).__name__)

def isVector(value):
    return ('Vector' in type(value).__name__ and
            'Vector3' not in type(value).__name__)

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

def unwrapPoly(wrapper, source):
    if 'Poly1D' in type(source).__name__:
        source = Poly1D.fromArray(wrapper)
    elif 'Poly2D' in type(source).__name__:
        source = Poly2D.fromArray(wrapper)
    else:
        raise Exception('Could not recognize {0} as Polynomial'.format(
                type(source).__name__))

    return source

def unwrapSmartPointer(wrapper, source):
    if wrapper is None:
        source.reset()
    else:
        constructorName = 'make' + type(source).__name__
        source = globals()[constructorName]()
        unwrap(wrapper, source)
    return source

def unwrapToVector(wrapper, source):
    # The extra element is used to figured out
    # type stuff during further unwrapping
    # We discard it at the end
    source.resize(len(wrapper))
    sampleElement = source[0]

    if isSmartPointer(sampleElement):
        constructorName = type(sampleElement).__name__
        constructorName = 'make' + constructorName
        for ii in range(len(wrapper)):
            sourceElement = globals()[constructorName]()
            element = wrapper[ii]
            if element is None:
                sourceElement.reset()
            else:
                unwrap(element, sourceElement)
        source[ii] = sourceElement
    else:
        for ii in range(len(wrapper)):
            source[ii] = unwrap(wrapper[ii], source[ii])
    return source

def unwrap(wrapper, source):
    if isLikeAPrimitive(source):
        return wrapper
    for (name, unwrappedValue) in getMemberVariables(source):
        wrappedValue = getattr(wrapper, name)
        if isLikeAPrimitive(unwrappedValue):
            unwrappedValue = wrappedValue
        elif 'PolyVector3' in type(unwrappedValue).__name__:
            unwrappedValue = wrappedValue
        elif isVector(unwrappedValue):
            if len(wrappedValue) == 0:
                unwrappedValue.clear()
            else:
                unwrappedValue = unwrapToVector(wrappedValue, unwrappedValue)
        elif isSmartPointer(unwrappedValue):
            unwrappedValue = unwrapSmartPointer(wrappedValue, unwrappedValue)
        elif isPoly(unwrappedValue):
            unwrappedValue = unwrapPoly(wrappedValue, unwrappedValue)
        else:
            unwrap(wrappedValue, unwrappedValue)
        setattr(source, name, unwrappedValue)
    return source

