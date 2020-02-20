/* =========================================================================
 * This file is part of polygon-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * polygon-c++ is free software; you can redistribute it and/or modify
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
#include <vector>
#include <algorithm>
#include <iostream>

#include <sys/Conf.h>
#include <mem/ScopedArray.h>
#include <sio/lite/FileWriter.h>

#include <polygon/DrawPolygon.h>

int main(int argc, char** argv)
{
    try
    {
        std::vector<types::RowCol<double> > points;
        points.push_back(types::RowCol<double>(400, 100));
        points.push_back(types::RowCol<double>(100, 310));
        points.push_back(types::RowCol<double>(270, 590));
        points.push_back(types::RowCol<double>(445, 576));
        points.push_back(types::RowCol<double>(600, 350));

        const types::RowCol<size_t> dims(1000, 1000);
        mem::ScopedArray<int> out(new int[dims.row * dims.col]);

        std::fill_n(out.get(), dims.area(), 0);
        polygon::drawPolygon(points, dims.row, dims.col, 1, out.get());
        sio::lite::writeSIO(out.get(), dims.row, dims.col, "polygon.sio",
                            sio::lite::FileHeader::SIGNED);

        std::fill_n(out.get(), dims.area(), 0);
        polygon::drawPolygon(points, dims.row, dims.col, 1, out.get(), true);
        sio::lite::writeSIO(out.get(), dims.row, dims.col,
                            "polygon_inverted.sio",
                            sio::lite::FileHeader::SIGNED);

        const types::RowCol<sys::SSize_T> offset(50, 75);
        std::fill_n(out.get(), dims.area(), 0);
        polygon::drawPolygon(points, dims.row, dims.col, 1, out.get(), false,
                             offset);
        sio::lite::writeSIO(out.get(), dims.row, dims.col,
                            "polygon_offset.sio",
                            sio::lite::FileHeader::SIGNED);

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }

    return 1;
}
