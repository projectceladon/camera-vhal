/*
 * Copyright (C) 2013 The Android Open Source Project
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
 */

#ifndef CAMERA_SOCKET_SERVER_H
#define CAMERA_SOCKET_SERVER_H

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <utils/Mutex.h>
#include <utils/String8.h>
#include <utils/Thread.h>
#include <utils/Vector.h>
#include <string>
#include <memory>
#include <atomic>
#include <array>
#include <chrono>
#include <thread>
#ifdef ENABLE_FFMPEG
#include "CGCodec.h"
#endif
#include "CameraSocketCommand.h"
#include <linux/vm_sockets.h>
#include "VirtualBuffer.h"

namespace android {

enum tranSock
{
    UNIX  = 0,
    TCP   = 1,
    VSOCK = 2,
    PIPE = 3,
};

class VirtualCameraFactory;
class CameraSocketServerThread : public Thread {
public:
#ifdef ENABLE_FFMPEG
    CameraSocketServerThread(std::string suffix, std::shared_ptr<CGVideoDecoder> decoder,
            std::atomic<socket::CameraSessionState> &state);
#else
    CameraSocketServerThread(std::string suffix,
            std::atomic<socket::CameraSessionState> &state);
#endif
    ~CameraSocketServerThread();

    virtual void requestExit();
    virtual status_t requestExitAndWait();
    int getClientFd();
    ssize_t size_update = 0;
    pthread_cond_t mSignalHotplug = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mHotplugLock = PTHREAD_MUTEX_INITIALIZER;

    bool configureCapabilities(bool skipCapRead);
    int UpdateCameraInfo();

private:
    virtual status_t readyToRun();
    virtual bool threadLoop() override;
    bool ProcessCameraDataFromPipe(ClientVideoBuffer *handle);

    void setCameraResolution(uint32_t resolution);
    void setCameraMaxSupportedResolution(int32_t width, int32_t height);

    Mutex mMutex;
    bool mRunning;  // guarding only when it's important
    int mSocketServerFd = -1;
    std::string mSocketPath;
    int mClientFd = -1;
    int mNumOfCamerasRequested;  // Number of cameras requested to support by client.

#ifdef ENABLE_FFMPEG
    std::shared_ptr<CGVideoDecoder> mVideoDecoder;
#endif
    std::atomic<socket::CameraSessionState> &mCameraSessionState;

    // maximum size of a H264 packet in any aggregation packet is 65535 bytes.
    // Source: https://tools.ietf.org/html/rfc6184#page-13
    std::array<uint8_t, 200 * 1024> mSocketBuffer = {};
    size_t mSocketBufferSize = 0;

    struct ValidateClientCapability {
        bool validCodecType = false;
        bool validResolution = false;
        bool validOrientation = false;
        bool validCameraFacing = false;
    };
};
}  // namespace android

#endif
