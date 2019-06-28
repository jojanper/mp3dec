#include <gtest/gtest.h>

#include "core/membuffer.h"
#include "interface/defs.h"

TEST(MemoryBufferTestSuite, Init)
{
    MemoryBuffer buffer;

    EXPECT_FALSE(buffer.init(0, draaldecoder::kLinearBuffer)); // Invalid buffer size
    EXPECT_EQ(buffer.SeekBuffer(CURRENT_POS, 3), (int32_t) 0); // Seek fails

    EXPECT_TRUE(buffer.init(128, draaldecoder::kLinearBuffer)); // Valid buffer size
}

static const uint8_t data[] = { 3, 100, 192, 4, 77 };

TEST(MemoryBufferTestSuite, OverWriteBuffer)
{
    MemoryBuffer buffer;

    // GIVEN initialized buffer handle
    ASSERT_TRUE(buffer.init(sizeof(data), draaldecoder::kOverWriteBuffer, "foo"));
    ASSERT_TRUE(buffer.setBuffer(data, sizeof(data)));
    ASSERT_STREQ(buffer.GetStreamName(), "foo");
    ASSERT_EQ(buffer.GetStreamSize(), (uint32_t) 5);

    uint8_t readData[12];

    // WHEN reading 0 length data from buffer
    // THEN it should fail
    EXPECT_EQ(buffer.ReadToBuffer(readData, 0), (uint32_t) 0);


    // WHEN seeking to invalid position
    // THEN it should fail and return current position
    EXPECT_EQ(buffer.SeekBuffer(CURRENT_POS, 6), (int32_t) 0);


    // WHEN seeking the buffer
    // THEN it should succeed
    EXPECT_EQ(buffer.SeekBuffer(START_POS, 4), (int32_t) 4);
    EXPECT_EQ(buffer.SeekBuffer(CURRENT_POS, -4), (int32_t) 0);


    // WHEN reading first 3 bytes from buffer
    // THEN it should succeed
    EXPECT_EQ(buffer.ReadToBuffer(readData, 3), (uint32_t) 3);
    EXPECT_EQ(readData[0], 3);
    EXPECT_EQ(readData[1], 100);
    EXPECT_EQ(readData[2], 192);

    // AND buffer position is correct
    EXPECT_EQ(buffer.SeekBuffer(CURRENT_POS, 3), (int32_t) 3);


    // WHEN reading next 3 bytes
    // THEN it should indicate that only 2 bytes were read
    EXPECT_EQ(buffer.ReadToBuffer(readData, 3), (uint32_t) 2);
    EXPECT_EQ(readData[0], 4);
    EXPECT_EQ(readData[1], 77);
    EXPECT_EQ(readData[2], 0);

    // AND buffer position is at the end of the data
    EXPECT_EQ(buffer.SeekBuffer(CURRENT_POS, 2), (int32_t) 5);


    // WHEN reading next 3 bytes
    // THEN it should fail as we are at the end of the data
    EXPECT_EQ(buffer.ReadToBuffer(readData, 3), (uint32_t) 0);


    // WHEN seeking 2 bytes towards the start from the end of the data
    // THEN it should indicate correct position
    EXPECT_EQ(buffer.SeekBuffer(END_POS, 2), (int32_t) 3);


    // WHEN reading 3 next bytes
    // THEN it should indicate that only 2 bytes were read
    EXPECT_EQ(buffer.ReadToBuffer(readData, 3), (uint32_t) 2);
    EXPECT_EQ(readData[0], 4);
    EXPECT_EQ(readData[1], 77);
    EXPECT_EQ(readData[2], 0);
}

TEST(MemoryBufferTestSuite, LinearBuffer)
{
    MemoryBuffer buffer;
    uint8_t readData[12];

    // GIVEN initialized buffer handle
    ASSERT_TRUE(buffer.init(2 * sizeof(data), draaldecoder::kLinearBuffer));

    // WHEN adding new data
    // THEN it should succeed
    EXPECT_TRUE(buffer.setBuffer(data, sizeof(data)));

    // AND read data is as expected
    EXPECT_EQ(buffer.ReadToBuffer(readData, 4), (uint32_t) 4);
    EXPECT_EQ(readData[0], 3);
    EXPECT_EQ(readData[1], 100);
    EXPECT_EQ(readData[2], 192);
    EXPECT_EQ(readData[3], 4);
    EXPECT_EQ(buffer.SeekBuffer(CURRENT_POS, 4), (int32_t) 4);


    // WHEN adding again new data
    // THEN it should succeed
    EXPECT_TRUE(buffer.setBuffer(data, sizeof(data)));

    // AND read data is as expected
    EXPECT_EQ(buffer.ReadToBuffer(readData, 6), (uint32_t) 6);
    EXPECT_EQ(readData[0], 77);
    EXPECT_EQ(readData[1], 3);
    EXPECT_EQ(readData[2], 100);
    EXPECT_EQ(readData[3], 192);
    EXPECT_EQ(readData[4], 4);
    EXPECT_EQ(readData[5], 77);
    EXPECT_EQ(buffer.SeekBuffer(CURRENT_POS, 6), (int32_t) 10);


    // WHEN adding again new data
    // THEN it should fail as data will not fit into the buffer
    EXPECT_FALSE(buffer.setBuffer(data, sizeof(data)));
}

TEST(MemoryBufferTestSuite, ModuloBuffer)
{
    MemoryBuffer buffer;
    uint8_t readData[12];

    // GIVEN initialized buffer handle
    ASSERT_TRUE(buffer.init(2 * sizeof(data), draaldecoder::kModuloBuffer));

    // WHEN adding new data
    // THEN it should succeed
    EXPECT_TRUE(buffer.setBuffer(data, sizeof(data)));

    // AND read data is as expected
    EXPECT_EQ(buffer.ReadToBuffer(readData, 3), (uint32_t) 3);
    EXPECT_EQ(readData[0], 3);
    EXPECT_EQ(readData[1], 100);
    EXPECT_EQ(readData[2], 192);
    EXPECT_EQ(buffer.SeekBuffer(CURRENT_POS, 3), (int32_t) 3);


    // WHEN adding new data
    // THEN it should succeed
    EXPECT_TRUE(buffer.setBuffer(data, sizeof(data)));

    // AND read data is as expected
    EXPECT_EQ(buffer.ReadToBuffer(readData, 3), (uint32_t) 3);
    EXPECT_EQ(readData[0], 4);
    EXPECT_EQ(readData[1], 77);
    EXPECT_EQ(readData[2], 3);
}
