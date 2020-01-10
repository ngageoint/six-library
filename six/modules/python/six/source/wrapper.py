import inspect
import numpy
import types
from coda.math_poly import Poly1D, Poly2D
from pysix.six_sicd import *
from pysix.six_base import *

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
    return [method for method in
            inspect.getmembers(obj, inspect.ismethod) if not
            method[0].startswith('_')]

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
    return isinstance(value, (int, float, str, type(None), bool, tuple))

def wrapVector(vector):
    wrappedVector = []
    for ii in range(vector.size()):
        wrappedVector.append(wrap(vector[ii]))
    return wrappedVector

def wrapMethod(obj, parent):

    def wrappedMethod(self, *args):
        unwrap(self)
        val = wrap(getattr(self._source, obj.__name__)(*args))
        updateWrapper(self)
        return val

    return wrappedMethod

def updateWrapper(wrapper):
    for (name, value) in getMemberVariables(wrapper._source):
        setattr(wrapper, name, wrap(value))

def wrap(obj):
    '''
    Dynamically create new type to represent the given-object in a more
    Python-friendly manner. Poly1D and Poly2D members will be converted to
    numpy arrays. You shouldn't segfault from misusing smart pointers.
    Vectors become lists.

    WARNING: The object you call this function on becomes a member of the
    wrapper class. I don't think it will cause any problems to continue
    using the original object after you call this function, but it is probably
    a bad idea.
    '''
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
    setattr(wrapper, '_source', obj)
    for (name, value) in getMemberVariables(obj):
        setattr(wrapper, name, wrap(value))
    for (name, value) in getMethods(obj):
        setattr(wrapper, name, types.MethodType(wrapMethod(value, obj), wrapper))
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
        unwrapImpl(wrapper, source.get())
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
                unwrapImpl(element, sourceElement)
            source[ii] = sourceElement
    else:
        for ii in range(len(wrapper)):
            source[ii] = unwrapImpl(wrapper[ii], source[ii])
    return source

def unwrap(wrapper):
    '''
    Return the original type from a dynamic wrapper.

    WARNING: The returned object is still a member of the wrapper. You should
    not continue using the wrapper after you unwrap it. Calling the wrapper's
    methods can change the data in the returned object.

    If you really need to continue using both, you can try calling clone
    on the result
    '''
    return unwrapImpl(wrapper, wrapper._source)

def unwrapImpl(wrapper, source):
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
            unwrapImpl(wrappedValue, unwrappedValue)
        setattr(source, name, unwrappedValue)
    return source

