/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_SUBWINDOW_HPP__
#define __NITF_SUBWINDOW_HPP__

#include "nitf/SubWindow.h"
#include "nitf/DownSampler.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file SubWindow.hpp
 *  \brief  Contains wrapper implementation for SubWindow
 */
namespace nitf
{

/*!
 *  \class SubWindow
 *  \brief  The C++ wrapper for the nitf_SubWindow
 *
 *  This object specifies a sub-window.  It is used as an
 *  argument to image read and write requests to specify
 *  the data requested.
 *
 *  The bandList field specifies the desired bands. Bands are
 *  numbered starting with zero and ordered as the bands
 *  appear physically in the file.  The user must supply
 *  one buffer for each requested band.  Each buffer must
 *  have enough storage for one band of the requested subimage
 *  size.
 *
 *  The DownSampler determines what downsampling (if any) is
 *  performed on this sub-window request.  There is no default
 *  DownSampler.  If you just want to perform pixel skipping, call
 *  setDownSampler() with a PixelSkip object.
 */
DECLARE_CLASS(SubWindow)
{
public:
    //! Copy constructor
    SubWindow(const SubWindow & x);

    //! Assignment Operator
    SubWindow & operator=(const SubWindow & x);

    //! Set native object
    SubWindow(nitf_SubWindow * x);

    //! Constructor
    SubWindow() throw(nitf::NITFException);

    //! Destructor
    ~SubWindow();

    nitf::Uint32 getStartRow() const;
    nitf::Uint32 getNumRows() const;
    nitf::Uint32 getStartCol() const;
    nitf::Uint32 getNumCols() const;
    nitf::Uint32 getBandList(int i);
    nitf::Uint32 getNumBands() const;

    void setStartRow(nitf::Uint32 value);
    void setNumRows(nitf::Uint32 value);
    void setStartCol(nitf::Uint32 value);
    void setNumCols(nitf::Uint32 value);
    void setBandList(nitf::Uint32 * value);
    void setNumBands(nitf::Uint32 value);

    /*!
     * Reference a DownSampler within the SubWindow
     * The SubWindow does NOT own the DownSampler
     * \param downSampler  The down sampler to reference
     */
    void setDownSampler(nitf::DownSampler* downSampler)
        throw (nitf::NITFException);

    /*!
     * Return the DownSampler that is referenced by this SubWindow.
     * If no DownSampler is referenced, a NITFException is thrown.
     */
    nitf::DownSampler* getDownSampler() throw (nitf::NITFException);

private:
    nitf::DownSampler* mDownSampler;
    nitf_Error error;
};

}
#endif
