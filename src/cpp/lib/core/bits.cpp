/*-- Project Headers. --*/
#include "bits.h"

/*
  Purpose:     Number of shifts needed to turn multiplication and/or
               division into shifting operation.
  Explanation: - */
#define NATIVE_WORD_SHIFT 3

/*
  Purpose:     Number of bits in one buffer item element.
  Explanation: - */
#define SLOT_BITS         8

/*
   Purpose:     Global buffer for masking purposes.
   Explanation: - */
static const uint32_t mask[] = {
 0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF,
 0x1FFFL, 0x3FFFL, 0x7FFFL, 0xFFFFL, 0x1FFFFL, 0x3FFFFL, 0x7FFFFL, 0xFFFFFL,
 0x1FFFFFL, 0x3FFFFFL, 0x7FFFFFL, 0xFFFFFFL, 0x1FFFFFFL, 0x3FFFFFFL, 0x7FFFFFFL,
 0xFFFFFFFL, 0x1FFFFFFFL, 0x3FFFFFFFL, 0x7FFFFFFFL, 0xFFFFFFFFL
};

/*
   Purpose:     Macro for swapping the byte order of a 32-bit word.
   Explanation: - */
#ifndef X86_ASM
#ifndef NATIVE_64_BIT
#define swap_int32(uint_32) (((uint_32) << 24) |               \
                            (((uint_32) << 8) & 0x00ff0000L) | \
                            (((uint_32) >> 8) & 0x0000ff00L) | \
                            ( (uint_32) >> 24))
#else
/*
   Purpose:     Macro for swapping the byte order of a 64-bit word.
   Explanation: - */
#define swap_int64(uint_64) (((uint_64) << 56) |                        \
                            (((uint_64) << 40) & 0x00ff000000000000L) | \
                            (((uint_64) << 24) & 0x0000ff0000000000L) | \
                            (((uint_64) << 8)  & 0x000000ff00000000L) | \
                            (((uint_64) >> 8)  & 0x00000000ff000000L) | \
                            (((uint_64) >> 24) & 0x0000000000ff0000L) | \
                            (((uint_64) >> 40) & 0x000000000000ff00L) | \
                            ( (uint_64) >> 56))

#endif /* not NATIVE_64_BIT */
#endif /* not X86_ASM */

/**************************************************************************
  Title        : Bit_Stream

  Purpose      : Class constructor.

  Usage        : Bit_Stream()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

__stdcall
Bit_Stream::Bit_Stream(void)
{
#ifdef BS_WRITE_ROUTINES
  device_mode = READ_MODE;
  bits_written = 0;
#endif
  bit_buffer = NULL;
  byte_buffer = NULL;
  buf_len = 0;
  bit_counter = 0;
  buf_index = 0;
  eobs = 1;
  buffer_empty = 0;
  streamSize = 0;
  sBuf = NULL;
  bsMode = UNKNOWN_BUFFER;
  bOrder = ORDER_UNKNOWN;
}


/**************************************************************************
  Title        : ~Bit_Stream

  Purpose      : Class destructor.

  Usage        : ~Bit_Stream()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

__stdcall
Bit_Stream::~Bit_Stream(void)
{
  if(bit_buffer)
    delete [] bit_buffer;
  bit_buffer = NULL;

  if(bsMode == FILE_BUFFER)
  {
    if(byte_buffer)
      delete [] byte_buffer;
    byte_buffer = NULL;
  }

  eobs = 1;
  buf_len = 0;
  bit_counter = 0;
  buf_index = 0;
  buffer_empty = 0;
  streamSize = 0;
  sBuf = NULL;

  bsMode = UNKNOWN_BUFFER;
  bOrder = ORDER_UNKNOWN;
}


/**************************************************************************
  Title       : SwapArray

  Purpose     : Swaps the byte order of the input array.

  Usage       : SwapArray(in_buffer, out_buffer, len)

  Input       : in_buffer  - bytes to be swapped
                len        - length of the array

  Output      : out_buffer - swapped bytes

  Author(s)   : Juha Ojanpera
  *************************************************************************/

void __fastcall
Bit_Stream::SwapArray(uint32 *in_buffer, uint32 *out_buffer, int len)
{
#ifdef X86_ASM
  __asm {
        // edx = in_buffer
        // ecx = out_buffer
        // dword ptr[ebp + 8] = len

        mov eax, 0                      // i = 0
        mov edi, dword ptr[ebp + 8]
        shl edi, 2                      // len
      .MainLoop :
        mov esi, dword ptr [edx + eax]
        bswap esi                       // swap byte order of in_buffer[i]
        mov dword ptr [ecx + eax], esi  // out_buffer[i] = swapped in_buffer[i]

        add eax, 4                      // i++
        cmp eax, edi                    // if(i < len)
        jl .MainLoop                    //   goto .MainLoop
  }
#else
  register uint32 dword, *dwordp;

  /* Swap the bytes. */
  dwordp = out_buffer;
  for(int i = 0; i < len; i++)
  {
    dword = in_buffer[i];//*dwordp;
#ifndef NATIVE_64_BIT
    *dwordp++ = swap_int32 (dword);
#else
    *dwordp++ = swap_int64 (dword);
#endif /* not NATIVE_64_BIT */
  }

#endif /* X86_ASM */
}


/**************************************************************************
  Title       : DetermineByteOrder

  Purpose     : Routine to determine byte order.

  Usage       : y = DetermineByteOrder()

  Output      : y - byte order of this machine

  Author(s)   : Juha Ojanpera
  *************************************************************************/

BsByteOrder __fastcall
Bit_Stream::DetermineByteOrder(void)
{
  char s[sizeof(long) + 1];
  union
  {
    long longval;
    char charval[sizeof(long)];
  } probe;

  probe.longval = 0x41424344L;  /* ABCD in ASCII */
  strncpy(s, probe.charval, sizeof(long));
  s[ sizeof(long) ] = '\0';

  if(strcmp(s, "ABCD") == 0)
    return (ORDER_BIGENDIAN);
  else
    if(strcmp(s, "DCBA") == 0)
      return (ORDER_LITTLEENDIAN);
    else
      return (ORDER_UNKNOWN);
}


/**************************************************************************
  Title       : Open

  Purpose     : Opens the bitstream.

  Usage       : y = Open(hInst, stream, mode, size, sBuf)

  Input       : hInst  - current instance handle
                stream - name of the stream to be opened
                mode   - READ_MODE, WRITE_MODE or APPEND_MODE
                size   - size of the bitstream buffer (in units of 'uint32')
                sBuf   - streaming buffer

  Output      : y - TRUE on success, FALSE otherwise;
                    throws AdvanceExcpt * on allocation failure

  Explanation : When streaming, the 'stream' and 'size' parameters are ignored.

  Author(s)   : Juha Ojanpera
  *************************************************************************/

BOOL __stdcall
Bit_Stream::Open(HINSTANCE hInst, const char *stream, int mode, int size,
                 StreamBuffer *sBuf)
{
  bit_buffer = NULL;

  /*-- Are we streaming ? --*/
  this->sBuf = sBuf;
  bsMode = (sBuf) ? STREAM_BUFFER : FILE_BUFFER;
  if(FILE_BUFFER && stream == NULL)
    bsMode = SINGLE_BUFFER;

  /*-- When streaming, the bits are read from the StreamBuffer. --*/
  switch(bsMode)
  {
    case FILE_BUFFER:
      /*-- Open the file, internal access method uses multimedia file I/O. --*/
      if(!ioBuf.OpenBuffer(hInst, stream, mode, WINDOWS_IO))
        return (FALSE);
      break;

    case STREAM_BUFFER:
      /*-- What is the maximum size that can be read at a time. --*/
      size = sBuf->GetStreamBufferLen();
      break;

    default:
      break;
  }

  switch(mode)
  {
    case READ_MODE:
      bit_counter = 0;
      break;

    case WRITE_MODE:
      bit_counter = sizeof(uint32) << 3;
      break;

    case APPEND_MODE:
      bit_counter = sizeof(uint32) << 3;
      break;
  }

  /*-- Allocate the bit buffer. --*/
  buf_len_old = buf_len = size;
  if(buf_len)
  {
    bit_buffer = (uint32 *)Chunk::GetChunk(UINT32_CHUNK, buf_len);
    switch(bsMode)
    {
      case FILE_BUFFER:
        byte_buffer = (BYTE *)Chunk::GetChunk(BYTE_CHUNK, buf_len << 2);
        break;

      default:
        byte_buffer = NULL;
    }
  }

#ifdef BS_WRITE_ROUTINES
  device_mode = mode;
  bits_written = 0;
#endif
  buffer_empty = 1;
  buf_index = -1;
  eobs = 0;

  bOrder = DetermineByteOrder();

  /*-- The size is needed when calculating the total length of the file. --*/
  switch(bsMode)
  {
    case FILE_BUFFER:
      streamSize = ioBuf.GetStreamSize();
      break;

    default:
      streamSize = 0;
  }

  return (TRUE);
}

BOOL __stdcall
Bit_Stream::Open(HINSTANCE hInst, BYTE *bsBuffer, int size)
{
  int newBufSize;
  BOOL result = TRUE;

  newBufSize = (size >> 2);
  if(bit_buffer == NULL)
    this->Open(hInst, NULL, READ_MODE, 16384, NULL);
  buf_len = newBufSize + 2;

  SwapArray((uint32 *)bsBuffer, bit_buffer, newBufSize);
  int mod = size & 3;
  if(mod)
  {
    size = newBufSize << 2;
    switch(mod)
    {
      case 1:
        bit_buffer[newBufSize] = ((uint32)bsBuffer[size] << 24);
        break;

      case 2:
        bit_buffer[newBufSize] = ((uint32)bsBuffer[size]     << 24) |
                                 ((uint32)bsBuffer[size + 1] << 16);
        break;

      case 3:
        bit_buffer[newBufSize] = ((uint32)bsBuffer[size]     << 24) |
                                 ((uint32)bsBuffer[size + 1] << 16) |
                                 ((uint32)bsBuffer[size + 2] << 8);
        break;
    }
  }

  buf_index = 0;
  buffer_empty = 0;
  bit_counter = uint32_BIT;

  return (result);
}


/**************************************************************************
  Title       : Close

  Purpose     : Closes the opened bitstream.

  Usage       : Close()

  Author(s)   : Juha Ojanpera
  *************************************************************************/

void __stdcall
Bit_Stream::Close(void)
{
#ifdef BS_WRITE_ROUTINES
  if(device_mode == WRITE_MODE && ((buf_index | bit_counter) != 0))
    ff_buffer(1);
#endif /* BS_WRITE_ROUTINES */

  if(bit_buffer)
    delete [] bit_buffer;
  bit_buffer = NULL;

  if(bsMode == FILE_BUFFER)
  {
    if(byte_buffer)
      delete [] byte_buffer;
    byte_buffer = NULL;

    ioBuf.CloseBuffer();
  }
}


/**************************************************************************
  Title       : ff_buffer

  Purpose     : Refills the bitstream buffer (in READ_MODE) or writes the
                bitstream buffer into a device (in WRITE_MODE).

  Usage       : ff_buffer(write_force)

  Input       : force_write - if set, writes the bitstream buffer into the
                              output device (used only in WRITE_MODE)

  Explanation : This function updates the read/write index of the bitstream
                buffer. In read-mode the buffer is refilled, if it is empty.
                In write-mode the buffer, if full, is stored into the device.

  Author(s)   : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::ff_buffer(int force_write)
{
#ifdef BS_WRITE_ROUTINES
  switch(device_mode)
  {
    case READ_MODE:
#endif
      if(!eobs)
      {
        buf_index++;
        if(buf_index == buf_len || buffer_empty)
        {
          uint32 nitems;

          switch(bsMode)
          {
            case FILE_BUFFER:
              /*
               * Update the file pointer before reading new bits to to bit
               * buffer. If 'buf_index' == 0 we are reading for the first
               * time so no need to update the pointer.
               */
              if(!buffer_empty && buf_index != 0)
                ioBuf.SeekBuffer(CURRENT_POS, buf_index << 2);
              nitems = ioBuf.ReadToBuffer(byte_buffer, buf_len << 2);
              break;

            case STREAM_BUFFER:
              /*
               * Read new bits from the StreamBuffer class. If no more data
               * is available, the execution will be stopped. It is therefore
               * recommended that bitstream parsing is part of a thread. The
               * StreamBuffer will then flag when internal buffers are full.
               * If end-of-stream has been found, buffer size of 0 will be
               * returned.
               */
              nitems = buf_len << 2;
              byte_buffer = sBuf->GetStreamBuffer(&nitems);
              break;

            default:
              eobs = 1;
              buf_index = 0;
              memset(bit_buffer, 0, buf_len * sizeof(uint32));
              return;
          }
          nitems >>= 2;

          if(nitems == 0)
          {
            /*
             * Fill the first bit buffer index with zero if end of the
             * bitstream has been reached. This ensures that there should
             * be no sudden changes at the output.
             */
            eobs = 1;
            memset(bit_buffer, 0, buf_len * sizeof(uint32));
            memset(byte_buffer, 0, buf_len * sizeof(uint32));
            return;
          }

          buf_index = 0;
          buffer_empty = 0;
          buf_len = (int)nitems;

          /*-- Convert byte order to big endian. --*/
#ifndef X86_ASM
          if(bOrder == ORDER_LITTLEENDIAN)
#endif
            SwapArray((uint32 *)byte_buffer, bit_buffer, buf_len);
        }
      }
#ifdef BS_WRITE_ROUTINES
      break;
#endif

#ifdef BS_WRITE_ROUTINES
    case WRITE_MODE:
      buf_index++;

      /* Check whether the buffer is full. */
      if(buf_index == buf_len || force_write)
      {
        /*
         * Check how much we actually need to save. If 'force_write'
         * is equal to one we might actually need to save only some
         * part of the buffer.
         */
        buf_index = (buf_index == buf_len) ? buf_len : buf_index;

        /* Convert byte order big endian. */
        if(bOrder == ORDER_LITTLEENDIAN)
          SwapArray(bit_buffer, buf_len);

        /* Write the buffer to output stream. */
        nitems = ioBuf.WriteFromBuffer(bit_buffer, buf_index << 2);
        nitems >>= 2;

        if(nitems != buf_index)
          ; // Add error handling here if needed.

        buf_index = 0;
      }
      break;
#endif /* BS_WRITE_ROUTINES */

#ifdef BS_WRITE_ROUTINES
  }
#endif
}


#ifdef BS_WRITE_ROUTINES
/**************************************************************************
  Title        : putbits

  Purpose      : Writes bits to the bit stream.

  Usage        : putbits(n, word);

  Input:       : n    - how many bits are appended to the bit stream
                 word - bits to write

  Explanation  : Note that the maximum number of bits that can be appended
                 is 'uint32_BIT'.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void __stdcall
Bit_Stream::putbits(int n, uint32 word)
{
  bits_written += n;

  /*-- Update the bitstream buffer index. --*/
  if(bit_counter == 0)
  {
    ff_buffer(bs, 0);
    bit_counter = uint32_BIT;
    bit_buffer[buf_index] = 0;
  }

  /*
   *  If 'n' is bigger than 'bit_counter', 'word' must be splitted
   *  into two parts.
   *
   *  e.g. 'word' = 00000011111101010101010111111100 (= 16642095)
   *       'n' = 26;
   *       'bit_counter' = 20
   *
   *        000000  11111101010101010111        111100
   *                |<---upper part--->|  |<--lower part-->|
   */
  if((bit_counter-n) < 0)
  {
    int end;
    uint32 next;

    /*-- Number of bits needed to store the lower part. --*/
    end = n-bit_counter;

    /*-- Store the upper part to the bitstream buffer. --*/
    bit_buffer[buf_index] |= ((uint32)word >> end);

    /*-- Mask the upper part from 'word' to zero. --*/
    next = (uint32)word & mask[end];

    /*-- Update the bitstream buffer index. --*/
    ff_buffer(bs, 0);
    bit_counter = uint32_BIT;
    bit_buffer[buf_index] = 0;

    bit_counter -= end;

    /*-- Store the lower part to the bitstream buffer. --*/
    bit_buffer[buf_index] |= ((uint32)next << bit_counter);
  }
  else
  {
    bit_counter -= n;

    /*-- For safety, mask the unwanted bits to zero. --*/
    bit_buffer[buf_index] |= ((word & mask[n]) << bit_counter);
  }

}

#endif /* BS_WRITE_ROUTINES */

uint32_t
Bit_Stream::getbits(int n)
{

}

/**************************************************************************
  Title        : getbits

  Purpose      : Reads bits from the bitstream.

  Usage        : y = getbits(n);

  Input        : n - number of bits to be read

  Output       : y - bits read

  Explanation  : Note that the maximum number of bits that can be read
                 from the bitstream buffer is 'uint32_BIT' bits.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

uint32_t
Bit_Stream::getbits8(int n)
{
  int idx;
  uint32_t tmp;

  /*-- Update the bitstream buffer index. --*/
  if(bit_counter == 0)
  {
    ff_buffer(0);
    bit_counter = SLOT_BITS;
  }

  idx = bit_counter - n;
  if(idx < 0)
  {
    /*-- Mask the unwanted bits to zero. --*/
    tmp = (bit_buffer[buf_index] & mask[bit_counter]) << -idx;

    /*-- Update the bit stream buffer. --*/
    ff_buffer(0);
    bit_counter = SLOT_BITS + idx;
    tmp |= (bit_buffer[buf_index] >> bit_counter) & mask[-idx];
  }
  else
  {
    bit_counter = idx; // bit_counter -= n;
    tmp = (bit_buffer[buf_index] >> bit_counter) & mask[n];
  }

  return tmp;
}


/**************************************************************************
  Title        : skipbits

  Purpose      : Flushes bits from the bitstream.

  Usage        : y = skipbits(n);

  Input        : n - number of bits to be flushed

  Output       : y - flushed read

  Explanation  : The upper limit for the number of bits to be flushed is
                 'uint32_BIT'.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::skipbits(int n)
{
  int idx;

  /*-- Update the bitstream buffer index. --*/
  if(bit_counter == 0)
  {
    ff_buffer(0);
    bit_counter = SLOT_BITS;
  }

  idx = bit_counter - n;
  if(idx < 0)
  {
    /*-- Update the bit stream buffer. --*/
    ff_buffer(0);
    bit_counter = SLOT_BITS + idx;
  }
  else
    bit_counter = idx; //bit_counter -= n;
}


/**************************************************************************
  Title        : skip_Nbits

  Purpose      : Discard bits from the bitstream.

  Usage        : skipN_bits(n);

  Input        : n - number of bits to be discarded

  Explanation  : No upper limit for the number of bits to be discarded exists.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::skipN_bits(int n)
{
  int i, bytes, bits_left;

  /*-- Shifting is more efficient than division. --*/
  bytes = (n >> NATIVE_WORD_SHIFT);
  bits_left = n - (bytes << NATIVE_WORD_SHIFT);

  for(i = 0; i < bytes; i++)
    skipbits(SLOT_BITS);

  if(bits_left)
    skipbits(bits_left);
}


/**************************************************************************
  Title        : byte_align

  Purpose      : Byte aligns the bitstream.

  Usage        : byte_align()

  Explanation  : Use this function only when you are reading the bitstream.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
Bit_Stream::byte_align(void)
{
  int bits_to_byte_align;

  bits_to_byte_align = bit_counter & 7; // % 8;
  if(bits_to_byte_align)
    skipbits(bits_to_byte_align);

  return bits_to_byte_align;
}


/**************************************************************************
  Title        : look_ahead

  Purpose      : Looks ahead for the next 'N' bits from the bitstream and
                 returns the read 'N' bits.

  Usage        : y = look_ahead(N);

  Input        : N - number of bits to be read from the bitstream

  Output       : y - bits read

  Author(s)    : Juha Ojanpera
  *************************************************************************/

uint32
Bit_Stream::look_ahead(int N)
{
  Bit_Stream bs_tmp;
  uint32 dword;

  /*-- Store only the most important variables. --*/
  bs_tmp.eobs = eobs;
  bs_tmp.buf_len = buf_len;
  bs_tmp.bit_counter = bit_counter;
  bs_tmp.buf_index = buf_index;
  bs_tmp.buffer_empty = buffer_empty;

  /*
   * Due to the implementation mode of bitstream parsing, there must be
   * a way to "cancel" the bit buffer update. When streaming, the previous
   * buffer must not be released to receive new data from the stream.
   * Following method provides us this feature.
   */
  this->m_ioBuf->SetLookAheadMode(TRUE);

  dword = getbits(N);

  /*
   * Restore previous state of the bitstream buffer.
   * First check if bitstream buffer has been updated.
   */
  if(buf_index < bs_tmp.buf_index)
  {
    /*
     * Remember that the stream pointer has to be explicitly
     * updated. Due to this, the file pointer has been advanced
     * only by one buffer size. To restore the previous state,
     * we only have to "cancel" the file pointer update to get
     * the previous buffer state.
     *
     * Get the previous streaming buffer. It can be easily obtained
     * since we are in lookahead mode from the StreamBuffer point of view,
     * meaning that the previous buffer has not been released for other use.
     */
    this->m_ioBuf->SeekBuffer(CURRENT_POS, -bs_tmp.buf_len);
    buf_len = this->m_ioBuf->ReadToBuffer(byte_buffer, bs_tmp.buf_len);
  }

  /*-- Back to "normal" mode of operation. --*/
  this->m_ioBuf->SetLookAheadMode(FALSE);

  eobs = bs_tmp.eobs;
  buf_len = bs_tmp.buf_len;
  bit_counter = bs_tmp.bit_counter;
  buf_index = bs_tmp.buf_index;
  buffer_empty = bs_tmp.buffer_empty;

  return (dword);
}


/**************************************************************************
  Title        : FlushStream

  Purpose      : Flushes the bitstream. This has the advantage that when
                 the getbits function is next time called, the reading starts
                 from the current stream position.

  Usage        : FlushStream()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::FlushStream(void)
{
  int byte_offset;

  byte_align();

  /* Calculate how much the stream pointer needs to be updated. */
  byte_offset = buf_index;
  byte_offset += (SLOT_BITS - bit_counter) >> 3;
  if(byte_offset)
    this->m_ioBuf->SeekBuffer(CURRENT_POS, byte_offset);

  /*
    * This has the effect that the next time we start to read the bit
    * stream, the stream pointer is not updated before reading. This is
    * because if this function is called then it is assumed that the stream
    * pointer is already in its correct place.
    */
  buffer_empty = 1;
  buf_index = -1;
  bit_counter = 0;
}

int32_t SeekStream(FilePos filePos, int32_t offset)
{
  return this->m_ioBuf->SeekBuffer(filePos, offset);
}
