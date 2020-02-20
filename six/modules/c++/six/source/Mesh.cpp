/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include <six/Mesh.h>

namespace six
{
std::vector<six::Parameter> Mesh::fieldsToParameters() const
{
    std::vector<six::Parameter> parameters;
    six::Parameter p;

    // Set the mesh name
    p.setName("name");
    p.setValue(getName());
    parameters.push_back(p);

    // Set the mesh dimensions
    const types::RowCol<size_t> meshDims = getMeshDims();
    p.setName("number of row elements");
    p.setValue(meshDims.row);
    parameters.push_back(p);

    p.setName("number of column elements");
    p.setValue(meshDims.col);
    parameters.push_back(p);

    // Set the field information
    std::vector<Field> fields = getFields();
    for (size_t ii = 0; ii < fields.size(); ++ii)
    {
        // Set the field name
        p.setName("field " + str::toString<size_t>(ii+1) + " name");
        p.setValue(fields[ii].name);
        parameters.push_back(p);

        // Set the field type
        p.setName("field " + str::toString<size_t>(ii+1) + " type");
        p.setValue(fields[ii].type);
        parameters.push_back(p);
    }

    return parameters;
}
}
