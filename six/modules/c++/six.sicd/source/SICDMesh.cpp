/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include <six/sicd/SICDMesh.h>
#include <six/Serialize.h>

namespace six
{
namespace sicd
{
const char SICDMeshes::SLANT_PLANE_MESH_ID[] = "Slant_Plane_Mesh";
const char SICDMeshes::OUTPUT_PLANE_MESH_ID[] = "Output_Plane_Mesh";
const char SICDMeshes::NOISE_MESH_ID[] = "Noise_Mesh";

PlanarCoordinateMesh::PlanarCoordinateMesh(const std::string& name):
    mSwapBytes(!sys::isBigEndianSystem()),
    mName(name)
{
}

PlanarCoordinateMesh::PlanarCoordinateMesh(
    const std::string& name,
    const types::RowCol<size_t>& meshDims,
    const std::vector<double>& x,
    const std::vector<double>& y):
    mSwapBytes(!sys::isBigEndianSystem()),
    mName(name),
    mMeshDims(meshDims),
    mX(x),
    mY(y)
{
}

std::vector<Mesh::Field> PlanarCoordinateMesh::getFields() const
{
    std::vector<Mesh::Field> fields;
    Field f;

    f.name = "Distance from SCP row";
    f.type = "double";
    fields.push_back(f);

    f.name = "Distance from SCP col";
    f.type = "double";
    fields.push_back(f);

    return fields;
}

void PlanarCoordinateMesh::serialize(std::vector<sys::byte>& values) const
{
    six::serialize(mMeshDims.row, mSwapBytes, values);
    six::serialize(mMeshDims.col, mSwapBytes, values);
    six::serialize(mX, mSwapBytes, values);
    six::serialize(mY, mSwapBytes, values);
}

void PlanarCoordinateMesh::deserialize(const sys::byte*& values)
{
    six::deserialize(values, mSwapBytes, mMeshDims.row);
    six::deserialize(values, mSwapBytes, mMeshDims.col);
    six::deserialize(values, mSwapBytes, mX);
    six::deserialize(values, mSwapBytes, mY);
}

std::vector<Mesh::Field> NoiseMesh::getFields() const
{
    std::vector<Mesh::Field> fields = PlanarCoordinateMesh::getFields();
    Field f;

    f.name = "Main beam noise";
    f.type = "double";
    fields.push_back(f);

    f.name = "Azimuth ambiguity noise";
    f.type = "double";
    fields.push_back(f);

    f.name = "Combined noise";
    f.type = "double";
    fields.push_back(f);

    return fields;
}

NoiseMesh::NoiseMesh(const std::string& name):
    PlanarCoordinateMesh(name)
{
}

NoiseMesh::NoiseMesh(const std::string& name,
                     const types::RowCol<size_t>& meshDims,
                     const std::vector<double>& x,
                     const std::vector<double>& y,
                     const std::vector<double>& mainBeamNoise,
                     const std::vector<double>& azimuthAmbiguityNoise,
                     const std::vector<double>& combinedNoise):
    PlanarCoordinateMesh(name, meshDims, x, y),
    mMainBeamNoise(mainBeamNoise),
    mAzimuthAmbiguityNoise(azimuthAmbiguityNoise),
    mCombinedNoise(combinedNoise)        
{
}

void NoiseMesh::serialize(std::vector<sys::byte>& values) const
{
    PlanarCoordinateMesh::serialize(values);

    six::serialize(mMainBeamNoise, mSwapBytes, values);
    six::serialize(mAzimuthAmbiguityNoise, mSwapBytes, values);
    six::serialize(mCombinedNoise, mSwapBytes, values);
}

void NoiseMesh::deserialize(const sys::byte*& values)
{
    PlanarCoordinateMesh::deserialize(values);

    six::deserialize(values, mSwapBytes, mMainBeamNoise);
    six::deserialize(values, mSwapBytes, mAzimuthAmbiguityNoise);
    six::deserialize(values, mSwapBytes, mCombinedNoise);
}
}
}
