%module(package="coda") sio_lite

%feature("autodoc", "1");
%import "sys.i"
%import "io.i"

%import "except.i"

%{
    #include "import/sio/lite.h"
%}

// NOTE: In the cases below, need to use 'long long' rather
//       than 'size_t'.  Otherwise, Swig will generate code
//       using PyInt_FromLong().  This will work fine on
//       64-bit Unix where sizeof(long) == 8 and works fine
//       on 64-bit Windows where sizeof(long) == 4... until
//       a value gets too large to represent in 4 bytes at
//       which point you'll get cryptic/confusing runtime
//       errors.  This happens in particular when trying to
//       send NumPy arrays to/from C++ when you allocate an
//       array > 4 GB.  It seems like Swig should be smarter
//       in what it auto-generates to avoid this.
%extend sio::lite::FileWriter
{
    void write(sio::lite::FileHeader* header, long long data)
    {
        const void* buffer = reinterpret_cast<const void*>(data);
        $self->write(header, buffer);
    }
};

%extend sio::lite::StreamReader
{
    sys::SSize_T read(long long data, long long size)
    {
        sys::byte* buffer = reinterpret_cast<sys::byte*>(data);
        return $self->read(buffer, size);
    }
};


%include "sio/lite/FileHeader.h"
%include "sio/lite/FileWriter.h"
%include "sio/lite/StreamReader.h"
%include "sio/lite/FileReader.h"

%pythoncode
%{
import numpy

def dtypeFromSioType(elementType, elementSize):
    """
    Convert an SIO type & size to a NumPy dtype
    Complex integer types are not supported
    """
    typeMap = {FileHeader.UNSIGNED: 'uint',
               FileHeader.SIGNED: 'int',
               FileHeader.FLOAT: 'float',
               FileHeader.COMPLEX_FLOAT: 'complex'}

    if not elementType in typeMap:
        raise Exception("Unknown element type: " + str(elementType))

    dtypeStr = "%s%s" % (typeMap[elementType], elementSize * 8)

    return numpy.dtype(dtypeStr)


def sioTypeFromDtype(dtype):
    """
    Convert a NumPy dtype into an SIO type & size
    """
    # Handle dtypes, strings, numpy types
    dt = numpy.dtype(dtype)

    kindToType = {'i': FileHeader.SIGNED,
                  'u': FileHeader.UNSIGNED,
                  'f': FileHeader.FLOAT,
                  'c': FileHeader.COMPLEX_FLOAT}

    if not dt.kind in kindToType:
        raise Exception("Unknown element type: " + str(dt.kind))

    return kindToType[dt.kind]
%}

%pythoncode
%{
def write(numpyArray, outputPathname, elementType = None):
    # Make sure this array is sized properly
    if len(numpyArray.shape) != 2:
        raise Exception("Only 2 dimensional images are supported")
    if elementType is None:
        elementType = numpyArray.dtype

    if type(elementType) != int:
        elementType = sioTypeFromDtype(elementType)

    if not numpyArray.flags['C_CONTIGUOUS']:
        numpyArray = numpy.ascontiguousarray(numpyArray)

    header = FileHeader(numpyArray.shape[0],
                        numpyArray.shape[1],
                        numpyArray.strides[1],
                        elementType);

    pointer, ro = numpyArray.__array_interface__['data']

    if pointer == 0 or pointer == None:
        raise Exception("Attempting to write a NULL image")

    writer = FileWriter(outputPathname)
    writer.write(header, pointer)
%}

%pythoncode
%{
def read(inputPathname):
    reader = FileReader(inputPathname)
    header = reader.getHeader()

    elementSize = header.getElementSize()
    dtype = dtypeFromSioType(header.getElementType(), elementSize)

    numpyArray = numpy.empty(shape = (header.getNumLines(),
                                      header.getNumElements()),
                             dtype = dtype)
    pointer, ro = numpyArray.__array_interface__['data']
    reader.read(pointer, numpyArray.shape[0] * numpyArray.shape[1] * elementSize)
    return numpyArray;
%}
