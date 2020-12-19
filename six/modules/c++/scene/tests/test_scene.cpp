/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * scene-c++ is free software; you can redistribute it and/or modify
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
#include <import/io.h>
#include <import/util.h>
#include <import/scene.h>
#include <string>
#include <map>

using namespace io;
using namespace util;
using namespace scene;

class SceneInfo
{
public:
    
    Vector3 operator[](std::string k) const
    {
        std::map<std::string, Vector3>::const_iterator p = mVecs.find(k);
        if (p == mVecs.end())
            throw except::Exception(
                Ctxt(
                    FmtX("Key not found: %s", k.c_str())
                    )
                );
        return p->second;
    }

    void put(std::string k, Vector3 v)
    {
        mVecs[k] = v;
    }

private:
    std::map<std::string, Vector3> mVecs;
};

SceneInfo loadSceneInfo(std::string infoProps)
{
    SceneInfo info;

    Dictionary d(new BasicKeyFirstLoader(" =\r\n\t", "#"),
                 new BasicKeyFirstWriter());
    
    FileInputStream fis(infoProps);
    d.deserialize(fis);
    
    for (int i = 1; i <= d.getNumLines(); ++i)
    {
        std::string k = d.at(i);
        if (!str::startsWith(k, "[\\s+]"))
        {
            std::string v = d[k].str();
            std::vector<std::string> toks = str::Tokenizer(v, ",");
            if (toks.size() != 3)
                throw except::Exception(
                    Ctxt(
                        FmtX("Expected triple, received: %s", v.c_str())
                        )
                    );
            
            Vector3 vec;
            vec[0] = str::toType<double>(toks[0]);
            vec[1] = str::toType<double>(toks[1]);
            vec[2] = str::toType<double>(toks[2]);
            info.put(k, vec);
        }
    }
    return info;
    
}

void showInfo(SceneGeometry& sg)
{
    std::cout << "==============================================" << std::endl;
    std::cout << "S    C    E    N    E         I    N    F    O" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "Grazing Angle: " << sg.getGrazingAngle() << std::endl;
    std::cout << "Tilt Angle: " << sg.getTiltAngle() << std::endl;
    std::cout << "Squint Angle: " << sg.getSquintAngle() << std::endl;
    std::cout << "Azimuth Angle: " << sg.getAzimuthAngle() << std::endl;
    std::cout << "North Angle: " << sg.getNorthAngle() << std::endl;
    std::cout << "Slope Angle: " << sg.getSlopeAngle() << std::endl;
    AngleMagnitude am = sg.getLayover();
    std::cout << "Layover Angle, Mag: " << am.angle << ", " << am.magnitude << std::endl;
    std::cout << "Doppler Cone Angle: " << sg.getDopplerConeAngle() << std::endl;
    am = sg.getShadow();
    std::cout << "Shadow Angle, Mag: " << am.angle << ", " << am.magnitude << std::endl;
    
    std::string sideOfTrack = 
        str::toString<SideOfTrack>(sg.getSideOfTrack());
    
    std::cout << "Side of Track: " << sideOfTrack << std::endl;


    std::cout << "ecfXs: " << sg.getSlantPlaneX() << std::endl;
    std::cout << "ecfYs: " << sg.getSlantPlaneY() << std::endl;
    std::cout << "ecfZs: " << sg.getSlantPlaneZ() << std::endl;
    //std::cout << "ecfYs: " << ecfYs << std::endl;
    //std::cout << "ecfZs: " << ecfZs << std::endl;

    Vector3 ecfZg = sg.getGroundPlaneNormal();
    std::cout << "ecfZg: " << ecfZg << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

}

int main(int argc, char** argv)
{
    try
    {
	if (argc != 2)
	    die_printf("Usage %s <dictionary file>\n", argv[0]);
        
	SceneInfo info = loadSceneInfo(argv[1]);


        Vector3 row = info["RowUnitVector"];
        Vector3 col = info["ColumnUnitVector"];
        SceneGeometry sg(info["ARPVelocity"],
                         info["ARPPosition"],
                         info["ORPPosition"],
                         &row,
                         &col);
        showInfo(sg);
        //pcsInfo(sg);


    }
    catch (const except::Exception& ex)
    {
	std::cout << "Exception: " << ex.getMessage() << std::endl;
	
    }
}

