#ifndef __SIX_TEST_UTILS_H__
#define __SIX_TEST_UTILS_H__

#include <import/six.h>

/*!
 *  This function converts DMS corners into decimal degrees using NITRO,
 *  and then puts them into a lat-lon
 */
six::LatLonCorners makeUpCornersFromDMS()
{
    int latTopDMS[3] = { 42, 17, 50 };
    int latBottomDMS[3] = { 42, 15, 14 };
    int lonEastDMS[3] = { -83, 42, 12 };
    int lonWestDMS[3] = { -83, 45, 44 };

    const double latTopDecimal =
        nitf::Utils::geographicToDecimal(latTopDMS[0],
                                         latTopDMS[1],
                                         latTopDMS[2]);

    const double latBottomDecimal =
        nitf::Utils::geographicToDecimal(latBottomDMS[0],
                                         latBottomDMS[1],
                                         latBottomDMS[2]);

    const double lonEastDecimal =
        nitf::Utils::geographicToDecimal(lonEastDMS[0],
                                         lonEastDMS[1],
                                         lonEastDMS[2]);

    const double lonWestDecimal =
        nitf::Utils::geographicToDecimal(lonWestDMS[0],
                                         lonWestDMS[1],
                                         lonWestDMS[2]);

    six::LatLonCorners corners;
    corners.upperLeft = six::LatLon(latTopDecimal, lonWestDecimal);
    corners.upperRight = six::LatLon(latTopDecimal, lonEastDecimal);
    corners.lowerRight = six::LatLon(latBottomDecimal, lonEastDecimal);
    corners.lowerLeft = six::LatLon(latBottomDecimal, lonWestDecimal);
    return corners;
}

#endif
