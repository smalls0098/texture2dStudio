#include <cassert>
#include <stb_image.h>
#include "texture2d.h"

int DecompressEtc2() {
    const char *input = R"(tests/ktx.ktx)";
    const char *output = R"(tests/ktx.png)";

    FILE *fd = fopen(input, "rb");
    assert(fd);
    size_t size;
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    uint8_t *in = new uint8_t[size];
    fread(in, size, 1, fd);
    fclose(fd);

    uint8_t *out = nullptr;
    size_t outsize = 0;
    DecompressEtc2(in, 1024, 1024, &out, &outsize);

    FILE *ofd = fopen(output, "wb");
    assert(ofd);
    fseek(ofd, 0, SEEK_SET);
    fwrite(out, outsize, 1, ofd);
    fclose(ofd);
    return 1;
}

int CompressEtc2() {

    const char *input = R"(tests/c/ktx.png)";
    const char *output = R"(tests/c/pngtoktx.ktx)";

    FILE *fd = fopen(input, "rb");
    assert(fd);
    size_t size;
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    uint8_t *in = new uint8_t[size];
    fread(in, size, 1, fd);
    fclose(fd);

    uint8_t *out = nullptr;
    size_t outsize = 0;
    CompressEtc2RGBA(in, size, 0, 0, 1, 1, &out, &outsize);

    FILE *ofd = fopen(output, "wb");
    assert(ofd);
    fseek(ofd, 0, SEEK_SET);
    fwrite(out, outsize, 1, ofd);
    fclose(ofd);
    return 1;
}

//int compAstc() {
//    const char *input = R"(tests\img.png)";
//    const char *output = R"(tests\paint.ktx)";
//
//    FILE *fd = fopen(input, "rb");
//    assert(fd);
//    size_t size;
//    fseek(fd, 0, SEEK_END);
//    size = ftell(fd);
//    fseek(fd, 0, SEEK_SET);
//
//    uint8_t *in = new uint8_t[size];
//    fread(in, size, 1, fd);
//    fclose(fd);
//
//    uint8_t *out = nullptr;
//    size_t outsize = 0;
//    CompressAstc(in, size, 0, 8, 8, 1, &out, &outsize);
//
//    FILE *ofd = fopen(output, "wb");
//    assert(ofd);
//    fseek(ofd, 0, SEEK_SET);
//    fwrite(out, outsize, 1, ofd);
//    fclose(ofd);
//    return 1;
//}


int main() {
    DecompressEtc2();
//    CompressEtc2();
    return 0;
}