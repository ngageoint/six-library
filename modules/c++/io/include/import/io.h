/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once
#ifndef CODA_OSS_import_io_h_INCLUDED_
#define CODA_OSS_import_io_h_INCLUDED_

/*!
 *  \file io.h
 *  \brief Includes all headers necessary for IO.
 *
 *  This package is based on the Java io package.
 *  For the upcoming 0.1.1 release of the modules,
 *  we will support the io.File API as well.
 *
 *  The io package takes a more simple but still powerful
 *  approach to streaming.  We have a built in buffering mechanism
 *  that can be activated simply by using the InputStream interface's
 *  streamTo() method, connecting it to an OutputStream.
 *
 *  Many of the streams are also implemented as filters, allowing them
 *  to pipe or stream, or sort data that is incoming, and affect its
 *  appearance as it is outgoing.
 *
 *  Furthermore, BidirectionalStream implementors share the duties of
 *  InputStream and OutputStream, allowing for a 2-way abstraction that
 *  Java is lacking, while supporting a whole slew of InputStream sources
 *  that C++ does not support at all (URLs, sockets, serializers, etc).
 *
 *  Finally, we support the beginnings of serialization at this level,
 *  although we do little to enforce standards for it (we recommend XML/SOAP
 *  for most applications.  Check out the SOAPMessage class in xml.soap.
 *
 */

#include <io/BidirectionalStream.h>
#include <io/BufferViewStream.h>
#include <io/ByteStream.h>
#include <io/DataStream.h>
#include <io/DbgStream.h>
#include <io/InputStream.h>
#include <io/OutputStream.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <io/Seekable.h>
#include <io/Serializable.h>
#include <io/SerializableFile.h>
#include <io/PipeStream.h>
#include <io/StandardStreams.h>
#include <io/StringStream.h>
#include <io/NullStreams.h>
#include <io/ProxyStreams.h>
#include <io/FileUtils.h>
#include <io/SerializableArray.h>
#include <io/CountingStreams.h>
#include <io/RotatingFileOutputStream.h>
#include <io/StreamSplitter.h>

//#include "io/MMapInputStream.h"
//using namespace io;

#endif  // CODA_OSS_import_io_h_INCLUDED_
