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

#pragma once
#ifndef NITRO_nitf_import_nitf_h_INCLUDED_
#define NITRO_nitf_import_nitf_h_INCLUDED_

#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4820) // '...' : '...' bytes padding added after data member '...'
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' function under -EHc. Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 5105) // macro expansion producing '...' has undefined behavior
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' 
#pragma warning(disable: 5105) // macro expansion producing '...' has undefined behavior
#endif

#if _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#undef min
#undef max
#endif

#include "nitf/BandInfo.h"
#include "nitf/BandSource.h"
#include "nitf/ComponentInfo.h"
#include "nitf/DESegment.h"
#include "nitf/DESubheader.h"
#include "nitf/DefaultTRE.h"
#include "nitf/DownSampler.h"
#include "nitf/Extensions.h"
#include "nitf/Field.h"
#include "nitf/FieldWarning.h"
#include "nitf/FileHeader.h"
#include "nitf/FileSecurity.h"
#include "nitf/GraphicSegment.h"
#include "nitf/GraphicSubheader.h"
#include "nitf/ImageIO.h"
#include "nitf/ImageReader.h"
#include "nitf/ImageSegment.h"
#include "nitf/ImageSource.h"
#include "nitf/ImageSubheader.h"
#include "nitf/ImageWriter.h"
#include "nitf/LabelSegment.h"
#include "nitf/LabelSubheader.h"
#include "nitf/LookupTable.h"
#include "nitf/PluginIdentifier.h"
#include "nitf/PluginRegistry.h"
#include "nitf/RESegment.h"
#include "nitf/RESubheader.h"
#include "nitf/RowSource.h"
#include "nitf/Reader.h"
#include "nitf/Record.h"
#include "nitf/SegmentReader.h"
#include "nitf/SegmentSource.h"
#include "nitf/StreamIOWriteHandler.h"
#include "nitf/SubWindow.h"
#include "nitf/System.h"
#include "nitf/TRE.h"
#include "nitf/TREUtils.h"
#include "nitf/TextSegment.h"
#include "nitf/TextSubheader.h"
#include "nitf/WriteHandler.h"
#include "nitf/Writer.h"
#include "nitf/DirectBlockSource.h"
#include "nitf/WriterOptions.h"

#if _MSC_VER
#pragma warning(pop)
#endif

#endif // NITRO_nitf_import_nitf_h_INCLUDED_