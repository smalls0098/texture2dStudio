#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS (1)
#endif

#include "KtxFileHeader.h"
#include "KtxFile.h"

#include <Etc.h>

using namespace Etc;


KtxFile::KtxFile(Image::Format a_imageformat,
                 unsigned int a_uiNumMipmaps,
                 RawImage *a_pMipmapImages,
                 unsigned int a_uiSourceWidth,
                 unsigned int a_uiSourceHeight,
                 unsigned int writeHeader) {
    m_imageformat = a_imageformat;

    m_uiNumMipmaps = a_uiNumMipmaps;
    m_pMipmapImages = new RawImage[m_uiNumMipmaps];
    m_pMipmapImagesSize = 0;
    m_writeHeader = writeHeader;

    for (unsigned int mip = 0; mip < m_uiNumMipmaps; mip++) {
        m_pMipmapImages[mip] = a_pMipmapImages[mip];
        m_pMipmapImagesSize += m_pMipmapImages[mip].uiEncodingBitsBytes;
    }

    m_uiSourceWidth = a_uiSourceWidth;
    m_uiSourceHeight = a_uiSourceHeight;

    m_pheader = new KtxFileHeader(this);
}

KtxFile::KtxFile(Image::Format a_imageformat,
                 unsigned char *a_paucEncodingBits,
                 unsigned int a_uiEncodingBitsBytes,
                 unsigned int a_uiSourceWidth,
                 unsigned int a_uiSourceHeight,
                 unsigned int a_uiExtendedWidth,
                 unsigned int a_uiExtendedHeight,
                 unsigned int writeHeader) {

    m_imageformat = a_imageformat;

    m_writeHeader = writeHeader;

    m_pMipmapImagesSize = a_uiEncodingBitsBytes;
    m_uiNumMipmaps = 1;
    m_pMipmapImages = new RawImage[m_uiNumMipmaps];
    m_pMipmapImages[0].paucEncodingBits = std::shared_ptr<unsigned char>(a_paucEncodingBits,
                                                                         [](unsigned char *p) { delete[] p; });
    m_pMipmapImages[0].uiEncodingBitsBytes = a_uiEncodingBitsBytes;
    m_pMipmapImages[0].uiExtendedWidth = a_uiExtendedWidth;
    m_pMipmapImages[0].uiExtendedHeight = a_uiExtendedHeight;

    m_uiSourceWidth = a_uiSourceWidth;
    m_uiSourceHeight = a_uiSourceHeight;

    m_pheader = new KtxFileHeader(this);

}

KtxFile::~KtxFile() {
    if (m_pMipmapImages != nullptr) {
        delete[] m_pMipmapImages;
    }
    if (m_pheader != nullptr) {
        delete m_pheader;
        m_pheader = nullptr;
    }
}

void KtxFile::UseSingleBlock(int a_iPixelX, int a_iPixelY) {
    if (a_iPixelX <= -1 || a_iPixelY <= -1)
        return;
    if (a_iPixelX > (int) m_uiSourceWidth) {
        //if we are using a ktx thats the size of a single block or less
        //then make sure we use the 4x4 image as the single block
        if (m_uiSourceWidth <= 4) {
            a_iPixelX = 0;
        } else {
            printf("blockAtHV: H coordinate out of range, capped to image width\n");
            a_iPixelX = m_uiSourceWidth - 1;
        }
    }
    if (a_iPixelY > (int) m_uiSourceHeight) {
        //if we are using a ktx thats the size of a single block or less
        //then make sure we use the 4x4 image as the single block
        if (m_uiSourceHeight <= 4) {
            a_iPixelY = 0;
        } else {
            printf("blockAtHV: V coordinate out of range, capped to image height\n");
            a_iPixelY = m_uiSourceHeight - 1;
        }
    }

    unsigned int origWidth = m_uiSourceWidth;
    unsigned int origHeight = m_uiSourceHeight;

    m_uiSourceWidth = 4;
    m_uiSourceHeight = 4;

    Block4x4EncodingBits::Format encodingbitsformat = Image::DetermineEncodingBitsFormat(m_imageformat);
    unsigned int uiEncodingBitsBytesPerBlock = Block4x4EncodingBits::GetBytesPerBlock(encodingbitsformat);

    int numMipmaps = 1;
    auto *pMipmapImages = new RawImage[numMipmaps];
    pMipmapImages[0].uiExtendedWidth = Image::CalcExtendedDimension((unsigned short) m_uiSourceWidth);
    pMipmapImages[0].uiExtendedHeight = Image::CalcExtendedDimension((unsigned short) m_uiSourceHeight);
    pMipmapImages[0].uiEncodingBitsBytes = 0;
    pMipmapImages[0].paucEncodingBits = std::shared_ptr<unsigned char>(
            new unsigned char[uiEncodingBitsBytesPerBlock],
            [](unsigned char *p) { delete[] p; });

    //block position in pixels
    // remove the bottom 2 bits to get the block coordinates
    unsigned int iBlockPosX = (a_iPixelX & 0xFFFFFFFC);
    unsigned int iBlockPosY = (a_iPixelY & 0xFFFFFFFC);

    int numXBlocks = (origWidth / 4);
    int numYBlocks = (origHeight / 4);

    unsigned int num = numXBlocks * numYBlocks;
    unsigned int uiH = 0, uiV = 0;
    unsigned char *pEncodingBits = m_pMipmapImages[0].paucEncodingBits.get();
    for (unsigned int uiBlock = 0; uiBlock < num; uiBlock++) {
        if (uiH == iBlockPosX && uiV == iBlockPosY) {
            memcpy(pMipmapImages[0].paucEncodingBits.get(), pEncodingBits, uiEncodingBitsBytesPerBlock);
            break;
        }
        pEncodingBits += uiEncodingBitsBytesPerBlock;
        uiH += 4;

        if (uiH >= origWidth) {
            uiH = 0;
            uiV += 4;
        }
    }

    delete[] m_pMipmapImages;
    m_pMipmapImages = pMipmapImages;
}

bool KtxFile::Write(uint8_t **out, size_t *size) {
    if (m_writeHeader) {
        auto *buf = (uint8_t *) malloc(m_pMipmapImagesSize * sizeof(uint8_t));
        size_t offset = 0;
        for (unsigned int mip = 0; mip < m_uiNumMipmaps; mip++) {
            uint32_t u32ImageSize = m_pMipmapImages[mip].uiEncodingBitsBytes;
            memcpy(&buf[offset], m_pMipmapImages[mip].paucEncodingBits.get(), u32ImageSize);
            offset += u32ImageSize;
        }
        *out = buf;
        *size = offset;
        return true;
    } else {
        auto header = m_pheader->GetData();
        int headerSize = sizeof(header);
        auto *buf = (uint8_t *) malloc((m_pMipmapImagesSize + headerSize + (m_uiNumMipmaps * 4)) * sizeof(uint8_t));
        memcpy(&buf[0], &header, headerSize);
        size_t offset = headerSize;
        for (unsigned int mip = 0; mip < m_uiNumMipmaps; mip++) {
            uint32_t u32ImageSize = m_pMipmapImages[mip].uiEncodingBitsBytes;
            memcpy(&buf[offset], &u32ImageSize, sizeof(u32ImageSize));
            offset += sizeof(u32ImageSize);
            memcpy(&buf[offset], m_pMipmapImages[mip].paucEncodingBits.get(), u32ImageSize);
            offset += u32ImageSize;
        }
        *out = buf;
        *size = offset;
        return true;
    }

}

bool KtxFile::WriteToFile(char const *fileName) {
    FILE *pfile = fopen(fileName, "wb");
    if (pfile == nullptr) {
        printf("Error: couldn't open Etc file (%s)\n", "img.ktx");
        exit(1);
    }
    m_pheader->Write(pfile);
    for (unsigned int mip = 0; mip < m_uiNumMipmaps; mip++) {
        // Write u32 image size
        uint32_t u32ImageSize = m_pMipmapImages[mip].uiEncodingBitsBytes;
        uint32_t szBytesWritten = fwrite(&u32ImageSize, 1, sizeof(u32ImageSize), pfile);
        assert(szBytesWritten == sizeof(u32ImageSize));
        unsigned int iResult = (int) fwrite(m_pMipmapImages[mip].paucEncodingBits.get(), 1,
                                            m_pMipmapImages[mip].uiEncodingBitsBytes, pfile);
        if (iResult != m_pMipmapImages[mip].uiEncodingBitsBytes) {
            printf("Error: write Etc file failed\n");
            return false;
        }
    }
    fclose(pfile);
    return true;
}