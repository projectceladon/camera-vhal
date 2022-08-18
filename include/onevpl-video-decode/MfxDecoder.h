/**
 *
 * Copyright (c) 2022 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MFX_DECODER_H
#define MFX_DECODER_H

#include "onevpl-video-decode/MfxFrameConstructor.h"
#include <mfxvideo++.h>
#include <mfxdispatcher.h>
#include <va/va.h>
#include <va/va_android.h>
#include <va/va_backend.h>
#include <android/hardware/graphics/mapper/2.0/IMapper.h>
#include <list>
#include <mutex>

using ::android::hardware::graphics::mapper::V2_0::YCbCrLayout;

#define MIN_NUMBER_OF_REQUIRED_FRAME_SURFACE 4 // Required for smooth camera previcw.
#define MFX_TIMEOUT_INFINITE 0xEFFFFFFF
#define ONEVPL_ALIGN32(value) (((value + 31) >> 5) << 5) // round up to a multiple of 32

enum MemType {
    VIDEO_MEMORY  = 1,
    SYSTEM_MEMORY = 2,
};

enum DecoderType {
    DECODER_H264 = 1,
    DECODER_H265 = 2,
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MfxDecoder
{
public:
    MfxDecoder();
    ~MfxDecoder();

    mfxStatus Init(uint32_t codec_type, uint32_t width, uint32_t height);
    void Release();
    mfxStatus DecodeFrame(uint8_t *pData, size_t size);
    bool GetOutput(YCbCrLayout &out);

private:
    mfxStatus ResetSettings(uint32_t codec_type);
    void ClearFrameSurface();
    mfxStatus InitDecoder(mfxBitstream **bit_stream);
    mfxStatus PrepareSurfaces();
    void GetAvailableSurface(mfxFrameSurface1 **pWorkSurface);
    uint32_t GetAvailableSurfaceIndex();

private:
    mfxLoader mMfxLoader;
    mfxIMPL mDecImplementation;
    mfxSession mMfxDecSession;
    mfxVideoParam mMfxVideoDecParams;

    MemType mDecodeMemType;
    mfxFrameSurface1 *mOutFrameSurface;
    std::list<mfxFrameSurface1*> mOutFrameSurfList;
    uint32_t mOutSurfaceNum;

    uint32_t mResWidth;
    uint32_t mResHeight;

    bool mIsDecoderInitialized;

    std::mutex mMemMutex;

    std::unique_ptr<MfxFrameConstructor> mMfxFrameConstructor;
};
#endif /* MFX_DECODER_H */
