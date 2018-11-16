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

    /*!
     * Deserializes from binary to a mesh. 
     * \param values Data to deserialize.
     */
    virtual void deserialize(const sys::byte*& values);

protected:
    const bool mSwapBytes;
    std::string mName;
    types::RowCol<size_t> mMeshDims;
    std::vector<double> mX;
    std::vector<double> mY;
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

    /*!
     * Deserializes from binary to a mesh. 
     * \param values Data to deserialize.
     */
    virtual void deserialize(const sys::byte*& values);

protected:
    std::vector<double> mMainBeamNoise;
    std::vector<double> mAzimuthAmbiguityNoise;
    std::vector<double> mCombinedNoise;
};
}
}
#endif
