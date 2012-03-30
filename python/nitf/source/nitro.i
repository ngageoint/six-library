/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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


%typemap(out) nitf_Uint32, nitf_Int32{$result = PyInt_FromLong($1);}
%typemap(in) nitf_Uint32{$1 = (nitf_Uint32)PyInt_AsLong($input);}
%typemap(out) nitf_Off{$result = PyLong_FromLong($1);}
%typemap(in) nitf_Off{$1 = (nitf_Off)PyLong_AsLong($input);}

/**
 * Setup some typemaps for the destructors
 */
%typemap(in) nitf_DataSource**{
    if ($input)
    {
        nitf_DataSource* temp = 0;
        if (SWIG_ConvertPtr($input, &temp, $descriptor(nitf_DataSource *), 0 |  0 ) != -1 && temp)
            $1 = (nitf_DataSource**)&temp;
    }
}

%typemap(in) nitf_Writer**{
    if ($input)
    {
        nitf_Writer* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_Writer*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_Writer**)&temp;
    }
}
%typemap(in) nitf_ImageWriter**{
    if ($input)
    {
        nitf_ImageWriter* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_ImageWriter*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_ImageWriter**)&temp;
    }
}
%typemap(in) nitf_ImageSource**{
    if ($input)
    {
        nitf_ImageSource* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_ImageSource*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_ImageSource**)&temp;
    }
}
%typemap(in) nitf_ImageReader**{
    if ($input)
    {
        nitf_ImageReader* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_ImageReader*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_ImageReader**)&temp;
    }
}
%typemap(in) nitf_Reader**{
    if ($input)
    {
        nitf_Reader* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_Reader*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_Reader**)&temp;
    }
}
%typemap(in) nitf_DownSampler**{
    if ($input)
    {
        nitf_DownSampler* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_DownSampler*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_DownSampler**)&temp;
    }
}
%typemap(in) nitf_Record**{
    if ($input)
    {
        nitf_Record* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_Record*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_Record**)&temp;
    }
}

%typemap(in) nitf_TREEnumerator**{
    if ($input)
    {
        nitf_TREEnumerator* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_TREEnumerator*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_TREEnumerator**)&temp;
    }
}

%typemap(in) nitf_SegmentReader**{
    if ($input)
    {
        nitf_SegmentReader* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_SegmentReader*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_SegmentReader**)&temp;
    }
}

%typemap(in) nitf_SegmentSource**{
    if ($input)
    {
        nitf_SegmentSource* temp = 0;
        if (SWIG_ConvertPtr($input, &temp,$descriptor(nitf_SegmentSource*), 0 |  0 ) != -1 && temp)
            $1 = (nitf_SegmentSource**)&temp;
    }
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


%typemap(out) nitf_Uint8 {$result = SWIG_From_char((char)($1));}


%include "nrt/Defines.h"
%include "nrt/Types.h"
%include "nitf/Defines.h"
%include "nitf/Types.h"
%include "nrt/Error.h"
%include "nrt/IOHandle.h"
%include "nrt/IOInterface.h"
%include "nitf/System.h"



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
    
    nitf_Uint32 py_Field_getInt(nitf_Field *field, nitf_Error *error)
    {
        nitf_Uint32 intVal;
        NITF_TRY_GET_UINT32(field, &intVal, error);
        return intVal;
        
      CATCH_ERROR:
        PyErr_SetString(PyExc_RuntimeError, "Unable to convert Field to int");
        return 0;
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
        nitf_Uint32 num;
        
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



%inline %{
    
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
        window->bandList = (nitf_Uint32*)NITF_MALLOC(sizeof(nitf_Uint32) * window->numBands);
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
        nitf_Uint8 **buf = NULL;
        PyObject* result = NULL;
        int i, padded, rowSkip, colSkip, subimageSize;
        nitf_SubWindow* newWindow = nitf_SubWindow_construct(error);
        
        rowSkip = window->downsampler ? window->downsampler->rowSkip : 1;
        colSkip = window->downsampler ? window->downsampler->colSkip : 1;
        subimageSize = (window->numRows/rowSkip) * (window->numCols/colSkip) * NITF_NBPP_TO_BYTES(nbpp);
        
        buf = (nitf_Uint8**) NITF_MALLOC(sizeof(nitf_Uint8*));
        if (!buf)
        {
            PyErr_NoMemory();
            goto CATCH_ERROR;
        }
        
        /* copy the window */
        newWindow->downsampler = window->downsampler;
        newWindow->startRow = window->startRow;
        newWindow->startCol = window->startCol;
        newWindow->numRows = window->numRows;
        newWindow->numCols = window->numCols;
        newWindow->numBands = 1;
        
        result = PyList_New(window->numBands);
        for (i = 0; i < window->numBands; i++)
        {
            PyObject* buffObj = PyBuffer_New(subimageSize * sizeof(nitf_Uint8));
            if (!buffObj) goto CATCH_ERROR;
            buffObj->ob_type->tp_as_buffer->bf_getwritebuffer(buffObj, 0, (void **)&buf[0]);
            
            newWindow->bandList = &window->bandList[i];
            if (!nitf_ImageReader_read(reader, newWindow, buf, &padded, error))
            {
                nitf_Error_print(error, stderr, "Read failed");
                goto CATCH_ERROR;
            }
            PyList_SetItem(result, i, buffObj);
        }
        /* destroy temp window */
        newWindow->bandList = NULL;
        newWindow->downsampler = NULL;
        nitf_SubWindow_destruct(&newWindow);
        
        NITF_FREE(buf);
        return result;
      
      CATCH_ERROR:
        if (buf) NITF_FREE(buf);
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
        char* buf = NULL;
        PyObject* bufObj = NULL;
        buf = NITF_MALLOC(size);
        
        if (!buf)
        {
            PyErr_NoMemory();
            return NULL;
        }
        
        source->iface->read(source->data, buf, size, error);
        bufObj = PyBuffer_FromMemory((void*)buf, size);
        return bufObj;
    }
    
    PyObject* py_SegmentReader_read(nitf_SegmentReader* reader, size_t size, nitf_Error* error)
    {
        char* buf = NULL;
        PyObject* bufObj = NULL;
        buf = NITF_MALLOC(size);
        
        if (!buf)
        {
            PyErr_NoMemory();
            return NULL;
        }
        
        nitf_SegmentReader_read(reader, buf, size, error);
        bufObj = PyBuffer_FromMemory((void*)buf, size);
        return bufObj;
    }
    
    PyObject* py_IOHandle_read(nitf_IOHandle handle, size_t size, nitf_Error* error)
    {
        char* buf = NULL;
        PyObject* bufObj = NULL;
        buf = NITF_MALLOC(size);
        
        if (!buf)
        {
            PyErr_NoMemory();
            return NULL;
        }
            
        nitf_IOHandle_read(handle, buf, size, error);
        bufObj = PyBuffer_FromMemory((void*)buf, size);
        return bufObj;
    }
%}
