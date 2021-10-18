#pragma once

#include <EtcColorFloatRGBA.h>

namespace Etc {
    class SourceImage {

    public:

        SourceImage(uint8_t *file,
                    size_t filesize,
                    int a_iPixelX = -1,
                    int a_iPixelY = -1);

        SourceImage(const char *filepath,
                    int a_iPixelX = -1,
                    int a_iPixelY = -1);

        ~SourceImage();

        void NormalizeXYZ();

        inline uint8_t *GetFile() {
            return m_file;
        }

        inline size_t GetFileSize() const {
            return m_filesize;
        }

        inline unsigned int GetWidth() const {
            return m_uiWidth;
        }

        inline unsigned int GetHeight() const {
            return m_uiHeight;
        }

        inline unsigned int GetDIM_Z() const {
            return m_uiWidth;
        }

        inline ColorFloatRGBA *GetPixels() {
            return m_pafrgbaPixels;
        }

        inline ColorFloatRGBA *GetPixel(unsigned int a_uiColumn, unsigned int a_uiRow) {
            if (m_pafrgbaPixels == nullptr) {
                return nullptr;
            }
            return &m_pafrgbaPixels[a_uiRow * m_uiWidth + a_uiColumn];
        }


    private:

        void Read(int a_iPixelX = -1, int a_iPixelY = -1);

        uint8_t *m_file;
        size_t m_filesize;

        const char *m_filepath = nullptr;

        unsigned int m_uiWidth;             // not necessarily block aligned
        unsigned int m_uiHeight;            // not necessarily block aligned
        unsigned int m_dim_z;            // not necessarily block aligned
        ColorFloatRGBA *m_pafrgbaPixels;

    };
} // namespace Sm

