//
// Created by smalls on 2021/8/14.
//

#include <astcenc.h>
#include <astcenccli_internal.h>
#include <stb_image.h>
#include <chrono>
#include <cstring>
#include "Astc.h"

Astc::Astc(const char *in,
           float quality, unsigned int block_x, unsigned int block_y, unsigned int block_z) {
    m_in = in;
    m_quality = quality;
    m_block_x = block_x;
    m_block_y = block_y;
    m_block_z = block_z;
}


Astc::Astc(uint8_t *file, size_t filesize,
           float quality, unsigned int block_x, unsigned int block_y, unsigned int block_z, int header) {
    m_file = file;
    m_filesize = filesize;
    m_quality = quality;
    m_block_x = block_x;
    m_block_y = block_y;
    m_block_z = block_z;
    writeHeader = header;
}

void Astc::Clear() {
    free_image(image_uncomp_in);
    astcenc_context_free(codec_context);
}

int Astc::Read() {

    auto start = std::chrono::steady_clock::now();

    astcenc_config config{};
    astcenc_profile profile = ASTCENC_PRF_LDR;

    unsigned int flags = 0;

    flags |= ASTCENC_FLG_SELF_DECOMPRESS_ONLY;

    astcenc_error status = astcenc_config_init(profile, m_block_x, m_block_y, m_block_z,
                                               m_quality, flags, &config);

    if (status != ASTCENC_SUCCESS) {
        printf("ERROR: astcenc_config_init failed\n");
        return 0;
    }

    // Initialize cli_config_options with default values
    cli_config_options cli_config{1, 1, false, false, -10, 10,
                                  {ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A},
                                  {ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A}};

    astcenc_error codec_status;
    codec_status = astcenc_context_alloc(&config, cli_config.thread_count, &codec_context);

    if (codec_status != ASTCENC_SUCCESS) {
        printf("ERROR: Codec context alloc failed: %s\n", astcenc_get_error_string(codec_status));
        return 0;
    }

    int dim_x, dim_y;
    uint8_t *data = nullptr;
    if (m_in == nullptr) {
        if (m_filesize == 0) {
            return 0;
        }
        assert(m_file);
        data = stbi_load_from_memory(m_file, m_filesize, &dim_x, &dim_y, nullptr, STBI_rgb_alpha);
    } else {
        assert(m_in);
        data = stbi_load(m_in, &dim_x, &dim_y, nullptr, STBI_rgb_alpha);
    }

    image_uncomp_in = astc_img_from_unorm8x4_array(data,
                                                   dim_x,
                                                   dim_y,
                                                   false);


    unsigned int blocks_x = (image_uncomp_in->dim_x + config.block_x - 1) / config.block_x;
    unsigned int blocks_y = (image_uncomp_in->dim_y + config.block_y - 1) / config.block_y;
    unsigned int blocks_z = (image_uncomp_in->dim_z + config.block_z - 1) / config.block_z;
    size_t buffer_size = blocks_x * blocks_y * blocks_z * 16;
    uint8_t *buffer = new uint8_t[buffer_size];

    astcenc_error astcenc_error = astcenc_compress_image(
            codec_context, image_uncomp_in, &cli_config.swz_encode,
            buffer, buffer_size, 0);

    auto end = std::chrono::steady_clock::now();
    std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    encodingTime = (int) elapsed.count();

    if (astcenc_error != ASTCENC_SUCCESS) {
        printf("ERROR: Codec compress failed: %s\n", astcenc_get_error_string(astcenc_error));
        return 0;
    }

    image_comp.block_x = config.block_x;
    image_comp.block_y = config.block_y;
    image_comp.block_z = config.block_z;
    image_comp.dim_x = image_uncomp_in->dim_x;
    image_comp.dim_y = image_uncomp_in->dim_y;
    image_comp.dim_z = image_uncomp_in->dim_z;
    image_comp.data = buffer;
    image_comp.data_len = buffer_size;

    return 1;
}

/* ============================================================================
	ASTC compressed file loading
============================================================================ */
struct astc_header {
    uint8_t magic[4];
    uint8_t block_x;
    uint8_t block_y;
    uint8_t block_z;
    uint8_t dim_x[3];            // dims = dim[0] + (dim[1] << 8) + (dim[2] << 16)
    uint8_t dim_y[3];            // Sizes are given in texels;
    uint8_t dim_z[3];            // block count is inferred
};

static const uint32_t ASTC_MAGIC_ID = 0x5CA1AB13;

bool Astc::Write(uint8_t **out, size_t *size) const {
    if (writeHeader) {
        astc_header hdr{};
        hdr.magic[0] = ASTC_MAGIC_ID & 0xFF;
        hdr.magic[1] = (ASTC_MAGIC_ID >> 8) & 0xFF;
        hdr.magic[2] = (ASTC_MAGIC_ID >> 16) & 0xFF;
        hdr.magic[3] = (ASTC_MAGIC_ID >> 24) & 0xFF;

        hdr.block_x = static_cast<uint8_t>(image_comp.block_x);
        hdr.block_y = static_cast<uint8_t>(image_comp.block_y);
        hdr.block_z = static_cast<uint8_t>(image_comp.block_z);

        hdr.dim_x[0] = image_comp.dim_x & 0xFF;
        hdr.dim_x[1] = (image_comp.dim_x >> 8) & 0xFF;
        hdr.dim_x[2] = (image_comp.dim_x >> 16) & 0xFF;

        hdr.dim_y[0] = image_comp.dim_y & 0xFF;
        hdr.dim_y[1] = (image_comp.dim_y >> 8) & 0xFF;
        hdr.dim_y[2] = (image_comp.dim_y >> 16) & 0xFF;

        hdr.dim_z[0] = image_comp.dim_z & 0xFF;
        hdr.dim_z[1] = (image_comp.dim_z >> 8) & 0xFF;
        hdr.dim_z[2] = (image_comp.dim_z >> 16) & 0xFF;

        size_t offset = 0;
        auto *buf = (uint8_t *) malloc(sizeof(astc_header) + image_comp.data_len * sizeof(uint8_t));
        memcpy(&buf[offset], &hdr, sizeof(astc_header));
        offset += sizeof(astc_header);
        memcpy(&buf[offset], image_comp.data, image_comp.data_len);
        offset += image_comp.data_len;
        *out = buf;
        *size = offset;
    } else {
        size_t offset = 0;
        auto *buf = (uint8_t *) malloc(image_comp.data_len * sizeof(uint8_t));
        memcpy(&buf[offset], image_comp.data, image_comp.data_len);
        offset += image_comp.data_len;
        *out = buf;
        *size = offset;
    }
    delete[] image_comp.data;
    return true;
}

bool Astc::WriteToFile(const char *out) {
    int error = store_cimage(image_comp, out);
    delete[] image_comp.data;
    if (error) {
        printf("ERROR: Failed to store compressed image\n");
        return false;
    }
    return true;
}