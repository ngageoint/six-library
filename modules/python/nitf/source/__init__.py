"""
 * =========================================================================
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
 *
"""

"""
Python wrapper for the NITRO library
"""
##############################################################################
import nitf.nitropy
from nitf.nitropy import NITF_VER_20, NITF_VER_21, NITF_VER_UNKNOWN
import logging, types
import numpy

from nitf.nitropy import nrt_Error as Error
Error.__repr__=lambda s: s.message

__all__ = ['BandInfo', 'BandSource', 'ComponentInfo', 'DESegment', 'DESubheader',
           'DataSource', 'DownSampler', 'Error', 'Extensions', 'Field',
           'FieldHeader', 'FileBandSource', 'FileHeader', 'FileSecurity',
           'FileSegmentSource', 'GraphicSegment', 'GraphicSubheader', 'Header',
           'IOHandle', 'ImageReader', 'ImageSegment', 'ImageSource',
           'ImageSubheader', 'ImageWriter', 'LabelSegment', 'LabelSubheader',
           'MaxDownSampler', 'MemoryBandSource', 'MemorySegmentSource',
           'NITF_VER_20', 'NITF_VER_21', 'NITF_VER_UNKNOWN',
           'PixelSkipDownSampler', 'PluginRegistry', 'RESegment', 'RESubheader',
           'Reader', 'Record', 'RecordSegment', 'SegmentReader',
           'SegmentSource', 'SegmentWriter', 'SubWindow', 'TRE', 'TextSegment',
           'TextSubheader', 'Writer', 'read', 'metadata']

FILE_HEADER_FIELDS = [
    {'id' : 'FHDR', 'name' : 'fileHeader', },
    {'id' : 'FVER', 'name' : 'fileVersion', },
    {'id' : 'CLEVEL', 'name' : 'complianceLevel', },
    {'id' : 'STYPE', 'name' : 'systemType', },
    {'id' : 'OSTAID', 'name' : 'originStationID', },
    {'id' : 'FDT', 'name' : 'fileDateTime', },
    {'id' : 'FTITLE', 'name' : 'fileTitle', },
    {'id' : 'FSCLAS', 'name' : 'classification', },
    {'id' : 'FSCOP', 'name' : 'messageCopyNum', },
    {'id' : 'FSCPYS', 'name' : 'messageNumCopies', },
    {'id' : 'ENCRYP', 'name' : 'encrypted', },
    {'id' : 'FBKGC', 'name' : 'backgroundColor', },
    {'id' : 'ONAME', 'name' : 'originatorName', },
    {'id' : 'OPHONE', 'name' : 'originatorPhone', },
    {'id' : 'FL', 'name' : 'fileLength', },
    {'id' : 'HL', 'name' : 'headerLength', },

    {'id' : 'NUMI', 'name' : 'numImages', },
    {'id' : 'NUMS', 'name' : 'numGraphics', },
    {'id' : 'NUMX', 'name' : 'numLabels', },
    {'id' : 'NUMT', 'name' : 'numTexts', },
    {'id' : 'NUMDES', 'name' : 'numDataExtensions', },
    {'id' : 'NUMRES', 'name' : 'numReservedExtensions', },

    {'id' : 'UDHDL', 'name' : 'userDefinedHeaderLength', },
    {'id' : 'UDHOFL', 'name' : 'userDefinedOverflow', },
    {'id' : 'XHDL', 'name' : 'extendedHeaderLength', },
    {'id' : 'XHDLOFL', 'name' : 'extendedHeaderOverflow', },
]

BAND_INFO_FIELDS = [
    {'id' : 'IREPBAND', 'name' : 'representation', },
    {'id' : 'ISUBCAT', 'name' : 'subcategory', },
    {'id' : 'IFC', 'name' : 'imageFilterCondition', },
    {'id' : 'IMFLT', 'name' : 'imageFilterCode', },
    {'id' : 'NLUTS', 'name' : 'numLUTs', },
    {'id' : 'NELUT', 'name' : 'bandEntriesPerLUT', },
]

IMAGE_SUBHEADER_FIELDS = [
    {'id' : 'IM', 'name' : 'filePartType', },
    {'id' : 'IID1', 'name' : 'imageId', },
    {'id' : 'IDATIM', 'name' : 'imageDateAndTime', },
    {'id' : 'TGTID', 'name' : 'targetId', },
    {'id' : 'IID2', 'name' : 'imageTitle', },
    {'id' : 'ISCLAS', 'name' : 'imageSecurityClass', },
    {'id' : 'ISORCE', 'name' : 'imageSource', },
    {'id' : 'NROWS', 'name' : 'numRows', },
    {'id' : 'NCOLS', 'name' : 'numCols', },
    {'id' : 'PVTYPE', 'name' : 'pixelValueType', },
    {'id' : 'IREP', 'name' : 'imageRepresentation', },
    {'id' : 'ICAT', 'name' : 'imageCategory', },
    {'id' : 'ABPP', 'name' : 'actualBitsPerPixel', },
    {'id' : 'PJUST', 'name' : 'pixelJustification', },
    {'id' : 'ICORDS', 'name' : 'imageCoordinateSystem', },
    {'id' : 'IGEOLO', 'name' : 'cornerCoordinates', },
    {'id' : 'NICOM', 'name' : 'numImageComments', },

    {'id' : 'IC', 'name' : 'imageCompression', },
    {'id' : 'COMRAT', 'name' : 'compressionRate', },
    {'id' : 'NBANDS', 'name' : 'numImageBands', },
    {'id' : 'XBANDS', 'name' : 'numMultispectralImageBands', },

    {'id' : 'ISYNC', 'name' : 'imageSyncCode', },
    {'id' : 'IMODE', 'name' : 'imageMode', },
    {'id' : 'NBPR', 'name' : 'numBlocksPerRow', },
    {'id' : 'NBPC', 'name' : 'numBlocksPerCol', },
    {'id' : 'NPPBH', 'name' : 'numPixelsPerHorizBlock', },
    {'id' : 'NPPBV', 'name' : 'numPixelsPerVertBlock', },
    {'id' : 'NBPP', 'name' : 'numBitsPerPixel', },
    {'id' : 'IDLVL', 'name' : 'imageDisplayLevel', },
    {'id' : 'IALVL', 'name' : 'imageAttachmentLevel', },
    {'id' : 'ILOC', 'name' : 'imageLocation', },
    {'id' : 'IMAG', 'name' : 'imageMagnification', },

    {'id' : 'UDIDL', 'name' : 'userDefinedImageDataLength', },
    {'id' : 'UDOFL', 'name' : 'userDefinedOverflow', },
    {'id' : 'IXSHDL', 'name' : 'extendedHeaderLength', },
    {'id' : 'IXSOFL', 'name' : 'extendedHeaderOverflow', },
]

def _bufferFromData(data, dtype=numpy.uint8):
    """Return a 2-tuple consistning of
    1). The memory address of the beginning of an array-like data object.
    2). The size of the buffer, in bytes

    Raise ValueError if unable to convert data to Numpy array
    """
    buf = None
    try:
        buf = numpy.ascontiguousarray(data, dtype=dtype)
    except:
        raise ValueError("Cannot convert data to Numpy array")

    address = buf.__array_interface__['data'][0]
    size = buf.size * buf.itemsize
    return address, size



#NITF IOHandle class
class IOHandle:
    """
    The IOHandle class.
    """

    #some constants
    CREATE = nitropy.PY_NITF_CREATE
    TRUNCATE = nitropy.PY_NITF_TRUNCATE
    OPEN_EXISTING = nitropy.PY_NITF_OPEN_EXISTING
    ACCESS_READONLY = nitropy.PY_NITF_ACCESS_READONLY
    ACCESS_WRITEONLY = nitropy.PY_NITF_ACCESS_WRITEONLY
    ACCESS_READWRITE = nitropy.PY_NITF_ACCESS_READWRITE

    SEEK_SET = nitropy.PY_NITF_SEEK_SET
    SEEK_CUR = nitropy.PY_NITF_SEEK_CUR
    SEEK_END = nitropy.PY_NITF_SEEK_END

    def __init__(self, filename, accessFlags=ACCESS_READONLY, createFlags=OPEN_EXISTING):
        self.filename = filename
        self.error = Error()
        self.ref = nitropy.py_IOHandle_create(
            filename, accessFlags, createFlags, self.error)
        if self.ref < 0:
            raise Exception('Unable to open IOHandle at location %s' % filename)
        self.open = True

    #close the handle
    def __del__(self):
        self.close()

    def write(self, data, size=-1, dtype=numpy.uint8):
        address, bufferSize = _bufferFromData(data, dtype)
        if size == -1:
            size = bufferSize
        elif size > bufferSize:
            raise ValueError("Attempting to write {0} bytes from {1} "
                "bytes of data".format(size, bufferSize))
        return nitropy.py_IOHandle_write(self.ref, address, size,
            self.error) == 1

    def read(self, numElements, dtype=numpy.dtype('b')):
        size = numElements * dtype.itemsize
        byteArray = numpy.frombuffer(nitropy.py_IOHandle_read(
                self.ref, size, self.error), dtype=numpy.dtype('b'))
        return numpy.fromstring(byteArray.tostring(), dtype)

    def tell(self):
        return nitropy.nrt_IOHandle_tell(self.ref, self.error)

    def seek(self, offset, whence=SEEK_CUR):
        return nitropy.py_IOHandle_seek(self.ref, offset, whence, self.error)

    def getSize(self):
        return nitropy.nrt_IOHandle_getSize(self.ref, self.error)

    def close(self):
        if hasattr(self, 'open') and self.open:
            nitropy.nrt_IOHandle_close(self.ref)
            self.open = False


class ImageReader:
    """
    The ImageReader object

    This is created by calling Reader.newImageReader()
    """
    def __init__(self, ref, nbpp):
        self.ref = ref
        self.error = Error()
        self.nbpp = nbpp

    def __del__(self):
        logging.debug('destruct ImageReader')
        if self.ref: nitropy.nitf_ImageReader_destruct(self.ref)

    def read(self, window, downsampler=None):
        win = nitropy.py_SubWindow_construct(window.startRow, window.startCol, window.numRows,
            window.numCols, window.bandList, downsampler, self.error)
        dataBuf = nitropy.py_ImageReader_read(self.ref, win, self.nbpp, self.error)
        if self.error.level:
            raise Exception(self.error.message)
        return dataBuf


class SegmentReader:
    """
    The SegmentReader object
    """

    def __init__(self, ref):
        self.ref = ref
        self.error = Error()

    def __del__(self):
        logging.debug('destruct SegmentReader')
        if self.ref: nitropy.nitf_SegmentReader_destruct(self.ref)

    def read(self, size, datatype=numpy.dtype('b')):
        size = size * datatype.itemsize
        byteArray = numpy.frombuffer(nitropy.py_SegmentReader_read(
                self.ref, size, self.error))
        return numpy.fromstring(byteArray.tostring(), datatype)

    def getSize(self):
        return nitropy.nitf_SegmentReader_getSize(self.ref, self.error)

#    def seek(self, offset=0, whence=None):
#        return nitropy.nitf_SegmentReader_seek(self.ref, offset, whence, self.error)

    def tell(self):
        return nitropy.nitf_SegmentReader_tell(self.ref, self.error)


class DownSampler:
    """ Base DownSampler class """
    def __init__(self, ref):
        self.ref = ref

    def __del__(self):
        logging.debug('destruct Downsampler')
        if self.ref: nitropy.nitf_DownSampler_destruct(self.ref)

class PixelSkipDownSampler(DownSampler):
    """
    PixelSkip DownSampler class
    """
    def __init__(self, rowSkip = 1, colSkip = 1):
        """
        Default row and column skips are 1, meaning no downsampling
        """
        self.rowSkip = rowSkip
        self.colSkip = colSkip
        self.error = Error()
        DownSampler.__init__(self, nitropy.nitf_PixelSkip_construct(rowSkip, colSkip, self.error))

class MaxDownSampler(DownSampler):
    """
    Max DownSampler class
    """
    def __init__(self, rowSkip = 1, colSkip = 1):
        """
        Default row and column skips are 1, meaning no downsampling
        """
        self.rowSkip = rowSkip
        self.colSkip = colSkip
        self.error = Error()
        DownSampler.__init__(self, nitropy.nitf_MaxDownSample_construct(rowSkip, colSkip, self.error))


class SubWindow:
    """
    The SubWindow class

    This is used to specify a subwindow into the image
    """
    def __init__(self, image_subheader=None):
        self.startRow = 0
        self.startCol = 0
        self.bandList = []

        if image_subheader:
            self.numRows = int(image_subheader['numRows'])
            self.numCols = int(image_subheader['numCols'])
            bands = int(image_subheader['numImageBands']) + \
                int(image_subheader['numMultispectralImageBands'])
            for i in range(bands):
                self.bandList.append(i)
        else:
            self.numRows = 0
            self.numCols = 0

    def __str__(self):
        return '%s, %s, %s, %s, %s' % (self.startRow, self.startCol, self.numRows,
            self.numCols, len(self.bandList))


#NITF Reader class
class Reader:
    """
    The Reader class.

    Construct a Reader if you need to read a NITF
    """
    def __init__(self):
        self.error = Error()
        self.ref = nitropy.nitf_Reader_construct(self.error)
        self.io, self.record = None, None

    def __del__(self):
        logging.debug('destruct Reader')
        if self.io: self.io.close()
        if self.ref: nitropy.nitf_Reader_destruct(self.ref)

    def read(self, handle):
        self.io = handle #must set this so it doesn't get ref-counted away
        record = nitropy.nitf_Reader_read(self.ref, self.io.ref, self.error)
        if not record:
            raise Exception(self.error.message)
        self.record = Record(record)
        return self.record

    def newImageReader(self, num, options = None):
        nbpp = int(self.record.getImages()[num]['numBitsPerPixel'])
        reader = nitropy.nitf_Reader_newImageReader(self.ref, num, options, self.error)
        if not reader: raise Exception('Unable to get new ImageReader')
        return ImageReader(reader, nbpp)

    def newTextReader(self, num):
        reader = nitropy.nitf_Reader_newTextReader(self.ref, num, self.error)
        if not reader: raise Exception('Unable to get new TextReader')
        return SegmentReader(reader)

    def newGraphicReader(self, num):
        reader = nitropy.nitf_Reader_newGraphicReader(self.ref, num, self.error)
        if not reader: raise Exception('Unable to get new GraphicReader')
        return SegmentReader(reader)

    @staticmethod
    def getNITFVersion(filename):
        return nitropy.nitf_Reader_getNITFVersion(filename)


#NITF Record class
class Record:
    """
    The Record class.

    The NITF Record is the top-level container of the NITF contents.
    This object contains all of the sub-components of the NITF.
    """
    def __init__(self, ref=None, version=NITF_VER_21):
        self.error = Error()
        if ref is not None:
            self.ref = ref
        else:
            self.ref = nitropy.nitf_Record_construct(version, self.error)
        self.header = FileHeader(self.ref.header)

    def newImageSegment(self):
        """ Creates a new image segment, and updates the numImages value """
        image = nitropy.nitf_Record_newImageSegment(self.ref, self.error)
        return ImageSegment(image)

    def newGraphicSegment(self):
        """ Creates a new graphic segment, and updates the numGraphics value """
        graphic = nitropy.nitf_Record_newGraphicSegment(self.ref, self.error)
        return GraphicSegment(graphic)

    def newTextSegment(self):
        """ Creates a new text segment, and updates the numTexts value """
        image = nitropy.nitf_Record_newTextSegment(self.ref, self.error)
        return TextSegment(image)

    def newDataExtensionSegment(self):
        """ Creates a new data extension segment, and updates the numDataExtensions value """
        image = nitropy.nitf_Record_newDataExtensionSegment(self.ref, self.error)
        return DESegment(image)

    def getImages(self):
        return self._get_listof('images')
    def getGraphics(self):
        return self._get_listof('graphics')
    def getLabels(self):
        return self._get_listof('labels')
    def getTexts(self):
        return self._get_listof('texts')
    def getDataExtensions(self):
        return self._get_listof('dataExtensions')
    def getReservedExtensions(self):
        return self._get_listof('reservedExtensions')

    def __del__(self):
        logging.debug('destruct Record')
        if self.ref: nitropy.nitf_Record_destruct(self.ref)

    #returns an array of the section given
    def _get_listof(self, type):
        type = type.lower()
        if type == 'image' or type == 'images':
            type = 'Image'
        elif type == 'graphic' or type == 'graphics':
            type = 'Graphic'
        elif type == 'text' or type == 'texts':
            type = 'Text'
        elif type == 'label' or type == 'labels':
            type = 'Label'
        elif type == 'dataextension' or type == 'dataextensions' \
            or type == 'de' or type == 'des':
            type = 'DE'
        elif type == 'reservedextension' or type == 'reservedextensions' \
            or type == 're' or type == 'res':
            type = 'RE'
        else:
            return None
        arr = []
        error = Error()
        num = eval('nitropy.py_Record_getNum%ss(self.ref)' % type)
        for i in range(num):
            arr.append(eval('%sSegment( \
                nitropy.py_Record_get%s(self.ref, i, error))' % (type, type)))
        #could use frozenset to be immutable list...
        return tuple(arr)



#NITF Field object
class Field:

    NITF_BCS_A  = nitropy.NITF_BCS_A
    NITF_BCS_N  = nitropy.NITF_BCS_N
    NITF_BINARY = nitropy.NITF_BINARY

    def __init__(self, ref):
        self.ref = ref

    def __repr__(self):
        return self.__str__()

    def __int__(self):
        return int(self.__str__())

    def getLength(self):
        return self.ref.length

    def __str__(self):
        return self.getString()

    def getString(self):
        error = Error()
        return nitropy.py_Field_getString(self.ref, error)

    def setString(self, data):
        error = Error()
        retval = nitropy.nitf_Field_setString(self.ref, str(data), error)

    def getRawData(self):
        error=  Error()
        return nitropy.py_Field_getRawData(self.ref, error)

    def setRawData(self, data):
        error = Error()
        retval = nitropy.py_Field_setRawData(self.ref, data, len(data), error)

    def intValue(self):
        error = Error()
        return nitropy.py_Field_getInt(self.ref, error)

    def getType(self):
        return self.ref.type


#base class for headers that contain many nitf_Field objects
class FieldHeader:
    """
    Base class for any Header that has fields

    This provides list-based get access to the fields

    e.g.
    >>> numImages = header['numImages']
    """
    def __init__(self, ref, field_list=None):
        self.ref = ref
        self.error = Error()
        self.fieldMap = {}

        #set up the map of fields
        for func in dir(ref):
            name = str(func)
            obj = ref.__getattribute__(name)

            #check if it is a Field
            if isinstance(obj, nitropy.nitf_Field):
                self.fieldMap[name.lower()] = 'self.ref.%s' % name

        if field_list:
            for field in field_list:
                self.fieldMap[field['id'].lower()] = 'self.ref.%s' % field['name']

    def __getitem__(self, key):
        """ Provide container get """
        key = key.lower()
        if key in self.fieldMap:
            return Field(eval(self.fieldMap[key]))
        raise KeyError('Field not found: %s' % key)

    def __setitem__(self, key, value):
        """ Provide the container 'set' functionality """
        #truncate the value if it is too long...
        key = key.lower()
        if key in self.fieldMap:
            field = Field(eval(self.fieldMap[key]))
            field.setString(str(value)[0:field.getLength()])
        else:
            raise KeyError('Field not found: %s' % key)

    def __contains__(self, key):
        return (key.lower() in self.fieldMap)


    def __iter__(self):
        fields = []
        for key in self.fieldMap:
            field = self[key]
            fields.append((key, field))
        return fields.__iter__()

    def __str__(self):
        val = ''
        for field_tuple in self:
            key = field_tuple[0]
            field = field_tuple[1]
            if isinstance(field, Field):
                val += '%s(%s) = \'%s\'\n' % (key, field.getLength(), field.getString())
            else:
                val += '%s = \'%s\'\n' % (key, field)
        return val


#base class for actual NITF headers
class Header(FieldHeader):

    def __init__(self, ref, field_list=None):
        FieldHeader.__init__(self, ref, field_list=field_list)
        self.fileSecurity = FileSecurity(self.ref.securityGroup)

    def __iter__(self):
        fields = list(FieldHeader.__iter__(self))
        fields.extend(list(self.fileSecurity.__iter__()))
        return fields.__iter__()


class ComponentInfo:
    def __init__(self, ref):
        self.ref = ref

    def getDataLength(self):
        return Field(self.ref.lengthData)

    def getHeaderLength(self):
        return Field(self.ref.lengthSubheader)

    def __str__(self):
        return '(hl=%s, dl=%s)' % (self.getHeaderLength(), self.getDataLength())



#NITF FileHeader class
class FileHeader(Header):
    """
    The FileHeader class

    Besides having list-based access to its fields, this class
    also provides list-based access to its componentInfos.

    Supported keys are (case-insensitive):
        imageInfo, graphicInfo, textInfo, labelInfo, dataExtensionInfo, reservedExtensionInfo

    e.g.
    >>> infoList = header['imageinfo']
    >>> #print the first component info object
    >>> print(infoList[0])
    """

    def __init__(self, ref):
        Header.__init__(self, ref, field_list=FILE_HEADER_FIELDS)

        #setup the map so we can fetch the componentinfos dictionary style
        self.infoMap = {}
        self.infoMap['imageinfo'] = (self.getComponentInfos, 'image')
        self.infoMap['graphicinfo'] = (self.getComponentInfos, 'graphic')
        self.infoMap['textinfo'] = (self.getComponentInfos, 'text')
        self.infoMap['labelinfo'] = (self.getComponentInfos, 'label')
        self.infoMap['dataextensioninfo'] = (self.getComponentInfos, 'dataExtension')
        self.infoMap['reservedextensioninfo'] = (self.getComponentInfos, 'reservedExtension')

    def __getitem__(self, key):
        try:
            val = Header.__getitem__(self, key)
        except:
            #try this one
            lowkey = key.lower()
            if lowkey in self.infoMap:
                func, type = self.infoMap[lowkey]
                return func(type)
            else:
                raise KeyError('Field not found: %s' % key)
        return val

    def getComponentInfo(self, type, index):
        error = Error()
        return ComponentInfo(
            nitropy.py_FileHeader_getComponentInfo(self.ref, index, str(type).lower(), error))

    def getComponentInfos(self, type):
        num = 0
        type = type.lower()
        if type == 'image' or type == 'graphic' or type == 'label' or type == 'text':
            num = int(self['num%ss' % type])
        elif type == 'de' or type == 'dataextension':
            num, type = int(self['numDataExtensions']), 'de'
        elif type == 're' or type == 'reservedextension':
            num, type = int(self['numReservedExtensions']), 're'
        infos = []
        for i in range(num):
            infos.append(self.getComponentInfo(type, i))
        return infos

    def getXHD(self):
        return Extensions(self.ref.extendedSection)

    def getUDHD(self):
        return Extensions(self.ref.userDefinedSection)


    def __iter__(self):
        fields = []
        fields.extend([
            ('FHDR', self['fileHeader']),
            ('FVER', self['fileVersion']),
            ('CLEVEL', self['complianceLevel']),
            ('STYPE', self['systemType']),
            ('OSTAID', self['originStationID']),
            ('FDT', self['fileDateTime']),
            ('FTITLE', self['fileTitle']),
            ('FSCLAS', self['classification']),
        ])
        fields.extend(list(self.fileSecurity.__iter__(prefix='F')))
        fields.extend([
            ('FSCOP', self['messageCopyNum']),
            ('FSCPYS', self['messageNumCopies']),
            ('ENCRYP', self['encrypted']),
            ('FBKGC', self['backgroundColor']),
            ('ONAME', self['originatorName']),
            ('OPHONE', self['originatorPhone']),
            ('FL', self['fileLength']),
            ('HL', self['headerLength']),
        ])

        for (segment, segid, segprefix) in (('image', 'NUMI', 'I'),
                        ('graphic', 'NUMS', 'S'),
                        ('label', 'NUMX', 'L'),
                        ('text', 'NUMT', 'T'),
                        ('dataExtension', 'NUMDES', 'D'),
                        ('reservedExtension', 'NUMRES', 'RE')):
            fname = 'num%ss' % segment.title()
            fields.append((segid, self[fname]))
            for i in range(int(self[fname])):
                fields.append(('L%sSH' % segprefix, self['%sInfo' % segment][i].getHeaderLength()))
                fields.append(('L%s' % segprefix, self['%sInfo' % segment][i].getDataLength()))

        fields.extend([
            ('UDHDL', self['userDefinedHeaderLength']),
            ('UDHOFL', self['userDefinedOverflow']),
            ('XHDL', self['extendedHeaderLength']),
            ('XHDLOFL', self['extendedHeaderOverflow']),
        ])
        return fields.__iter__()

#NITF BandInfo class
#TODO: Add the LookupTable as well
class BandInfo(FieldHeader):
    def __init__(self, ref):
        FieldHeader.__init__(self, ref, field_list=BAND_INFO_FIELDS)

    def __iter__(self):
        fields = []
        fields.extend([
            ('IREPBAND', self['representation']),
            ('ISUBCAT', self['subcategory']),
            ('IFC', self['imageFilterCondition']),
            ('IMFLT', self['imageFilterCode']),
            ('NLUTS', self['numLUTs']),
            ('NELUT', self['bandEntriesPerLUT']),
        ])
        return fields.__iter__()

#NITF ImageSubheader class
class ImageSubheader(Header):

    def __init__(self, ref):
        Header.__init__(self, ref, field_list=IMAGE_SUBHEADER_FIELDS)

    def getXHD(self):
        return Extensions(self.ref.extendedSection)

    def getUDHD(self):
        return Extensions(self.ref.userDefinedSection)

    def getComments(self):
        return [Field(comment) for comment in self.ref.imageComments]

    def insertComment(self, comment, index=-1):
        """ Inserts a comment. By default, adds it to the end """
        return nitropy.nitf_ImageSubheader_insertImageComment(self.ref, comment, index, self.error) and True

    def removeComment(self, index):
        """ Removes the comment at the specified index. If index is out of bounds, nothing is done """
        return nitropy.nitf_ImageSubheader_removeImageComment(self.ref, index, self.error) and True

    def getBandCount(self):
        return nitropy.nitf_ImageSubheader_getBandCount(self.ref, self.error)

    def getBandInfo(self, index):
        return BandInfo(nitropy.nitf_ImageSubheader_getBandInfo(self.ref, index, self.error))

    def clone(self):
        return ImageSubheader(nitropy.nitf_ImageSubheader_clone(self.ref, self.error))

    def __iter__(self):
        fields = []
        fields.extend([
            ('IM', self['filePartType']),
            ('IID1', self['imageId']),
            ('IDATIM', self['imageDateAndTime']),
            ('TGTID', self['targetId']),
            ('IID2', self['imageTitle']),
            ('ISCLAS', self['imageSecurityClass']),
        ])
        fields.extend(list(self.fileSecurity.__iter__(prefix='I')))
        fields.extend([
            ('ISORCE', self['imageSource']),
            ('NROWS', self['numRows']),
            ('NCOLS', self['numCols']),
            ('PVTYPE', self['pixelValueType']),
            ('IREP', self['imageRepresentation']),
            ('ICAT', self['imageCategory']),
            ('ABPP', self['actualBitsPerPixel']),
            ('PJUST', self['pixelJustification']),
            ('ICORDS', self['imageCoordinateSystem']),
            ('IGEOLO', self['cornerCoordinates']),
            ('NICOM', self['numImageComments']),
        ])

        for i, comment in enumerate(self.getComments()):
            fields.append(('ICOM[%s]' % i, comment))

        numBands = self['numImageBands'].intValue() + self['numMultispectralImageBands'].intValue()
        for band in range(numBands):
            fields.append(('BANDINFO[%s]' % band, self.getBandInfo(band)))

        fields.extend([
            ('IC', self['imageCompression']),
            ('COMRAT', self['compressionRate']),
            ('NBANDS', self['numImageBands']),
            ('XBANDS', self['numMultispectralImageBands']),
            ('ISYNC', self['imageSyncCode']),
            ('IMODE', self['imageMode']),
            ('NBPR', self['numBlocksPerRow']),
            ('NBPC', self['numBlocksPerCol']),
            ('NPPBH', self['numPixelsPerHorizBlock']),
            ('NPPBV', self['numPixelsPerVertBlock']),
            ('NBPP', self['numBitsPerPixel']),
            ('IDLVL', self['imageDisplayLevel']),
            ('IALVL', self['imageAttachmentLevel']),
            ('ILOC', self['imageLocation']),
            ('IMAG', self['imageMagnification']),
            ('UDIDL', self['userDefinedImageDataLength']),
            ('UDOFL', self['userDefinedOverflow']),
            ('IXSHDL', self['extendedHeaderLength']),
            ('IXSOFL', self['extendedHeaderOverflow']),
        ])
        return fields.__iter__()

#NITF GraphicSubheader class
class GraphicSubheader(Header):

    def __iter__(self):
        fields = [
            ('SY', self['filePartType']),
            ('SID', self['graphicID']),
            ('SNAME', self['name']),
            ('SSCLAS', self['securityClass']),
        ]
        fields.extend(list(self.fileSecurity.__iter__(prefix='S')))
        fields.extend([
            ('SFMT', self['stype']),
            ('SSTRUCT', self['res1']),
            ('SDLVL', self['displayLevel']),
            ('SALVL', self['attachmentLevel']),
            ('SLOC', self['location']),
            ('SBND1', self['bound1Loc']),
            ('SCOLOR', self['color']),
            ('SBND2', self['bound2Loc']),
            ('SRES2', self['res2']),
            ('SXSHDL', self['extendedHeaderLength']),
            ('SXSOFL', self['extendedHeaderOverflow']),
        ])
        return fields.__iter__()

    def clone(self):
        return GraphicSubheader(nitropy.nitf_GraphicSubheader_clone(self.ref, self.error))

    def getXHD(self):
        return Extensions(self.ref.extendedSection)

#NITF LabelSubheader class
class LabelSubheader(Header):
    def clone(self):
        return LabelSubheader(nitropy.nitf_LabelSubheader_clone(self.ref, self.error))


#NITF TextSubheader class
class TextSubheader(Header):
    def __iter__(self):
        fields = [
            ('TE', self['filePartType']),
            ('TEXTID', self['textID']),
            ('TXTALVL', self['attachmentLevel']),
            ('TXTDT', self['dateTime']),
            ('TXTITL', self['title']),
            ('TSCLAS', self['securityClass']),
        ]
        fields.extend(list(self.fileSecurity.__iter__(prefix='T')))
        fields.extend([
            ('TXTFMT', self['format']),
            ('TXSHDL', self['extendedHeaderLength']),
            ('TXSOFL', self['extendedHeaderOverflow']),
        ])
        return fields.__iter__()

    def clone(self):
        return TextSubheader(nitropy.nitf_TextSubheader_clone(self.ref, self.error))

    def getXHD(self):
        return Extensions(self.ref.extendedSection)

#NITF DESubheader class
class DESubheader(Header):

    def __iter__(self):
        fields = [
            ('DE', self['filePartType']),
            ('DESTAG', self['typeID']),
            ('DESVER', self['version']),
            ('DESCLAS', self['securityClass']),
        ]
        fields.extend(list(self.fileSecurity.__iter__(prefix='D')))
        for (k, f) in [('DESOFLW', 'overflowHeaderType'),
                       ('DESITEM', 'dataItemOverflowed'),
                       ('DESSHL', 'subheaderFieldsLength')]:
            if f in self:
                fields.append((k, self[f]))
        return fields.__iter__()

    def newSubheaderFields(self, tag, id = None):
        self.ref.subheaderFields = nitropy.nitf_TRE_construct(tag, id, self.error)
        if not self.ref.subheaderFields:
            raise Exception('Unable to create TRE with tag \'%s\'' % tag)
        return TRE(self.ref.subheaderFields)

    def getSubheaderFields(self):
        return TRE(self.ref.subheaderFields)

    def getUDHD(self):
        return Extensions(self.ref.userDefinedSection)

    def clone(self):
        return DESubheader(nitropy.nitf_DESubheader_clone(self.ref, self.error))

#NITF RESubheader class
class RESubheader(Header):
    def clone(self):
        return RESubheader(nitropy.nitf_RESubheader_clone(self.ref, self.error))

#NITF FileSecurity class
class FileSecurity(FieldHeader):
    def __iter__(self, prefix=''):
        fields = []
        fields.extend([
            ('%sSCLSY' % prefix, self['classificationSystem']),
            ('%sSCODE' % prefix, self['codewords']),
            ('%sSCTLH' % prefix, self['controlAndHandling']),
            ('%sSREL' % prefix, self['releasingInstructions']),
            ('%sSDCTP' % prefix, self['declassificationType']),
            ('%sSDCDT' % prefix, self['declassificationDate']),
            ('%sSDCXM' % prefix, self['declassificationExemption']),
            ('%sSDG' % prefix, self['downgrade']),
            ('%sSDGDT' % prefix, self['downgradeDateTime']),
            ('%sSCLTX' % prefix, self['classificationText']),
            ('%sSCATP' % prefix, self['classificationAuthorityType']),
            ('%sSCAUT' % prefix, self['classificationAuthority']),
            ('%sSCRSN' % prefix, self['classificationReason']),
            ('%sSSRDT' % prefix, self['securitySourceDate']),
            ('%sSCTLN' % prefix, self['securityControlNumber']),
        ])
        return fields.__iter__()

class RecordSegment:
    """
    Abstract class for Segments
    Not intended to be instantiated itself
    """
    def __init__(self, ref):
        self.ref = ref
        self.error = Error()
        self.subheader = None

    def __getitem__(self, key):
        """ Forward to the SubHeader """
        return self.subheader[key]

    def __str__(self):
        """ Forward to the SubHeader """
        return self.subheader.__str__()


#NITF ImageSegment class
class ImageSegment(RecordSegment):
    def __init__(self, ref):
        RecordSegment.__init__(self, ref)
        self.subheader = ImageSubheader(self.ref.subheader)

    def addBands(self, num=1):
        """ adds the specified number of bands to the image """
        if num < 0:
            num = 0
        nitropy.nitf_ImageSubheader_createBands(self.subheader.ref, num, self.error)


#NITF GraphicSegment class
class GraphicSegment(RecordSegment):
    def __init__(self, ref):
        RecordSegment.__init__(self, ref)
        self.subheader = GraphicSubheader(self.ref.subheader)

#NITF LabelSegment class
class LabelSegment(RecordSegment):
    def __init__(self, ref):
        RecordSegment.__init__(self, ref)
        self.subheader = LabelSubheader(self.ref.subheader)

#NITF TextSegment class
class TextSegment(RecordSegment):
    def __init__(self, ref):
        RecordSegment.__init__(self, ref)
        self.subheader = TextSubheader(self.ref.subheader)

#NITF DESegment class
class DESegment(RecordSegment):
    def __init__(self, ref):
        RecordSegment.__init__(self, ref)
        self.subheader = DESubheader(self.ref.subheader)

#NITF RESegment class
class RESegment(RecordSegment):
    def __init__(self, ref):
        RecordSegment.__init__(self, ref)
        self.subheader = RESubheader(self.ref.subheader)


class TRE:
    """ Tagged Record Extension """
    DEFAULT_LENGTH = nitropy.NITF_TRE_DEFAULT_LENGTH

    def __init__(self, ref):
        """
        Create either a new TRE or refer to an existing underlying TRE
        The ref parameter is checked
        """
        self.error = Error()

        if isinstance(ref, str):
            tag = ref
            ref = nitropy.nitf_TRE_construct(tag, None, self.error)
            if not ref:
                raise Exception('Unable to create TRE with tag \'%s\'' % tag)
        self.ref = ref

    def getTag(self):
        return self.ref.tag

    def getCurrentSize(self):
        return nitropy.nitf_TRE_getCurrentSize(self.ref, self.error)

    def __contains__(self, field):
        return nitropy.nitf_TRE_exists(self.ref, field)

    def __str__(self):
        return '%s(%s)' % (self.getTag(), self.getLength())

    def __iter__(self):
        it = nitropy.nitf_TRE_begin(self.ref, self.error)
        while nitropy.py_TREEnumerator_hasNext(it):
            pair = nitropy.py_TREEnumerator_next(it, self.error)

            if pair:
                field = nitropy.py_Pair_getFieldData(pair)
                if field:
                    field = Field(field)
                    yield pair.key, field
                    continue
            break

    def clone(self):
        return TRE(nitropy.py_TRE_clone(self.ref, self.error))

    def getField(self, name):
        field = nitropy.nitf_TRE_getField(self.ref, name)
        return field and Field(field) or None

    def setField(self, name, value):
        field = nitropy.py_TRE_setField(self.ref, name, value, len(value), self.error)

    def __getitem__(self, field):
        return self.getField(field)


class Extensions:
    """ TRE Extensions """
    def __init__(self, ref):
        self.ref = ref
        self.error = Error()

    def __getitem__(self, key):
        return nitropy.nitf_Extensions_getTREsByName(self.ref, key)

    def __len__(self):
        return len(self())

    def __call__(self):
        iter = nitropy.nitf_Extensions_begin(self.ref)
        end = nitropy.nitf_Extensions_end(self.ref)
        tres = []
        while nitropy.nitf_ExtensionsIterator_notEqualTo(iter, end):
            tres.append(TRE(nitropy.nitf_ExtensionsIterator_get(iter)))
            nitropy.nitf_ExtensionsIterator_increment(iter)
        return tres

    def __iter__(self):
        return list(self()).__iter__()

    def append(self, tre):
        """ appends the given TRE to this Extensions object """
        return nitropy.nitf_Extensions_appendTRE(self.ref, tre.ref, self.error) == 1

    def removeTREsByName(self, tag):
        """ removes all TREs with the given name """
        nitropy.nitf_Extensions_removeTREsByName(self.ref, tag)


class ImageSource:
    def __init__(self):
        self.error = Error()
        self.ref = nitropy.nitf_ImageSource_construct(self.error)
        self._attached = []

    def __del__(self):
        logging.debug('destruct ImageSource')
        if not hasattr(self, 'attached') or not self.attached:
            if self.ref: nitropy.nitf_ImageSource_destruct(self.ref)

    def addBand(self, band):
        val = nitropy.nitf_ImageSource_addBand(self.ref, band.ref, self.error) == 1
        if val:
            band.attached = True
            self._attached.append(band)
        return val

    def getBand(self, index):
        band = BandSource(nitropy.nitf_ImageSource_getBand(self.ref, index, self.error))
        band.attached = True
        return band

    def __call__(self):
        print("ImageSource.__call__")
        arr = []
        print("Size: " + str(self.ref.size))
        for index in range(self.ref.size):
            print(index)
            arr.append(self.getBand(index))
        return arr

    def __iter__(self):
        return list(self()).__iter__()


class DataSource:
    """ Base DataSource class """
    def __init__(self, ref):
        #if not ref:
        #    raise Exception("Unable to construct DataSource")
        self.ref = ref
        if not hasattr(self, 'error'):
            self.error = Error()

    def __del__(self):
        logging.debug('destruct DataSource')
        if self.ref: nitropy.nitf_DataSource_destruct(self.ref)

    def read(self, numElements, dtype=numpy.dtype('b')):
        size = numElements * dtype.itemsize
        byteArray = numpy.frombuffer(nitropy.py_DataSource_read(
                self.ref, size, self.error), dtype=numpy.dtype('b'))
        return numpy.fromstring(byteArray.tostring(), dtype)


class BandSource(DataSource):
    """ Base BandSource class """
    def __init__(self, ref):
        DataSource.__init__(self, ref)

    def __del__(self):
        logging.debug('destruct BandSource')
        #bands that are attached to an image source can't be destructed
        if not hasattr(self, 'attached') or not self.attached:
            if self.ref: nitropy.nitf_DataSource_destruct(self.ref)


class MemoryBandSource(BandSource):
    """ BandSource derived from memory """
    def __init__(self, data, size=-1, start=0, nbpp=1, pixelskip=0):
        self.error = Error()

        if size == -1:
            size = len(data)

        if size * nbpp > data.size * data.itemsize:
            raise ValueError("Attempting to read {0} bytes from buffer "
            "of size {1} bytes.", size * nbpp, data.size * data.itemsize)

        BandSource.__init__(self, nitropy.py_nitf_MemorySource_construct(
            data.__array_interface__['data'][0], size, start, nbpp, pixelskip,
            self.error))


class FileBandSource(BandSource):
    """ BandSource derived from an IOHandle """
    def __init__(self, handle, start=0, nbpp=1, pixelskip=0):
        self.error = Error()
        self.handle = handle #must set this so it doesn't get ref-counted away
        ref = nitropy.nitf_FileSource_construct(handle.ref, start, nbpp, pixelskip, self.error)
        BandSource.__init__(self, nitropy.nitf_FileSource_construct(handle.ref, start, nbpp, pixelskip, self.error))


class SegmentSource:
    """ Base SegmentSource class """
    def __init__(self, ref):
        self.ref = ref
        self.attached = False
        if not hasattr(self, 'error'):
            self.error = Error()

    def __del__(self):
        logging.debug('destruct SegmentSource')
        if not self.attached:
            if self.ref: nitropy.nitf_DataSource_destruct(self.ref)


class MemorySegmentSource(SegmentSource):
    """ SegmentSource derived from memory """
    def __init__(self, data, size=-1, start=0, byteSkip=0):
        self.error = Error()
        address, buffersize = _bufferFromData(data, data.dtype)

        if size == -1:
            size = buffersize
        SegmentSource.__init__(self,
                nitropy.py_nitf_SegmentMemorySource_construct(
                data.__array_interface__['data'][0], size, start, byteSkip,
                True, self.error))


class FileSegmentSource(SegmentSource):
    """ SegmentSource derived from an IOHandle """
    def __init__(self, handle, start=0, byteSkip=0):
        self.error = Error()
        self.handle = handle #must set this so it doesn't get ref-counted away
        SegmentSource.__init__(self, nitropy.nitf_SegmentFileSource_construct(handle.ref, start, byteSkip, self.error))



class ImageWriter:
    def __init__(self, ref):
        self.ref = ref
        self.error = Error()

    def __del__(self):
        logging.debug('destruct ImageWriter')
        if not hasattr(self, 'attached') or not self.attached:
            if self.ref: nitropy.nitf_ImageWriter_destruct(self.ref)

    def attachSource(self, imagesource):
        val = nitropy.nitf_ImageWriter_attachSource(self.ref, imagesource.ref, self.error) == 1
        if val:
            imagesource.attached = True
        return val



class SegmentWriter:
    def __init__(self, ref):
        self.ref = ref
        self.error = Error()

    def __del__(self):
        logging.debug('destruct SegmentWriter')
        if not hasattr(self, 'attached') or not self.attached:
            if self.ref: nitropy.nitf_SegmentWriter_destruct(self.ref)

    def attachSource(self, source):
        val = nitropy.nitf_SegmentWriter_attachSource(self.ref, source.ref, self.error) == 1
        if val:
            # source.attached = True does not work for some reason
            setattr(source, 'attached', True)
        return val


class Writer:
    def __init__(self):
        self.error = Error()
        self.ref = nitropy.py_nitf_Writer_construct(self.error)
        if not self.ref:
            raise Exception('Unable to create Writer')
        self._imageWriters, self._graphicWriters = [], []
        self.io = None

    def __del__(self):
        logging.debug('destruct Writer')
        if self.io: self.io.close()
        if self.ref: nitropy.py_nitf_Writer_destruct(self.ref)

    def newImageWriter(self, imagenum, options = None):
        writer = nitropy.nitf_Writer_newImageWriter(self.ref, imagenum, options, self.error)
        if writer:
            writer = ImageWriter(writer)
            self._imageWriters.append(writer)
            writer.attached = True
            return writer
        raise Exception('Unable to get new ImageWriter: (%s)' % self.error.message)

    def newGraphicWriter(self, num):
        writer = nitropy.nitf_Writer_newGraphicWriter(self.ref, num, self.error)
        if writer:
            writer = SegmentWriter(writer)
            self._graphicWriters.append(writer)
            writer.attached = True
            return writer
        raise Exception('Unable to get new GraphicWriter: (%s)' % self.error.message)

    def prepare(self, record, handle):
        self.io = handle #must set this so it doesn't get ref-counted away
        return nitropy.py_nitf_Writer_prepare(self.ref, record.ref, self.io.ref, self.error) == 1

    def write(self):
        if not nitropy.nitf_Writer_write(self.ref, self.error) == 1:
            raise Exception('Write Failed: (%s)' % self.error.message)


class PluginRegistry:
    __instance = None
    def __init__(self):
        if PluginRegistry.__instance:
            self = PluginRegistry.__instance
        else:
            PluginRegistry.__instance = self
            PluginRegistry.__instance.error = Error()
            PluginRegistry.__instance.ref = nitropy.nitf_PluginRegistry_getInstance(PluginRegistry.__instance.error)

    @staticmethod
    def instance():
        return PluginRegistry().__instance

    @staticmethod
    def loadDir(dirname):
        """ Loads any plugins in the given directory """
        error = Error()
        if not nitropy.nitf_PluginRegistry_loadDir(dirname, error):
            raise Exception('Unable to load the directory: %s (error=%s)' % (dirname, error.message))

    def canHandleTRE(self, tre_name):
        return nitropy.py_nitf_PluginRegistry_canRetrieveTREHandler(self.ref, tre_name, self.error)


def read(filename):
    '''
    Opens and reads the given file, returning a (Reader, Record) tuple.
    '''
    handle = IOHandle(filename)
    reader = Reader()
    return (reader, reader.read(handle))

def metadata(filename):
    '''
    Dumps the metadata from the given nitf file. You can optionally pass
    a nitf Record object instead of a filename.
    '''
    def dumpTRE(tre, section):
        print('--- %s TRE [%s] - (%d) ---' % (section, tre.getTag(), tre.getCurrentSize()))
        for fieldname, field in tre:
            print("%s(%s) = '%s'" % (fieldname,
                                     field.getLength(), str(field)))

    def dumpHeader(header, desc, extensions={}):
        print('--- %s ---' % desc)
        print(str(header))
        for section, tres in extensions.items():
            for tre in tres:
                dumpTRE(tre, section)

    if type(filename) == str:
        reader, record = read(filename)
    else:
        record = filename

    dumpHeader(record.header, "FileHeader",
               {'UDHD':record.header.getUDHD(),
                'XHD':record.header.getXHD()})

    for i, segment in enumerate(record.getImages()):
        dumpHeader(segment.subheader, 'Image [%d]' % (i + 1),
                     {'UDHD':segment.subheader.getUDHD(),
                      'XHD':segment.subheader.getXHD()})
    for i, segment in enumerate(record.getGraphics()):
        dumpHeader(segment.subheader, 'Graphic [%d]' % (i + 1),
                     {'XHD':segment.subheader.getXHD()})
    for i, segment in enumerate(record.getTexts()):
        dumpHeader(segment.subheader, 'Text [%d]' % (i + 1),
                   {'XHD':segment.subheader.getXHD()})
    for i, segment in enumerate(record.getDataExtensions()):
        dumpHeader(segment.subheader, 'DES [%d]' % (i + 1),
                   {'XHD':segment.subheader.getUDHD()})


#set the NITF_PLUGIN_PATH
#import os, inspect
#try:
#    curdir = os.getcwd()
#    nitf_dir = os.path.split(inspect.getfile(nitropy))[0]
#    os.chdir(nitf_dir)
#    os.environ['NITF_PLUGIN_PATH'] = os.path.join(os.path.split(inspect.getfile(nitropy))[0], 'plugins' + os.sep)
#    PluginRegistry.loadDir(os.environ['NITF_PLUGIN_PATH'])
#    os.chdir(curdir)
#except: {}

##############################################################################
