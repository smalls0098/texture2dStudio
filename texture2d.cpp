//
// Created by smalls on 2021/8/15.
//

#include <Etc.h>
#include <astcenccli_internal.h>
#include <etcDecoder.h>
#include <astcDecoder.h>
#include <lodepng.h>
#include "Astc.h"
#include "Ktx.h"
#include <stb_image_write.h>
#include <stb_image.h>
#include "texture2d.h"

int CompressEtc1(uint8_t *src, size_t size, int mipmap, float fEffort, int jobs, int header,
                 uint8_t **dst, size_t *filesize) {
    Ktx ktx{src, size, mipmap == 1, Etc::Image::Format::ETC1, fEffort, jobs, header};
    bool result = ktx.Write(dst, filesize);
    if (result) {
        printf("CompressEtc1 time = %dms\n", ktx.encodingTime);
        return 1;
    } else {
        return 0;
    }
}

int CompressEtc2RGB(uint8_t *src, size_t size, int mipmap, float fEffort, int jobs, int header,
                    uint8_t **dst, size_t *filesize) {
    Ktx ktx{src, size, mipmap == 1, Etc::Image::Format::RGB8, fEffort, jobs, header};
    bool result = ktx.Write(dst, filesize);
    if (result) {
        printf("CompressEtc2RGB time = %dms\n", ktx.encodingTime);
        return 1;
    } else {
        return 0;
    }
}

int CompressEtc2RGBA(uint8_t *src, size_t size, int mipmap, float fEffort, int jobs, int header,
                     uint8_t **dst, size_t *filesize) {
    Ktx ktx{src, size, mipmap == 1, Etc::Image::Format::RGBA8, fEffort, jobs, header};
    bool result = ktx.Write(dst, filesize);
    if (result) {
        printf("CompressEtc2RGBA time = %dms\n", ktx.encodingTime);
        return 1;
    } else {
        return 0;
    }
}


int CompressEtc1WithFile(const char *input, const char *output,
                         int mipmap, float fEffort, int jobs) {
    Ktx ktx{input, mipmap == 1, Etc::Image::Format::ETC1, fEffort, jobs};
    bool result = ktx.WriteToFile(output);
    if (result) {
        printf("CompressEtc1WithFile encode time = %dms\n", ktx.encodingTime);
        return 1;
    } else {
        return 0;
    }
}

int CompressEtc2RGBWithFile(const char *input, const char *output,
                            int mipmap, float fEffort, int jobs) {
    Ktx ktx{input, mipmap == 1, Etc::Image::Format::RGB8, fEffort, jobs};
    bool result = ktx.WriteToFile(output);
    if (result) {
        printf("CompressEtc2RGBWithFile encode time = %dms\n", ktx.encodingTime);
        return 1;
    } else {
        return 0;
    }
}

int CompressEtc2RGBAWithFile(const char *input, const char *output,
                             int mipmap, float fEffort, int jobs) {
    Ktx ktx{input, mipmap == 1, Etc::Image::Format::RGBA8, fEffort, jobs};
    bool result = ktx.WriteToFile(output);
    if (result) {
        printf("CompressEtc2RGBAWithFile encode time = %dms\n", ktx.encodingTime);
        return 1;
    } else {
        return 0;
    }
}

int CompressAstc(uint8_t *src, size_t size, float fEffort,
                 unsigned int block_x, unsigned int block_y, unsigned int block_z, int header,
                 uint8_t **dst, size_t *filesize) {
    Astc astc{src, size, fEffort, block_x, block_y, block_z, header};
    int state = astc.Read();
    if (state) {
        bool result = astc.Write(dst, filesize);
        astc.Clear();
        if (result) {
            printf("CompressAstc encode time = %dms\n", astc.encodingTime);
            return 1;
        } else {
            return 0;
        }
    } else {
        astc.Clear();
        return 0;
    }
    return 1;
}

int CompressAstcWithFile(const char *input, const char *output,
                         float fEffort, unsigned int block_x, unsigned int block_y, unsigned int block_z) {
    Astc astc{input, fEffort, block_x, block_y, block_z};
    int state = astc.Read();
    if (state) {
        bool result = astc.WriteToFile(output);
        astc.Clear();
        if (result) {
            printf("CompressAstcWithFile encode time = %dms\n", astc.encodingTime);
            return 1;
        } else {
            return 0;
        }
    } else {
        astc.Clear();
        return 0;
    }
}

int decode(uint8_t *src, long w, long h, uint32_t **dst, size_t *filesize,
           int (func)(const uint8_t *, const long, const long, uint32_t *)) {
    uint32_t *image = (uint32_t *) malloc(w * h * 4);
    int error = func(src, w, h, image);
    if (error != 1) {
        return error;
    }
    *dst = image;
    *filesize = w * h * 4;
    return 1;
}

int DecompressEtc1(uint8_t *src, long w, long h, uint32_t **dst, size_t *filesize) {
    return decode(src, w, h, dst, filesize, decode_etc1);
}

int DecompressEtc2(uint8_t *src, long w, long h, uint32_t **dst, size_t *filesize) {
    return decode(src, w, h, dst, filesize, decode_etc2);
}

int DecompressEtc2a1(uint8_t *src, long w, long h, uint32_t **dst, size_t *filesize) {
    return decode(src, w, h, dst, filesize, decode_etc2a1);
}

int DecompressEtc2a8(uint8_t *src, long w, long h, uint32_t **dst, size_t *filesize) {
    return decode(src, w, h, dst, filesize, decode_etc2a8);
}

int DecompressAstc(uint8_t *src, long w, long h, long block_width, long block_height, uint32_t **dst,
                   size_t *filesize) {
    uint32_t *image = (uint32_t *) malloc(w * h * 4);
    int error = decode_astc(src, w, h, block_width, block_height, image);
    if (error != 1) {
        return error;
    }
    *dst = image;
    *filesize = w * h * 4;
    return 1;
}

int decodeToFile(uint8_t *src, long w, long h, const char *out,
                 int (func)(const uint8_t *, const long, const long, uint32_t *)) {
    uint32_t *image = (uint32_t *) malloc(w * h * 4);
    int error = func(src, w, h, image);
    if (error != 1) {
        return error;
    }
    lodepng_encode_file(out, reinterpret_cast<const unsigned char *>(image), w, h, LCT_RGBA, 8);
    delete[] image;
    return 1;
}


int DecompressEtc1ToFile(uint8_t *src, long w, long h, const char *out) {
    return decodeToFile(src, w, h, out, decode_etc1);
}

int DecompressEtc2ToFile(uint8_t *src, long w, long h, const char *out) {
    return decodeToFile(src, w, h, out, decode_etc2);
}

int DecompressEtc2a1ToFile(uint8_t *src, long w, long h, const char *out) {
    return decodeToFile(src, w, h, out, decode_etc2a1);
}

int DecompressEtc2a8ToFile(uint8_t *src, long w, long h, const char *out) {
    return decodeToFile(src, w, h, out, decode_etc2a8);
}

int DecompressAstcToFile(uint8_t *src, long w, long h, long block_width, long block_height, const char *out) {
    uint32_t *image = (uint32_t *) malloc(w * h * 4);
    int error = decode_astc(src, w, h, block_width, block_height, image);
    if (error != 1) {
        return error;
    }
    lodepng_encode_file(out, reinterpret_cast<const unsigned char *>(image), w, h, LCT_RGBA, 8);
    delete[] image;
    return 1;
}

