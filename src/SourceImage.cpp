#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS (1)
#endif

#include "EtcConfig.h"
#include "SourceImage.h"
#include "Etc.h"

#if USE_STB_IMAGE_LOAD
#include "stb_image.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "lodepng.h"


namespace Etc {

    SourceImage::SourceImage(uint8_t *file, size_t filesize, int a_iPixelX, int a_iPixelY) {
        m_uiWidth = 0;
        m_uiHeight = 0;
        m_dim_z = 0;
        m_pafrgbaPixels = nullptr;
        m_file = file;
        m_filesize = filesize;
        Read(a_iPixelX, a_iPixelY);
    }

    SourceImage::SourceImage(const char *filepath, int a_iPixelX, int a_iPixelY) {
        m_uiWidth = 0;
        m_uiHeight = 0;
        m_dim_z = 0;
        m_pafrgbaPixels = nullptr;
        m_filepath = filepath;
        Read(a_iPixelX, a_iPixelY);
    }

    SourceImage::~SourceImage() {
        if (m_filepath != nullptr) {
            delete[] m_filepath;
        }
    }

    void SourceImage::Read(int a_iPixelX, int a_iPixelY) {

        unsigned char *paucPixels = nullptr;

        int iWidth = 0;
        int iHeight = 0;
        bool bool16BitImage = false;

        //we can load 8 or 16 bit pngs
        int iBitDepth = 16;
        int error = 0;
        if (m_filepath == nullptr) {
            error = lodepng_decode_memory(&paucPixels,
                                          (unsigned int *) &iWidth,
                                          (unsigned int *) &iHeight,
                                          m_file,
                                          m_filesize,
                                          LCT_RGBA,
                                          iBitDepth);
        } else {
            error = lodepng_decode_file(&paucPixels,
                                        (unsigned int *) &iWidth, (unsigned int *) &iHeight,
                                        m_filepath,
                                        LCT_RGBA, iBitDepth);
        }
        bool16BitImage = (iBitDepth == 16) ? true : false;
        if (error) {
            printf("lodePNG error %u: %s\n", error, lodepng_error_text(error));
            assert(0);
            exit(1);
        }

        //the pixel cords for the top left corner of the block
        int iBlockX = 0;
        int iBlockY = 0;
        if (a_iPixelX > -1 && a_iPixelY > -1) {
            // in 1 block mode, we basically will have an img thats 4x4
            m_uiWidth = 4;
            m_uiHeight = 4;

            if (a_iPixelX > iWidth)
                a_iPixelX = iWidth;
            if (a_iPixelY > iHeight)
                a_iPixelY = iHeight;

            // remove the bottom 2 bits to get the block coordinates
            iBlockX = (a_iPixelX & 0xFFFFFFFC);
            iBlockY = (a_iPixelY & 0xFFFFFFFC);
        } else {
            m_uiWidth = iWidth;
            m_uiHeight = iHeight;
        }

        m_pafrgbaPixels = new ColorFloatRGBA[m_uiWidth * m_uiHeight];
        assert(m_pafrgbaPixels);

        int iBytesPerPixel = bool16BitImage ? 8 : 4;
        unsigned char *pucPixel;    // = &paucPixels[(iBlockY * iWidth + iBlockX) * iBytesPerPixel];
        ColorFloatRGBA *pfrgbaPixel = m_pafrgbaPixels;

        // convert pixels from RGBA* to ColorFloatRGBA
        for (unsigned int uiV = iBlockY; uiV < (iBlockY + m_uiHeight); ++uiV) {
            // reset coordinate for each row
            pucPixel = &paucPixels[(uiV * iWidth + iBlockX) * iBytesPerPixel];

            // read each row
            for (unsigned int uiH = iBlockX; uiH < (iBlockX + m_uiWidth); ++uiH) {
                if (bool16BitImage) {
                    unsigned short ushR = (pucPixel[0] << 8) + pucPixel[1];
                    unsigned short ushG = (pucPixel[2] << 8) + pucPixel[3];
                    unsigned short ushB = (pucPixel[4] << 8) + pucPixel[5];
                    unsigned short ushA = (pucPixel[6] << 8) + pucPixel[7];

                    *pfrgbaPixel++ = ColorFloatRGBA((float) ushR / 65535.0f,
                                                    (float) ushG / 65535.0f,
                                                    (float) ushB / 65535.0f,
                                                    (float) ushA / 65535.0f);
                } else {
                    *pfrgbaPixel++ = ColorFloatRGBA::ConvertFromRGBA8(pucPixel[0], pucPixel[1],
                                                                      pucPixel[2], pucPixel[3]);
                }

                pucPixel += iBytesPerPixel;
            }
        }

        free(paucPixels);

    }

    void SourceImage::NormalizeXYZ(void) {
        int iPixels = m_uiWidth * m_uiHeight;

        ColorFloatRGBA *pfrgbaPixel = m_pafrgbaPixels;
        for (int iPixel = 0; iPixel < iPixels; iPixel++) {
            float fX = 2.0f * pfrgbaPixel->fR - 1.0f;
            float fY = 2.0f * pfrgbaPixel->fG - 1.0f;
            float fZ = 2.0f * pfrgbaPixel->fB - 1.0f;

            float fLength2 = fX * fX + fY * fY + fZ * fZ;

            if (fLength2 == 0.0f) {
                pfrgbaPixel->fR = 1.0f;
                pfrgbaPixel->fG = 0.0f;
                pfrgbaPixel->fB = 0.0f;
            } else {
                float fLength = sqrtf(fLength2);

                float fNormalizedX = fX / fLength;
                float fNormalizedY = fY / fLength;
                float fNormalizedZ = fZ / fLength;

                pfrgbaPixel->fR = 0.5f * (fNormalizedX + 1.0f);
                pfrgbaPixel->fG = 0.5f * (fNormalizedY + 1.0f);
                pfrgbaPixel->fB = 0.5f * (fNormalizedZ + 1.0f);
            }

            pfrgbaPixel++;
        }

    }
}