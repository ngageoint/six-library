/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SIDD_UTILITIES_H__
#define __SIX_SIDD_UTILITIES_H__

#include <memory>
#include <vector>
#include <std/filesystem>
#include <std/string>

#include <import/scene.h>
#include <types/RgAz.h>
#include <six/Utilities.h>
#include <six/sidd/DerivedData.h>

namespace six
{
namespace sidd
{
class Utilities
{
public:
    // TODO: This is only needed because getSceneGeometry() isn't implemented
    //       for all projection types
    static scene::SideOfTrack getSideOfTrack(const DerivedData* derived);

    static mem::auto_ptr<scene::SceneGeometry>
    getSceneGeometry(const DerivedData* derived);

    static mem::auto_ptr<scene::GridGeometry>
    getGridGeometry(const DerivedData* derived);

    static mem::auto_ptr<scene::GridECEFTransform>
    getGridECEFTransform(const DerivedData* derived);

    static void setProductValues(Poly2D timeCOAPoly, PolyXYZ arpPoly,
            ReferencePoint ref, const Vector3* row, const Vector3* col,
            types::RgAz<double>res, Product* product);

    static void setProductValues(Vector3 arpVel, Vector3 arpPos,
            Vector3 refPos, const Vector3* row, const Vector3* col,
            types::RgAz<double>res, Product* product);

    static void setCollectionValues(Poly2D timeCOAPoly, PolyXYZ arpPoly,
            ReferencePoint ref, const Vector3* row, const Vector3* col,
            Collection* collection);

    static void setCollectionValues(Vector3 arpVel, Vector3 arpPos,
            Vector3 refPos, const Vector3* row, const Vector3* col,
            Collection* collection);

    static std::pair<six::PolarizationSequenceType, six::PolarizationSequenceType>
            convertDualPolarization(six::DualPolarizationType pol);

    static mem::auto_ptr<scene::ProjectionModel>
    getProjectionModel(const DerivedData* data);


    /*!
     * Create a fake SIDD that's populated enough for
     * general testing code to run without throwing exceptions
     *
     * \return mock DerivedData object
     */
    static mem::auto_ptr<DerivedData> createFakeDerivedData();
    static std::unique_ptr<DerivedData> createFakeDerivedData(const std::string& strVersion);


    /*
    * Parses the XML in 'xmlStream' and converts it into a DerivedData object.
    * Throws if the underlying type is not derived.
    *
    * \param xmlStream Input stream containing XML
    * \param schemaPaths Schema path(s)
    * \param log Logger
    *
    * \return Data representation of 'xmlStr'
    */
    static mem::auto_ptr<DerivedData> parseData(
        ::io::InputStream& xmlStream,
        const std::vector<std::string>& schemaPaths,
        logging::Logger& log);
    static std::unique_ptr<DerivedData> parseData(::io::InputStream& xmlStream,
        const std::vector<std::filesystem::path>*, logging::Logger&);

    /*
    * Parses the XML in 'pathname' and converts it into a DerivedData object.
    * Throws if the underlying type is not derived.
    *
    * \param pathname File containing plain text XML (not a NITF)
    * \param schemaPaths Schema path(s)
    * \param log Logger
    *
    * \return Data representation of the contents of 'pathname'
    */
    static mem::auto_ptr<DerivedData> parseDataFromFile(
        const std::string& pathname,
        const std::vector<std::string>& schemaPaths,
        logging::Logger& log);
    static std::unique_ptr<DerivedData> parseDataFromFile(const std::filesystem::path&,
        const std::vector<std::filesystem::path>*, logging::Logger* pLogger = nullptr);

    /*
    * Parses the XML in 'xmlStr' and converts it into a DerivedData object.
    *
    * \param xmlStr XML document as a string
    * \param schemaPaths Schema path(s)
    * \param log Logger
    *
    * \return Data representation of 'xmlStr'
    */
    static mem::auto_ptr<DerivedData> parseDataFromString(
        const std::string& xmlStr,
        const std::vector<std::string>& schemaPaths,
        logging::Logger& log);
    static std::unique_ptr<DerivedData> parseDataFromString(
        const std::u8string& xmlStr,
        const std::vector<std::filesystem::path>* pSchemaPaths,
        logging::Logger* pLogger=nullptr);

    /*
     * Converts 'data' back into a formatted XML string
     *
     * \param data Representation of SIDD data
     * \param schemaPaths Schema paths.  If empty, the SIX_SCHEMA_PATH
     *  environment variable will be used.
     * \param logger Logger.  If nullptr, no logger will be used.
     *
     * \return XML string representation of
     *'data'
     */
    static std::string toXMLString(const DerivedData& data,
            const std::vector<std::string>& schemaPaths,
            logging::Logger* logger);
    static std::u8string toXMLString(const DerivedData&,
        const std::vector<std::filesystem::path>*, logging::Logger* pLogger = nullptr);
};
}
}
#endif
