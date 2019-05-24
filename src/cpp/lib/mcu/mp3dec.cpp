#include <stdio.h>
#include <string.h>

#include "codec/mp3/mp3.h"
#include "codec/mp3/mp3def.h"
#include "codec/mp3/mp3info.h"
#include "codec/mp3/mstream.h"
#include "codec/mp3/param.h"
#include "core/bits.h"
#include "core/meta.h"
#include "mp3dec.h"

#define MAX_SLOTS ((MAX_FRAME_SLOTS << 1) + 1)

namespace draaldecoder {

static bool initialized = false;

static void
setRestParam(Out_Param *out_param)
{
    out_param->sampling_frequency /= out_param->decim_factor;
    out_param->num_samples = (int16)(SBLIMIT / out_param->decim_factor);
    out_param->num_out_samples = (int16)(SBLIMIT / out_param->decim_factor);
    out_param->window_offset = (int16)(SBLIMIT * out_param->window_pruning_idx);
    out_param->num_subwindows = (int16)(NUM_SUBWIN - 2 * out_param->window_pruning_idx);
}


MP3Decoder::MP3Decoder() :
    BaseDecoder(),
    m_dec(new MP_Stream()),
    m_bs(new BitStream()),
    m_outInfo(new Out_Info()),
    m_trackInfo(new ::TrackInfo())
{
    m_attrs = new AudioAttributes();

    memset(m_outInfo, 0, sizeof(Out_Info));

    // Initialize MP3 decoder tables etc
    if (!initialized)
        InitMP3DecoderData();
    initialized = true;
}

MP3Decoder::~MP3Decoder()
{
    if (this->m_dec)
        delete this->m_dec;
    this->m_dec = nullptr;

    if (this->m_bs)
        delete this->m_bs;
    this->m_bs = nullptr;

    if (this->m_outInfo)
        delete this->m_outInfo;
    this->m_outInfo = nullptr;

    if (this->m_trackInfo)
        delete this->m_trackInfo;
    this->m_trackInfo = nullptr;
}

bool
MP3Decoder::init(IStreamBuffer *input, CodecInitParam *param, IOutputStream *output)
{
    m_output = output;
    m_bs->open(input, MAX_SLOTS);
    m_dec->initDecoder(m_bs, &m_outInfo->param, &m_outInfo->complex);

    /*-- This will determine the output quality. --*/
    this->setQuality(param);
    ReInitEngine(m_dec);

    /*-- Store the equalizer settings into the dequantizer module. --*/
    m_dec->dbScale = m_eq->getdBScale();

    this->fillTrackInfo();
    m_attrs->setInt32Data(kKeySampleRate, m_outInfo->param.sampling_frequency);
    m_attrs->setInt32Data(kKeyChannels, m_outInfo->param.num_out_channels);

    return true;
}

bool
MP3Decoder::decode()
{
    SEEK_STATUS sync = SYNC_FOUND;

    /*-- Get the output samples. --*/
    if (!DecodeFrame(m_dec, m_dec->buffer->pcm_sample))
        return false;

    /*-- Write to output. --*/
    m_output->writeBuffer(m_dec->buffer->pcm_sample, m_dec->out_param->num_out_samples);

    /*-- Find the start of the next frame. --*/
    sync = SeekSync(m_dec);

    return sync == SYNC_FOUND;
}

bool
MP3Decoder::close()
{
    return true;
}

void
MP3Decoder::setQuality(CodecInitParam *param)
{
    Out_Param *out_param = &m_outInfo->param;

    m_dec->side_info->sfbData.bandLimit = MAX_MONO_SAMPLES;
    if (param->bandLimit > 0 && param->bandLimit <= MAX_MONO_SAMPLES)
        m_dec->side_info->sfbData.bandLimit = param->bandLimit;

    out_param->sampling_frequency = m_dec->header->frequency();
    out_param->num_out_channels = (int16) m_dec->header->channels();
    if (param->channels > 0 && param->channels <= m_dec->header->channels())
        out_param->num_out_channels = param->channels;

    out_param->decim_factor = 1;
    if (param->decim_factor == 2 || param->decim_factor == 4)
        out_param->decim_factor = param->decim_factor;

    out_param->window_pruning_idx = WINDOW_PRUNING_START_IDX;
    if (param->window_pruning > 0 && param->window_pruning < SBLIMIT + 1)
        out_param->window_pruning_idx = param->window_pruning;

    out_param->num_samples = SBLIMIT;

    m_dec->complex->subband_pairs = 15;
    if (param->alias_bands >= 0 && param->alias_bands < SBLIMIT - 1)
        m_dec->complex->subband_pairs = param->alias_bands;

    m_dec->complex->imdct_subbands = MAX_MONO_SAMPLES;
    if (param->imdct_sbs > 0 && param->imdct_sbs <= SBLIMIT)
        m_dec->complex->imdct_subbands = param->imdct_sbs * SSLIMIT;

    m_dec->complex->fix_window = param->fix_window;

    setRestParam(out_param);
}

void
MP3Decoder::fillTrackInfo()
{
    const MP_Header *header = m_dec->header;

    if (header->version())
        strcpy(m_trackInfo->Version, "MPEG-1");
    else if (header->mp25version())
        strcpy(m_trackInfo->Version, "MPEG-2.5");
    else
        strcpy(m_trackInfo->Version, "MPEG-2 LSF");

    m_trackInfo->Channels = header->channels();
    m_trackInfo->Frequency = header->frequency();
    strcpy(m_trackInfo->Layer, header->layer_string());
    strcpy(m_trackInfo->Mode, header->mode_string());
    strcpy(m_trackInfo->Private_bit, header->private_bit() ? "Yes" : "No");
    strcpy(m_trackInfo->De_emphasis, header->de_emphasis());
    strcpy(m_trackInfo->Copyright, header->copyright() ? "Yes" : "No");
    strcpy(m_trackInfo->Stereo_mode, header->mode_string());
    strcpy(m_trackInfo->Error_protection, header->error_protection() ? "Yes" : "No");
    strcpy(m_trackInfo->Original, header->original() ? "Yes" : "No");

    /*
      m_trackInfo->Length = player->brInfo->GetTotalTime();
      m_trackInfo->bitRate = player->brInfo->GetBitRate();
      m_trackInfo->SizeInBytes = player->mp->bs->GetStreamSize();
      m_trackInfo->TotalFrames = player->brInfo->GetTotalFrames();
      */
}

const char *
MP3Decoder::getTrackProperties(char *buf)
{
    char tmpTxtBuf[64];

    /*-- Collect the info from the structure to the message buffer. --*/
    strcpy(buf, "");
    sprintf(tmpTxtBuf, "\nVersion : %s", m_trackInfo->Version);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nLayer : ");
    strcat(buf, m_trackInfo->Layer);

    strcat(buf, "\nChecksums ? : ");
    strcat(buf, m_trackInfo->Error_protection);

    strcat(buf, "\nBitrate : ");
    sprintf(tmpTxtBuf, "%i kbps", m_trackInfo->bitRate);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nSample Rate : ");
    sprintf(tmpTxtBuf, "%i Hz", m_trackInfo->Frequency);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nFrames : ");
    sprintf(tmpTxtBuf, "%i", m_trackInfo->TotalFrames);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nLength : ");
    sprintf(tmpTxtBuf, "%i s", m_trackInfo->Length / 1000);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nPrivate bit ? : ");
    strcat(buf, m_trackInfo->Private_bit);

    strcat(buf, "\nMode String : ");
    strcat(buf, m_trackInfo->Mode);

    strcat(buf, "\nCopyright ? : ");
    strcat(buf, m_trackInfo->Copyright);

    strcat(buf, "\nOriginal ? : ");
    strcat(buf, m_trackInfo->Original);

    strcat(buf, "\nDe-emphasis : ");
    strcat(buf, m_trackInfo->De_emphasis);

    return buf;
}

} // namespace draaldecoder
