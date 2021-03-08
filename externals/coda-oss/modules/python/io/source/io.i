/*
 * =========================================================================
 * This file is part of coda_io-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * coda_io-python is free software; you can redistribute it and/or modify
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
 */

%module(package="coda") coda_io

%feature("autodoc","1");
%include "std_string.i"

%{
  #include <sstream>
  #include "import/sys.h"
  #include "import/io.h"
  using namespace io;
%}

%include "io/InputStream.h"
%include "io/OutputStream.h"
%include "io/BidirectionalStream.h"
%include "io/Seekable.h"
%include "io/SeekableStreams.h"

%feature("notabstract") io::StringStream;
%include "io/StringStream.h"
%extend io::StringStream
{
	std::string str()
	{
		return $self->stream().str();
	}
}
%include "io/NullStreams.h"

%feature("notabstract") io::FileOutputStreamOS;
%rename("FileInputStream") io::FileInputStreamOS;
%rename("FileOutputStream") io::FileOutputStreamOS;

%include "io/FileInputStreamOS.h"
%include "io/FileOutputStreamOS.h"


%extend io::StringStream
{
    void writeBytes(PyObject* bytes)
    {
        $self->write(PyBytes_AsString(bytes), PyBytes_Size(bytes));
    }
}
