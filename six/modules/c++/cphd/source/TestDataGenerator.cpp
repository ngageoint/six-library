/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <cphd/TestDataGenerator.h>

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

#include <nitf/coda-oss.hpp>

#include <cphd/Enums.h>
#include <cphd/Types.h>
#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>
#include <cphd/Metadata.h>

namespace cphd
{

double getRandom()
{
    const double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    return -1000.0  + r * 2000.0;
}

Vector3 getRandomVector3()
{
    Vector3 ret;
    ret[0] = getRandom();
    ret[1] = getRandom();
    ret[2] = getRandom();
    return ret;
}

void setPVPXML(Pvp& pvp)
{
    // Size, Offset, Format
    pvp.append(pvp.txTime);
    pvp.append(pvp.txPos);
    pvp.append(pvp.txVel);
    pvp.append(pvp.rcvTime);
    pvp.append(pvp.rcvPos);
    pvp.append(pvp.rcvVel);
    pvp.append(pvp.srpPos);
    pvp.append(pvp.aFDOP);
    pvp.append(pvp.aFRR1);
    pvp.append(pvp.aFRR2);
    pvp.append(pvp.fx1);
    pvp.append(pvp.fx2);
    pvp.append(pvp.toa1);
    pvp.append(pvp.toa2);
    pvp.append(pvp.tdTropoSRP);
    pvp.append(pvp.sc0);
    pvp.append(pvp.scss);
}

void setVectorParameters(size_t channel,
                          size_t vector,
                          PVPBlock& pvpBlock)
{
    const double txTime = getRandom();
    pvpBlock.setTxTime(txTime, channel, vector);

    const Vector3 txPos = getRandomVector3();
    pvpBlock.setTxPos(txPos, channel, vector);

    const Vector3 txVel = getRandomVector3();
    pvpBlock.setTxVel(txVel, channel, vector);

    const double rcvTime = getRandom();
    pvpBlock.setRcvTime(rcvTime, channel, vector);

    const Vector3 rcvPos = getRandomVector3();
    pvpBlock.setRcvPos(rcvPos, channel, vector);

    const Vector3 rcvVel = getRandomVector3();
    pvpBlock.setRcvVel(rcvVel, channel, vector);

    const Vector3 srpPos = getRandomVector3();
    pvpBlock.setSRPPos(srpPos, channel, vector);

    const double aFDOP = getRandom();
    pvpBlock.setaFDOP(aFDOP, channel, vector);

    const double aFRR1 = getRandom();
    pvpBlock.setaFRR1(aFRR1, channel, vector);

    const double aFRR2 = getRandom();
    pvpBlock.setaFRR2(aFRR2, channel, vector);

    const double fx1 = getRandom();
    pvpBlock.setFx1(fx1, channel, vector);

    const double fx2 = getRandom();
    pvpBlock.setFx2(fx2, channel, vector);

    const double toa1 = getRandom();
    pvpBlock.setTOA1(toa1, channel, vector);

    const double toa2 = getRandom();
    pvpBlock.setTOA2(toa2, channel, vector);

    const double tdTropoSRP = getRandom();
    pvpBlock.setTdTropoSRP(tdTropoSRP, channel, vector);

    const double sc0 = getRandom();
    pvpBlock.setSC0(sc0, channel, vector);

    const double scss = getRandom();
    pvpBlock.setSCSS(scss, channel, vector);
}

void setUpMetadata(Metadata& metadata)
{
    // We must have a collectType set
    metadata.collectionID.collectType = CollectType::MONOSTATIC;
    metadata.collectionID.setClassificationLevel("Unclassified");
    metadata.collectionID.releaseInfo = "Release";
    // We must have a radar mode set
    metadata.collectionID.radarMode = RadarModeType::SPOTLIGHT;

    // We must set pvpNumBytes
    // Set default pvp size based on setPVP function
    metadata.data.numBytesPVP = (1*12 + 3*5) * 8;

    metadata.sceneCoordinates.iarp.ecf = getRandomVector3();
    metadata.sceneCoordinates.iarp.llh = LatLonAlt(0,0,0);
    metadata.sceneCoordinates.referenceSurface.planar.reset(new Planar());
    metadata.sceneCoordinates.referenceSurface.planar->uIax = getRandomVector3();
    metadata.sceneCoordinates.referenceSurface.planar->uIay = getRandomVector3();
    // We must have corners set
    for (size_t ii = 0; ii < six::Corners<double>::NUM_CORNERS; ++ii)
    {
        metadata.sceneCoordinates.imageAreaCorners.getCorner(ii).clearLatLon();
    }
    metadata.channel.fxFixedCphd = true;
    metadata.channel.toaFixedCphd = false;
    metadata.channel.srpFixedCphd = false;
    metadata.referenceGeometry.srp.ecf = getRandomVector3();
    metadata.referenceGeometry.srp.iac = getRandomVector3();
    metadata.referenceGeometry.monostatic.reset(new Monostatic());
    metadata.referenceGeometry.monostatic->arpPos = getRandomVector3();
    metadata.referenceGeometry.monostatic->arpVel = getRandomVector3();
}
}
