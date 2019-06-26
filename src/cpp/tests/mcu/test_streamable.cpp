#include <gtest/gtest.h>

#include "core/meta.h"
#include "interface/decoder.h"

namespace draaldecodertest {

TEST(StreambleDecoderStaticTestSuite, Create)
{
    draaldecoder::IStreamableDecoder *dec;
    auto attrs = draaldecoder::AudioAttributes();

    // No instance when unsupported MIME type specified
    attrs.setString("mime", "foo/bar");
    dec = draaldecoder::IStreamableDecoder::create(attrs);
    EXPECT_TRUE(dec == nullptr);
}

class StreambleDecoderTestSuite : public testing::Test
{
protected:
    draaldecoder::IStreamableDecoder *m_dec;

    virtual void SetUp()
    {
        auto attrs = draaldecoder::AudioAttributes();

        // Request MP3 decoder instance
        attrs.setString("mime", draaldecoder::MP3MIME);
        m_dec = draaldecoder::IStreamableDecoder::create(attrs);
        ASSERT_TRUE(m_dec != nullptr);
    }

    virtual void TearDown()
    {
        if (m_dec)
            m_dec->destroy();
    }
};

TEST_F(StreambleDecoderTestSuite, Decode)
{
    auto result = m_dec->decode();
    EXPECT_FALSE(result);
}

} // namespace draaldecodertest
