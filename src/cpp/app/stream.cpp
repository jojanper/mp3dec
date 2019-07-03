#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./parse.h"
#include "core/defines.h"
#include "core/io/console.h"
#include "interface/attributes.h"
#include "interface/defs.h"

int
main(int argc, char **argv)
{
    FILE *fp;
    Console *console = new Console();

    BOOL waveOut = FALSE;
    char *inStream, *outStream;
    if (!ParseMPCommandLine(&inStream, &outStream, &waveOut, argc, argv, 22, nullptr))
        return EXIT_FAILURE;

    // Open input file
    fp = fopen(inStream, "rb");
    if (!fp) {
        fprintf(stderr, "Unable to open file %s\n", inStream);
        return EXIT_FAILURE;
    }

    // Size of file
    fseek(fp, 0, SEEK_END);
    auto size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    size = 16384;

    // Prepare decoder
    auto attrs = draaldecoder::IAttributes::create();
    attrs->setString("mime", draaldecoder::MP3MIME);
    attrs->setInt32Data(draaldecoder::kBufferSize, size);
    attrs->setInt32Data(draaldecoder::kBufferMode, draaldecoder::kModuloBuffer);

    auto dec = draaldecoder::StreamableDecoder::create(*attrs);
    if (!dec) {
        fprintf(stderr, "Unable to create decoder instance\n");
        return EXIT_FAILURE;
    }

    // Read entire file into memory
    auto *buffer = new uint8_t[size];
    size = fread(buffer, sizeof(uint8_t), size, fp);

    printf("OK 1\n");

    if (!dec->init(*attrs, buffer, size)) {
        fprintf(stderr, "Unable to initialize decoder for file %s\n", inStream);
        return EXIT_FAILURE;
    }

    printf("OK 2\n");

    // Open output (file)
    if (!console->open(outStream, 44100, 2, waveOut))
        return EXIT_FAILURE;

    // Decode until end of stream found
    size_t frames = 0;
    bool result = true;

    fprintf(stdout, "\n");
    fflush(stdout);

    do {
        printf("OK 3\n");
        do {
            result = dec->decode();
            if (result) {
                size_t size;
                auto data = dec->getDecodedAudio(size);

                console->writeBuffer(data, size);

                fprintf(stdout, "Frames decoded: %zu\r", frames++);
                fflush(stdout);
            }

        } while (result);

        printf("OK 4\n");
        size = fread(buffer, sizeof(uint8_t), size, fp);
        if (size) {
            dec->addInput(buffer, size);
        }

    } while (size);

    fprintf(stdout, "\n");
    fflush(stdout);

    delete[] buffer;

    attrs->destroy();
    dec->destroy();

    console->close();
    delete console;

    return EXIT_SUCCESS;
}
