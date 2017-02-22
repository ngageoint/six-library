import inspect
from functools import wraps
import numpy
from coda.math_poly import Poly1D, Poly2D

def isSmartPointer(typeArg):
    return (('ScopedCloneable' in typeArg.__name__ or
            'ScopedCopyable' in typeArg.__name__)
            and 'Vector' not in typeArg.__name__)

def unwrapObject(obj, typemap, key):
    if obj is None:
        return None

    if type(obj).__name__ == 'instancemethod':
        return obj

    if isSmartPointer(typemap[key]) and hasattr(obj, 'get'):
        return unwrapObject(typemap[key](obj.get()), typemap, key)

    if typemap[key] == Poly1D:
        unwrappedPoly = Poly1D()
        unwrappedPoly.thisown = 1
        for ii in range(len(obj)):
            unwrappedPoly.coeffs().push_back(obj[ii])
        return unwrappedPoly

    if typemap[key] == Poly2D:
        return Poly2D.fromArray(obj)

    if isinstance(obj, list):
        # We only need the source for getting type info, so it should
        # work to just pass the first element
        # TODO: list handle in map making
        return [unwrapObject(element, typemap, key) for element in obj]

    if hasattr(obj, '__dict__') and not inspect.ismethod(obj):
        unwrappedObject = type(typemap[key].__name__, (object,), {})()
        cleanedKey = key + '.'
        if key == '':
            cleanedKey = ''
        for member, value in inspect.getmembers(obj):
            if not member.startswith('_'):
                childKey = cleanedKey + member
                setattr(unwrappedObject, member,
                        unwrapObject(value, typemap, childKey))
        return unwrappedObject

    return obj

def wrapObject(obj, parent):
    # TODO: Kill typemap. Unwrap in parallel with source
    # What's going on with vectors?
    typemap = {}
    #if parent != '':
        #parent += '.'

    if ('Vector' in type(obj).__name__ and
        hasattr(obj, 'pop')):
        wrappedVector = []
        for ii in range(obj.size()):
            wrappedElement, typemap = wrapObject(obj[ii], parent)
            wrappedVector.append(wrappedElement)
        return wrappedVector, typemap

    if isSmartPointer(type(obj)):
        return wrapObject(obj.get(), parent)

    if isinstance(obj, list):
        return [wrapObject(element) for element in obj]

    if isinstance(obj, Poly1D):
        wrappedPoly = numpy.zeros(obj.size())
        for ii in range(obj.size()):
            wrappedPoly[ii] = obj[ii]
        return wrappedPoly

    if isinstance(obj, Poly2D):
        return obj.asArray()

    if hasattr(obj, '__dict__') and not inspect.ismethod(obj):
        wrappedObject = type('Wrapped' + type(obj).__name__, (object,), {})()
        for member, value in inspect.getmembers(obj):
            if not member.startswith('_'):
                setattr(wrappedObject, member, wrapObject(value))
        return wrappedObject

    return obj

def storeOriginalTypes(obj, parent):
    typemap = {}
    if parent != '':
        parent += '.'

    if (hasattr(obj, 'pop') and hasattr(obj, 'push_back') and not obj.empty()):
        sys.exit(type(obj))
    if hasattr(obj, '__dict__') and not inspect.ismethod(obj):
        for member, value in inspect.getmembers(obj):
            key = parent + member
            if not member.startswith('_'):
                typemap[key] = type(value)
                if not (isSmartPointer(type(value)) and value.get() is None):
                    typemap.update(storeOriginalTypes(value, key))
    return typemap

def toPython(obj):
    typemap, wrappedObject = wrapObject(obj, '')
    typemap[''] = type(obj)
    wrappedObject._typemap = typemap
    return wrappedObject

def fromPython(obj):
    return unwrapObject(obj, obj._typemap, '')

