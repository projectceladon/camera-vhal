/*
 * Copyright (C) 2023 Intel Corporation
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

#include <string>
#include <list>
#include <thread>
#include <chrono>
#include "CameraFixture.h"
#include "CameraSocketCommand.h"

using namespace std;
using namespace android;
using namespace std::chrono_literals;

void CameraFixture::runStreamer(){
    mCameraClient.startDummyStreamer();
}

CameraFixture::CameraFixture()
{
    SetCallback();
}

void CameraFixture::SetUp()
{
    t1 = new thread(&CameraFixture::runStreamer, this);
    this_thread::sleep_for(1500ms);
}

void CameraFixture::TearDown()
{
    mCameraClient.stopDummyStreamer();
    t1->join();
    delete t1;
    t1 = nullptr;
}

CameraFixture::~CameraFixture()
{
    DestroyCallbacks();
}

TEST_F(CameraFixture, ClientWithOneCamera)
{
    ASSERT_EQ(NO_CAMERA_PRESENT, gVirtualCameraFactory.get_number_of_cameras());
    mCameraClient.RequestCameraCapability();
    mCameraClient.sendOneCameraConfig();
    this_thread::sleep_for(500ms); 
    ASSERT_EQ(ONE_CAMERA_CLIENT, gVirtualCameraFactory.get_number_of_cameras());
    gVirtualCameraFactory.clearCameraInfo(client_id);
}

TEST_F(CameraFixture, ClientWithTwoCamera)
{
    mCameraClient.RequestCameraCapability();
    mCameraClient.sendTwoCameraConfig();
    this_thread::sleep_for(500ms); 
    ASSERT_EQ(TWO_CAMERA_CLIENT, gVirtualCameraFactory.get_number_of_cameras());
    gVirtualCameraFactory.clearCameraInfo(client_id);
}

TEST_F(CameraFixture, ClientWithMultiCamera)
{
    mCameraClient.RequestCameraCapability();
    mCameraClient.sendMultipleCameraConfig();
    this_thread::sleep_for(500ms); 
    ASSERT_EQ(MULTI_CAMERA_CLIENT, gVirtualCameraFactory.get_number_of_cameras());
    gVirtualCameraFactory.clearCameraInfo(client_id);
}

TEST_F(CameraFixture, CheckForCodecType)
{
    ASSERT_EQ(VALID_TYPE, mCapabilitiesHelper.IsCodecTypeValid((uint32_t)android::socket::VideoCodecType::kH264));
    ASSERT_EQ(VALID_TYPE, mCapabilitiesHelper.IsCodecTypeValid((uint32_t)android::socket::VideoCodecType::kH265));
    ASSERT_EQ(INVALID_TYPE, mCapabilitiesHelper.IsCodecTypeValid((uint32_t)INVALID_WAV));
}

TEST_F(CameraFixture, CheckForFacing)
{
    ASSERT_EQ(VALID_TYPE, mCapabilitiesHelper.IsCameraFacingValid((uint32_t)android::socket::CameraFacing::BACK_FACING));
    ASSERT_EQ(VALID_TYPE, mCapabilitiesHelper.IsCameraFacingValid((uint32_t)android::socket::CameraFacing::FRONT_FACING));
    ASSERT_EQ(INVALID_TYPE, mCapabilitiesHelper.IsCameraFacingValid((uint32_t)FRONT_FACING_SECOND));
}

TEST_F(CameraFixture, CheckForOrientation)
{
    ASSERT_EQ(VALID_TYPE, mCapabilitiesHelper.IsSensorOrientationValid((uint32_t)android::socket::SensorOrientation::ORIENTATION_90));
    ASSERT_EQ(VALID_TYPE, mCapabilitiesHelper.IsSensorOrientationValid((uint32_t)android::socket::SensorOrientation::ORIENTATION_270));
    ASSERT_EQ(INVALID_TYPE, mCapabilitiesHelper.IsSensorOrientationValid((uint32_t)INVALID_ORIENTATION_360));
}

TEST_F(CameraFixture, CheckForResolution)
{
    ASSERT_EQ(VALID_TYPE, mCapabilitiesHelper.IsResolutionValid((uint32_t)android::socket::FrameResolution::k720p));
    ASSERT_EQ(VALID_TYPE, mCapabilitiesHelper.IsResolutionValid((uint32_t)android::socket::FrameResolution::k1080p));
    ASSERT_EQ(INVALID_TYPE, mCapabilitiesHelper.IsResolutionValid((uint32_t)INVALID_k2160p));
}

TEST_F(CameraFixture, MultipleCameraCapReq)
{
    mCameraClient.RequestCameraCapability();
    mCameraClient.RequestCameraCapability();
    mCameraClient.RequestCameraCapability();
    mCameraClient.sendMultipleCameraConfig();
    this_thread::sleep_for(500ms);
    ASSERT_EQ(MULTI_CAMERA_CLIENT, gVirtualCameraFactory.get_number_of_cameras());
    gVirtualCameraFactory.clearCameraInfo(client_id);
} 

