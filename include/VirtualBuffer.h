/*
 * Copyright (C) 2019-2022 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HW_EMULATOR_CAMERA_VIRTUALD_CAMERA_FACTORY_H_K
#define HW_EMULATOR_CAMERA_VIRTUALD_CAMERA_FACTORY_H_K

#include <mutex>

#define BPP_NV12 1.5  // 12 bpp

namespace android {

extern bool gIsInFrameI420;
extern bool gIsInFrameH264;

enum class VideoBufferType {
    kI420,
    kARGB,
};

struct Resolution {
    int width = 0;
    int height = 0;
};
/// Video buffer and its information
struct VideoBuffer {
    /// Video buffer
    uint8_t* buffer = NULL;
    /// Resolution for the Video buffer
    Resolution resolution;
    // Buffer type
    VideoBufferType type = VideoBufferType::kARGB;
    VideoBuffer() {}
    ~VideoBuffer() {}
    VideoBuffer(int width, int height)
    : resolution{width, height}{
    }

    // To clear buffer based on current resolution.
    void clearBuffer() {
        std::fill(buffer, buffer + resolution.width * resolution.height, 0x10);
        uint8_t* uv_offset = buffer + resolution.width * resolution.height;
        std::fill(uv_offset, uv_offset + (resolution.width * resolution.height) / 2, 0x80);
        decoded = false;
    }

    bool decoded = false;
};

class ClientVideoBuffer {
public:

    struct VideoBuffer clientBuf;
    unsigned int clientRevCount = 0;
    unsigned int clientUsedCount = 0;

    size_t receivedFrameNo = 0;
    size_t decodedFrameNo = 0;

    ClientVideoBuffer(int width, int height)
    : clientBuf(width, height) {
        clientBuf.buffer = new uint8_t[width * height * BPP_NV12];
        clientRevCount = 0;
        clientUsedCount = 0;
    }

    ~ClientVideoBuffer() {
        delete[] clientBuf.buffer;
    }

    void clearBuffer() {
        clientBuf.clearBuffer();
        clientRevCount = clientUsedCount = 0;
        receivedFrameNo = decodedFrameNo = 0;
    }
};
extern std::mutex client_buf_mutex;
};  // namespace android

#endif  // HW_EMULATOR_CAMERA_VIRTUALD_CAMERA_FACTORY_H_K
