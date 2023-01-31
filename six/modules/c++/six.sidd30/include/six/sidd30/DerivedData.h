/* =========================================================================
 * This file is part of six.sidd30-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd30-c++ is free software; you can redistribute it and/or modify
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
#ifndef SIX_six_sicd30_DerivedData_h_INCLUDED_
#define SIX_six_sicd30_DerivedData_h_INCLUDED_
#pragma once

#include <six/sidd/DerivedData.h>

namespace six
{
	namespace sidd30
	{
		using DerivedData = six::sidd::DerivedData;

		using DerivedClassification = six::sidd::DerivedClassification;
		using Display = six::sidd::Display;
		using Measurement = six::sidd::Measurement;
		using ExploitationFeatures = six::sidd::ExploitationFeatures;
		using Compression = six::sidd::Compression;
		using InteractiveProcessing = six::sidd::InteractiveProcessing;
		using NonInteractiveProcessing = six::sidd::NonInteractiveProcessing;
		using Filter = six::sidd::Filter;
		using LookupTable = six::sidd::LookupTable;
		using J2KCompression = six::sidd::J2KCompression;
		using DigitalElevationData = six::sidd::DigitalElevationData;
		using ProductGenerationOptions = six::sidd::ProductGenerationOptions;
		using BandEqualization = six::sidd::BandEqualization;
		using RRDS = six::sidd::RRDS;
		using GeometricTransform = six::sidd::GeometricTransform;
		using SharpnessEnhancement = six::sidd::SharpnessEnhancement;
		using ColorSpaceTransform = six::sidd::ColorSpaceTransform;
		using DynamicRangeAdjustment = six::sidd::DynamicRangeAdjustment;
	}
}
#endif // SIX_six_sicd30_DerivedData_h_INCLUDED_
