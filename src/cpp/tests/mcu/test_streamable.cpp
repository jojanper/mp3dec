#include <gtest/gtest.h>

#include "core/meta.h"
#include "interface/decoder.h"
#include "interface/defs.h"

namespace draaldecodertest {

TEST(StreambleDecoderStaticTestSuite, Create)
{
    draaldecoder::StreamableDecoder *dec;
    auto attrs = draaldecoder::AudioAttributes();

    // No instance when unsupported MIME type specified
    attrs.setString("mime", "foo/bar");
    dec = draaldecoder::StreamableDecoder::create(attrs);
    EXPECT_TRUE(dec == nullptr);
}

class StreambleDecoderTestSuite : public testing::Test
{
protected:
    draaldecoder::StreamableDecoder *m_dec;

    virtual void SetUp()
    {
        auto attrs = draaldecoder::AudioAttributes();

        // Request MP3 decoder instance
        attrs.setString("mime", draaldecoder::MP3MIME);
        m_dec = draaldecoder::StreamableDecoder::create(attrs);
        ASSERT_TRUE(m_dec != nullptr);
    }

    virtual void TearDown()
    {
        if (m_dec)
            m_dec->destroy();
    }
};

TEST_F(StreambleDecoderTestSuite, NotInitialized)
{
    auto result = m_dec->addInput(nullptr, 0);
    EXPECT_FALSE(result);

    result = m_dec->decode();
    EXPECT_FALSE(result);

    size_t size;
    auto ptr = m_dec->getDecodedAudio(size);
    EXPECT_TRUE(ptr == nullptr);
    EXPECT_EQ(size, (size_t) 0);
}

} // namespace draaldecodertest
