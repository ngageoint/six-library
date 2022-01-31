#ifndef __SIX_SICD_COMPLEXTOAMP8IPHS8I__
#define __SIX_SICD_COMPLEXTOAMP8IPHS8I__

#include <six/sicd/ImageData.h>

namespace six
{
namespace sicd
{
/*!
 * \brief A utility that's used to convert complex values into 8-bit amplitude and phase values.
 */
class ComplexToAMP8IPHS8I {
public:
    /*!
     * Create a lookup structure that converts from complex to amplitude and phase.
     * @param pAmplitudeTable optional amplitude table.
     */
    explicit ComplexToAMP8IPHS8I(const six::AmplitudeTable* pAmplitudeTable = nullptr);

    /*!
     * Get the nearest amplitude and phase value given a complex value
     * @param v complex value to query with
     * @return nearest amplitude and phase value
     */
    ImageData::AMP8I_PHS8I_t nearest_neighbor(const std::complex<double>& v) const;

private:
    //! The sorted set of possible magnitudes order from small to large.
    std::array<double, UINT8_MAX + 1> magnitudes;
    //! The difference in phase angle between two UINT phase values.
    double phase_delta;
    //! Unit vector rays that represent each direction that phase can point.
    std::array<std::complex<double>, UINT8_MAX + 1> phase_directions;

    /*!
     * Get the phase of a complex value.
     * @param v complex value
     * @return phase between [0, 2PI]
     */
    static double GetPhase(const std::complex<double>& v);
};
}
}
#endif
