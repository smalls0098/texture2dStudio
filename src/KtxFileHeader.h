#pragma once

#include <stdio.h>
#include <inttypes.h>
#include "KtxFile.h"

namespace Etc {

    class KtxFileHeader {

    public:

        KtxFileHeader(KtxFile *a_pfile);

        ~KtxFileHeader();

        typedef struct {
            uint32_t u32KeyAndValueByteSize;
        } KeyValuePair;

        typedef struct {
            uint8_t m_au8Identifier[12];
            uint32_t m_u32Endianness;
            uint32_t m_u32GlType;
            uint32_t m_u32GlTypeSize;
            uint32_t m_u32GlFormat;
            uint32_t m_u32GlInternalFormat;
            uint32_t m_u32GlBaseInternalFormat;
            uint32_t m_u32PixelWidth;
            uint32_t m_u32PixelHeight;
            uint32_t m_u32PixelDepth;
            uint32_t m_u32NumberOfArrayElements;
            uint32_t m_u32NumberOfFaces;
            uint32_t m_u32NumberOfMipmapLevels;
            uint32_t m_u32BytesOfKeyValueData;
        } Data;

        enum class InternalFormat {
            ETC1_RGB8 = 0x8D64,
            ETC1_ALPHA8 = ETC1_RGB8,
            //
            ETC2_R11 = 0x9270,
            ETC2_SIGNED_R11 = 0x9271,
            ETC2_RG11 = 0x9272,
            ETC2_SIGNED_RG11 = 0x9273,
            ETC2_RGB8 = 0x9274,
            ETC2_SRGB8 = 0x9275,
            ETC2_RGB8A1 = 0x9276,
            ETC2_SRGB8_PUNCHTHROUGH_ALPHA1 = 0x9277,
            ETC2_RGBA8 = 0x9278
        };

        enum class BaseInternalFormat {
            ETC2_R11 = 0x1903,
            ETC2_RG11 = 0x8227,
            ETC1_RGB8 = 0x1907,
            ETC1_ALPHA8 = ETC1_RGB8,
            //
            ETC2_RGB8 = 0x1907,
            ETC2_RGB8A1 = 0x1908,
            ETC2_RGBA8 = 0x1908,
        };

        virtual void Write(FILE *a_pfile);

        void AddKeyAndValue(KeyValuePair *a_pkeyvaluepair);

        Data GetData();

        KeyValuePair *pkeyvaluepair();

    private:

        KtxFile *m_pfile;

        Data m_data{};
        KeyValuePair *m_pkeyvaluepair;

        uint32_t m_u32Images;
        uint32_t m_u32KeyValuePairs;
    };

} // namespace Sm
