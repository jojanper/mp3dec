#include <gtest/gtest.h>

#include "core/bits.h"
#include "core/io/iobuf.h"
#include "test_utils.h"

static const char *filename = "foo.bs";


TEST(BitStreamTestSuite, WriteBits)
{
    FileBuf fp;
    BitStream bs;

    fp.open(filename, kFileWriteMode);

    bs.open(&fp, 32768);
    bs.putBits(15, 444); // Write 15 bits
    bs.putBits(5, 15);   // Write 5 bits
    bs.putBits(30, 9);   // Write 30 bits
    bs.putBits(6, 30);   // Write 6 bits
    bs.putBits(7, 7);    // Write 7 bits
    bs.putBits(1, 0);    // Write 1 bit
    bs.putBits(5, 28);   // Write 5 bits

    bs.close();
    fp.close();

    fp.open(filename, kFileReadMode);
    bs.open(&fp, 8);

    // First 15 bits are correct
    EXPECT_EQ(bs.getBits(15), (uint32_t) 444);

    // Next 5 bits are correct
    EXPECT_EQ(bs.getBits(5), (uint32_t) 15);

    // Skip next 30 bits in 3 parts
    bs.skipBits(15);
    bs.skipBits(5);
    bs.skipBits(10);

    // Next 6 bits are then correct
    EXPECT_EQ(bs.getBits(6), (uint32_t) 30);

    // Next 7 bits are correct
    EXPECT_EQ(bs.getBits(7), (uint32_t) 7);

    // 1 bit is read when bitstream is byte aligned
    EXPECT_EQ(bs.byteAlign(), (int) 1);

    // Next 5 bits are correct
    EXPECT_EQ(bs.lookAhead(5), (uint32_t) 28);
    EXPECT_EQ(bs.getBits(5), (uint32_t) 28);

    bs.close();
    fp.close();
}

TEST(BitStreamTestSuite, EndOfBitstream)
{
    BitStream bs;
    auto fp = new FileBuf();

    createTestBitstream(filename);

    fp->open(filename, kFileReadMode);

    // Reading bits must be correct
    bs.open(fp, 2);
    EXPECT_EQ(bs.getBits(2), (uint32_t) 1);
    EXPECT_EQ(bs.lookAhead(2), (uint32_t) 3);
    EXPECT_EQ(bs.getBits(2), (uint32_t) 3);
    EXPECT_EQ(bs.getBits(4), (uint32_t) 14);
    EXPECT_EQ(bs.getBits(2), (uint32_t) 3);
    EXPECT_EQ(bs.getBits(6), (uint32_t) 0);
    EXPECT_EQ(bs.getBits(8), (uint32_t) 255);
    EXPECT_EQ(bs.getBits(12), (uint32_t) 123);

    // Reading further bits return 0
    EXPECT_EQ(bs.getBits(27), (uint32_t) 0);
    // And end of bitstream is signalled
    EXPECT_TRUE(bs.endOfStream());

    bs.close();

    fp->close();
    delete fp;
}

TEST(BitStreamTestSuite, SeekAndFlush)
{
    BitStream bs;
    auto fp = new FileBuf();

    createTestBitstream(filename);

    fp->open(filename, kFileReadMode);

    // Reading bits must be correct
    bs.open(fp, 3);
    EXPECT_EQ(bs.getBits(2), (uint32_t) 1);
    bs.skipBits(2);
    EXPECT_EQ(bs.getBits(4), (uint32_t) 14);

    // One byte read so far, 2 bytes to go before buffer runs out of bits
    EXPECT_EQ(bs.getSlotsLeft(), (size_t) 2);

    // Read 1 bit which means that current read position is no longer byte-aligned
    EXPECT_EQ(bs.getBits(1), (uint32_t) 1);

    // Flush the bits read so far (+ byte-align)
    bs.flushStream();

    // Next bits whould be this one
    EXPECT_EQ(bs.getBits(8), (uint32_t) 255);

    // Flush, we have now read in total 3 bytes
    bs.flushStream();

    // Go to start of stream
    EXPECT_EQ(bs.seekStream(CURRENT_POS, -3), 0);
    EXPECT_EQ(bs.getBits(2), (uint32_t) 1);

    bs.close();

    fp->close();
    delete fp;
}

TEST(BitStreamTestSuite, Positions)
{
    BitStream bs;
    FileBuf fp;

    createTestBitstream(filename);

    fp.open(filename, kFileReadMode);

    // Reading bits must be correct
    bs.open(&fp, 3);
    EXPECT_EQ(bs.getStreamSize(), (uint32_t) 5);
    bs.skipBits(8);

    // Current position is one byte from the start of the file
    EXPECT_EQ(bs.getCurrStreamPos(), (uint32_t) 1);

    // Current position is always byte-aligned with respect to the start of the current
    // slot, that is, unless all bits are read from current slot, then that slot is not
    // excluded from position determination.
    bs.skipBits(2);
    EXPECT_EQ(bs.getCurrStreamPos(), (uint32_t) 1);

    // Skip next 6 bits to achieve byte-aligned read position
    bs.skipBits(6);
    // Skip next slot
    bs.advanceBufferSlot(1);
    // Reading next 12 bits should return expected value
    EXPECT_EQ(bs.getBits(12), (uint32_t) 123);

    // Seek to the start of the file
    bs.reset();
    EXPECT_EQ(bs.seekStream(START_POS, 0), 0);

    // First 2 bits are correct
    EXPECT_EQ(bs.getBits(2), (uint32_t) 1);
    // The buffer handle gives correct data
    EXPECT_EQ(bs.getBuffer()[0], 126); // 01 11 1110

    bs.close();

    fp.close();
}
