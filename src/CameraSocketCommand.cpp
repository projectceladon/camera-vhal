/**
 * @file CameraSocketCommand.cpp
 * @author Shakthi Prashanth M (shakthi.prashanth.m@intel.com)
 * @brief
 * @version 0.1
 * @date 2021-03-14
 *
 * Copyright (c) 2021 Intel Corporation
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
#include "CameraSocketCommand.h"

namespace android {
namespace socket {
const std::unordered_map<CameraSessionState, std::string> kCameraSessionStateNames = {
    {CameraSessionState::kNone, "None"},
    {CameraSessionState::kCameraOpened, "Camera opened"},
    {CameraSessionState::kDecodingStarted, "Decoding started"},
    {CameraSessionState::kCameraClosed, "Camera closed"},
    {CameraSessionState::kDecodingStopped, "Decoding stopped"},
};
}
}  // namespace android