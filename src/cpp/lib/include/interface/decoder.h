#pragma once

#include "interface/attributes.h"
#include "interface/console.h"
#include "interface/eqband.h"
#include "interface/stream.h"

namespace draaldecoder {

/**
 * Track related data.
 */
typedef struct TrackInfoStr
{
    /**
     * Sample rate (in Hz).
     */
    int32_t sampleRate;

    /**
     * Number of output channels.
     */
    int32_t outChannels;

} TrackInfo;


// Base class for audio decoding
class BaseDecoder
{
public:
    // Delete instance and its related resources
    void destroy() { delete this; }

    // Initialize decoder
    virtual bool
    init(IStreamBuffer *input, IOutputStream *output, const IAttributes *attrs) = 0;

    // Decode one frame from input stream to output stream
    virtual bool decode() = 0;

    // Close decoder
    virtual bool close() = 0;

    // Return equalizer settings
    EQ_Band *getEQ() const { return m_eq; }

    // Return track info
    const TrackInfo *getTrackInfo()
    {
        if (m_attrs) {
            m_attrs->getInt32Data(kKeySampleRate, m_info.sampleRate);
            m_attrs->getInt32Data(kKeyChannels, m_info.outChannels);
            return &m_info;
        }

        return nullptr;
    }

    // Query track properties as string buffer
    virtual const char *getTrackProperties(char *buf) = 0;

protected:
    BaseDecoder() : m_eq(new EQ_Band()), m_output(nullptr), m_attrs(nullptr) {}

    virtual ~BaseDecoder()
    {
        if (m_eq)
            delete m_eq;
        m_eq = nullptr;

        if (m_attrs)
            m_attrs->destroy();
        m_attrs = nullptr;
    }

    EQ_Band *m_eq;           // Equalizer settings
    IOutputStream *m_output; // Output stream interface
    IAttributes *m_attrs;    // Decoder attributes
    TrackInfo m_info;        // Decoding information
};

// Decoder interface that accepts only buffers as input data
class StreamableDecoder
{
public:
    // Create decoder instance based on specified attributes
    static StreamableDecoder *create(const IAttributes &attrs);

    // Destroy decoder instance
    void destroy();

    // Return attributes handle
    virtual IAttributes *getAttributes() = 0;

    // Initialize decoder based on (modified) attributes
    virtual bool init() = 0;

    // Append new input data to decoder
    virtual bool addInput(const uint8_t *buffer, size_t size) = 0;

    // Assign output stream for the decoder
    virtual void setOutput(IOutputStream *output) = 0;

    // Decode frame from input buffer
    virtual bool decode() = 0;

protected:
    StreamableDecoder() {}
    virtual ~StreamableDecoder() {}
};

} // namespace draaldecoder
