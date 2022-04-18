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
#ifndef __SIX_SICD_MESH_H__
#define __SIX_SICD_MESH_H__

#include <map>

#include <mem/ScopedCopyablePtr.h>
#include <six/Mesh.h>

namespace six
{
namespace sicd
{
/*!
 * \struct SICDMeshes
 * \brief Struct holds the default names for SICD Mesh objects
 */
struct SICDMeshes
{
    //! Type ID for SICD slant plane mesh
    static const char SLANT_PLANE_MESH_ID[];

    //! Type ID for SICD output plane mesh
    static const char OUTPUT_PLANE_MESH_ID[];

    //! Type ID for SICD noise mesh
    static const char NOISE_MESH_ID[];

    //! Type ID for SICD scalar mesh
    static const char SCALAR_MESH_ID[];
};

/*!
 *  \class PlanarCoordinateMesh
 *  \brief A storage structure that holds position and value
 *   information. A planar coordinate mesh holds position information
 *   in (x,y) coordinates.
 */ 
class PlanarCoordinateMesh : public Mesh
{
public:

    /*!
     * PlanarCoordinateMesh constructor. Mesh data is uninitialized.
     * \param name Name of the mesh
     */
    PlanarCoordinateMesh(const std::string& name);

    /*!
     * PlanarCoordinateMesh constructor. Mesh data is initialized.
     * \param name Name of the mesh.
     * \param meshDims The mesh dimensions.
     * \param x The x-coordinates for the mesh.
     * \param y The y-coordinates for the mesh.
     */
    PlanarCoordinateMesh(const std::string& name,
                         const types::RowCol<size_t>& meshDims,
                         const std::vector<double>& x,
                         const std::vector<double>& y);

    PlanarCoordinateMesh(const PlanarCoordinateMesh&) = delete;
    PlanarCoordinateMesh& operator=(const PlanarCoordinateMesh&) = delete;
    PlanarCoordinateMesh(PlanarCoordinateMesh&&) = default;
    PlanarCoordinateMesh& operator=(PlanarCoordinateMesh&&) = delete;

    //! \return The mesh name
    std::string getName() const
    {
        return mName;
    }

    //! \return The mesh dimensions
    types::RowCol<size_t> getMeshDims() const
    {
        return mMeshDims;
    }

    //! \return The flattened x-coordinates of the mesh
    const std::vector<double>& getX() const
    {
        return mX;
    }

    //! \return The flattened y-coordinates of the mesh
    const std::vector<double>& getY() const
    {
        return mY;
    }

    //! \return The vector of Field descriptors (file name, type)
    virtual std::vector<Mesh::Field> getFields() const;

    /*!
     * Serializes the mesh to binary
     * \param[out] values The serialized data.
     */
    virtual void serialize(std::vector<sys::byte>& values) const;
    void serialize(std::vector<std::byte>& values) const override
    {
        std::vector<sys::byte> values_;
        serialize(values_);
        void* pData = values_.data();
        auto begin = static_cast<std::byte*>(pData);
        auto end = begin + values_.size();
        values.insert(values.end(), begin, end);
    }

    /*!
     * Deserializes from binary to a mesh. 
     * \param values Data to deserialize.
     */
    virtual void deserialize(const sys::byte*& values);
    void deserialize(const std::byte*& values) override
    {
        auto& values_ = reinterpret_cast<const sys::byte*&>(values);
        deserialize(values_);
    }

protected:
    const bool mSwapBytes;
    std::string mName;
    types::RowCol<size_t> mMeshDims;
    std::vector<double> mX;
    std::vector<double> mY;
};

/*!
 *  \class ScalarMesh
 *  \brief A storage structure that associates an arbitrary number of
 *         scalar values with an x,y coordinate pair.
 */
class ScalarMesh : public PlanarCoordinateMesh
{
public:
    /**
     * Constructor. Mesh data is uninitialized.
     *
     * \param name Name of the mesh.
     */
    ScalarMesh(const std::string& name);

    /**
     * Constructor. Mesh data is initialized.
     *
     * \param name Name of the mesh.
     * \param meshDims The mesh dimensions.
     * \param x The x-coordinates for the mesh.
     * \param y The y-coordinates for the mesh.
     * \param numScalarsPerCoord Number of scalars per x,y coordinate
     *        i.e the size of the provided scalars map.
     * \param scalars The scalar values for the mesh. This is a map
     *        from the scalar name (e.g "temperature", "height" etc.)
     *        to the associated values.
     */
    ScalarMesh(const std::string& name,
               const types::RowCol<size_t>& meshDims,
               const std::vector<double>& x,
               const std::vector<double>& y,
               size_t numScalarsPerCoord,
               const std::map<std::string, std::vector<double> >& scalars);


    ScalarMesh(const ScalarMesh&) = delete;
    ScalarMesh& operator=(const ScalarMesh&) = delete;

    /*!
     * \returns The scalar values over the mesh.
     */
    const std::map<std::string, std::vector<double> >& getScalars() const
    {
        return mScalars;
    }

    /*!
     * \returns Number of scalars at a given x,y coordinate i.e
     *          the size of the map returned by getScalars().
     */
    size_t getNumScalarsPerCoord() const
    {
        return mNumScalarsPerCoord;
    }

    /*!
     * Get the mesh fields and types.
     *
     * \return The vector of Field descriptions
     *
     * \note The fields of this mesh are not known until a map of
     *       scalars has been provided. This method will throw if the mesh
     *       is uninitialized.
     */
    virtual std::vector<Mesh::Field> getFields() const;

    /*!
     * Serializes the mesh to binary
     *
     * \param[out] values The serialized data.
     */
    virtual void serialize(std::vector<sys::byte>& values) const;
    void serialize(std::vector<std::byte>& values) const override
    {
        std::vector<sys::byte> values_;
        serialize(values_);
        auto begin = reinterpret_cast<std::byte*>(values_.data());
        auto end = begin + values_.size();
        values.insert(values.end(), begin, end);
    }

    /*!
     * Deserializes from binary to a mesh.
     *
     * \param values Data to deserialize.
     */
    virtual void deserialize(const sys::byte*& values);
    void deserialize(const std::byte*& values) override
    {
        auto& values_ = reinterpret_cast<const sys::byte*&>(values);
        deserialize(values_);
    }

private:
    //! The number of scalars per x,y coordinate.
    size_t mNumScalarsPerCoord;

    //! Map between the scalar identifier and the values over
    //! the mesh.
    std::map<std::string, std::vector<double> > mScalars;
};

/*!
 *  \class NoiseMesh
 *  \brief A storage structure that holds position and noise
 *   information. A noise mesh holds position information and
 *   noise-related values at these positions.
 */
class NoiseMesh : public PlanarCoordinateMesh
{
public:

    /*!
     * NoiseMesh constructor. Mesh data is uninitialized.
     * \param name Name of the mesh.
     */
    NoiseMesh(const std::string& name);

    /*!
     * NoiseMesh constructor. Mesh data is initialized.
     * \param name Name of the mesh.
     * \param meshDims The mesh dimensions.
     * \param x The x-coordinates for the mesh.
     * \param y The y-coordinates for the mesh.
     * \param mainBeamNoise Main beam noise values.
     * \param azimuthAmbiguityNoise Azimuth ambiguity noise values.
     * \param combinedNoise Combined noise values.
     */
    NoiseMesh(const std::string& name,
              const types::RowCol<size_t>& meshDims,
              const std::vector<double>& x,
              const std::vector<double>& y,
              const std::vector<double>& mainBeamNoise,
              const std::vector<double>& azimuthAmbiguityNoise,
              const std::vector<double>& combinedNoise);

    NoiseMesh(const NoiseMesh&) = delete;
    NoiseMesh& operator=(const NoiseMesh&) = delete;

    //! \return The flat main beam noise over the mesh (amp)
    const std::vector<double>& getMainBeamNoise() const
    {
        return mMainBeamNoise;
    }

    //! \return The flat azimuth ambiguity noise over the mesh (amp)
    const std::vector<double>& getAzimuthAmbiguityNoise() const
    {
        return mAzimuthAmbiguityNoise;
    }

    //! \return The flat noise floor values over the mesh (power)
    const std::vector<double>& getCombinedNoise() const
    {
        return mCombinedNoise;
    }

    /*!
     * Get the mesh fields and types.
     * \return The vector of Field descriptions
     */
    std::vector<Mesh::Field> getFields() const;

    /*!
     * Serializes the mesh to binary
     * \param[out] values The serialized data.
     */
    virtual void serialize(std::vector<sys::byte>& values) const;
    void serialize(std::vector<std::byte>& values) const override
    {
        std::vector<sys::byte> values_;
        serialize(values_);
        auto begin = reinterpret_cast<std::byte*>(values_.data());
        auto end = begin + values_.size();
        values.insert(values.end(), begin, end);
    }

    /*!
     * Deserializes from binary to a mesh. 
     * \param values Data to deserialize.
     */
    virtual void deserialize(const sys::byte*& values);
    void deserialize(const std::byte*& values) override
    {
        auto& values_ = reinterpret_cast<const sys::byte*&>(values);
        deserialize(values_);
    }

protected:
    std::vector<double> mMainBeamNoise;
    std::vector<double> mAzimuthAmbiguityNoise;
    std::vector<double> mCombinedNoise;
};
}
}
#endif
