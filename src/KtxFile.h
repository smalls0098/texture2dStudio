#pragma once

#include <EtcImage.h>
#include <Etc.h>

namespace Etc {

    class KtxFileHeader;

    class SourceImage;

    class KtxFile {

    public:

        KtxFile(Image::Format a_imageformat,
                unsigned char *a_paucEncodingBits,
                unsigned int a_uiEncodingBitsBytes,
                unsigned int a_uiSourceWidth,
                unsigned int a_uiSourceHeight,
                unsigned int a_uiExtendedWidth,
                unsigned int a_uiExtendedHeight,
                unsigned int writeHeader);

        KtxFile(Image::Format a_imageformat,
                unsigned int a_uiNumMipmaps,
                RawImage *pMipmapImages,
                unsigned int a_uiSourceWidth,
                unsigned int a_uiSourceHeight,
                unsigned int writeHeader);

        ~KtxFile();

        bool Write(uint8_t **out, size_t *size);

        bool WriteToFile(char const *fileName);

        inline unsigned int GetSourceWidth() const {
            return m_uiSourceWidth;
        }

        inline unsigned int GetSourceHeight() const {
            return m_uiSourceHeight;
        }

        inline unsigned int GetExtendedWidth(unsigned int mipmapIndex = 0) {
            if (mipmapIndex < m_uiNumMipmaps) {
                return m_pMipmapImages[mipmapIndex].uiExtendedWidth;
            } else {
                return 0;
            }
        }

        inline unsigned int GetExtendedHeight(unsigned int mipmapIndex = 0) {
            if (mipmapIndex < m_uiNumMipmaps) {
                return m_pMipmapImages[mipmapIndex].uiExtendedHeight;
            } else {
                return 0;
            }
        }

        inline Image::Format GetImageFormat() {
            return m_imageformat;
        }

        inline unsigned int GetEncodingBitsBytes(unsigned int mipmapIndex = 0) {
            if (mipmapIndex < m_uiNumMipmaps) {
                return m_pMipmapImages[mipmapIndex].uiEncodingBitsBytes;
            } else {
                return 0;
            }
        }

        inline unsigned char *GetEncodingBits(unsigned int mipmapIndex = 0) {
            if (mipmapIndex < m_uiNumMipmaps) {
                return m_pMipmapImages[mipmapIndex].paucEncodingBits.get();
            } else {
                return nullptr;
            }
        }

        inline unsigned int GetNumMipmaps() const {
            return m_uiNumMipmaps;
        }

        void UseSingleBlock(int a_iPixelX = -1, int a_iPixelY = -1);

    private:

        int m_writeHeader = 0;

        Image::Format m_imageformat;
        KtxFileHeader *m_pheader;

        unsigned int m_uiNumMipmaps;
        RawImage *m_pMipmapImages;
        size_t m_pMipmapImagesSize = 0;
        unsigned int m_uiSourceWidth;
        unsigned int m_uiSourceHeight;
    };

} // namespace Sm


