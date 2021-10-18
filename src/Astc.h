//
// Created by smalls on 2021/8/14.
//


#include <cstdint>
#include "SourceImage.h"

class Astc {

public:

    Astc(const char *in, float quality, unsigned int block_x, unsigned int block_y, unsigned int block_z);

    Astc(uint8_t *file, size_t filesize, float quality, unsigned int block_x, unsigned int block_y,
         unsigned int block_z,
         int header);

    int Read();

    void Clear();

    bool Write(uint8_t **out, size_t *size) const;

    bool WriteToFile(const char *out);

    int encodingTime = 0;

private:

    int writeHeader = 0;

    uint8_t *m_file = nullptr;
    size_t m_filesize = 0;
    const char *m_in = nullptr;

    float m_quality;
    unsigned int m_block_x;
    unsigned int m_block_y;
    unsigned int m_block_z;

    astcenc_image *image_uncomp_in = nullptr;
    astcenc_context *codec_context{};
    astc_compressed_image image_comp{};

};


