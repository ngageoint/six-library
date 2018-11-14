/* =========================================================================
 * This file is part of six-c++
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

namespace
{

/*!
 * \struct serializer
 * \tparam T Scalar type
 * \brief Implements serialization and deserialization for scalar
 *  types
 */
template<typename T>
struct serializer
{
    /*!
     * Serialize a value into a byte buffer.
     * \param val The value to serialize.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] values The serialized data.
     */
    static void serializeImpl(const T& val,
                              bool swapBytes,
                              std::vector<sys::byte>& buffer)
    {
        const size_t length = sizeof(T);
        const sys::byte* data = reinterpret_cast<const sys::byte*>(&val);

        if (swapBytes)
        {
            const size_t prevLength = buffer.size();
            buffer.resize(prevLength + length);
            sys::byteSwap(data,
                          static_cast<unsigned short>(length),
                          1,
                          &buffer[prevLength]);
        }
        else
        {
            std::copy(data, data + length, std::back_inserter(buffer));
        }
    }

    /*!
     * Deserialize a byte stream into a value.
     * \param values The data to deserialize.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] val The value to deserialize into.
     */
    static void deserializeImpl(const sys::byte*& buffer, bool swapBytes, T& val)
    {
        const size_t length = sizeof(T);
        sys::byte* data = reinterpret_cast<sys::byte*>(&val);
        std::copy(buffer, buffer + length, data);
        if (swapBytes)
        {
            sys::byteSwap(data, static_cast<unsigned short>(length), 1);
        }

        buffer += length;
    }
};

/*!
 * \struct serializer
 * \tparam T Scalar type
 * \brief Implements serialization and deserialization for vectors of
 *  scalar types
 */
template<typename T>
struct serializer<std::vector<T> >
{
    /*!
     * Serialize a vector of values into a byte buffer.
     * \param val The vector of values to serialize.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] values The serialized data.
     */
    static void serializeImpl(const std::vector<T>& val,
                              bool swapBytes,
                              std::vector<sys::byte>& buffer)
    {
        const size_t length = val.size();

        ::serializer<size_t>::serializeImpl(length, swapBytes, buffer);
        for (size_t ii = 0; ii < length; ++ii)
        {
            ::serializer<T>::serializeImpl(val[ii], swapBytes, buffer);
        }
    }

    /*!
     * Deserialize a byte stream into a value.
     * \param values The data to deserialize.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] val The vector of values to deserialize into.
     */
    static void deserializeImpl(const sys::byte*& buffer,
                                bool swapBytes,
                                std::vector<T>& val)
    {
        const size_t currentVectorLength = val.size();
        size_t length;
        ::serializer<size_t>::deserializeImpl(buffer, swapBytes, length);
        val.resize(currentVectorLength + length);

        for (size_t ii = 0; ii < length; ++ii)
        {
            ::serializer<T>::deserializeImpl(buffer, swapBytes,
                val[currentVectorLength + ii]);
        }
    }
};

/*!
 * Function interface to serialize
 * \param val Values to serialize
 * \param[out] buffer Address to serialize into
 * \param swapBytes Should the bytes be swapped?
 */
template<typename T>
void serialize(const T& val, bool swapBytes, std::vector<sys::byte>& buffer)
{
    serializer<T>::serializeImpl(val, swapBytes, buffer);
}

/*!
 * Function interface to deserialize
 * \param buffer Address to deserialize from
 * \param swapBytes Should bytes be swapped?
 * \param[out] Value to deserialize into.
 */
template<typename T>
void deserialize(const sys::byte*& buffer, bool swapBytes, T& val)
{
    serializer<T>::deserializeImpl(buffer, swapBytes, val);
}
}

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
    ::serialize(mMeshDims.row, mSwapBytes, values);
    ::serialize(mMeshDims.col, mSwapBytes, values);
    ::serialize(mX, mSwapBytes, values);
    ::serialize(mY, mSwapBytes, values);
}

void PlanarCoordinateMesh::deserialize(const sys::byte*& values)
{
    ::deserialize(values, mSwapBytes, mMeshDims.row);
    ::deserialize(values, mSwapBytes, mMeshDims.col);
    ::deserialize(values, mSwapBytes, mX);
    ::deserialize(values, mSwapBytes, mY);
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

    ::serialize(mMainBeamNoise, mSwapBytes, values);
    ::serialize(mAzimuthAmbiguityNoise, mSwapBytes, values);
    ::serialize(mCombinedNoise, mSwapBytes, values);
}

void NoiseMesh::deserialize(const sys::byte*& values)
{
    PlanarCoordinateMesh::deserialize(values);

    ::deserialize(values, mSwapBytes, mMainBeamNoise);
    ::deserialize(values, mSwapBytes, mAzimuthAmbiguityNoise);
    ::deserialize(values, mSwapBytes, mCombinedNoise);
}
}
}
