/* =========================================================================
 * This file is part of six
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six is free software; you can redistribute it and/or modify
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

%module six
%{
#include <import/six.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>
#include <import/nitf.hpp>
%}

%inline %{

    PyObject* readXML(const char* filename)
    {
        nitf::Reader reader;
        nitf::IOHandle io(filename);
        nitf::Record record = reader.read(io);

        std::vector<char*> xmls;

        nitf::Uint32 numDES = record.getNumDataExtensions();
        for (nitf::Uint32 i = 0; i < numDES; ++i)
        {
            nitf::DESegment segment = record.getDataExtensions()[i];
            nitf::DESubheader subheader = segment.getSubheader();

            nitf::SegmentReader deReader = reader.newDEReader(i);
            nitf::Off size = deReader.getSize();

            char* xml = new char[size];
            deReader.read(xml, size);
            xmls.push_back(xml);
        }
        io.close();

        PyObject* l = PyList_New(xmls.size());
        for (size_t i = 0, n = xmls.size(); i < n; ++i)
        {
            PyList_SetItem(l, i, PyString_FromString(xmls[i]));
            delete xmls[i];
        }
        return l;
    }

    PyObject* readImages(const char* filename)
    {
        six::XMLControlFactory::getInstance(). addCreator(
                six::DataType::COMPLEX,
                new six::XMLControlCreatorT<
                six::sicd::ComplexXMLControl>());

        six::XMLControlFactory::getInstance(). addCreator(
                six::DataType::DERIVED,
                new six::XMLControlCreatorT<
                six::sidd::DerivedXMLControl>());

        six::ReadControl* reader = new six::NITFReadControl();
        reader->load(filename);

        six::Container *container = reader->getContainer();
        six::Region region;

        size_t numData = container->getNumData();
        PyObject* l = PyList_New(numData);

        for (size_t i = 0; i < numData; ++i)
        {
            six::Data* d = container->getData(i);
            size_t nbpp = d->getNumBytesPerPixel();
            size_t height = d->getNumRows();
            size_t width = d->getNumCols();
            size_t imageSize = nbpp * width * height;
            six::UByte *buf = new six::UByte[imageSize];
            region.setBuffer(buf);
            reader->interleaved(region, i);
            
            PyObject *pySize = Py_BuildValue("(ii)", width, height);
            PyObject *pyData = PyBuffer_FromMemory((void*)buf, imageSize);

            PyList_SetItem(l, i, Py_BuildValue("(OO)", pyData, pySize));
        }
        return l;
    }

%}
