/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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

#include "tiff/KnownTags.h"

#include <string>

#include "tiff/Common.h"
#include "tiff/IFDEntry.h"
#include <import/mt.h>

tiff::KnownTags::KnownTags()
{
    addEntry(254, tiff::Const::Type::LONG, "NewSubfileType");
    addEntry(255, tiff::Const::Type::SHORT, "SubfileType");
    addEntry(256, tiff::Const::Type::LONG, tiff::KnownTags::IMAGE_WIDTH);
    addEntry(257, tiff::Const::Type::LONG, tiff::KnownTags::IMAGE_LENGTH);
    addEntry(258, tiff::Const::Type::SHORT, tiff::KnownTags::BITS_PER_SAMPLE);
    addEntry(259, tiff::Const::Type::SHORT, tiff::KnownTags::COMPRESSION);
    addEntry(262, tiff::Const::Type::SHORT, tiff::KnownTags::PHOTOMETRIC_INTERPRETATION);
    addEntry(263, tiff::Const::Type::SHORT, "Thresholding");
    addEntry(264, tiff::Const::Type::SHORT, "CellWidth");
    addEntry(265, tiff::Const::Type::SHORT, "CellLength");
    addEntry(266, tiff::Const::Type::SHORT, "FillOrder");
    addEntry(269, tiff::Const::Type::ASCII, "DocumentName");
    addEntry(270, tiff::Const::Type::ASCII, "ImageDescription");
    addEntry(271, tiff::Const::Type::ASCII, "Make");
    addEntry(272, tiff::Const::Type::ASCII, "Model");
    addEntry(273, tiff::Const::Type::LONG, "StripOffsets");
    addEntry(274, tiff::Const::Type::SHORT, "Orientation");
    addEntry(277, tiff::Const::Type::SHORT, tiff::KnownTags::SAMPLES_PER_PIXEL);
    addEntry(278, tiff::Const::Type::LONG, "RowsPerStrip");
    addEntry(279, tiff::Const::Type::LONG, "StripByteCounts");
    addEntry(280, tiff::Const::Type::SHORT, "MinSampleValue");
    addEntry(281, tiff::Const::Type::SHORT, "MaxSampleValue");
    addEntry(282, tiff::Const::Type::RATIONAL, "XResolution");
    addEntry(283, tiff::Const::Type::RATIONAL, "YResolution");
    addEntry(284, tiff::Const::Type::SHORT, "PlanarConfiguration");
    addEntry(285, tiff::Const::Type::ASCII, "PageName");
    addEntry(286, tiff::Const::Type::RATIONAL, "XPosition");
    addEntry(287, tiff::Const::Type::RATIONAL, "YPosition");
    addEntry(288, tiff::Const::Type::LONG, "FreeOffsets");
    addEntry(289, tiff::Const::Type::LONG, "FreeByteCounts");
    addEntry(290, tiff::Const::Type::SHORT, "GrayResponseUnit");
    addEntry(291, tiff::Const::Type::SHORT, "GrayResponseCurve");
    addEntry(292, tiff::Const::Type::LONG, "T4Options");
    addEntry(293, tiff::Const::Type::LONG, "T6Options");
    addEntry(296, tiff::Const::Type::SHORT, "ResolutionUnit");
    addEntry(297, tiff::Const::Type::SHORT, "PageNumber");
    addEntry(301, tiff::Const::Type::SHORT, "TransferFunction");
    addEntry(305, tiff::Const::Type::ASCII, "Software");
    addEntry(306, tiff::Const::Type::ASCII, "DateTime");
    addEntry(315, tiff::Const::Type::ASCII, "Artist");
    addEntry(316, tiff::Const::Type::ASCII, "HostComputer");
    addEntry(317, tiff::Const::Type::SHORT, "Predictor");
    addEntry(318, tiff::Const::Type::RATIONAL, "WhitePoint");
    addEntry(319, tiff::Const::Type::RATIONAL, "PrimaryChromaticities");
    addEntry(320, tiff::Const::Type::SHORT, "ColorMap");
    addEntry(321, tiff::Const::Type::SHORT, "HalftoneHints");
    addEntry(322, tiff::Const::Type::LONG, "TileWidth");
    addEntry(323, tiff::Const::Type::LONG, "TileLength");
    addEntry(324, tiff::Const::Type::LONG, "TileOffsets");
    addEntry(325, tiff::Const::Type::LONG, "TileByteCounts");
    addEntry(326, tiff::Const::Type::LONG, "BadFaxLines");
    addEntry(327, tiff::Const::Type::LONG, "CleanFaxData");
    addEntry(328, tiff::Const::Type::LONG, "ConsecutiveBadFaxLines");
    addEntry(332, tiff::Const::Type::SHORT, "InkSet");
    addEntry(333, tiff::Const::Type::ASCII, "InkNames");
    addEntry(334, tiff::Const::Type::SHORT, "NumberOfInks");
    addEntry(336, tiff::Const::Type::SHORT, "DotRange");
    addEntry(337, tiff::Const::Type::ASCII, "TargetPrinter");
    addEntry(338, tiff::Const::Type::SHORT, "ExtraSamples");
    addEntry(339, tiff::Const::Type::SHORT, tiff::KnownTags::SAMPLE_FORMAT);
    addEntry(340, tiff::Const::Type::UNDEFINED, "SMinSampleValue");
    addEntry(341, tiff::Const::Type::UNDEFINED, "SMaxSampleValue");
    addEntry(342, tiff::Const::Type::SHORT, "TransferRange");
    addEntry(512, tiff::Const::Type::SHORT, "JPEGProc");
    addEntry(513, tiff::Const::Type::LONG, "JPEGInterchangeFormat");
    addEntry(514, tiff::Const::Type::LONG, "JPEGInterchangeFormatLngth");
    addEntry(515, tiff::Const::Type::SHORT, "JPEGRestartInterval");
    addEntry(517, tiff::Const::Type::SHORT, "JPEGLosslessPredictors");
    addEntry(518, tiff::Const::Type::SHORT, "JPEGPointTransforms");
    addEntry(519, tiff::Const::Type::LONG, "JPEGQTables");
    addEntry(520, tiff::Const::Type::LONG, "JPEGDCTables");
    addEntry(521, tiff::Const::Type::LONG, "JPEGACTables");
    addEntry(529, tiff::Const::Type::RATIONAL, "YCbCrCoefficients");
    addEntry(530, tiff::Const::Type::SHORT, "YCbCrSubSampling");
    addEntry(531, tiff::Const::Type::SHORT, "YCbCrPositioning");
    addEntry(532, tiff::Const::Type::LONG, "ReferenceBlackWhite");
    addEntry(559, tiff::Const::Type::LONG, "StripRowCounts");
    addEntry(700, tiff::Const::Type::BYTE, "XMP");
    addEntry(32781, tiff::Const::Type::ASCII, "ImageID");
    addEntry(33432, tiff::Const::Type::ASCII, "Copyright");
    addEntry(34732, tiff::Const::Type::LONG, "ImageLayer");

    // GeoTIFF Tags
    addEntry(33550, tiff::Const::Type::DOUBLE, "ModelPixelScaleTag");
    addEntry(33922, tiff::Const::Type::DOUBLE, "ModelTiepointTag");
    addEntry(34264, tiff::Const::Type::DOUBLE, "ModelTransformationTag");
    addEntry(34735, tiff::Const::Type::SHORT, "GeoKeyDirectoryTag");
    addEntry(34736, tiff::Const::Type::DOUBLE, "GeoDoubleParamsTag");
    addEntry(34737, tiff::Const::Type::ASCII, "GeoAsciiParamsTag");
}


tiff::KnownTags::~KnownTags()
{
    for(std::map<unsigned short, tiff::IFDEntry*>::iterator it = mKnownTags.begin();
        it != mKnownTags.end(); ++it)
        delete it->second;
    mKnownTags.clear();
}

void tiff::KnownTags::addEntry(const unsigned short tag, 
                               const unsigned short type, 
                               const std::string& name)
{
    std::map<unsigned short, tiff::IFDEntry*>::iterator pos =
        mKnownTags.find(tag);
    if (pos != mKnownTags.end())
        return;

    mNameMap[name] = tag;
    mKnownTags[tag]   = new tiff::IFDEntry(tag, type, name);
}

tiff::IFDEntry *tiff::KnownTags::operator[] (const std::string& nameKey)
{
    unsigned short tagKey = mNameMap[nameKey];
    return (*this)[tagKey];
}

tiff::IFDEntry *tiff::KnownTags::operator[] (const unsigned short tagKey)
{
    std::map<unsigned short, tiff::IFDEntry*>::iterator pos =
        mKnownTags.find(tagKey);
    return pos != mKnownTags.end() ? pos->second : nullptr;
}

