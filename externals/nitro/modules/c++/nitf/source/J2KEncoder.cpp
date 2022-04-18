/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2017, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#include "nitf/J2KEncoder.hpp"

#include <sstream>
#include <cmath>
#include <std/memory>

#include <except/Exception.h>
#include <sys/Conf.h>
#include <gsl/gsl.h>

#undef min
#undef max

namespace
{
    void errorHandlerImpl(const char* msg, void* userData)
    {
        auto& encoder = *static_cast<j2k::Encoder*>(userData);
        encoder.setError(msg);
    }
}

struct j2k::Encoder::Impl final
{
    //! Scoped encoder - Internal RAII wrapper around opj_codec_t -
    //! calls opj_create_compress() on construction and opj_destroy_codec()
    //! on destruction.
    std::unique_ptr<j2k_codec_t, decltype(&j2k_destroy_codec)> mEncoder;
    std::unique_ptr<j2k_cparameters_t, decltype(&j2k_destroy_encoder_parameters)> mEncoderParameters;

    //! The openjpeg error message.
    std::string mErrorMessage;

    Impl() noexcept : 
        mEncoder(j2k_create_compress(/*OPJ_CODEC_J2K*/), j2k_destroy_codec),
        mEncoderParameters(j2k_set_default_encoder_parameters(), j2k_destroy_encoder_parameters)
    {
    }
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
};

j2k::Encoder::~Encoder() = default;

j2k::Encoder::Encoder(Image& image, const CompressionParameters& compressionParams)
    : pImpl_(std::make_unique<Impl>())
{
    j2k_initEncoderParameters(pImpl_->mEncoderParameters.get(),
        compressionParams.getTileDims().row, compressionParams.getTileDims().col,
        compressionParams.getCompressionRatio(),
        compressionParams.getNumResolutions());

    const auto handlerSuccess = j2k_set_error_handler(getNative(), errorHandlerImpl, this);
    if (!handlerSuccess)
    {
        throw except::Exception(Ctxt("Failed to setup openjpeg encoder error handler."));
    }

    const auto setupSuccess = j2k_setup_encoder(getNative(), pImpl_->mEncoderParameters.get(), image.getNative());
    if (!setupSuccess)
    {
        if (errorOccurred())
        {
            const std::string opjErrorMsg = getErrorMessage();
            clearError();

            std::ostringstream os;
            os << "Failed to setup openjpeg encoder with openjpeg error: " << opjErrorMsg;
            throw except::Exception(Ctxt(os.str()));
        }

        throw except::Exception(Ctxt("Failed to setup openjpeg encoder."));
    }
}

j2k_codec_t* j2k::Encoder::getNative() const noexcept
{
    return pImpl_->mEncoder.get();
}

std::string j2k::Encoder::getErrorMessage() const
{
    return pImpl_->mErrorMessage;
}

bool j2k::Encoder::errorOccurred() const
{
    return !getErrorMessage().empty();
}

void j2k::Encoder::setError(const std::string& msg)
{
    pImpl_->mErrorMessage = msg;
}

void j2k::Encoder::clearError() noexcept
{
    pImpl_->mErrorMessage.clear();
}
