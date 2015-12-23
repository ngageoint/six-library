/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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


#include <import/sys.h>
#include <import/io.h>
#include <import/except.h>

using namespace sys;
using namespace io;
using namespace except;
using namespace std;

#define DEBUG_A true

class A : public Serializable
{
public:
    A() 
    {
	dbg.set(new FileOutputStream("dbg.out"), DEBUG_A);
	
	vec[0] = 0.0;
	vec[1] = 0.0;
	vec[2] = 0.0;
    }
    virtual ~A() {}
    virtual void serialize(OutputStream& os)
    {
	os.writeln("Class A");
	os.writeln(FmtX("%f", vec[0]));
	os.writeln(FmtX("%f", vec[1]));
	os.writeln(FmtX("%f", vec[2]));

    }
    virtual void deserialize(InputStream& is)
    {
	string classType = fillString(is);
	string vec_0 = fillString(is);
	string vec_1 = fillString(is);
	string vec_2 = fillString(is);

	assert(classType == "Class A");
	dbg.writeln(FmtX("vec[0] = %s", vec_0.c_str()));
	dbg.writeln(FmtX("vec[1] = %s", vec_1.c_str()));
	dbg.writeln(FmtX("vec[2] = %s", vec_2.c_str()));

	vec[0] = str::toType<float>(vec_0);
	vec[1] = str::toType<float>(vec_1);
	vec[2] = str::toType<float>(vec_2);

    }
    std::string fillString(io::InputStream &is)
    {
	std::string toFill;
	sys::byte b[1];
	while (true)
	{
	    if (is.read(b, 1) ==  InputStream::IS_EOF)
		throw IOException(Ctxt("Source corrupt") );
	
	    else if (b[0] == '\n')
	    {
		return toFill;
	    }
	    else if (b[0] == '\r')
	    {
		cout << "Warning: detected CR, ignoring" << endl;
	    }
	    else
	    {
		toFill.append((const char*)b, 1);
	    }
	}
	   
    }
    void setVector(float* f3v)
    {
	memcpy(vec, f3v, sizeof(float) * 3);
    }
    void getVector(float* f3v)
    {
	memcpy(f3v, vec, sizeof(float) * 3);
    }

    bool operator==(const A& a) const
    {
	return (a.vec[0] == vec[0] &&
		a.vec[1] == vec[1] &&
		a.vec[2] == vec[2]);
    }
protected:
    DbgStream dbg;
    float vec[3];
};

int main(int, char**)
{
    StandardOutStream out;
    A a;
    a.serialize(out);
    float f3v[3];
    f3v[0] = -2.0;
    f3v[1] = 1.2;
    f3v[2] = 8.6;
    a.setVector(f3v);

    FileOutputStream fos("A.save");
    a.serialize(fos);
    fos.close();

    A a2;
    FileInputStream fis("A.save");
    a2.deserialize(fis);
    fis.close();

    if (a == a2)
    {
	cout << "Successfully copied object a2 through object a via \"A.save\"" << endl;
	a2.serialize(out);
    }
    else
    {
	cout << "Serialization process failed" << endl;
	a2.serialize(out);
    }

}

