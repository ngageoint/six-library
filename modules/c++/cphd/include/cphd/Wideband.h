/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/



#ifndef __CPHD_WIDEBAND_H__
#define __CPHD_WIDEBAND_H__

#include <string>
#include <complex>

#include <sys/Conf.h>
#include <cphd/Data.h>
#include <mem/ScopedArray.h>
#include <mem/SharedPtr.h>
#include <io/SeekableStreams.h>
#include <algs/BufferView.h>
#include <types/RowCol.h>

namespace cphd
{
//  This class contains information about the Wideband CPHD data.
//  It contains the cphd::Data structure (for channel and vector sizes).
//  
//  Due to the large size of CPHD wideband, this object does not contain
//  any actual wideband data

class Wideband
{
public:
    static const size_t ALL;

    /*
     * \param pathname Input CPHD pathname
     * \param data Data section from CPHD
     * \param startWB CPHD header keyword "CPHD_BYTE_OFFSET"
     * \param sizeWB CPHD header keyword "CPHD_DATA_SIZE"
     */
    Wideband(const std::string& pathname,
             const cphd::Data& data,
             sys::Off_T startWB,
             sys::Off_T sizeWB);

    /*
     * \param inStream Input stream to an already opened CPHD file
     * \param data Data section from CPHD
     * \param startWB CPHD header keyword "CPHD_BYTE_OFFSET"
     * \param sizeWB CPHD header keyword "CPHD_DATA_SIZE"
     */
    Wideband(mem::SharedPtr<io::SeekableInputStream> inStream,
             const cphd::Data& data,
             sys::Off_T startWB,
             sys::Off_T sizeWB);

    // Return offset from start of CPHD file for a vector and sample for a channel
    // first channel is 0!
    // 0-based vector in channel
    // 0-based sample in channel
    sys::Off_T getFileOffset(size_t channel,
                             size_t vector,
                             size_t sample) const;

    /*
     * Read the specified channel, vector(s), and sample(s)
     * Performs endian swapping if necessary
     *
     * \param channel 0-based channel
     * \param firstVector 0-based first vector to read (inclusive)
     * \param lastVector 0-based last vector to read (inclusive).  Use ALL to
     * read all vectors
     * \param firstSample 0-based first sample to read (inclusive)
     * \param lastSample 0-based last sample to read (inclusive).  Use ALL to
     * read all samples
     * \param numThreads Number of threads to use for endian swapping if
     * necessary
     * \param data Will contain the read in data.  Throws if buffer has not
     * been allocated to a sufficient size
     * (numVectors * numSamples * elementSize)
     */
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              const algs::BufferView<sys::ubyte>& data);

    // Same as above but allocates the memory
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              mem::ScopedArray<sys::ubyte>& data);

    // Same as above but also applies a per-vector scale factor
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              const std::vector<double>& vectorScaleFactors,
              size_t numThreads,
              const algs::BufferView<sys::ubyte>& scratch,
              const algs::BufferView<std::complex<float> >& data);

private:
    void initialize();

    void checkReadInputs(size_t channel,
                         size_t firstVector,
                         size_t& lastVector,
                         size_t firstSample,
                         size_t& lastSample,
                         types::RowCol<size_t>& dims) const;

    // Just performs the read
    // No allocation, endian swapping, or scaling
    void readImpl(size_t channel,
                  size_t firstVector,
                  size_t lastVector,
                  size_t firstSample,
                  size_t lastSample,
                  void* data);

    static
    bool allOnes(const std::vector<double>& vectorScaleFactors);

private:
    // Noncopyable
    Wideband(const Wideband& );
    const Wideband& operator=(const Wideband& );

private:
    const mem::SharedPtr<io::SeekableInputStream> mInStream;
    cphd::Data mData;                 // contains numChan, numVectors
    const sys::Off_T mWBOffset;       // offset in bytes to start of wideband
    const size_t mWBSize;             // total size in bytes of wideband
    const size_t mElementSize;        // element size (bytes / complex sample)

    std::vector<sys::Off_T> mOffsets; // Offset to start of each channel

    friend std::ostream& operator<< (std::ostream& os, const Wideband& d);
};
}

#endif
