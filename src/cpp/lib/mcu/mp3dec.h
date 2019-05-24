#pragma once

#include "interface/decoder.h"
#include "interface/stream.h"

class MP_Stream;
class BitStream;
typedef struct Out_InfoStr Out_Info;
typedef struct TrackInfoStr TrackInfo;
typedef struct CodecInitParamStr CodecInitParam;

namespace draaldecoder {

class MP3Decoder : public BaseDecoder
{
public:
    MP3Decoder();
    virtual ~MP3Decoder();

    bool init(IStreamBuffer *input, CodecInitParam *param, IOutputStream *output);

    virtual bool decode() override;
    virtual bool close() override;

    const Out_Info *getInfo() const { return m_outInfo; }
    const char *getTrackProperties(char *buf);

protected:
    // Initializes decoder engine parameters based on the quality parameters
    // that are given as an input
    void setQuality(CodecInitParam *param);

    void fillTrackInfo();

    MP_Stream *m_dec;
    BitStream *m_bs;
    Out_Info *m_outInfo;
    TrackInfo *m_trackInfo;
};

} // namespace draaldecoder
