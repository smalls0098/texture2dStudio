//
// Created by smalls on 2021/8/15.
//

#ifndef TEXTURE2DSTUDIO_TEXTURE2D_H
#define TEXTURE2DSTUDIO_TEXTURE2D_H


#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

#define ETCCOMP_MIN_LEVEL (0.0f)
#define ETCCOMP_DEFAULT_LEVEL (40.0f)
#define ETCCOMP_MAX_LEVEL (100.0f)

#define ASTCENC_MIN_LEVEL (0.0f)
#define ASTCENC_DEFAULT_LEVEL (40.0f)
#define ASTCENC_MAX_LEVEL (100.0f)


int
CompressEtc1(uint8_t *src, size_t size, int mipmap, float fEffort, int jobs, int header, uint8_t **dst,
             size_t *filesize);

int
CompressEtc2RGB(uint8_t *src, size_t size, int mipmap, float fEffort, int jobs, int header, uint8_t **dst,
                size_t *filesize);

int
CompressEtc2RGBA(uint8_t *src, size_t size, int mipmap, float fEffort, int jobs, int header, uint8_t **dst,
                 size_t *filesize);


int CompressEtc1WithFile(const char *input, const char *output,
                         int mipmap, float fEffort, int jobs);

int CompressEtc2RGBWithFile(const char *input, const char *output,
                            int mipmap, float fEffort, int jobs);

int CompressEtc2RGBAWithFile(const char *input, const char *output,
                             int mipmap, float fEffort, int jobs);


int
CompressAstc(uint8_t *src, size_t size, float fEffort, unsigned int block_x, unsigned int block_y, unsigned int block_z,
             int header,
             uint8_t **dst, size_t *filesize);

int
CompressAstcWithFile(const char *input, const char *output, float fEffort, unsigned int block_x, unsigned int block_y,
                     unsigned int block_z);


int DecompressEtc1(uint8_t *src, long w, long h, uint32_t **dst, size_t *filesize);

int DecompressEtc2(uint8_t *src, long w, long h, uint32_t **dst, size_t *filesize);

int DecompressEtc2a1(uint8_t *src, long w, long h, uint32_t **dst, size_t *filesize);

int DecompressEtc2a8(uint8_t *src, long w, long h, uint32_t **dst, size_t *filesize);

int
DecompressAstc(uint8_t *src, long w, long h, long block_width, long block_height, uint32_t **dst, size_t *filesize);

int DecompressEtc1ToFile(uint8_t *src, long w, long h, const char *out);

int DecompressEtc2ToFile(uint8_t *src, long w, long h, const char *out);

int DecompressEtc2a1ToFile(uint8_t *src, long w, long h, const char *out);

int DecompressEtc2a8ToFile(uint8_t *src, long w, long h, const char *out);

int
DecompressAstcToFile(uint8_t *src, long w, long h, long block_width, long block_height, const char *out);


#ifdef __cplusplus
}
#endif

#endif //TEXTURE2DSTUDIO_TEXTURE2D_H
