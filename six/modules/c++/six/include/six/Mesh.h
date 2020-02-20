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
#ifndef __SIX_MESH_H__
#define __SIX_MESH_H__

#include <vector>
#include <iomanip>
#include <string>

#include <sys/Conf.h>
#include <six/Parameter.h>
#include <types/RowCol.h>

namespace six
{
/*!
 *  \class Mesh
 *  \brief A mesh is the abstract class for objects that hold position
 *   information and values at these positions.
 */ 
class Mesh
{
public:

    virtual ~Mesh()
    {
    }

    /*!
     * \struct Field
     * \brief Simple (name, data type) tuple describing mesh field
     */
    struct Field
    {
        std::string name;
        std::string type;
    };

    /*! 
     * Convert Field entries to Parameter types.
     * \return Vector of Parameters created from Fields.
     */
    std::vector<six::Parameter> fieldsToParameters() const;

    //! \return The mesh name
    virtual std::string getName() const = 0;

    //! \return The mesh dimensions
    virtual types::RowCol<size_t> getMeshDims() const = 0;

    //! \return The vector of Field descriptions
    virtual std::vector<Field> getFields() const = 0;

    /*!
     * Serializes the mesh to an array of byte data. Method is used in
     * conjunction with deserialize() to provide a mechanism for
     * porting the contents of a Mesh between processes. See
     * deserialize() for more information
     * \param[out] values The serialized data.
     */
    virtual void serialize(std::vector<sys::byte>& values) const = 0;

    /*!
     * Deserializes an array of byte data to populate a Mesh. This is
     * the reverse operation of serialize(), and any implementation
     * must be symmetric:
     *
     * Consider DerivedMesh inheriting from Mesh with its own member
     * data and implementations of serialize() and deserialize(). The
     * following code snippet demonstrates the symmetry of the
     * operations:
     *    DerivedMesh mesh(...); // Populated at construction
     *    std::vector<sys::byte> serializedData;
     *    mesh.serialize(buffer);
     *    DerivedMesh meshCopy;  // Not populated
     *    const sys::byte* buffer = &serializedData[0];
     *    meshCopy.deserialize(buffer); // meshCopy == mesh
     * Any implementation of serialize() and deserialize() must
     * satisfy this property.
     *
     * Note that this interface expects a reference to a
     * pointer. Calling this function will advance the pointer by a
     * length equal to the storage size of the serialized Mesh object.
     * The intent is to allow chaining together of
     * serialize/deserialize calls via inheritance. For example:
     *   LatLonMesh inherits from Mesh
     *   - Stores grid of latitude and longitude values
     *   - Serializes and deserializes two std::vector<double> for the
     *     latitude and longitude values
     *   HeightMesh inherits from LatLonMesh
     *   - Stores height values over a (lat,lon) grid
     *   - To serialize, it first calls LatLonMesh::serialize() then
     *     serializes its height info to the end of the same byte
     *     array
     *   - To deserialize, it first calls LatLonMesh::deserialize()
     *     then deserializes its height info. The pointer for this
     *     buffer will have advanced to the end of the serialized
     *     LatLonMesh
     * \param values Data to deserialize. Pointer is incremented by
     *  the serialized storage size of Mesh after calling this
     *  function.
     */
    virtual void deserialize(const sys::byte*& values) = 0;
};
}
#endif
