//
// Created by smalls on 2021/8/14.
//

#include "KtxFileHeader.h"


namespace Etc {

    KtxFileHeader::KtxFileHeader(KtxFile *a_pfile) {
        m_pfile = a_pfile;

        static const uint8_t s_au8Itentfier[12] =
                {
                        0xAB, 0x4B, 0x54, 0x58, // first four bytes of Byte[12] identifier
                        0x20, 0x31, 0x31, 0xBB, // next four bytes of Byte[12] identifier
                        0x0D, 0x0A, 0x1A, 0x0A  // final four bytes of Byte[12] identifier
                };

        for (unsigned int ui = 0; ui < sizeof(s_au8Itentfier); ui++) {
            m_data.m_au8Identifier[ui] = s_au8Itentfier[ui];
        }

        m_data.m_u32Endianness = 0x04030201;
        m_data.m_u32GlType = 0;
        m_data.m_u32GlTypeSize = 1;
        m_data.m_u32GlFormat = 0;

        switch (m_pfile->GetImageFormat()) {
            case Image::Format::RGB8:
            case Image::Format::SRGB8:
                m_data.m_u32GlInternalFormat = (unsigned int) InternalFormat::ETC2_RGB8;
                m_data.m_u32GlBaseInternalFormat = (unsigned int) BaseInternalFormat::ETC2_RGB8;
                break;

            case Image::Format::RGBA8:
            case Image::Format::SRGBA8:
                m_data.m_u32GlInternalFormat = (unsigned int) InternalFormat::ETC2_RGBA8;
                m_data.m_u32GlBaseInternalFormat = (unsigned int) BaseInternalFormat::ETC2_RGBA8;
                break;

            case Image::Format::RGB8A1:
            case Image::Format::SRGB8A1:
                m_data.m_u32GlInternalFormat = (unsigned int) InternalFormat::ETC2_RGB8A1;
                m_data.m_u32GlBaseInternalFormat = (unsigned int) BaseInternalFormat::ETC2_RGB8A1;
                break;

            case Image::Format::R11:
                m_data.m_u32GlInternalFormat = (unsigned int) InternalFormat::ETC2_R11;
                m_data.m_u32GlBaseInternalFormat = (unsigned int) BaseInternalFormat::ETC2_R11;
                break;

            case Image::Format::SIGNED_R11:
                m_data.m_u32GlInternalFormat = (unsigned int) InternalFormat::ETC2_SIGNED_R11;
                m_data.m_u32GlBaseInternalFormat = (unsigned int) BaseInternalFormat::ETC2_R11;
                break;

            case Image::Format::RG11:
                m_data.m_u32GlInternalFormat = (unsigned int) InternalFormat::ETC2_RG11;
                m_data.m_u32GlBaseInternalFormat = (unsigned int) BaseInternalFormat::ETC2_RG11;
                break;

            case Image::Format::SIGNED_RG11:
                m_data.m_u32GlInternalFormat = (unsigned int) InternalFormat::ETC2_SIGNED_RG11;
                m_data.m_u32GlBaseInternalFormat = (unsigned int) BaseInternalFormat::ETC2_RG11;
                break;

            default:
                m_data.m_u32GlInternalFormat = (unsigned int) InternalFormat::ETC1_RGB8;
                m_data.m_u32GlBaseInternalFormat = (unsigned int) BaseInternalFormat::ETC1_RGB8;
                break;
        }

        m_data.m_u32PixelWidth = 0;
        m_data.m_u32PixelHeight = 0;
        m_data.m_u32PixelDepth = 0;
        m_data.m_u32NumberOfArrayElements = 0;
        m_data.m_u32NumberOfFaces = 0;
        m_data.m_u32BytesOfKeyValueData = 0;

        m_pkeyvaluepair = nullptr;

        m_u32Images = 0;
        m_u32KeyValuePairs = 0;

        m_data.m_u32PixelWidth = m_pfile->GetSourceWidth();
        m_data.m_u32PixelHeight = m_pfile->GetSourceHeight();
        m_data.m_u32PixelDepth = 0;
        m_data.m_u32NumberOfArrayElements = 0;
        m_data.m_u32NumberOfFaces = 1;
        m_data.m_u32NumberOfMipmapLevels = m_pfile->GetNumMipmaps();
    }

    KtxFileHeader::~KtxFileHeader() {

    }

    KtxFileHeader::Data KtxFileHeader::GetData() {
        return m_data;
    }

    void KtxFileHeader::Write(FILE *a_pfile) {
        size_t szBytesWritten;

        // Write header
        szBytesWritten = fwrite(&m_data, 1, sizeof(Data), a_pfile);
        assert(szBytesWritten == sizeof(Data));

        // Write KeyAndValuePairs
        if (m_u32KeyValuePairs) {
            fwrite(m_pkeyvaluepair, m_pkeyvaluepair->u32KeyAndValueByteSize, 1, a_pfile);
        }
    }

    void KtxFileHeader::AddKeyAndValue(KtxFileHeader::KeyValuePair *a_pkeyvaluepair) {

    }

    KtxFileHeader::KeyValuePair *KtxFileHeader::pkeyvaluepair() {
        return m_pkeyvaluepair;
    }
} // namespace Sm

