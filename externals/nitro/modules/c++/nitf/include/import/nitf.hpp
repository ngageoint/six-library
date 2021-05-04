/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __IMPORT_NITF_HPP__
#define __IMPORT_NITF_HPP__
#pragma once

#include "nitf/exports.hpp"
#if NITRO_NITFCPP_EXPORTS
	// use the files in nitf/ directly
	#pragma error "Do not #include this file when building a DLL/so."
#endif

#include "nitf/coda-oss.hpp"

#include "nitf/BandInfo.hpp"
#include "nitf/BandSource.hpp"
#include "nitf/BlockingInfo.hpp"
#include "nitf/BufferedReader.hpp"
#include "nitf/BufferedWriter.hpp"
#include "nitf/ByteProvider.hpp"
#include "nitf/ComponentInfo.hpp"
#include "nitf/CompressedByteProvider.hpp"
#include "nitf/DataSource.hpp"
#include "nitf/DateTime.hpp"
#include "nitf/DESegment.hpp"
#include "nitf/DESubheader.hpp"
#include "nitf/DownSampler.hpp"
#include "nitf/Extensions.hpp"
#include "nitf/Field.hpp"
#include "nitf/FieldWarning.hpp"
#include "nitf/FileHeader.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/GraphicSegment.hpp"
#include "nitf/GraphicSubheader.hpp"
#include "nitf/Handle.hpp"
#include "nitf/HandleManager.hpp"
#include "nitf/HashTable.hpp"
#include "nitf/IOInterface.hpp"
#include "nitf/IOHandle.hpp"
#include "nitf/IOStreamReader.hpp"
#include "nitf/ImageBlocker.hpp"
#include "nitf/ImageReader.hpp"
#include "nitf/ImageSegment.hpp"
#include "nitf/ImageSegmentComputer.h"
#include "nitf/ImageSource.hpp"
#include "nitf/ImageSubheader.hpp"
#include "nitf/ImageWriter.hpp"
#include "nitf/LabelSegment.hpp"
#include "nitf/LabelSubheader.hpp"
#include "nitf/List.hpp"
#include "nitf/LookupTable.hpp"
#include "nitf/MemoryIO.hpp"
#include "nitf/NITFBufferList.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/Object.hpp"
#include "nitf/Pair.hpp"
#include "nitf/PluginRegistry.hpp"
#include "nitf/RESegment.hpp"
#include "nitf/RESubheader.hpp"
#include "nitf/Reader.hpp"
#include "nitf/Record.hpp"
#include "nitf/SegmentReader.hpp"
#include "nitf/SegmentSource.hpp"
#include "nitf/SegmentWriter.hpp"
#include "nitf/SubWindow.hpp"
#include "nitf/System.hpp"
#include "nitf/TRE.hpp"
#include "nitf/TextSegment.hpp"
#include "nitf/TextSubheader.hpp"
#include "nitf/Utils.hpp"
#include "nitf/WriteHandler.hpp"
#include "nitf/Writer.hpp"
#include "nitf/WriterOptions.hpp"

#endif
