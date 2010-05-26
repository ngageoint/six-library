#ifndef __SIX_TEST_UTILS_H__
#define __SIX_TEST_UTILS_H__

#include <import/six.h>

/*!
 *  This function converts DMS corners into decimal degrees using NITRO,
 *  and then puts them into a lat-lon
 */
std::vector<six::LatLon> makeUpCornersFromDMS()
{
    int latTopDMS[3] = { 42, 17, 50 };
    int latBottomDMS[3] = { 42, 15, 14 };
    int lonEastDMS[3] = { -83, 42, 12 };
    int lonWestDMS[3] = { -83, 45, 44 };

    double latTopDecimal = nitf::Utils::geographicToDecimal(latTopDMS[0],
                                                            latTopDMS[1],
                                                            latTopDMS[2]);

    double latBottomDecimal = nitf::Utils::geographicToDecimal(latBottomDMS[0],
                                                               latBottomDMS[1],
                                                               latBottomDMS[2]);

    double lonEastDecimal = nitf::Utils::geographicToDecimal(lonEastDMS[0],
                                                             lonEastDMS[1],
                                                             lonEastDMS[2]);

    double lonWestDecimal = nitf::Utils::geographicToDecimal(lonWestDMS[0],
                                                             lonWestDMS[1],
                                                             lonWestDMS[2]);

    std::vector<six::LatLon> c(4);
    c[six::CornerIndex::FIRST_ROW_FIRST_COL] = six::LatLon(latTopDecimal,
                                                           lonWestDecimal);
    c[six::CornerIndex::FIRST_ROW_LAST_COL] = six::LatLon(latTopDecimal,
                                                          lonEastDecimal);
    c[six::CornerIndex::LAST_ROW_LAST_COL] = six::LatLon(latBottomDecimal,
                                                         lonEastDecimal);
    c[six::CornerIndex::LAST_ROW_FIRST_COL] = six::LatLon(latBottomDecimal,
                                                          lonWestDecimal);
    return c;
}

#endif
