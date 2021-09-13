/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2021 Maxar Technologies, Inc.
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
#ifndef SIX_sicd_NITFReadComplexXMLControl_h_INCLUDED_
#define SIX_sicd_NITFReadComplexXMLControl_h_INCLUDED_
#pragma once

#include <vector>
#include <std/filesystem>
#include <memory>
#include <std/cstddef>

#include <logging/Logger.h>

#include "six/XMLControlFactory.h"
#include "six/NITFReadControl.h"
#include "six/Region.h"
#include "six/sicd/ComplexData.h"
#include "six/sicd/SICDMesh.h"

namespace six
{
	namespace sicd
	{
		class NITFReadComplexXMLControl final
		{
			six::XMLControlRegistry xmlRegistry;
			six::NITFReadControl reader;
			std::unique_ptr<logging::Logger> pLog;

		public:
			NITFReadComplexXMLControl();
			~NITFReadComplexXMLControl() = default;
			NITFReadComplexXMLControl(const NITFReadComplexXMLControl&) = delete;
			NITFReadComplexXMLControl& operator=(const NITFReadComplexXMLControl&) = delete;
			NITFReadComplexXMLControl(NITFReadComplexXMLControl&&) = delete;
			NITFReadComplexXMLControl& operator=(NITFReadComplexXMLControl&&) = delete;

			const six::NITFReadControl& NITFReadControl() const { return reader; }
			six::NITFReadControl& NITFReadControl() { return reader; }

			void load(const std::string& fromFile,
				const std::vector<std::string>& schemaPaths);
			void load(const std::filesystem::path& fromFile,
				const std::vector<std::filesystem::path>& schemaPaths);
			void load(const std::filesystem::path& fromFile)
			{
				static const std::vector<std::filesystem::path> schemaPaths;
				load(fromFile, schemaPaths);
			}

			std::shared_ptr<const six::Container> getContainer() const;
			std::shared_ptr<six::Container> getContainer();

			std::unique_ptr<ComplexData> getComplexData();

			std::vector<std::complex<float>> getWidebandData(const ComplexData&);
			void getWidebandData(const ComplexData&, const types::RowCol<size_t>& offset, const types::RowCol<size_t>& extent,
				std::complex<float>* buffer);

			void getMeshes(std::unique_ptr<NoiseMesh>&, std::unique_ptr<ScalarMesh>&) const;

			void setXMLControlRegistry();
			void setLogger();

			void interleaved(Region& region);
			std::vector<std::byte> interleaved();
		};
	}
}

#endif // SIX_sicd_NITFReadComplexXMLControl_h_INCLUDED_