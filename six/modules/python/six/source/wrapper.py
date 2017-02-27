import inspect
from functools import wraps
import numpy
import types
from coda.math_poly import Poly1D, Poly2D

def getMemberVariables(obj):
    return [(attribute, getattr(obj, attribute)) for attribute in dir(obj)
            if not inspect.ismethod(getattr(obj, attribute))
            and not attribute.startswith('_')]


def getMethods(obj):
    return inspect.getmembers(obj, inspect.ismethod)


def wrapMember(wrapper, name, value):
    setattr(wrapper, name, value)

def unwrapMember(obj, name, value):
    setattr(obj, name, value)

def reflectChangesInWrapper(wrapper, obj):
    for (name, value) in getMemberVariables(wrapper):
        unwrapMember(obj, name, value)

def reflectChangesInObject(wrapper, obj):
    for (name, value) in getMemberVariables(obj):
        wrapMember(wrapper, name, value)

def wrapMethod(obj, wrapper, name, method):
    def wrappedMethod(self, *args):
        reflectChangesInWrapper(wrapper, obj)
        returnValue = method(*args)
        reflectChangesInObject(wrapper, obj)
        return returnValue
    wrappedMethod.__doc__ = method.__doc__
    wrappedMethod.__name__ = method.__name__
    setattr(wrapper, name, types.MethodType(wrappedMethod, wrapper))


def wrap(obj):
    wrappedObject = type(type(obj).__name__, (), {})
    for (name, value) in getMemberVariables(obj):
        wrapMember(wrappedObject, name, value)
    for (name, method) in getMethods(obj):
        wrapMethod(obj, wrappedObject, name, method)
    return wrappedObject

