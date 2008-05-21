/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_RECORD_H__
#define __NITF_RECORD_H__

#include "nitf/System.h"
#include "nitf/Error.h"
#include "nitf/List.h"
#include "nitf/FileHeader.h"
#include "nitf/ImageSegment.h"
#include "nitf/GraphicSegment.h"
#include "nitf/LabelSegment.h"
#include "nitf/TextSegment.h"
#include "nitf/DESegment.h"
#include "nitf/RESegment.h"

NITF_CXX_GUARD

/*!
 *  \struct nitf_Record
 *  \brief This is a record of the file
 *
 *  The Record object is a representation of the data within
 *  a file.  It is structurally hierarchical in a manner that
 *  corresponds one-to-one with the components of an NITF file.
 *  For most occurences, you can assume that reader will
 *  est the header
 */
typedef struct _nitf_Record
{
    nitf_FileHeader *header;    /*  The file header                    */
    nitf_List *images;          /*  A list of ImageSegment*            */
    nitf_List *graphics;        /*  A list of GraphicsSegment*         */
    nitf_List *labels;          /*  A list of LabelSegment* (2.0 only) */
    nitf_List *texts;           /*  A list of TextSegment*             */
    nitf_List *dataExtensions;  /*  data extensions segments    */
    nitf_List *reservedExtensions;      /*  reserved segments       */
}
nitf_Record;

/*!
 *  Create a new record.  This allocates the Record itself, its FileHeader,
 *  and inits all of the Lists of segments to be empty.
 *  It NULL-sets everything else.
 *  The FVER (fileVersion) field of the FileHeader is also set, based
 *  on the version specified. If an invalid version is specified,
 *  NITF_VER_21 is assumed, creating a NITF 2.1 record.
 *
 *  If an error should occur, the function will return NULL,
 *  and the error object will be populated
 *  \param version The nitf_Version of the Record
 *  \param error An error to populate if a problem occurs
 *  \return A record to return
 *
 */
NITFAPI(nitf_Record *) nitf_Record_construct
(
    nitf_Version version,
    nitf_Error * error
);


/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_Record *) nitf_Record_clone(nitf_Record * source,
        nitf_Error * error);

/*!
 *  Destroy a record and NULL-set it
 *  \param record A record to destroy
 */
NITFAPI(void) nitf_Record_destruct(nitf_Record ** record);


/**
 * Returns the nitf_Version enum for the Record in the Reader
 * \param record the Record object
 * \param error The error to populate on failure
 * \return nitf_Version enum specifying the version
 */
NITFAPI(nitf_Version) nitf_Record_getVersion(nitf_Record * record,
        nitf_Error * error);


/**
 * Creates and returns a new ImageSegment* that is bound to the Record
 *
 * Note: The accompanying ComponentInfo object for this segment is also
 * created and added to the FileHeader structure for it
 *
 * \param record the Record object
 * \param error The error to populate on failure
 * \return nitf_ImageSegment* that is bound to the Record
 */
NITFAPI(nitf_ImageSegment*) nitf_Record_newImageSegment(nitf_Record * record,
        nitf_Error * error);


/**
 * Creates and returns a new GraphicSegment* that is bound to the Record
 *
 * Note: The accompanying ComponentInfo object for this segment is also
 * created and added to the FileHeader structure for it
 *
 * \param record the Record object
 * \param error The error to populate on failure
 * \return nitf_GraphicSegment* that is bound to the Record
 */
NITFAPI(nitf_GraphicSegment*) nitf_Record_newGraphicSegment(nitf_Record * record,
        nitf_Error * error);


/**
 * Creates and returns a new TextSegment* that is bound to the Record
 *
 * Note: The accompanying ComponentInfo object for this segment is also
 * created and added to the FileHeader structure for it
 *
 * \param record the Record object
 * \param error The error to populate on failure
 * \return nitf_TextSegment* that is bound to the Record
 */
NITFAPI(nitf_TextSegment*) nitf_Record_newTextSegment(nitf_Record * record,
        nitf_Error * error);


/**
 * Creates and returns a new DESegment* that is bound to the Record
 *
 * Note: The accompanying ComponentInfo object for this segment is also
 * created and added to the FileHeader structure for it
 *
 * \param record the Record object
 * \param error The error to populate on failure
 * \return nitf_DESegment* that is bound to the Record
 */
NITFAPI(nitf_DESegment*) nitf_Record_newDataExtensionSegment(
    nitf_Record * record,
    nitf_Error * error);


/**
 * This removes the segment at the given offset. The segment is
 * de-allocated, and removed from the system.
 *
 * CAUTION!!! -- Removing a segment removes the memory in the segment list,
 * so any usage of the segment will cause a segmentation fault.
 *
 * @param segmentNumber
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_removeImageSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
);

/**
 * This removes the segment at the given offset. The segment is
 * de-allocated, and removed from the system.
 *
 * CAUTION!!! -- Removing a segment removes the memory in the segment list,
 * so any usage of the segment will cause a segmentation fault.
 *
 * @param segmentNumber
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_removeGraphicSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
);

/**
 * This removes the segment at the given offset. The segment is
 * de-allocated, and removed from the system.
 *
 * CAUTION!!! -- Removing a segment removes the memory in the segment list,
 * so any usage of the segment will cause a segmentation fault.
 *
 * @param segmentNumber
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_removeLabelSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
);

/**
 * This removes the segment at the given offset. The segment is
 * de-allocated, and removed from the system.
 *
 * CAUTION!!! -- Removing a segment removes the memory in the segment list,
 * so any usage of the segment will cause a segmentation fault.
 *
 * @param segmentNumber
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_removeTextSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
);

/**
 * This removes the segment at the given offset. The segment is
 * de-allocated, and removed from the system.
 *
 * CAUTION!!! -- Removing a segment removes the memory in the segment list,
 * so any usage of the segment will cause a segmentation fault.
 *
 * @param segmentNumber
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_removeDataExtensionSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
);

/**
 * This removes the segment at the given offset. The segment is
 * de-allocated, and removed from the system.
 *
 * CAUTION!!! -- Removing a segment removes the memory in the segment list,
 * so any usage of the segment will cause a segmentation fault.
 *
 * @param segmentNumber
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_removeReservedExtensionSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
);

/**
 * This moves the segment from the oldIndex to the newIndex.
 * This is a shortcut for:
 *      nitf_List_move(record->images, oldIndex, newIndex, error);
 *
 * @param oldIndex  the index of the segment to move
 * @param newIndex  the index where the segment will be moved to
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_moveImageSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
);

/**
 * This moves the segment from the oldIndex to the newIndex.
 * This is a shortcut for:
 *      nitf_List_move(record->graphics, oldIndex, newIndex, error);
 *
 * @param oldIndex  the index of the segment to move
 * @param newIndex  the index where the segment will be moved to
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_moveGraphicSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
);

/**
 * This moves the segment from the oldIndex to the newIndex.
 * This is a shortcut for:
 *      nitf_List_move(record->labels, oldIndex, newIndex, error);
 *
 * @param oldIndex  the index of the segment to move
 * @param newIndex  the index where the segment will be moved to
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_moveLabelSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
);

/**
 * This moves the segment from the oldIndex to the newIndex.
 * This is a shortcut for:
 *      nitf_List_move(record->texts, oldIndex, newIndex, error);
 *
 * @param oldIndex  the index of the segment to move
 * @param newIndex  the index where the segment will be moved to
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_moveTextSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
);

/**
 * This moves the segment from the oldIndex to the newIndex.
 * This is a shortcut for:
 *      nitf_List_move(record->dataExtensions, oldIndex, newIndex, error);
 *
 * @param oldIndex  the index of the segment to move
 * @param newIndex  the index where the segment will be moved to
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_moveDataExtensionSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
);

/**
 * This moves the segment from the oldIndex to the newIndex.
 * This is a shortcut for:
 *      nitf_List_move(record->reservedExtensions, oldIndex, newIndex, error);
 *
 * @param oldIndex  the index of the segment to move
 * @param newIndex  the index where the segment will be moved to
 * @throws NITFException
 */
NITFAPI(NITF_BOOL) nitf_Record_moveReservedExtensionSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
);

/**
 * nitf_Record_mergeTREs merges TREs between all segments and the associate
 * TRE_OVERFLOW segments. After this call all TRE_OVERFLOW (DE) segments are
 * removed and their TRE's have been added to the TRE list of the corresponding
 * extension in the corresponding section.
 *
 *  \param The record or operate on
 *  \param error An error to populate if a problem occurs
 *  \return TRUE on success, false on failure
 * 
*/
NITFAPI(NITF_BOOL) nitf_Record_mergeTREs
(
    nitf_Record * record,
    nitf_Error * error
);

/**
 * nitf_Record_unmergeTREs scans all segments and creates TRE_OVERFLOW (DE)
 * segments for each segment that requires one. When an overflow segment is
 * created,TREs are transfered from the original segment to the overflow. 
 * The TRE order in the combined lists, original and overflows replicates the
 * order of the orginal TRE list. As many TREs as will fit are placed in the
 * original segment, but the orginal list is not reordered.
 *
 * This function will be automatically be called by the writer, but may be
 * called by the user. Once a segment has been unmerged to create an overflow
 * segment, subsequent unmerges will do nothing and additional TREs are added
 * to the overflow segment. Also after unmerging, the user must be cognicant
 * of the presence of the overflow segment when preocessing TREs for the
 * original segment. An unmerged segment can be remerged.
 *
 *  \param The record or operate on
 *  \param error An error to populate if a problem occurs
 *  \return TRUE on success, false on failure
 * 
*/
NITFAPI(NITF_BOOL) nitf_Record_unmergeTREs
(
    nitf_Record * record,
    nitf_Error * error
);


NITF_CXX_ENDGUARD

#endif
