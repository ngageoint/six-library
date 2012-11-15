/* =========================================================================
 * This file is part of six.sicd-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_COMPLEX_DATA_BUILDER_H__
#define __SIX_COMPLEX_DATA_BUILDER_H__

#include "six/sicd/ComplexData.h"

namespace six
{
namespace sicd
{
/*!
 *  \class ComplexDataBuilder
 *  \brief Incrementally builds a ComplexData object
 *
 *  This class implements the Builder design pattern by building 
 *  and maintaining a ComplexData instance.  A builder allows you to
 *  incrementally navigate the initialization of an object, and fills
 *  in many of the details for you.  Each method in this class can
 *  be cascaded, since it returns a reference to itself.
 *
    \code
    
    six::ComplexDataBuilder cdb;
    cdb.addImageCreation().addImageData(six::RE32F_IM32F);

    // Get a pointer to the data
    six::ComplexData* cd = cdb.steal();

    \endcode
 */
class ComplexDataBuilder
{
public:
    //! Constructor
    ComplexDataBuilder();

    //! Advanced constructor. Start with the shell of an object and continue
    ComplexDataBuilder(ComplexData *data);
    
    //! Destructor
    virtual ~ComplexDataBuilder();

    /*!
     *  Add an image creation block to the object
     *  Delete the existing one, if there is one already.
     *  The image creation block requires no sub-configuration.
     *  \return A reference to self
     */
    virtual ComplexDataBuilder& addImageCreation();

    /*!
     *  Add the radiometric params if they dont exist.
     *  Delete the existing one, if there is one already.
     *  The radiometric block requires no sub-configuration.
     *  \return A reference to self
     */
    virtual ComplexDataBuilder& addRadiometric();

    /*!
     *  Add the antenna parameters if they dont exist.
     *  Delete the existing one, if there is one already.
     *  Currently, this class does not attempt to initialize the optional
     *  sub-blocks.
     *
     *  \return A reference to self
     */
    virtual ComplexDataBuilder& addAntenna();

    /*!
     *  Add the error statistics if they dont exist.
     *  Delete the existing one, if there is one already.
     *  Currently, this class does not attempt to initialize the optional
     *  sub-blocks.
     *
     *  \return A reference to self
     */
    virtual ComplexDataBuilder& addErrorStatistics();

    /*!
     *  Add the match information.  When this optional object is constructed
     *  its constructor automatically creates one match collection underneath.
     *  \return A reference to self
     */
    virtual ComplexDataBuilder& addMatchInformation();

    /*!
     *  Reference an existing pointer.  This is not usually a good
     *  way to retrieve the object, since the pointer will be deleted
     *  when this object is brought down.  Prefer the steal() function.
     *
     *  In some cases, in conjunction with ComplexData::clone(), 
     *  this object may prove useful, if the builder is
     *  configuring a large number of SICDs.
     *
     *  \return A built object
     */
    ComplexData* get();
    
    /*!
     *  Take ownership of the builder's object.  The builder, at this
     *  point continues to be able to mutate the object, but it loses
     *  the ability to delete it.  The pointer in the builder is not
     *  unset, and it returns its pointer, which is now shared, and owned
     *  by the caller.
     *
     *  \return A built object
     *
     */
    ComplexData* steal();

protected:
    ComplexData *mData;
    bool mAdopt;
};

}
}
#endif
