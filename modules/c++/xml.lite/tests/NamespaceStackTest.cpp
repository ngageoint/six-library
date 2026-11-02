/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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

#include <iostream>
#include <import/xml/lite.h>
using namespace xml::lite;

using namespace std;

int main()
{
    NamespaceStack xmlNs;

    // Push new context
    xmlNs.push();

    xmlNs.newMapping("x", "some-uri");
    xmlNs.newMapping("y", "some-uri");

    xmlNs.push();

    xmlNs.newMapping("x", "new-uri");
    xmlNs.newMapping("z", "local");

    // Level two /////////////////////////////////


    // This exists, but it has been mapped over
    std::string uri_x2 = xmlNs.getMapping("x");
    cout << "In Level 2 (x): " << uri_x2 << endl;

    // This exists in global scope
    std::string uri_y2 = xmlNs.getMapping("y");
    cout << "In Level 2 (y): " << uri_y2 << endl;

    // This exists only in local scope
    std::string uri_z2 = xmlNs.getMapping("z");
    cout << "In Level 2 (z): " << uri_z2 << endl;

    // Level two /////////////////////////////////

    xmlNs.pop();

    // Level one /////////////////////////////////

    // This should now be the old value
    std::string uri_x1 = xmlNs.getMapping("x");
    cout << "In Level 1 (x): " << uri_x1 << endl;

    // This should not change
    std::string uri_y1 = xmlNs.getMapping("y");
    cout << "In Level 1 (y): " << uri_y1 << endl;

    // This should no longer exist, and thus be blank
    std::string uri_z1 = xmlNs.getMapping("z");
    cout << "In Level 1 (z): " << uri_z1 << endl;

    // Level one /////////////////////////////////

    xmlNs.pop();
    return 0;
}
