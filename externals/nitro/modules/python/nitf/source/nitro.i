/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */


%module nitropy
%{
#include <import/nitf.h>
#include <numpyutils/numpyutils.h>
#include <iostream>
#include <limits>
%}

#define NITF_LIST_TO_PYTHON_LIST(_type) \
    nitf_Error _error; \
    nitf_ListIterator iter; \
    nitf_ListIterator end; \
    int count = 0; \
    int size = 0; \
    \
    if ($1) \
        size = nitf_List_size($1); \
    \
    $result = PyList_New(size); \
    if ($1) \
    { \
        iter = nitf_List_begin($1); \
        end = nitf_List_end($1); \
        while (nitf_ListIterator_notEqualTo(&iter, &end)) \
        { \
            _type *thisObj= \
                (_type *) nitf_ListIterator_get(&iter); \
            PyObject *o = SWIG_NewPointerObj(SWIG_as_voidptr(thisObj), $descriptor(_type*), 0 |  0 ); \
            PyList_SetItem($result,count,o); \
            nitf_ListIterator_increment(&iter); \
            count++; \
        } \
    }


%typemap(out) uint32_t, int32_t{$result = PyInt_FromLong($1);}
%typemap(in) uint32_t{$1 = (uint32_t)PyInt_AsLong($input);}
%typemap(out) nitf_Off{$result = PyLong_FromLong($1);}
%typemap(in) nitf_Off{$1 = (nitf_Off)PyLong_AsLong($input);}

/**
 * Setup some typemaps for the destructors
 */
%typemap(in) nitf_DataSource**{
    if ($input)
    {
        nitf_DataSource* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp, $descriptor(nitf_DataSource *), 0 |  0 ) != -1 && temp)
            $1 = (nitf_DataSource**)&temp;
    }
}

%typemap(in) nitf_Writer**{
    if ($input)
    {
        nitf_Writer* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_Writer*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_Writer**)&temp;
    }
}
%typemap(in) nitf_ImageWriter**{
    if ($input)
    {
        nitf_ImageWriter* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_ImageWriter*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_ImageWriter**)&temp;
    }
}
%typemap(in) nitf_ImageSource**{
    if ($input)
    {
        nitf_ImageSource* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_ImageSource*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_ImageSource**)&temp;
    }
}
%typemap(in) nitf_ImageReader**{
    if ($input)
    {
        nitf_ImageReader* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_ImageReader*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_ImageReader**)&temp;
    }
}
%typemap(in) nitf_Reader**{
    if ($input)
    {
        nitf_Reader* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_Reader*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_Reader**)&temp;
    }
}
%typemap(in) nitf_DownSampler**{
    if ($input)
    {
        nitf_DownSampler* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_DownSampler*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_DownSampler**)&temp;
    }
}
%typemap(in) nitf_Record**{
    if ($input)
    {
        nitf_Record* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_Record*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_Record**)&temp;
    }
}

%typemap(in) nitf_TREEnumerator**{
    if ($input)
    {
        nitf_TREEnumerator* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_TREEnumerator*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_TREEnumerator**)&temp;
    }
}

%typemap(in) nitf_SegmentReader**{
    if ($input)
    {
        nitf_SegmentReader* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_SegmentReader*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_SegmentReader**)&temp;
    }
}

%typemap(in) nitf_SegmentSource**{
    if ($input)
    {
        nitf_SegmentSource* temp = 0;
        if (SWIG_ConvertPtr($input, (void**)&temp,$descriptor(nitf_SegmentSource*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_SegmentSource**)&temp;
    }
}

%typemap(in) nrt_IOHandle{
    if ($input)
    {
    %#ifdef SWIGWIN
        $1 = (nrt_IOHandle) PyLong_AsUnsignedLongLongMask($input);
    %#else
        $1 = (nrt_IOHandle) PyInt_AsLong($input);
    %#endif
    }
}

%typemap(out) nrt_IOHandle{
    %#ifdef SWIGWIN
        $result = PyInt_FromSize_t($1);
    %#else
        $result = PyInt_FromLong((long long)($1));
    %#endif
}

/* meant for nitf_PluginRegistry_retrieveTREHandler */
%typemap(in) int *had_error {
    $1 = &$input;
};
/* this lets us create a truth function to see if a given TRE exists in the registry */
%typemap(out) NITF_PLUGIN_TRE_HANDLER_FUNCTION {
    if ($1 == NULL)
        $result = Py_False;
    else
        $result = Py_True;
}


%typemap(out) uint8_t {$result = SWIG_From_char((char)($1));}

/* NRT_FILE is supposed to expand to the current source file during
 * preprocessing (and similar with the other ignored values below).
 * By the time SWIG gets at it, it's just a hardcoded path to what
 * is almost certainly a useless location.
 */
%ignore NRT_FILE;
%ignore NRT_LINE;
%ignore NRT_FUNC;
%ignore NITF_FILE;
%ignore NITF_LINE;
%ignore NITF_FUNC;

%include "nrt/Defines.h"
%include "nrt/Types.h"
%include "nitf/Defines.h"
%include "nitf/Types.h"
%include "nrt/Error.h"
%include "nrt/IOHandle.h"
%include "nrt/IOInterface.h"
%include "nitf/System.h"
%include "nrt/nrt_config.h"
%include "nitf/nitf_config.h"



/** There is something weird with how SWIG/Python
 *  deal with names.. we can't include the header because
 * ___nitf_HashTable_defaultHash can't be found.
 * I think it has to do with the leading underscores...
 * Will comment out for now, since we don't access it from Python.
 */
%ignore __nrt_HashTable_defaultHash;



%nodefaultctor;        // Don't create default constructors

%include "nitf/PluginRegistry.h"
/* warning list */
%typemap(out) nitf_List* {	NITF_LIST_TO_PYTHON_LIST(nitf_FieldWarning) }
// for whatever reason, Reader and Writer don't expand properly?
%include "nitf/Reader.h"
%include "nitf/Writer.h"
%include "nitf/Record.h"
%include "nitf/Field.h"
%include "nitf/FileSecurity.h"
%include "nitf/TRE.h"
/* for TREs */
%typemap(out) nitf_List* {	NITF_LIST_TO_PYTHON_LIST(nitf_TRE) }
%include "nitf/Extensions.h"
%include "nrt/HashTable.h"
%include "nrt/Pair.h"
/* for bandsource list */
%typemap(out) nitf_List* {	NITF_LIST_TO_PYTHON_LIST(nitf_BandSource) }
%include "nitf/ImageSource.h"
%include "nitf/DataSource.h"
%include "nitf/BandSource.h"
%include "nitf/ImageWriter.h"


%include "nitf/FileHeader.h"
%include "nitf/ImageSegment.h"
/* for comments */
%typemap(out) nitf_List* {	NITF_LIST_TO_PYTHON_LIST(nitf_Field) }
%include "nitf/ImageSubheader.h"
%include "nitf/GraphicSegment.h"
%include "nitf/GraphicSubheader.h"
%include "nitf/LabelSegment.h"
%include "nitf/LabelSubheader.h"
%include "nitf/TextSegment.h"
%include "nitf/TextSubheader.h"
%include "nitf/DESegment.h"
%include "nitf/DESubheader.h"
%include "nitf/RESegment.h"
%include "nitf/RESubheader.h"
%include "nitf/ComponentInfo.h"

%include "nitf/ImageReader.h"
%include "nitf/SegmentReader.h"
%include "nitf/SegmentWriter.h"
%include "nitf/SubWindow.h"
%include "nitf/DownSampler.h"
%include "nitf/SegmentSource.h"
%include "nitf/BandInfo.h"

%include "nitf/NitfWriter.h"
%include "nitf/NitfReader.h"

%clearnodefaultctor;   // Re-enable default constructors


// IOHandle
%inline %{

    #define PY_NITF_CREATE 1
    #define PY_NITF_TRUNCATE 12
    #define PY_NITF_OPEN_EXISTING 13
    #define PY_NITF_ACCESS_READONLY 1
    #define PY_NITF_ACCESS_WRITEONLY 2
    #define PY_NITF_ACCESS_READWRITE 3

    #define  PY_NITF_SEEK_CUR 1
    #define  PY_NITF_SEEK_SET 2
    #define  PY_NITF_SEEK_END 3

    nitf_IOHandle py_IOHandle_create(const char *fname,
            int accessFlag,
            int createFlag,
            nitf_Error * error)
    {
        int accessInt = 0;
        int createInt = 0;

        if (accessFlag == PY_NITF_ACCESS_READONLY)
        {
          accessInt = NITF_ACCESS_READONLY;
        }
        else if (accessFlag == PY_NITF_ACCESS_WRITEONLY)
        {
          accessInt = NITF_ACCESS_WRITEONLY;
        }
        else if (accessFlag == PY_NITF_ACCESS_READWRITE)
        {
          accessInt = NITF_ACCESS_READWRITE;
        }

        if (createFlag == PY_NITF_CREATE)
        {
          createInt = NITF_CREATE;
        }
        else if (createFlag == PY_NITF_TRUNCATE)
        {
          createInt = NITF_TRUNCATE;
        }
        else if (createFlag == PY_NITF_OPEN_EXISTING)
        {
          createInt = NITF_OPEN_EXISTING;
        }

        return nitf_IOHandle_create(fname, accessInt, createInt, error);
    }

    NRT_BOOL py_IOHandle_write(nitf_IOHandle handle, long long address,
        size_t size, nrt_Error* error)
    {
        return nrt_IOHandle_write(handle, (void*)(address), size, error);
    }

    nitf_Off py_IOHandle_seek(nitf_IOHandle handle,
            nitf_Off offset, int whence, nitf_Error * error)
    {
        int realWhence = NITF_SEEK_SET;

        if (whence == PY_NITF_SEEK_CUR)
            realWhence = NITF_SEEK_CUR;
        else if (whence == PY_NITF_SEEK_END)
            realWhence = NITF_SEEK_END;

        return nitf_IOHandle_seek(handle, offset, realWhence, error);
    }

%}

%typemap(in) (char* pfsrd_buf) {
    if ($input)
    {
        char *buffer;
        Py_ssize_t outlength;
        if(PyBytes_AsStringAndSize($input,&buffer,&outlength) == -1) {
            SWIG_fail;
        }
        $1 = buffer;
        //$2 = outlength;
    }
}
// Field
%inline %{

    char* py_Field_getString(nitf_Field * field,
                                      nitf_Error * error)
    {
        char* buf = (char*)NITF_MALLOC(field->length + 1);
        memset(buf, 0, field->length + 1);

        nitf_Field_get(field, (NITF_DATA*)buf, NITF_CONV_STRING, field->length + 1, error);
        return buf;
    }

    uint32_t py_Field_getInt(nitf_Field *field, nitf_Error *error)
    {
        uint32_t intVal;
        NITF_TRY_GET_UINT32(field, &intVal, error);
        return intVal;

      CATCH_ERROR:
        PyErr_SetString(PyExc_RuntimeError, "Unable to convert Field to int");
        return 0;
    }

    void py_Field_setRawData(nitf_Field *field, char* pfsrd_buf, int length,  nitf_Error *error)
    {
        nitf_Field_setRawData(field, (NITF_DATA*)pfsrd_buf, length, error);
    }

    void py_TRE_setField(nitf_TRE *tre, const char* tag, char* buf, int length, nitf_Error *error)
    {
        nitf_TRE_setField(tre, tag, (NITF_DATA*)buf, length, error);
    }

    PyObject* py_Field_getRawData(nitf_Field *field, nitf_Error *error)
    {
        return PyBytes_FromStringAndSize(field->raw, field->length);
    }

    nitf_TRE *py_TRE_clone(nitf_TRE *tre, nitf_Error *error)
    {
        return nitf_TRE_clone(tre, error);
    }

%}


// Record
%inline %{

    nitf_FileHeader* py_Record_getFileHeader(nitf_Record* record)
    {
        return record->header;
    }

    nitf_ImageSegment* py_Record_getImage(nitf_Record* record, int index, nitf_Error* error)
    {
        return (nitf_ImageSegment*)nitf_List_get(record->images, index, error);
    }

    int py_Record_getNumImages(nitf_Record* record)
    {
        return nitf_List_size(record->images);
    }

    nitf_GraphicSegment* py_Record_getGraphic(nitf_Record* record, int index, nitf_Error* error)
    {
        return (nitf_GraphicSegment*)nitf_List_get(record->graphics, index, error);
    }

    int py_Record_getNumGraphics(nitf_Record* record)
    {
        return nitf_List_size(record->graphics);
    }

    nitf_LabelSegment* py_Record_getLabel(nitf_Record* record, int index, nitf_Error* error)
    {
        return (nitf_LabelSegment*)nitf_List_get(record->labels, index, error);
    }

    int py_Record_getNumLabels(nitf_Record* record)
    {
        return nitf_List_size(record->labels);
    }


    nitf_TextSegment* py_Record_getText(nitf_Record* record, int index, nitf_Error* error)
    {
        return (nitf_TextSegment*)nitf_List_get(record->texts, index, error);
    }

    int py_Record_getNumTexts(nitf_Record* record)
    {
        return nitf_List_size(record->texts);
    }



    nitf_DESegment* py_Record_getDE(nitf_Record* record, int index, nitf_Error* error)
    {
        return (nitf_DESegment*)nitf_List_get(record->dataExtensions, index, error);
    }

    int py_Record_getNumDEs(nitf_Record* record)
    {
        return nitf_List_size(record->dataExtensions);
    }


    nitf_RESegment* py_Record_getRE(nitf_Record* record, int index, nitf_Error* error)
    {
        return (nitf_RESegment*)nitf_List_get(record->reservedExtensions, index, error);
    }

    int py_Record_getNumREs(nitf_Record* record)
    {
        return nitf_List_size(record->reservedExtensions);
    }


    void py_Record_destruct(nitf_Record *record)
    {
        nitf_Record_destruct(&record);
    }

    nitf_Version py_Record_getVersion(nitf_Record * record)
    {
        return nitf_Record_getVersion(record);
    }

%}


// FileHeader
%inline %{

    nitf_ComponentInfo* py_FileHeader_getComponentInfo(nitf_FileHeader* header, int index, char* type, nitf_Error* error)
    {
        uint32_t num;

        if (!type)
        	goto CATCH_ERROR;

        if (strcmp(type, "image") == 0)
        {
        	NITF_TRY_GET_UINT32(header->numImages, &num, error);
        	if (index >= num) goto CATCH_ERROR;
        	return (nitf_ComponentInfo*)header->imageInfo[index];
        }
        else if (strcmp(type, "graphic") == 0)
        {
        	NITF_TRY_GET_UINT32(header->numGraphics, &num, error);
        	if (index >= num) goto CATCH_ERROR;
        	return (nitf_ComponentInfo*)header->graphicInfo[index];
       	}
        else if (strcmp(type, "label") == 0)
        {
        	NITF_TRY_GET_UINT32(header->numLabels, &num, error);
        	if (index >= num) goto CATCH_ERROR;
        	return (nitf_ComponentInfo*)header->labelInfo[index];
        }
        else if (strcmp(type, "text") == 0)
        {
        	NITF_TRY_GET_UINT32(header->numTexts, &num, error);
        	if (index >= num) goto CATCH_ERROR;
        	return (nitf_ComponentInfo*)header->textInfo[index];
        }
        else if (strcmp(type, "de") == 0)
        {
        	NITF_TRY_GET_UINT32(header->numDataExtensions, &num, error);
        	if (index >= num) goto CATCH_ERROR;
        	return (nitf_ComponentInfo*)header->dataExtensionInfo[index];
        }
        else if (strcmp(type, "re") == 0)
        {
        	NITF_TRY_GET_UINT32(header->numReservedExtensions, &num, error);
        	if (index >= num) goto CATCH_ERROR;
        	return (nitf_ComponentInfo*)header->reservedExtensionInfo[index];
        }

        CATCH_ERROR:
          return NULL;
    }
%}

// PluginRegistry wrapper functions
%inline %{

    bool py_nitf_PluginRegistry_canRetrieveTREHandler(nitf_PluginRegistry * reg, const char *ident, nitf_Error * error)
    {
        int had_error;
        nitf_TREHandler *result = nitf_PluginRegistry_retrieveTREHandler(reg, ident, &had_error, error);
        if (had_error) { return false; }
        else { return (result != NULL); }
    }
%}


%inline %{
    nitf_ImageReader* py_nitf_Reader_newImageReader(nitf_Reader* reader, int imageSegmentNumber, PyObject* options, nitf_Error* error)
    {
        /* TODO: Support taking in options here and converting it */
        nrt_HashTable* nullOptions = NULL;
        return nitf_Reader_newImageReader(reader, imageSegmentNumber, nullOptions, error);
    }

    nitf_Writer* py_nitf_Writer_construct(nitf_Error *error)
    {
        return nitf_Writer_construct(error);
    }

    bool py_nitf_Writer_prepare(nitf_Writer *writer, nitf_Record *record, nitf_IOHandle ioHandle, nitf_Error *error)
    {
        return nitf_Writer_prepare(writer, record, ioHandle, error);
    }

    void py_nitf_Writer_destruct(nitf_Writer ** writer)
    {
        nitf_Writer_destruct(writer);
    }

    nitf_ImageWriter* py_nitf_Writer_newImageWriter(nitf_Writer* writer, int index, PyObject* options, nitf_Error* error)
    {
        /* TODO: Support taking in options here and converting it */
        nrt_HashTable* nullOptions = NULL;
        return nitf_Writer_newImageWriter(writer, index, nullOptions, error);
    }

    nitf_BandSource* py_nitf_MemorySource_construct(long long data, nitf_Off size, nitf_Off start, int numBytesPerPixel, int pixelSkip, nitf_Error * error)
    {
        return nitf_MemorySource_construct((void*)(data), size, start, numBytesPerPixel, pixelSkip, error);
    }

    /**
     * Helper function for MemoryBandSourceconstructor
     */
    nitf_SegmentSource* py_nitf_SegmentMemorySource_construct(
        long long data, nitf_Off size, nitf_Off start, int byteSkip,
        NITF_BOOL copyData, nitf_Error *error)
    {
        return nitf_SegmentMemorySource_construct((char*)(data), size,
            start, byteSkip, copyData, error);
    }

    /**
     * Helper function for the SubWindow constructor
     */
    nitf_SubWindow* py_SubWindow_construct(int startRow, int startCol, int numRows, int numCols, PyObject* bandList, nitf_DownSampler* downSampler, nitf_Error *error)
    {
        int i;
        nitf_SubWindow* window = nitf_SubWindow_construct(error);
        /* TODO check for error */
        window->startRow = startRow;
        window->startCol = startCol;
        window->numRows = numRows;
        window->numCols = numCols;
        window->downsampler = downSampler;

        if (!PySequence_Check(bandList)) {
            PyErr_SetString(PyExc_ValueError,"Expected a sequence");
            return NULL;
          }

        window->numBands = PySequence_Length(bandList);
        if (window->numBands < 0) window->numBands = 0;
        window->bandList = (uint32_t*)NITF_MALLOC(sizeof(uint32_t) * window->numBands);
        if (!window->bandList)
        {
            PyErr_NoMemory();
            return NULL;
        }

      for (i = 0; i < window->numBands; i++) {
        PyObject *o = PySequence_GetItem(bandList,i);
        if (PyNumber_Check(o)) {
          window->bandList[i] = (int) PyInt_AsLong(o);
        } else {
          PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");
         /* TODO clean up memory */
          return NULL;
        }
        }
        return window;
    }

    /**
     * Helper function for ImageReader_read ... necessary
     */
    PyObject* py_ImageReader_read(nitf_ImageReader* reader, nitf_SubWindow* window, int nbpp, nitf_Error* error)
    {
        /* TODO somehow get the NUMBITSPERPIXEL in the future */
        uint8_t **buf = NULL;
        uint8_t *pyArrayBuffer = NULL;
        PyObject* result = Py_None;
        int padded, rowSkip, colSkip;
        uint64_t subimageSize;
        uint32_t i;
        types::RowCol<size_t> dims;

        rowSkip = window->downsampler ? window->downsampler->rowSkip : 1;
        colSkip = window->downsampler ? window->downsampler->colSkip : 1;
        subimageSize = static_cast<uint64_t>(window->numRows/rowSkip) *
                (window->numCols/colSkip) *
                nitf_ImageIO_pixelSize(reader->imageDeblocker);
        if (subimageSize > std::numeric_limits<size_t>::max())
        {
            nitf_Error_print(error, stderr,
                             "Image is too large for this system\n");
            PyErr_SetString(PyExc_MemoryError, "");
            goto CATCH_ERROR;
        }

        dims.row = window->numBands;
        dims.col = subimageSize;

        numpyutils::createOrVerify(result, NPY_UINT8, dims);

        buf = (uint8_t**) NITF_MALLOC(sizeof(uint8_t*) * window->numBands);
        if (!buf)
        {
            PyErr_NoMemory();
            goto CATCH_ERROR;
        }

        memset(buf, 0, sizeof(uint8_t*) * window->numBands);

        for (i = 0; i < window->numBands; ++i)
        {
            buf[i] = (uint8_t*) NITF_MALLOC(sizeof(uint8_t) * subimageSize);
            if (!buf[i])
            {
                PyErr_NoMemory();
                goto CATCH_ERROR;
            }
        }

        if (!nitf_ImageReader_read(reader, window,
               buf, &padded, error))
        {
            nitf_Error_print(error, stderr, "Read failed");
            PyErr_SetString(PyExc_RuntimeError, "");
            goto CATCH_ERROR;
        }

        // Copy to output numpy array
        pyArrayBuffer = numpyutils::getBuffer<uint8_t>(result);
        for (i = 0; i < window->numBands; ++i)
        {
            memcpy(pyArrayBuffer + i * subimageSize,
                   buf[i], sizeof(uint8_t) * subimageSize);
        }

        for (i = 0; i < window->numBands; ++i)
        {
            NITF_FREE(buf[i]);
        }
        NITF_FREE(buf);
        return result;

      CATCH_ERROR:
        if (buf)
        {
            for (i = 0; i < window->numBands; ++i)
            {
                NITF_FREE(buf[i]);
            }
            NITF_FREE(buf);
        }
        if (result) Py_CLEAR(result);
        return NULL;
    }


    nitf_Field* py_Pair_getFieldData(nitf_Pair* pair)
    {
        return (nitf_Field*)pair->data;
    }


    PyObject* py_TREEnumerator_hasNext(nitf_TREEnumerator **it)
    {
        if (!(*it)) Py_RETURN_FALSE;
        if ((*it) && (*it)->hasNext(it) == NITF_SUCCESS)
            Py_RETURN_TRUE;
        Py_RETURN_FALSE;
    }

    nitf_Pair* py_TREEnumerator_next(nitf_TREEnumerator *it, nitf_Error *error)
    {
        if (!it) return NULL;
        return it->next(it, error);
    }


    PyObject* py_DataSource_read(nitf_DataSource* source, size_t size, nitf_Error* error)
    {
        void* buf = NULL;
        PyObject* pyArray = Py_None;
        types::RowCol<size_t> dims;

        dims.row = 1;
        dims.col = size;
        numpyutils::createOrVerify(pyArray, NPY_INT8, dims);
        buf = numpyutils::getBuffer<void>(pyArray);

        if (!source)
        {
             PyErr_SetString(PyExc_RuntimeError,
                 "NULL ptr passed to py_DataSource_read");
             return NULL;
        }
        if (!buf)
        {
            PyErr_NoMemory();
            return NULL;
        }

        source->iface->read(source->data, buf, size, error);
        return pyArray;
    }

    PyObject* py_SegmentReader_read(nitf_SegmentReader* reader, size_t size, nitf_Error* error)
    {
        void* buf = NULL;
        PyObject* pyArray = Py_None;
        types::RowCol<size_t> dims;

        dims.row = 1;
        dims.col = size;
        numpyutils::createOrVerify(pyArray, NPY_INT8, dims);
        buf = numpyutils::getBuffer<void>(pyArray);

        if (!buf)
        {
            PyErr_NoMemory();
            return NULL;
        }

        nitf_SegmentReader_read(reader, buf, size, error);
        return pyArray;
    }

    PyObject* py_IOHandle_read(nitf_IOHandle handle, size_t size, nitf_Error* error)
    {
        void* buf = NULL;
        PyObject* pyArray = Py_None;
        types::RowCol<size_t> dims;

        dims.row = 1;
        dims.col = size;
        numpyutils::createOrVerify(pyArray, NPY_INT8, dims);
        buf = numpyutils::getBuffer<void>(pyArray);

        if (!buf)
        {
            PyErr_NoMemory();
            return NULL;
        }

        nitf_IOHandle_read(handle, buf, size, error);
        return pyArray;
    }

%}
