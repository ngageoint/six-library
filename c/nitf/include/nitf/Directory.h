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

#ifndef __NITF_DIRECTORY_H__
#define __NITF_DIRECTORY_H__

#include "nitf/Defines.h"
#include "nitf/Types.h"
#include "nitf/Error.h"
#include "nitf/Memory.h"

NITF_CXX_GUARD

typedef void nitf_Directory;

/*!
 *  Create a directory object.  Gives back a directory object
 *  \param error An error to populate on failure
 *  \return A directory object
 */
NITFAPI(nitf_Directory *) nitf_Directory_construct(nitf_Error * error);

/*!
 *  Find the first file in a directory.
 *  \param dir The directory object to operate on
 *  \param path The path to assign this directory
 *  \return The name of the first file in the directory
 */
NITFAPI(const char *) nitf_Directory_findFirstFile(nitf_Directory * dir,
        const char *path);

/*!
 *  Opens the next file in a directory.  This call should be made
 *  after a call has been already made to findFirstFile
 *  \param dir The directory object to operate on
 *  \return The name of the next file in the directory, or NULL if none
 */
NITFAPI(const char *) nitf_Directory_findNextFile(nitf_Directory * dir);

/*!
 *  Destroy the directory object in question
 */
NITFAPI(void) nitf_Directory_destruct(nitf_Directory ** dir);

/*!
 *  Check if the directory exists
 *  \param dir The name of the directory to check
 *  \return 1 if exists, 0 if not exists
 */
NITFAPI(NITF_BOOL) nitf_Directory_exists(const char *dir);

/*

    DIR* dir = opendir(reg->path);
    struct dirent* entry;

 const char* p =

    if (!dir)
    {

 nitf_Error_initf(error,
    NITF_CTXT, NITF_ERR_OPENING_FILE,
    "Plugin directory '%s' could not be opened.  You may have forgotten to set your NITF_PLUGIN_PATH environment variable",
    reg->path);
 return (NITF_BOOL)0;
    }


    while (dir)
    {


 if ( (entry = readdir(dir)) != NULL )
 {
     char *end;

     if ((end = strstr( entry->d_name, NITF_DLL_EXTENSION )) != NULL)
     {

  const char* p = entry->d_name;
  char keyName[ NITF_MAX_PATH ] = "";
  char fullName[ NITF_MAX_PATH ] = "";
  int ok;
  int i;
  nitf_DLL* dll;
  const char** ident;

  strcpy(fullName, reg->path);
  strcat(fullName, entry->d_name);


  dll = nitf_DLL_construct(error);
  if (! dll )
  {

      nitf_Directory_destruct(&dir);
      return NITF_FAILURE;
  }

  ok = nitf_DLL_load( dll, fullName, error );

  if (! ok )
  {

      nitf_Directory_destruct(&dir);
      return NITF_FAILURE;
  }


  memset(keyName, 0, NITF_MAX_PATH);
  for(i = 0; p != end; i++) {  keyName[i] = *p++;  }


  ident = doInit(dll, keyName, error);



  if (!ident)
  {

      return NITF_FAILURE;
  }


  ok = insertPlugin(reg,
      ident,
      dll,
      error);



  if ( !ok )
  {
      closedir(dir);
      return (NITF_BOOL)0;
  }




     }
 }
 else
 {
        nitf_Directory_destruct(&dir);
     return NITF_SUCCESS;
 }

    }
    nitf_Directory_destruct(&dir);
 */
NITF_CXX_ENDGUARD

#endif
