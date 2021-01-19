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
#include <numeric>

#include <six/sicd/SICDMesh.h>
#include <six/Serialize.h>

namespace six
{
namespace sicd
{
const char SICDMeshes::SLANT_PLANE_MESH_ID[] = "Slant_Plane_Mesh";
const char SICDMeshes::OUTPUT_PLANE_MESH_ID[] = "Output_Plane_Mesh";
const char SICDMeshes::NOISE_MESH_ID[] = "Noise_Mesh";
const char SICDMeshes::SCALAR_MESH_ID[] = "Scalar Mesh";

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

ScalarMesh::ScalarMesh(const std::string& name):
    PlanarCoordinateMesh(name),
    mNumScalarsPerCoord(std::numeric_limits<size_t>::max())
{
}

ScalarMesh::ScalarMesh(
        const std::string& name,
        const types::RowCol<size_t>& meshDims,
        const std::vector<double>& x,
        const std::vector<double>& y,
        size_t numScalarsPerCoord,
        const std::map<std::string, std::vector<double> >& scalars):
    PlanarCoordinateMesh(name, meshDims, x, y),
    mNumScalarsPerCoord(numScalarsPerCoord),
    mScalars(scalars)
{
}

std::vector<Mesh::Field> ScalarMesh::getFields() const
{
    std::vector<Mesh::Field> fields = PlanarCoordinateMesh::getFields();

    // The fields of a scalar mesh are not known until
    // scalar data has been provided - throw if it is
    // uninitialized.
    if (mScalars.empty())
    {
        throw except::Exception(
                Ctxt("Requested fields of an uninitialized Scalar Mesh."));
    }

    const std::string doubleTypeStr = "double";
    std::map<std::string, std::vector<double> >::const_iterator it =
            mScalars.begin();
    for (; it != mScalars.end(); ++it)
    {
        Field field;
        field.name = it->first;
        field.type = doubleTypeStr;
        fields.push_back(field);
    }

    return fields;
}

void ScalarMesh::serialize(std::vector<sys::byte>& values) const
{
    PlanarCoordinateMesh::serialize(values);

    six::serialize(mNumScalarsPerCoord, mSwapBytes, values);

    // Serialize the map
    std::map<std::string, std::vector<double> >::const_iterator it =
            mScalars.begin();
    for (; it != mScalars.end(); ++it)
    {
        six::serialize(it->first, mSwapBytes, values);
        six::serialize(it->second, mSwapBytes, values);
    }
}

void ScalarMesh::deserialize(const sys::byte*& values)
{
    PlanarCoordinateMesh::deserialize(values);

    six::deserialize(values, mSwapBytes, mNumScalarsPerCoord);

    // Deserialize the std::string --> std::vector<double> scalar
    // data.
    for (size_t ii = 0; ii < mNumScalarsPerCoord; ++ii)
    {
        std::string key;
        six::deserialize(values, mSwapBytes, key);

        std::vector<double>& scalars = mScalars[key];
        six::deserialize(values, mSwapBytes, scalars);
    }
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
