//
// Created by smalls on 2021/8/14.
//


#include <cstdint>
#include "SourceImage.h"


class Ktx {

public:

    Ktx(uint8_t *file, size_t filesize, bool mipmap, Etc::Image::Format format, float fEffort, int jobs, int header);

    Ktx(uint8_t *file, size_t filesize, bool mipmap, Etc::Image::Format format, float fEffort, int jobs);

    Ktx(const char *filepath, bool mipmap, Etc::Image::Format format, float fEffort, int jobs);

    ~Ktx();

    bool Write(uint8_t **out, size_t *size);

    bool WriteToFile(const char *out);

    int encodingTime = 0;

private:

    void read(bool mipmap, Etc::Image::Format format, float fEffort, int jobs);

    Etc::SourceImage *m_sourceImage = nullptr;

    bool m_mipmap = false;

    int m_mipmap_count = 0;

    Etc::RawImage *pMipmapImages = nullptr;
    uint8_t *paucEncodingBits = nullptr;

    uint32_t uiEncodingBitsBytes = 0;
    uint32_t uiExtendedWidth = 0;
    uint32_t uiExtendedHeight = 0;

    int writeHeader = 0;

    bool isOK = false;
};


