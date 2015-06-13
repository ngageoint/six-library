%module sio_lite

%feature("autodoc", "1");
%include "sys.i"
%include "io.i"

%{
    #include "import/sio/lite.h"
%}

%extend sio::lite::FileWriter
{
    void write(sio::lite::FileHeader* header, size_t data)
    {
        const void* buffer = reinterpret_cast<const void*>(data);
        $self->write(header, buffer);
    }
};

%extend sio::lite::StreamReader
{
    sys::SSize_T read(size_t data, size_t size)
    {
        sys::byte* buffer = reinterpret_cast<sys::byte*>(data);
        return $self->read(buffer, size);
    }
};


%include "sio/lite/FileHeader.h"
%include "sio/lite/FileWriter.h"
%include "sio/lite/StreamReader.h"
%include "sio/lite/FileReader.h"

%pythoncode %{
def write(numpyArray, outputPathname, elementType = None):
    # Make sure this array is sized properly
    if len(numpyArray.shape) != 2:
        raise Exception("Only 2 dimensional images are supported")

    if elementType == None:
        signedType = ['int8', 'int16', 'int32', 'int64']
        unsignedType = ['uint8', 'uint16', 'uint32', 'uint64']
        floatType = ['float32', 'float64']
        complexFloatType = ['complex64', 'complex128']
        
        if numpyArray.dtype in signedType:
            elementType = FileHeader.SIGNED
        elif numpyArray.dtype in unsignedType:
            elementType = FileHeader.UNSIGNED
        elif numpyArray.dtype in floatType:
            elementType = FileHeader.FLOAT
        elif numpyArray.dtype in complexFloatType:
            elementType = FileHeader.COMPLEX_FLOAT
        else:
            raise Exception("Unknown element type: " + str(numpyArray.dtype))
    
    if not numpyArray.flags['C_CONTIGUOUS']:
        numpyArray = numpy.ascontiguousarray(numpyArray)
            
    header = FileHeader(numpyArray.shape[0], numpyArray.shape[1], numpyArray.strides[1], elementType);
    
    pointer, ro = numpyArray.__array_interface__['data']
    
    if pointer == 0 or pointer == None:
        raise Exception("Attempting to write a NULL image")
    
    writer = FileWriter(outputPathname)
    writer.write(header, pointer)
%}

%pythoncode %{
import numpy

def read(inputPathname):
    reader = FileReader(inputPathname)
    header = reader.getHeader()
    
    elementType = header.getElementType()
    elementSize = header.getElementSize()
    
    if elementType == FileHeader.UNSIGNED:
        dtype = 'uint'
    elif elementType == FileHeader.SIGNED:
        dtype = 'int'
    elif elementType == FileHeader.FLOAT:
        dtype = 'float'
    elif elementType == FileHeader.COMPLEX_FLOAT:
        dtype = 'complex'
    else:
        raise Exception("Unknown element type: " + str(elementType))
        
    dtype += str(elementSize * 8)

    numpyArray = numpy.empty(shape = (header.getNumLines(), header.getNumElements()), dtype = dtype)
    pointer, ro = numpyArray.__array_interface__['data']
    reader.read(pointer, numpyArray.shape[0] * numpyArray.shape[1] * elementSize)
    return numpyArray;
%}





