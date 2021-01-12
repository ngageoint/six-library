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

#include <import/io.h>

const int LENGTH_STR = 26;


void testSeekCurrent(io::StringStream& oss)
{
    char buffer[ LENGTH_STR + 1 ];
    buffer[ LENGTH_STR ] = 0;
    // Simple scan through....
    int readVal = 0;
    oss.seek(0, io::StringStream::START);
    

    
    for (int i = 0; readVal != io::StringStream::IS_END ; i++)
    {
	oss.seek(0, io::StringStream::CURRENT);
	readVal = oss.read(&buffer[i], 1);
	std::cout << i << std::endl;
	std::cout << readVal << std::endl;
    }

    std::cout << "'" << buffer << "'" << std::endl;

}

int main()
{

    try
    {
	
	const std::string str = "abcdefghijklmnopqrstuvwxyz";
	std::cout << "Operating on string: '" << str << "'" << std::endl;
	
	io::StringStream oss;
	oss.write(str.c_str(), str.length());
	
//	testSeekCurrent(oss);
	char buffer[ LENGTH_STR + 1 ];
	buffer[ LENGTH_STR ] = 0;
	// Simple scan through....
	int readVal = 0;
	oss.seek(0, io::StringStream::START);
		
	for (int i = 0; readVal != io::StringStream::IS_END ; i++)
	{
	    oss.seek(0, io::StringStream::CURRENT);
	    readVal = oss.read(&buffer[i], 1);
	}
	
	std::cout << "'" << buffer << "'" << std::endl;

	io::FileOutputStream fos("anything.txt");
	fos.write(buffer, LENGTH_STR);
	fos.close();


	memset(buffer, 0, LENGTH_STR + 1);
	
	io::FileInputStream fis("anything.txt");
	int avail = (int)fis.available();
	fis.seek(0, io::FileInputStream::END);
	readVal = 0;

	//std::cout << avail << std::endl;
	for (int i = 0; i != avail ; i++)
	{
	    fis.seek(-1, io::FileInputStream::CURRENT);
	    fis.read(&buffer[i], 1);
	    fis.seek(-1, io::FileInputStream::CURRENT);

	}
	std::cout << "'" << buffer << "'" << std::endl;
	io::StringStream bwd;
	bwd.write(buffer, LENGTH_STR);
	avail = bwd.available();
	if ( avail != LENGTH_STR )
	    throw except::Exception(Ctxt("Huh??"));

	
	memset(buffer, 0, LENGTH_STR + 1);

	for (int i = avail; i > 0; --i)
	{
	    int d = avail - i;
	    bwd.seek(i - 1, io::StringStream::START);
	    bwd.read(&buffer[d], 1);
	    //std::cout << "At: " << d << ": " << buffer[d] << std::endl;
	}
	
	std::cout << "'" << buffer << "'" << std::endl;
	
	return 0;
    }
    catch (except::Exception& ex)
    {
	std::cout << "Caught ex: " << ex.getTrace() << std::endl;
	exit(EXIT_FAILURE);
    }

}
