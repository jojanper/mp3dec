/**************************************************************************
  mp3mix.cpp - MPEG-1, MPEG-2 LSF, and MPEG-2.5 layer III mixing subroutines.

  Author(s): Juha Ojanpera
  Copyright (c) 2000 Juha Ojanpera.
  *************************************************************************/

/*-- This module is include file for 'mp3.cpp'. --*/

/*
 * Parses the current input frame of the specified mp3 stream.
 *
 * Returns TRUE on success, FALSE on failure.
 */
BOOL
mp3ParseFrame(MP_Stream *mp)
{
start:

    // Get the header and (optional) CRC codeword.
    decode_header(mp);

    // Now layer dependent parsing routines.
    switch (mp->header->layer_number()) {
        case 3:
            // Read the side info.
            III_get_side_info(mp);

            // Put the payload to the bit reservoir buffer.
            mp->br->addBytes(*mp->bs, mp->main_data_slots());

            // Bit reservoir processing.
            if (L3BitReservoir(mp) == FALSE) {
                if (ReSync(mp) == FALSE)
                    goto exit;
                else
                    goto start;
            }
            break;

        default:
            return (FALSE);
    }

    return (TRUE);

exit:

    return (FALSE);
}

/*
 * Obtains the reconstructed spectral samples for the specified
 * mp3 frame and granule within the frame.
 */
#define MP3GETSPECTRALSAMPLES(mp, gr)                         \
    {                                                         \
        BOOL isLSF = mp->side_info->sfbData.lsf;              \
        BOOL isIS = mp->side_info->sfbData.is_stereo;         \
        int part2 = mp->br->bitsRead();                       \
        ScaleFunc[isLSF](mp, gr, LEFT_CHANNEL);               \
        III_huffman_decode(mp, gr, LEFT_CHANNEL, part2);      \
        if (mp->header->channels() == 2) {                    \
            part2 = mp->br->bitsRead();                       \
            ScaleFunc[isLSF](mp, gr, RIGHT_CHANNEL);          \
            III_huffman_decode(mp, gr, RIGHT_CHANNEL, part2); \
        }                                                     \
        if (isIS)                                             \
            III_stereo_mode(mp, gr);                          \
        III_dequantize(mp, gr);                               \
    }

/*
 *
 * Returns TRUE on success, FALSE on failure.
 */
BOOL
MixFrame(MP_Stream *mp0, MP_Stream *mp2, int16 *pcm_sample)
{
    int16 *pcm[2];
    int ss, gr, ch;
    FLOAT *mixSrc[MAX_CHANNELS];
    FLOAT tsOut[MAX_CHANNELS][SSLIMIT][SBLIMIT];

    mixSrc[LEFT_CHANNEL] = &tsOut[LEFT_CHANNEL][0][0];
    mixSrc[RIGHT_CHANNEL] = &tsOut[RIGHT_CHANNEL][0][0];

    // Output data pointers.
    pcm[LEFT_CHANNEL] = pcm_sample;
    pcm[RIGHT_CHANNEL] = pcm_sample + 1;

    if (!mp3ParseFrame(mp0))
        return (FALSE);

    if (!mp3ParseFrame(mp2))
        return (FALSE);

    // Decode granule(s) and then write the ouput.
    for (gr = 0; gr < mp0->side_info->sfbData.max_gr; gr++) {
        MP3GETSPECTRALSAMPLES(mp0, gr);
        MP3GETSPECTRALSAMPLES(mp2, gr);

        // Compute the output.
        for (ch = 0; ch < mp2->out_param->num_out_channels; ch++) {
            // Reorder short blocks + alias reduction.
            III_reorder(mp2, ch, gr);
            III_antialias(mp2, ch, gr);

            // Apply inverse MDCT to each subband.
            III_hybrid2(mp2, tsOut[ch], ch, gr);
        }
        int mixBins = mp2->side_info->sfbData.sbHybrid;
        if (mp2->out_param->num_out_channels == 1)
            mixSrc[RIGHT_CHANNEL] = mixSrc[LEFT_CHANNEL];

        // Compute the output.
        for (ch = 0; ch < mp0->out_param->num_out_channels; ch++) {
            // Reorder short blocks + alias reduction.
            III_reorder(mp0, ch, gr);
            III_antialias(mp0, ch, gr);

            // Apply inverse MDCT to each subband.
            III_hybrid2(mp0, mp0->tsOut, ch, gr);

            // Mix the reconstructed subband samples.
            III_hybridmix(&mp0->tsOut[0][0], mixSrc[ch], mixBins);

            // Apply polyphase synthesis filter to each subband.
            for (ss = 0; ss < SSLIMIT; ss++, pcm[ch] += mp0->idx_increment)
                PolyPhaseFIR(mp0, mp0->tsOut[ss], pcm[ch], mp0->buffer->buf_idx + ch,
                             mp0->out_param, ch);
        }
    }

    return (TRUE);
}
