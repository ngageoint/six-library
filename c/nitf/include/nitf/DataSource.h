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

#ifndef __NITF_DATA_SOURCE_H__
#define __NITF_DATA_SOURCE_H__

#include "nitf/System.h"


NITF_CXX_GUARD

/*
 *  Function pointer for reading data (part of the data source interface)
 *  \param data     The ancillary "helper" data
 *  \param buf      The buffer to store the image data
 *  \param size     The size (usually numColumns * numBytesPerPixel)
 *  \param error    populated on error
 */
typedef NITF_BOOL(*NITF_IDATASOURCE_READ) (NITF_DATA *data,
        char *buf, nitf_Off size, nitf_Error *error);


/*
 *  Function pointer for destructing the data structure (part of the data source interface)
 *  \param data     The ancillary "helper" data
 */
typedef void (*NITF_IDATASOURCE_DESTRUCT) (NITF_DATA *);

/*!
 *  Get the size of the data source
 *
 */
typedef nitf_Off (*NITF_IDATASOURCE_GET_SIZE) (NITF_DATA *, nitf_Error*);

/*!
 *  Set the size of the data source.  (NITRO 2.0).  This is useful
 *  when we want to stream a file into our NITF.  That way our user
 *  does not have to write his/her own file source.
 */
typedef NITF_BOOL (*NITF_IDATASOURCE_SET_SIZE) (NITF_DATA *,
        nitf_Off size, nitf_Error*);



/*!
 *  \struct nitf_IDataSource
 *  \brief The data source interface
 *
 *  This is the interface on which all of the pull-filters are
 *  made.
 */
typedef struct _nitf_IDataSource
{
    NITF_IDATASOURCE_READ read;
    NITF_IDATASOURCE_DESTRUCT destruct;
    NITF_IDATASOURCE_GET_SIZE getSize;
    NITF_IDATASOURCE_SET_SIZE setSize;
}
nitf_IDataSource;

/*!
 *  \struct nitf_DataSource
 *  \brief The "derived" data source interface
 *
 *  This is the "derived" source.  It presumes that a "constructor"
 *  has brought it to life.
 *
 */
typedef struct _nitf_DataSource
{
    nitf_IDataSource *iface;
    NITF_DATA *data;
}
nitf_DataSource;


NITFAPI(void) nitf_DataSource_destruct(nitf_DataSource ** dataSource);


NITF_CXX_ENDGUARD

#endif
