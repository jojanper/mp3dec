/**************************************************************************
  mp3.cpp - MPEG-1, MPEG-2 LSF, and MPEG-2.5 playback subroutines.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- System Headers. --*/
#include <string.h>

/*-- Project Headers. --*/
#include "layer.h"
#include "mp3.h"
#include "mstream.h"
#include "param.h"

/**************************************************************************
  Internal Objects
  *************************************************************************/

static BOOL L3BitReservoir(MP_Stream *mp);

/*-- Mixing routines for playback. --*/
#include "mp3mix.cpp"

/*
 * Initializes common static data of the decoder. This function needs
 * to be called only once per application. The number of decoder
 * instances is unlimited.
 */
void
InitMP3DecoderData(void)
{
    /*-- Init IMDCT tables. --*/
    InitIMDCTData();

    /*-- Init inverse quantization tables. --*/
    InitMP3DequantModule();
}

void
InitCodecInitParam(CodecInitParam *param)
{
    param->fix_window = FALSE;
    param->channels = MAX_CHANNELS;
    param->decim_factor = 1;
    param->window_pruning = WINDOW_PRUNING_START_IDX;
    param->alias_bands = SBLIMIT - 1;
    param->imdct_sbs = SBLIMIT;
    param->bandLimit = MAX_MONO_SAMPLES;
}

/**************************************************************************
  Title        : DecodeFrame

  Purpose      : Decodes one MPEG-1, MPEG-2 LSF, or MPEG-2.5 frame from
                 the incoming stream.

  Usage        : y = DecodeFrame(mp, pcm_sample)

  Input        : mp            - MPEG audio stream parameters

  Output       : pcm_sample - decoded output samples (interleaved format
                              is used for stereo data)
                 y          - TRUE on success, FALSE otherwise

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
DecodeFrame(MP_Stream *mp, int16 *pcm_sample)
{
    int16 *pcm[2];
    int i, j, k, ss, gr, ch, idx_increment;

    idx_increment = mp->idx_increment;

start:

    // Get the header and (optional) CRC codeword.
    decode_header(mp);

    // Output data pointers.
    pcm[0] = pcm_sample;
    pcm[1] = pcm_sample + 1;

    // Now layer dependent decoding routines.
    switch (mp->header->layer_number()) {
        case 1:
            I_decode_bit_alloc(mp);
            I_decode_scale(mp);

            for (i = 0; i < SCALE_BLOCK; i++) {
                // Get the quantized samples
                I_buffer_sample(mp);

                // Get the reconstructed samples.
                I_dequantize_sample(mp);

                // Synthesis filter for left and right channel.
                PolyPhaseFIR(
                    mp,
                    mp->buffer->reconstructed,
                    pcm[0],
                    mp->buffer->buf_idx,
                    mp->out_param,
                    LEFT_CHANNEL);

                pcm[0] += idx_increment;

                if (mp->out_param->num_out_channels == 2) {
                    PolyPhaseFIR(
                        mp,
                        mp->buffer->reconstructed + SBLIMIT,
                        pcm[1],
                        mp->buffer->buf_idx + RIGHT_CHANNEL,
                        mp->out_param,
                        RIGHT_CHANNEL);

                    pcm[1] += idx_increment;
                }
            }
            break;

        case 2:
            II_decode_bit_alloc(mp);
            II_decode_scale(mp);

            for (i = 0; i < SCALE_BLOCK; i++) {
                static int16 offset, offset_idx;
                static int16 sample_offset[6] = { 0, 96, 32, 128, 64, 160 };

                // Get the quantized samples.
                II_buffer_sample(mp);
                k = i >> 2;
                k *= mp->header->subbands() << (mp->header->channels() - 1);

                // Get the reconstructed samples.
                II_dequantize_sample(mp, mp->frame->scale_factors + k);

                offset_idx = 1;

                // Synthesis filterbank for left and right channel.
                for (j = offset = 0; j < 3; j++) {
                    PolyPhaseFIR(
                        mp,
                        mp->buffer->reconstructed + offset,
                        pcm[0],
                        mp->buffer->buf_idx,
                        mp->out_param,
                        LEFT_CHANNEL);

                    pcm[0] += idx_increment;

                    if (mp->out_param->num_out_channels == 2)
                        offset = sample_offset[offset_idx++];
                    else
                        offset += SBLIMIT;

                    if (mp->out_param->num_out_channels == 2) {
                        PolyPhaseFIR(
                            mp,
                            mp->buffer->reconstructed + offset,
                            pcm[1],
                            mp->buffer->buf_idx + RIGHT_CHANNEL,
                            mp->out_param,
                            RIGHT_CHANNEL);

                        offset = sample_offset[offset_idx++];

                        pcm[1] += idx_increment;
                    }
                }
            }
            break;

        case 3:
            // Read the side info.
            III_get_side_info(mp);
            BOOL isLSF = mp->side_info->sfbData.lsf;
            BOOL isIS = mp->side_info->sfbData.is_stereo;

            // Put the payload to the bit reservoir buffer.
            mp->br->addBytes(*mp->bs, mp->main_data_slots());

            // Bit reservoir processing.
            if (L3BitReservoir(mp) == FALSE) {
                if (ReSync(mp) == FALSE)
                    goto exit;
                else
                    goto start;
            }

            // Decode granule(s) and then write the ouput.
            for (gr = 0; gr < mp->side_info->sfbData.max_gr; gr++) {
                int part2 = mp->br->bitsRead();

                // Get MPEG-1 or MPEG-2 LSF scale factors.
                ScaleFunc[isLSF](mp, gr, LEFT_CHANNEL);

                // Get quantized data for left channel.
                III_huffman_decode(mp, gr, LEFT_CHANNEL, part2);

                if (mp->header->channels() == 2) {
                    part2 = mp->br->bitsRead();

                    // Get MPEG-1 or MPEG-2 LSF scale factors.
                    ScaleFunc[isLSF](mp, gr, RIGHT_CHANNEL);

                    // Get quantized data for right channel.
                    III_huffman_decode(mp, gr, RIGHT_CHANNEL, part2);
                }

                // Get the stereo codes for each scalefactor band.
                if (isIS)
                    III_stereo_mode(mp, gr);

                // Get the reconstructed frame.
                III_dequantize(mp, gr);

                // Compute the output.
                for (ch = 0; ch < mp->out_param->num_out_channels; ch++) {
                    // Reorder short blocks.
                    III_reorder(mp, ch, gr);

                    // Alias-reduction for long blocks.
                    III_antialias(mp, ch, gr);

                    // Apply inverse MDCT to each subband.
                    III_hybrid2(mp, mp->tsOut, ch, gr);

                    // Apply polyphase synthesis filter to each subband.
                    for (ss = 0; ss < SSLIMIT; ss++, pcm[ch] += idx_increment)
                        PolyPhaseFIR(
                            mp,
                            mp->tsOut[ss],
                            pcm[ch],
                            mp->buffer->buf_idx + ch,
                            mp->out_param,
                            ch);
                }
            }
            break;
    }

    return (TRUE);

exit:
    return (FALSE);
}


/**************************************************************************
  Title        : L3BitReservoir

  Purpose      : Layer III bit reservoir subroutine.

  Usage        : y = L3BitReservoir(mp)

  Input        : mp - MP3 parameters

  Output       : y - FALSE if 'bytes_to_discard' is negative, TRUE otherwise

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
L3BitReservoir(MP_Stream *mp)
{
    int16 bytes_to_discard;
    int flush_main;

    /*------------ Start of bit reservoir processing. ------------------*/

    if (mp->WasSeeking == FALSE) {
        /*-- Byte alignment. --*/
        flush_main = mp->br->bitsRead() & 7;
        if (flush_main)
            mp->br->skipBits(8 - flush_main);

        /*
         * Determine how many bits were left from the previous frame.
         */
        if (mp->SkipBr == FALSE)
            mp->br->setBitsRead((mp->PrevSlots << 3) - mp->br->bitsRead());

        /*
         * Determine how many bytes need to be discarded from the previous
         * frame to find the start of next frame.
         */
        bytes_to_discard = (mp->br->bitsRead() >> 3) - mp->side_info->main_data_begin;

        // Reset the bit reservoir bit counter.
        mp->br->resetCounter();

        // # of slots available for this frame.
        mp->PrevSlots = mp->main_data_slots() + mp->side_info->main_data_begin;

        if (bytes_to_discard < 0) {
            mp->br->setBitsRead(mp->main_data_slots() << 3);
            mp->SkipBr = TRUE;

            return (FALSE);
        }

        mp->SkipBr = FALSE;

        if (bytes_to_discard) {
            mp->PrevSlots += bytes_to_discard;
            mp->br->skipBits(bytes_to_discard << 3);
        }
    }
    else {
        // Reset the bit reservoir bit counter.
        mp->br->resetCounter();

        // # of slots available for this frame.
        mp->PrevSlots = mp->main_data_slots();

        mp->SkipBr = FALSE;
        mp->WasSeeking = FALSE;

        if (mp->side_info->main_data_begin)
            mp->br->rewindNbits(mp->side_info->main_data_begin << 3);
    }
    /*-------------- End of bit reservoir processing. ------------------*/

    return (TRUE);
}


/**************************************************************************
  Title        : ResetEngine

  Purpose      : Resets some properties of the engine.

  Usage        : ResetEngine(mp)

  Input        : mp - MPx parameters

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
ResetEngine(MP_Stream *mp)
{
    for (int i = 0; i < mp->header->channels(); i++) {
        memset(mp->buffer->synthesis_buffer[i], 0, (HAN_SIZE << 1) * sizeof(FLOAT));
        memset(mp->buffer->Fixsynthesis_buffer[i], 0, (HAN_SIZE << 1) * sizeof(int16));
        mp->buffer->buf_idx[i] = 0;
    }

    if (mp->header->layer_number() == 3) {
        memset(mp->blc, 0, sizeof(int) * 2);
        memset(mp->tsOut, 0, SSLIMIT * SBLIMIT * sizeof(FLOAT));
        memset(&mp->OverlapBlock[0][0][0], 0, 4 * MAX_MONO_SAMPLES * sizeof(FLOAT));
    }
}


/**************************************************************************
  Title        : ReInitEngine

  Purpose      : Re-initializes the decoding engine.

  Usage        : ReInitEngine(mp)

  Input        : mp - MPx stream parametrs

  Output       : all important properties (from the decoding point of view)
                 of the engine are re-initialized.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
ReInitEngine(MP_Stream *mp)
{
    mp->out_param->num_out_samples =
        mp->out_param->num_out_channels * mp->out_param->num_samples;

    mp->idx_increment = mp->out_param->num_out_samples;

    if (mp->header->layer_number() == 1)
        mp->out_param->num_out_samples *= SCALE_BLOCK;
    else if (mp->header->layer_number() == 2)
        mp->out_param->num_out_samples *= (SCALE_BLOCK * 3);
    else // mp->header->layer_number() == 3
        mp->out_param->num_out_samples *= (SSLIMIT * mp->side_info->sfbData.max_gr);

    III_SfbDataInit(&mp->side_info->sfbData, mp->header);

    III_BandLimit(&mp->side_info->sfbData, mp->out_param->decim_factor);

    ResetEngine(mp);
}


/*
 * Recomputes the decoder parameters based on the input parameters.
 */
void
ReComputeEngineParameters(MP_Stream *mp, int decim_factor)
{
    Out_Param *out_p = mp->out_param;

    mp->complex->imdct_subbands = mp->complex->imdct_subbands * out_p->decim_factor;

    /*-- Default values --*/
    out_p->sampling_frequency = mp->header->frequency();
    out_p->decim_factor = decim_factor;
    out_p->num_samples = SBLIMIT;

    out_p->sampling_frequency /= out_p->decim_factor;
    out_p->num_samples = (int16)(SBLIMIT / out_p->decim_factor);

    out_p->num_out_samples = (int16)(SBLIMIT / out_p->decim_factor);
    out_p->window_offset = (int16)(SBLIMIT * out_p->window_pruning_idx);
    out_p->num_subwindows = (int16)(NUM_SUBWIN - 2 * out_p->window_pruning_idx);
    mp->complex->imdct_subbands /= out_p->decim_factor;
}


/**************************************************************************
  Title        : FindFreeFormatSlotCount

  Purpose      : Determines the payload of the free format layer 3 stream.

  Usage        : y = FindFreeFormatSlotCount(mp)

  Input        : mp - MP stream parameters

  Output       : y  - fixed payload size for this stream, -1 if payload
                      could not be determined

  Explanation  : The payload is determined by counting how many bytes there
                 exists between two frames. The value excludes the header and
                 side infor bytes. Since the bitrate is fixed, the decoder can
                 rely on the information computed here when determining the
                 size of the payload for the other frames.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
FindFreeFormatSlotCount(MP_Stream *mp)
{
    uint16 sync, mask;
    uint32 hdr;
    int nSlots, readBits, exitCheck = 1;

    // Get the header and (optional) CRC codeword.
    decode_header(mp);

    // Read the side info.
    III_get_side_info(mp);

    // Find the start of the next frame.
    sync = mp->bs->getBits(SYNC_WORD_LENGTH);
    mask = (1 << SYNC_WORD_LENGTH) - 1;
    readBits = SYNC_WORD_LENGTH;

    while (exitCheck) {
        while (sync != mp->syncInfo.sync_word && !mp->bs->endOfStream()) {
            readBits++;
            sync <<= 1;
            sync &= mask;
            sync |= (int) mp->bs->getBits(1);
        }

        readBits++;
        sync <<= 1;
        sync &= mask;
        sync |= (int) mp->bs->getBits(1);
        mp->side_info->sfbData.mpeg25 = !(sync & 1);

        /* Check the next frame header. */
        hdr = (!mp->side_info->sfbData.mpeg25) << HEADER_BITS;
        mp->header->SetHeader(hdr | mp->bs->lookAhead(HEADER_BITS));

        /* Detect false frame boundaries. */
        if (mp->header->channels() == mp->prev_header->channels() &&
            mp->header->sfreq() == mp->prev_header->sfreq() &&
            mp->header->bit_rate() == mp->prev_header->bit_rate() &&
            mp->header->layer_number() == mp->prev_header->layer_number() &&
            mp->header->version() == mp->prev_header->version() &&
            mp->header->error_protection() == mp->prev_header->error_protection() &&
            mp->header->private_bit() == mp->prev_header->private_bit() &&
            mp->header->copyright() == mp->prev_header->copyright() &&
            mp->header->original() == mp->prev_header->original() &&
            strcmp(mp->header->de_emphasis(), mp->prev_header->de_emphasis()) == 0)
            exitCheck = 0;
    }

    if (readBits != 0) {
        readBits -= SYNC_WORD_LENGTH + 1;
        nSlots = readBits >> 3;
    }
    else
        nSlots = -1;

    if (mp->prev_header->padding())
        nSlots--;

    return (nSlots);
}


/**************************************************************************
  Title        : SeekSync

  Purpose      : Seeks for a byte aligned sync word in the bitstream and
                 places the bitstream pointer right after the sync word.

  Usage        : y = SeekSync(n)

  Input        : mp - MPx stream parameters

  Output       : y - result of seeking

  Author(s)    : Juha Ojanpera
  *************************************************************************/

SEEK_STATUS
SeekSync(MP_Stream *mp)
{
#define SYNC_LIMIT (75000) // ~9 kB
    BOOL exitCheck = TRUE;
    int sync_cand;
    uint32 bits_read, hdr;

    mp->bs->byteAlign();
    bits_read = mp->syncInfo.sync_length;
    sync_cand = (int) mp->bs->getBits(mp->syncInfo.sync_length);

    while (exitCheck) {
        while (sync_cand != mp->syncInfo.sync_word && !mp->bs->endOfStream() &&
               bits_read < SYNC_LIMIT) {
            bits_read++;
            sync_cand = (sync_cand << 1) & mp->syncInfo.sync_mask;
            sync_cand |= (int) mp->bs->getBits(1);
        }

        if (mp->bs->endOfStream())
            return (EOF_STREAM);
        else if (bits_read > SYNC_LIMIT)
            return (SYNC_LOST);
        else {
            bits_read++;
            sync_cand = (sync_cand << 1) & mp->syncInfo.sync_mask;
            sync_cand |= (int) mp->bs->getBits(1);

            mp->side_info->sfbData.mpeg25 = !(sync_cand & 0x1);

            /* Check the next frame header. */
            hdr = (!mp->side_info->sfbData.mpeg25) << HEADER_BITS;
            mp->header->SetHeader(hdr | mp->bs->lookAhead(HEADER_BITS));

            /* Detect false frame boundaries. */
            switch (mp->syncInfo.sync_status) {
                case LAYER1_STREAM:
                    if (HEADER_MASK2(mp->header->header) ==
                            HEADER_MASK2(mp->prev_header->header) &&
                        mp->header->sfreq() != 3 && LAYER_MASK(mp->header->header) == 0x3)
                        exitCheck = FALSE;
                    break;

                case LAYER2_STREAM:
                    if (HEADER_MASK2(mp->header->header) ==
                            HEADER_MASK2(mp->prev_header->header) &&
                        mp->header->sfreq() != 3 && LAYER_MASK(mp->header->header) == 0x2)
                        exitCheck = FALSE;
                    break;

                case LAYER3_STREAM:
                    if (HEADER_MASK3(mp->header->header) ==
                            HEADER_MASK3(mp->prev_header->header) &&
                        mp->header->sfreq() != 3 && LAYER_MASK(mp->header->header) == 0x1)
                        exitCheck = FALSE;
                    break;

                case FIRST_FRAME_WITH_LAYER1:
                    if (mp->header->sfreq() != 3 && LAYER_MASK(mp->header->header) == 0x3) {
                        mp->prev_header->SetHeader(mp->header->header);
                        exitCheck = FALSE;
                    }
                    break;

                case FIRST_FRAME_WITH_LAYER2:
                    if (mp->header->sfreq() != 3 && LAYER_MASK(mp->header->header) == 0x2) {
                        mp->prev_header->SetHeader(mp->header->header);
                        exitCheck = FALSE;
                    }
                    break;

                case FIRST_FRAME_WITH_LAYER3:
                    if (mp->header->sfreq() != 3 && LAYER_MASK(mp->header->header) == 0x1 &&
                        (strcmp(mp->header->de_emphasis(), "none") == 0) &&
                        mp->header->bit_rate_idx() != 15) {
                        mp->prev_header->SetHeader(mp->header->header);
                        exitCheck = FALSE;
                    }
                    break;

                default:
                    break;
            }
        }
    }

    return (SYNC_FOUND);
}
