#include <EtcErrorMetric.h>
#include <Etc.h>
#include <EtcFilter.h>
#include "Ktx.h"
#include "KtxFile.h"

#define MAX_JOBS 1024

using namespace Etc;


Ktx::Ktx(uint8_t *file, size_t filesize, bool mipmap, Etc::Image::Format format, float fEffort, int jobs, int header) {
    int i_hPixel = -1;
    int i_vPixel = -1;
    m_sourceImage = new SourceImage(file, filesize, i_hPixel, i_vPixel);
    writeHeader = header;
    read(mipmap, format, fEffort, jobs);
}

Ktx::Ktx(uint8_t *file, size_t filesize, bool mipmap, Etc::Image::Format format, float fEffort, int jobs) {
    int i_hPixel = -1;
    int i_vPixel = -1;
    m_sourceImage = new SourceImage(file, filesize, i_hPixel, i_vPixel);
    read(mipmap, format, fEffort, jobs);
}

Ktx::Ktx(const char *filepath, bool mipmap, Etc::Image::Format format, float fEffort, int jobs) {
    int i_hPixel = -1;
    int i_vPixel = -1;
    m_sourceImage = new SourceImage(filepath, i_hPixel, i_vPixel);
    read(mipmap, format, fEffort, jobs);
}

void Ktx::read(bool mipmap, Etc::Image::Format format, float fEffort, int jobs) {
    m_mipmap = mipmap;
    unsigned int uiSourceWidth = m_sourceImage->GetWidth();
    unsigned int uiSourceHeight = m_sourceImage->GetHeight();
    m_mipmap_count = 1;
    ErrorMetric e_ErrMetric = ErrorMetric::BT709;
    if (m_mipmap) {
        int dim = (uiSourceWidth < uiSourceHeight) ? uiSourceWidth : uiSourceHeight;
        int maxMips = 0;
        while (dim >= 1) {
            maxMips++;
            dim >>= 1;
        }
        m_mipmap_count = maxMips;
        pMipmapImages = new RawImage[m_mipmap_count];
        EncodeMipmaps((float *) m_sourceImage->GetPixels(),
                      uiSourceWidth,
                      uiSourceHeight,
                      format,
                      e_ErrMetric,
                      fEffort,
                      jobs,
                      MAX_JOBS,
                      m_mipmap_count,
                      FILTER_WRAP_NONE,
                      pMipmapImages,
                      &encodingTime);
    } else {
        Etc::Encode((float *) m_sourceImage->GetPixels(),
                    uiSourceWidth,
                    uiSourceHeight,
                    format,
                    e_ErrMetric,
                    fEffort,
                    jobs,
                    MAX_JOBS,
                    &paucEncodingBits,
                    &uiEncodingBitsBytes,
                    &uiExtendedWidth,
                    &uiExtendedHeight,
                    &encodingTime);
    }
    isOK = true;
}

Ktx::~Ktx() {
    if (m_sourceImage != nullptr) {
        delete[] pMipmapImages;
    }
    if (m_sourceImage != nullptr) {
        delete[] m_sourceImage->GetPixels();
    }
}


bool Ktx::Write(uint8_t **out, size_t *size) {
    assert(m_sourceImage);
    if (!isOK) {
        return false;
    }
    unsigned int uiSourceWidth = m_sourceImage->GetWidth();
    unsigned int uiSourceHeight = m_sourceImage->GetHeight();
    if (m_mipmap) {
        KtxFile file(Etc::Image::Format::RGBA8,
                     m_mipmap_count,
                     pMipmapImages,
                     uiSourceWidth,
                     uiSourceHeight,
                     writeHeader);
        return file.Write(out, size);
    } else {
        assert(paucEncodingBits);
        assert(uiEncodingBitsBytes);
        KtxFile file(
                Etc::Image::Format::RGB8,
                paucEncodingBits,
                uiEncodingBitsBytes,
                uiSourceWidth,
                uiSourceHeight,
                uiExtendedWidth,
                uiExtendedHeight,
                writeHeader
        );
        return file.Write(out, size);
    }
}

bool Ktx::WriteToFile(const char *out) {
    assert(m_sourceImage);
    if (!isOK) {
        return false;
    }
    unsigned int uiSourceWidth = m_sourceImage->GetWidth();
    unsigned int uiSourceHeight = m_sourceImage->GetHeight();
    if (m_mipmap) {
        KtxFile file(Etc::Image::Format::RGBA8,
                     m_mipmap_count,
                     pMipmapImages,
                     uiSourceWidth,
                     uiSourceHeight,
                     writeHeader);
        return file.WriteToFile(out);
    } else {
        assert(paucEncodingBits);
        assert(uiEncodingBitsBytes);
        KtxFile file(
                Etc::Image::Format::RGB8,
                paucEncodingBits,
                uiEncodingBitsBytes,
                uiSourceWidth,
                uiSourceHeight,
                uiExtendedWidth,
                uiExtendedHeight,
                writeHeader
        );
        return file.WriteToFile(out);
    }
}