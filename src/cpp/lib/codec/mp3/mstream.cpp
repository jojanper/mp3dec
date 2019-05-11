/**************************************************************************
  mstream.cpp - Interface implementations for MP_Stream.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000, 2019 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- Project Headers. --*/
#include "mstream.h"
//#include "applicat.h"
//#include "def.h"
//#include "diagassert.h"
//#include "huftree.h"
//#include "layer.h"
//#include "mp3.h"
//#include "mp3chunk.h"

/**************************************************************************
  Title        : MP_Stream

  Purpose      : Constructor.

  Usage        : MP_Stream()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

MP_Stream::MP_Stream() : bs(NULL)
{
#if 0
    try {
        initialized = FALSE;
        reduced_class = FALSE;
        streamName[0] = '\0';
        bs = NULL;
        header = NULL;
        prev_header = NULL;
        frame = NULL;
        buffer = NULL;
        br = NULL;
        side_info = NULL;
        huffman = NULL;
        huftree = NULL;

        bs = (Bit_Stream *) MP3_Chunk::MP3_GetChunk(BITBUFFER_CHUNK);
        header = (MP_Header *) MP3_Chunk::MP3_GetChunk(MPHEADER_CHUNK);
        prev_header = (MP_Header *) MP3_Chunk::MP3_GetChunk(MPHEADER_CHUNK);
        frame = (MP_Frame *) MP3_Chunk::MP3_GetChunk(MPFRAME_CHUNK);
        buffer = (MP_Buffer *) MP3_Chunk::MP3_GetChunk(MPBUFFER_CHUNK);
        side_info = (III_Side_Info *) MP3_Chunk::MP3_GetChunk(SIDE_INFO_CHUNK);

        /*-- Initialize the Huffman decoding module. --*/
        huffman = (MP3_Huffman *) MP3_Chunk::MP3_GetChunk(HUFFMAN_CHUNK, 34);
        InitMP3Huffman(huffman);

        /*-- Build Huffman tree. --*/
        huftree = (HufNode **) MP3_Chunk::MP3_GetChunk2(HUFNODE_CHUNK, 34);
        for (int i = 0; i < 34; i++) {
            if (huffman[i].tree_len == 0)
                continue;

            huftree[i] = CreateMP3HuffmanTree(&huffman[i]);
        }
        /*-- End of Huffman decoding module initialization. --*/

        idx_increment = 0;
    } catch (AdvanceExcpt *e) {
        initialized = FALSE;
        ASSERTCONSTRUCTOR(_DLL_BUILD_);
    }

    initialized = TRUE;
#endif
}

MP_Stream::MP_Stream(BitStream *input) : bs(input)
{
#if 0
    try {
        initialized = FALSE;
        reduced_class = TRUE;
        streamName[0] = '\0';
        this->bs = NULL;
        header = NULL;
        prev_header = NULL;
        frame = NULL;
        buffer = NULL;
        br = NULL;
        side_info = NULL;
        huffman = NULL;
        huftree = NULL;

        this->bs = bs;
        header = (MP_Header *) MP3_Chunk::MP3_GetChunk(MPHEADER_CHUNK);
        prev_header = (MP_Header *) MP3_Chunk::MP3_GetChunk(MPHEADER_CHUNK);
        side_info = (III_Side_Info *) MP3_Chunk::MP3_GetChunk(SIDE_INFO_CHUNK);
    } catch (AdvanceExcpt *e) {
        initialized = FALSE;
        ASSERTCONSTRUCTOR(_DLL_BUILD_);
    }

    initialized = TRUE;
#endif
}


/**************************************************************************
  Title        : ~MP_Stream

  Purpose      : Destructor.

  Usage        : ~MP_Stream()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

MP_Stream::~MP_Stream(void)
{
#if 0
    if (!reduced_class)
        this->ReleaseDecoder();

    if (bs && !reduced_class)
        delete bs;
    bs = NULL;

    if (header)
        delete header;
    header = NULL;

    if (prev_header)
        delete prev_header;
    prev_header = NULL;

    if (frame)
        delete frame;
    frame = NULL;

    if (buffer)
        delete buffer;
    buffer = NULL;

    if (side_info)
        delete side_info;
    side_info = NULL;

    /*-- Huffman trees. --*/
    if (huftree) {
        for (int i = 0; i < 34; i++) {
            if (huffman[i].tree_len == 0)
                continue;

            DeleteTree(huftree[i]);
        }

        delete[] huftree;
        huftree = NULL;
    }

    /*-- Huffman codebooks. --*/
    if (huffman)
        delete[] huffman;
    huffman = NULL;

    initialized = FALSE;
#endif
}

#if 0

/**************************************************************************
  Title        : FillDataSlotTable

  Purpose      : Precomputes the # of main data slots to internal table.

  Usage        : FillDataSlotTable()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
MP_Stream::FillDataSlotTable(void)
{
    int *brTbl, Slots;

    brTbl = header->GetBitRateTable();
    for (int i = 1; i < 15; i++) {
        Slots = (int) ((144 * brTbl[i]) / (header->frequency() / 1000.));

        if (header->version() != MPEG_PHASE2_LSF) {
            if (header->channels() == 1)
                Slots -= 17;
            else
                Slots -= 32;
        }
        else {
            Slots >>= 1;
            if (header->channels() == 1)
                Slots -= 9;
            else
                Slots -= 17;
        }

        Slots -= 4;

        SlotTable[i] = Slots;
    }
}


/**************************************************************************
  Title        : main_data_slots

  Purpose      : Retrieves the number of bytes for the layer III payload. The
                 payload consists of the scalefactors and quantized data of
                 the channel(s).

  Usage        : main_data_slots()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int __fastcall MP_Stream::main_data_slots(void)
{
    int nSlots;

    if (header->bit_rate()) {
#if 0
    nSlots = (int)(144 * header->bit_rate()) / (header->frequency() / 1000.);

    if(header->version() != MPEG_PHASE2_LSF)
    {
      if(header->channels() == 1)
        nSlots -= 17;
      else
        nSlots -=32;
    }
    else
    {
      nSlots >>= 1;
      if(header->channels() == 1)
        nSlots -= 9;
      else
        nSlots -=17;
    }

    nSlots -= 4;

#else
        nSlots = SlotTable[header->bit_rate_idx()];
#endif

        if (header->padding())
            nSlots++;

        if (header->error_protection())
            nSlots -= 2;
    }
    else {
        nSlots = FreeFormatSlots;

        if (header->padding())
            nSlots++;
    }

    return (nSlots);
}


/**************************************************************************
  Title        : ReleaseDecoder

  Purpose      : Deletes resources allocated to the decoder.

  Usage        : ReleaseDecoder()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void __fastcall MP_Stream::ReleaseDecoder(void)
{
    if (initialized) {
        /*-- Close the stream. --*/
        bs->Close();

        /*-- Layers I and II use fixed bit allocation. --*/
        if (header->layer_number() != 3) {
            if (frame->bit_alloc)
                delete[] frame->bit_alloc;
            frame->bit_alloc = NULL;
        }

        /*-- Quantized samples. --*/
        if (frame->quant)
            delete[] frame->quant;
        frame->quant = NULL;

        /*-- Scalefactors. --*/
        if (frame->scale_factors)
            delete[] frame->scale_factors;
        frame->scale_factors = NULL;

        /*-- Output pcm samples. --*/
        if (buffer->pcm_sample)
            delete[] buffer->pcm_sample;
        buffer->pcm_sample = NULL;

        /*-- Synthesis buffer. --*/
        for (int i = 0; i < header->channels(); i++) {
            if (buffer->synthesis_buffer[i])
                delete[] buffer->synthesis_buffer[i];
            buffer->synthesis_buffer[i] = NULL;

            if (buffer->Fixsynthesis_buffer[i])
                delete[] buffer->Fixsynthesis_buffer[i];
            buffer->Fixsynthesis_buffer[i] = NULL;
        }

        /*-- Dequantized samples. --*/
        if (buffer->reconstructed) {
            buffer->reconstructed[0] = 0.0f;
            delete[] buffer->reconstructed;
            buffer->reconstructed = NULL;
        }

        /*-- Layer III parameters. --*/
        if (header->layer_number() == 3) {
            /*-- Bit reservoir --*/
            if (br)
                delete br;
            br = NULL;

            if (side_info) {
                for (int i = 0; i < header->channels(); i++) {
                    if (side_info->ch_info[i]) {
                        if (side_info->ch_info[i]->scale_fac)
                            delete side_info->ch_info[i]->scale_fac;
                        side_info->ch_info[i]->scale_fac = NULL;

                        if (side_info->ch_info[i]->gr_info[0])
                            delete side_info->ch_info[i]->gr_info[0];
                        side_info->ch_info[i]->gr_info[0] = NULL;

                        if (header->version() == MPEG_AUDIO_ID) {
                            if (side_info->ch_info[i]->gr_info[1])
                                delete side_info->ch_info[i]->gr_info[1];
                            side_info->ch_info[i]->gr_info[1] = NULL;
                        }

                        delete side_info->ch_info[i];
                    }
                    side_info->ch_info[i] = NULL;
                }

                if (side_info->s_mode_long)
                    delete[] side_info->s_mode_long;
                side_info->s_mode_long = NULL;

                for (int i = 0; i < 3; i++) {
                    if (side_info->s_mode_short[i])
                        delete[] side_info->s_mode_short[i];
                    side_info->s_mode_short[i] = NULL;
                }
            }
        }
    }
}


/**************************************************************************
  Title        : InitLayerIIICommonObjects

  Purpose      : Initializes layer III specific objects.

  Usage        : InitLayerIIICommonObjects()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
MP_Stream::InitLayerIIICommonObjects(void)
{
    int i, j;
    III_Channel_Info *ch_info;
    III_Scale_Factors *scale_fac;
    int idx[] = { 0, 23, 36, 49, 62, 85, 98, 111 };

    PrevSlots = 0;
    FreeFormatSlots = 0;
    FrameStart = 0;
    SkipBr = FALSE;
    WasSeeking = FALSE;

    /*-- Compute the payload for each legal bit rate. --*/
    FillDataSlotTable();

    /*-- Bit reservoir. --*/
    br = (Bit_Reservoir *) MP3_Chunk::MP3_GetChunk(BITRESER_CHUNK);
    br->OpenBr();

    /*-- Get scalefactor band related parameters. --*/
    III_SfbDataInit(&side_info->sfbData, header);

    /*-- Initialize sideinfo. --*/
    for (i = 0; i < header->channels(); i++) {
        side_info->ch_info[i] =
            (III_Channel_Info *) MP3_Chunk::MP3_GetChunk(III_CHANNEL_CHUNK);

        ch_info = side_info->ch_info[i];
        ch_info->gr_info[0] = (Granule_Info *) MP3_Chunk::MP3_GetChunk(GRANULE_CHUNK);

        if (header->version() == MPEG_AUDIO_ID) // MPEG-1 only
            ch_info->gr_info[1] = (Granule_Info *) MP3_Chunk::MP3_GetChunk(GRANULE_CHUNK);
    }

    /*-- Stereo modes. --*/
    side_info->s_mode_long = (StereoMode *) MP3_Chunk::MP3_GetChunk(STEREOMODE_CHUNK, 22);
    for (i = 0; i < 3; i++)
        side_info->s_mode_short[i] =
            (StereoMode *) MP3_Chunk::MP3_GetChunk(STEREOMODE_CHUNK, 13);

    /*-- Initialize scalefactor pointers. --*/
    for (i = j = 0; i < header->channels(); i++) {
        side_info->ch_info[i]->scale_fac =
            (III_Scale_Factors *) MP3_Chunk::MP3_GetChunk(III_SCALEFAC_CHUNK);

        scale_fac = side_info->ch_info[i]->scale_fac;
        scale_fac->scalefac_long = frame->scale_factors + idx[j++];
        scale_fac->scalefac_short[0] = frame->scale_factors + idx[j++];
        scale_fac->scalefac_short[1] = frame->scale_factors + idx[j++];
        scale_fac->scalefac_short[2] = frame->scale_factors + idx[j++];
    }

    /*-- Initialize re-ordering table. --*/
    init_III_reorder(side_info->sfbData.sfbShort, side_info->sfbData.sfbWidth);

    /*-- Zero out overlap-add samples for IMDCT. --*/
    memset(&OverlapBlock[0][0][0], 0, 4 * MAX_MONO_SAMPLES * sizeof(FLOAT));
    memset(blc, 0, sizeof(int) * 2);

    return (TRUE);
}


/*
 * Retrieves the MPEG layer of the specified input stream. The
 * layer is guessed based on the file extension. Default is layer 3.
 */
SYNC_STATUS
MP_Stream::GuessLayer(const char *stream)
{
    SYNC_STATUS layer;

    if ((lstrcmp(stream + lstrlen(stream) - 3, "MP1") == 0) ||
        (lstrcmp(stream + lstrlen(stream) - 3, "mp1") == 0))
        layer = FIRST_FRAME_WITH_LAYER1;
    else if ((lstrcmp(stream + lstrlen(stream) - 3, "MP2") == 0) ||
             (lstrcmp(stream + lstrlen(stream) - 3, "mp2") == 0))
        layer = FIRST_FRAME_WITH_LAYER2;
    else if ((lstrcmp(stream + lstrlen(stream) - 3, "MP3") == 0) ||
             (lstrcmp(stream + lstrlen(stream) - 3, "mp3") == 0))
        layer = FIRST_FRAME_WITH_LAYER3;
    else
        layer = LAYER_UNDEFINED;

    return (layer);
}


/*
 * Displays a dialog box for the user indicating that the layer of the
 * specified input stream could not be determined. The selection of the
 * correct layer is then up to the user.
 */
static BOOL CALLBACK
LayerSelectionDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage) {
        case WM_INITDIALOG:
            SetForegroundWindow(hDlg);
            return (TRUE);

        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam, lParam)) {
                case IDOK:
                    /*-- Layer I was selected. --*/
                    EndDialog(hDlg, FIRST_FRAME_WITH_LAYER1);
                    break;

                case IDCANCEL:
                    /*-- Layer II was selected. --*/
                    EndDialog(hDlg, FIRST_FRAME_WITH_LAYER2);
                    break;

                case IDNO:
                    /*-- Layer III was selected. --*/
                    EndDialog(hDlg, FIRST_FRAME_WITH_LAYER3);
                    break;

                default:
                    return (FALSE);
            }
            break;

        default:
            return (FALSE);
    }

    return (TRUE);
}


/*
 * Interface function to invoke the layer selection dialog box.
 */
SYNC_STATUS
MP_Stream::LayerSelection(HINSTANCE hInst, HWND hWnd)
{
    int layer;

    layer = DialogBox(hInst, MAKEINTRESOURCE(LAYERSELECTION), hWnd,
                      (DLGPROC) LayerSelectionDlgProc);
    if (layer == -1)
        layer = LAYER_UNDEFINED;

    return ((SYNC_STATUS) layer);
}

/*
 * Initializes the sync related parameters and searches the start of first
 * frame within the specified input stream. Returns SYNC_FOUND on success,
 * on failure SYNC_LOST is returned indicating that the start of first
 * frame could not be found. Possible reasons for this are invalid file
 * and/or the initial MPEG layer was not correct. The 'use_assert' parameter
 * invokes a error message on failure if set to TRUE.
 */
SEEK_STATUS
MP_Stream::Init_Sync(SYNC_STATUS layer, BOOL use_assert) throw(AdvanceExcpt *)
{
    int32 hdr;

    header->SetHeader(0);
    header->crc = 0;

    /*-- Set information for synchronization. --*/
    syncInfo.sync_word = (int16) SYNC_WORD;
    syncInfo.sync_length = (int16) SYNC_WORD_LENGTH;
    syncInfo.sync_mask = (1 << syncInfo.sync_length) - 1;
    syncInfo.sync_status = layer;

    /*-- Find the start of the stream. --*/
    if (SeekSync(this) != SYNC_FOUND) {
        if (use_assert) {
            char buf[64];
            wsprintf(buf, "%s - Decoder Error", _DLL_BUILD_);
            ASSERT2(sync != SYNC_FOUND, "Unable to find syncword.", buf, NULL);
        }
        else
            return (SYNC_LOST);
    }
    syncInfo.sync_status -= (SYNC_STATUS) 3;

    /*-- Check what layer we are about to be decoding. --*/
    hdr = (!side_info->sfbData.mpeg25) << HEADER_BITS;
    header->SetHeader(hdr | bs->look_ahead(HEADER_BITS));

    return (SYNC_FOUND);
}


/**************************************************************************
  Title        : InitDecoder

  Purpose      : Initializes MPEG audio decoding engine.

  Usage        : y = InitDecoder(hInst, hWnd, mp_stream, out_param, complex, sBuf)

  Input        : hInst     - current instance
                 hWnd      - handle of calling window
                 mp_stream - name of MPEG audio stream
                 out_param - output parameters of stream
                 complex   - complexity reduction parameters
                 sBuf      - buffer interface for mp3 streaming

  Output       : y - TRUE on success, FALSE otherwise

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
MP_Stream::InitDecoder(HINSTANCE hInst,
                       HWND hWnd,
                       const char *mp_stream,
                       Out_Param *out_param,
                       Out_Complexity *complex,
                       StreamBuffer *sBuf) throw(AdvanceExcpt *)
{
    BOOL result = TRUE;
    SYNC_STATUS initLayer;
    int i, j, groups = 1, extra = 1;

    if (initialized == FALSE) {
        ASSERTCONSTRUCTOR(_DLL_BUILD_);
    }

    this->complex = complex;
    this->out_param = out_param;

/*-- Open the bitstream. --*/
#define MAX_SLOTS ((MAX_FRAME_SLOTS << 1) + 1)
    result = bs->Open(hInst, mp_stream, READ_MODE, MAX_SLOTS, sBuf);
    if (result == FALSE)
        return (result);

    /*
     * Check whether this stream is the same than the previous stream.
     */
    if (sBuf == NULL && lstrcmp(streamName, mp_stream)) {
        initLayer = GuessLayer(mp_stream);

        /*
         * We cannot determine the layer based on the file extension.
         * Let the user select the layer.
         */
        if (initLayer == LAYER_UNDEFINED)
            initLayer = LayerSelection(hInst, hWnd);

        /*-- An error occured while processing dialog box. --*/
        if (initLayer == LAYER_UNDEFINED)
            initLayer = FIRST_FRAME_WITH_LAYER3;
    }
    else if (sBuf)
        initLayer = FIRST_FRAME_WITH_LAYER3;
    else
        initLayer = streamSync;

    /*-- Find the start of first frame. --*/
    Init_Sync(initLayer, TRUE);

    /*
     * Store the filename and layer. These are needed if we have playback loop.
     * Why to ask from user every time what is the layer, if we already know it !
     */
    streamSync = initLayer;
    lstrcpy(streamName, mp_stream);

    /*-- Size of memory chunks to be allocated. --*/
    groups = header->channels() * SBLIMIT;

    /*
     * Note that in layer I there are only one group, whereas layer II
     * consist of 3 groups. Remember also that each group consists of 12
     * samples per subband.
     */

    /*
     * Bit allocation for each group of layer I and II.
     * This is not needed in layer III, since the dequantized samples
     * are using variable length bit allocation (= Huffman coded).
     */
    if (header->layer_number() != 3)
        frame->bit_alloc = (BYTE *) MP3_Chunk::MP3_GetChunk(BYTE_CHUNK, groups);

    if (header->layer_number() != 1)
        groups *= 3;

    /*
     * Scale factors for each group. This is also used in layer III, but we
     * access this array through the 'side_info' structure.
     */
    frame->scale_factors = (BYTE *) MP3_Chunk::MP3_GetChunk(BYTE_CHUNK, groups);

    /*-- Layer III initialization. --*/
    if (header->layer_number() == 3) {
        InitLayerIIICommonObjects();

        /*-- Re-calculate the needed amount of memory. --*/
        groups = MAX_MONO_SAMPLES * header->channels();

    } /* if(layer_number() == 3) */

    /*
     * Quantized spectral coefficients.
     * When performing Huffman decoding, there is a possibility (NOT A BUG !!)
     * that we access this array beyond 'groups'-samples. At maximum, this offset
     * is 4 samples. So the extra 10 samples should quarantee safe execution.
     */
    frame->quant = (int16 *) MP3_Chunk::MP3_GetChunk(INT16_CHUNK, groups + 10);

    /*-- Reconstructed spectral coefficients. --*/
    buffer->reconstructed = (FLOAT *) MP3_Chunk::MP3_GetChunk(FLOAT_CHUNK, groups);

    /*-- Channel pointers for quantized and reconstructed data. --*/
    if (header->layer_number() == 3)
        for (i = 0; i < header->channels(); i++) {
            frame->ch_quant[i] = frame->quant + i * MAX_MONO_SAMPLES;

            buffer->ch_reconstructed[i] = buffer->reconstructed + i * MAX_MONO_SAMPLES;

            for (j = 0; j < SBLIMIT; j++)
                spectrum[i][j] = &buffer->ch_reconstructed[i][j * SSLIMIT];
        }

    if (header->layer_number() == 1)
        extra = SCALE_BLOCK;
    else if (header->layer_number() == 2)
        extra = SCALE_BLOCK * 3;
    else
        extra = side_info->sfbData.max_gr;

    /*-- Reconstructed time domain samples. --*/
    buffer->pcm_sample = (int16 *) MP3_Chunk::MP3_GetChunk(INT16_CHUNK, groups * extra);

    /*-- Synthesis buffer of the channel(s). --*/
    for (i = 0; i < header->channels(); i++) {
        buffer->buf_idx[i] = 0;

        /*-- For floating point arithmetic. --*/
        buffer->synthesis_buffer[i] =
            (FLOAT *) MP3_Chunk::MP3_GetChunk(FLOAT_CHUNK, HAN_SIZE << 1);

        /*-- For fixed point arithmetic. --*/
        buffer->Fixsynthesis_buffer[i] =
            (int16 *) MP3_Chunk::MP3_GetChunk(INT16_CHUNK, HAN_SIZE << 1);
    }

    /*-- If free format stream, determine the bit rate. --*/
    if (header->bit_rate() == 0) {
        FreeFormatSlots = FindFreeFormatSlotCount(this);
        if (FreeFormatSlots < 0) {
            char buf[64];
            wsprintf(buf, "%s - Decoder Error", _DLL_BUILD_);
            ASSERT2(FreeFormatSlots < 0,
                    "Unable to determine bit rate of the opened stream.", buf, NULL);
        }

        /*-- Rewind the stream back to the beginning. --*/
        bs->FlushStream();
        bs->SeekStream(START_POS, 0);

        /*
         * Find the start of the first frame again. This should not
         * cause any problems.
         */
        syncInfo.sync_status += (SYNC_STATUS) 3;
        SeekSync(this);
        syncInfo.sync_status -= (SYNC_STATUS) 3;
    }

    return (result);
}
#endif
