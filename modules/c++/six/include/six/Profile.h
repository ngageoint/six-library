/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_PROFILE_H__
#define __SIX_PROFILE_H__

#include "six/Options.h"
#include "six/Data.h"
#include "six/XMLControlFactory.h"
#include "six/Container.h"

namespace six
{
    /*!
     *  \class Profile
     *  \brief (Optional) organization for building a Data object
     *
     *  The Profile can be used by a developer that is implementing
     *  a specification profile (a specific flavor of SICD/SIDD) to
     *  setup data from an Options map.
     *
     *  Extending the Profile object makes it possible to use domain-
     *  specific builders for initialization, without tying those
     *  business logic details to the code that writes the data.
     *
     *  It also provides the opportunity to create a partially built
     *  Data object, where details can be filed at a later time.
     */
    class Profile
    {
    public:
	//!  Constructor
        Profile() {}

	//!  Destructor
        virtual ~Profile() {}

        /*!
         *  Create a Data*.  You can set Data*'s directly, but this
         *  helps if you have a rigid set of things that you are required
         *  to set.
         */
        virtual Data* newData(const Options& options) = 0;

        /*!
         *  Create a Container*.  You can set Container*'s directly, but
         *  this helps if you are working from a document and need the
         *  container to look a certain way.
         */
        virtual Container* newContainer(const Options& options) = 0;

        /*!
         *  You may override this function as necessary
         */
        virtual std::vector<ValidationContext> validate(Data* data, 
                                                        const Options& options) 
        {
            return std::vector<ValidationContext>();
        }
    };
    
    /*!
     *  \class StubProfile
     *  \brief Derived Profile that gets a Data object templated using XML
     * 
     *  This class is a special profile implementation that uses
     *  one or more XML files to initialize a Data object.  If the
     *  newData function is used, the Options map must use the
     *  OPT_XML_FILE key to denote a file name for the XML stub file.
     *
     *  If the newContainer function is used, an OPT_CONTAINER_FILE_LIST
     *  should be used to set up the Container object.  The file list
     *  will contain a comma separated (no spaces please) list of
     *  XML files that are ordered as the container should be.  This
     *  function, underneath will initialize the OPT_XML_FILE parameter
     *  in a copy of Options and hand it to the newData object.
     *
     */
    class StubProfile : public Profile
    {
    public:

	//!  Constructor
        StubProfile() {}

	//!  Destructor
        virtual ~StubProfile() {}

	//!  Pass in a comma-separated list of files to newContainer
        static const char OPT_CONTAINER_FILE_LIST[];

	//!  Key for a value containing the DataType enumeration for container
        static const char OPT_CONTAINER_TYPE[];

	//!  Pass in a string file name for a stub XML file
        static const char OPT_XML_FILE[];

	/*!
	 *  Generate a Data object.  This function requires a parameter
	 *  in its options with the key OPT_XML_FILE.  It will retrieve
	 *  this parameter as a std::string, assuming it to be a valid
	 *  XML file path, and it will load that using the
	 *  XMLControlFactory.  The returned Data object will be loaded
	 *  with the XML as parsed by the factory.
	 *
	 *  \params options Only uses OPT_XML_FILE (std::string)
	 *  \return a Data object initialized from XML
	 */
        virtual Data* newData(const Options& options);


	/*!
	 *  Generates a Container object with N Data objects inside,
	 *  where N is the number of right-hand-side, comma-delimited
	 *  substrings in the OPT_CONTAINER_FILE_LIST option.
	 *  We also require the OPT_CONTAINER_TYPE to be set with a
	 *  DataType object: TYPE_COMPLEX for SICD containers 
	 *  or TYPE_DERIVED for SIDD containers.  The latter is allowed
	 *  to have SICD ComplexData objects (generated from SICD XML
	 *  templates within the file list).
	 *
	 *  \param options Options map including OPT_CONTAINER_FILE_LIST
	 *  (std::string, comma-delimetd) and OPT_CONTAINER_TYPE
	 *  (TYPE_COMPLEX or TYPE_DERIVED)
	 *  \return A Container created from template XML files
	 */
        virtual Container* newContainer(const Options& options);

    };

}


#endif
