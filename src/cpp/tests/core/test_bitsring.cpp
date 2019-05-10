#include <gtest/gtest.h>

#include "core/bits.h"
#include "core/bitsring.h"
#include "core/iobuf.h"
#include "test_utils.h"

static const char *filename = "ring.bs";

class BitsRingBufferTestSuite : public testing::Test
{
protected:
    BitsRingBuffer m_br;
    Bit_Stream m_bs;
    FileBuf m_fp;

    virtual void SetUp()
    {
        auto size = createTestBitstream(filename);
        m_fp.open(filename, kFileReadMode);
        m_bs.open(&m_fp, 32);

        ASSERT_TRUE(m_br.open(4096));

        m_br.addBytes(m_bs, size);
    }

    virtual void TearDown()
    {
        m_bs.close();
        m_fp.close();
    }
};

TEST_F(BitsRingBufferTestSuite, Parse)
{
    // Append some data, the data is byte aligned at the bitstream level at this point
    m_br.putBits(5, 4);
    m_br.putBits(4, 1);
    m_br.putBits(7, 0);

    // The 1st 4 bits should return this value
    EXPECT_EQ(m_br.getBits(4), (uint32_t) 7);
    // 4 bits read so far
    EXPECT_EQ(m_br.totalBits(), (uint32_t) 4);
    // The next 4 bits should return this value
    EXPECT_EQ(m_br.lookAhead(4), (uint32_t) 14);

    // Skip value 14 (6 bits) and 3 (2 bits)
    m_br.skipBits(6);
    // Next 6 bits should return this value
    EXPECT_EQ(m_br.getBits(6), (uint32_t) 0);

    // Skip value 255 (8 bits) and 123 (12-bits)
    m_br.skipBits(20);
    // Amount of bits read so far
    EXPECT_EQ(m_br.totalBits(), (uint32_t) 36);

    // Rewind back to the start
    m_br.rewindNbits(6);
    m_br.rewindNbits(40);                      // 10 bits too much
    m_br.skipBits(10);                         // Compensate the extra 10 bits rewinded
    EXPECT_EQ(m_br.totalBits(), (uint32_t) 0); // No bits read so far

    // First 10 bits from buffer
    EXPECT_EQ(m_br.getBits(10), (uint32_t) 507); // 01 11 1110 11

    // Next 10 bits
    EXPECT_EQ(m_br.getBits(10), (uint32_t) 15); // 000000 1111

    // Remaining bits
    EXPECT_EQ(m_br.getBits(4), (uint32_t) 15);
    EXPECT_EQ(m_br.getBits(12), (uint32_t) 123);

    // Skip the bits added due to byte-alignment at the bitstream file creation time
    m_br.skipBits(4);

    // Expected amount of bits read
    EXPECT_EQ(m_br.totalBits(), (uint32_t) 40);

    // Read the appeded data (added at the start of this function)
    EXPECT_EQ(m_br.getBits(5), (uint32_t) 4);
    EXPECT_EQ(m_br.getBits(4), (uint32_t) 1);
    EXPECT_EQ(m_br.getBits(7), (uint32_t) 0);
    EXPECT_EQ(m_br.totalBits(), (uint32_t) 56);

    // Seek to the start of the file
    m_bs.reset();
    EXPECT_EQ(m_bs.seekStream(START_POS, 0), 0);
    m_br.addBytes(m_bs, 2, true); // Add 2 bytes (fast add possible)
    m_br.addBytes(m_bs, 2, true); // Add another 2 bytes (fast add possible)

    // Next 2 bits are as expected
    EXPECT_EQ(m_br.getBits(2), (uint32_t) 1);
    // Skip some bits
    m_br.skipBits(2 + 4 + 2 + 6);
    // The next 8 bits should match the expected value
    EXPECT_EQ(m_br.getBits(8), (uint32_t) 255);

    m_br.reset();
    EXPECT_EQ(m_br.totalBits(), (uint32_t) 0);
}
