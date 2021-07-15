/*
 * Copyright (C) 2011 The Android Open Source Project
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

#ifndef HW_EMULATOR_CAMERA_VIRTUALD_CAMERA_FACTORY_H
#define HW_EMULATOR_CAMERA_VIRTUALD_CAMERA_FACTORY_H

#include "VirtualBaseCamera.h"

#include <atomic>
#include <cutils/properties.h>

#include <utils/RefBase.h>
#include <vector>
#include <memory>
#include "CameraSocketServerThread.h"
#include "CGCodec.h"

namespace android {

class CameraSocketServerThread;

/*
 * Contains declaration of a class VirtualCameraFactory that manages cameras
 * available for the emulation. A global instance of this class is statically
 * instantiated and initialized when camera emulation HAL is loaded.
 */

/*
 * Class VirtualCameraFactoryManages cameras available for the emulation.
 *
 * When the global static instance of this class is created on the module load,
 * it enumerates cameras available for the emulation by connecting to the
 * emulator's 'camera' service. For every camera found out there it creates an
 * instance of an appropriate class, and stores it an in array of virtual
 * cameras. In addition to the cameras reported by the emulator, a fake camera
 * emulator is always created, so there is always at least one camera that is
 * available.
 *
 * Instance of this class is also used as the entry point for the camera HAL API,
 * including:
 *  - hw_module_methods_t::open entry point
 *  - camera_module_t::get_number_of_cameras entry point
 *  - camera_module_t::get_camera_info entry point
 *
 */
class VirtualCameraFactory {
public:
    /*
     * Constructs VirtualCameraFactory instance.
     * In this constructor the factory will create and initialize a list of
     * virtual cameras. All errors that occur on this constructor are reported
     * via mConstructedOK data member of this class.
     */
    VirtualCameraFactory();

    /*
     * Destructs VirtualCameraFactory instance.
     */
    ~VirtualCameraFactory();

public:
    /****************************************************************************
     * Camera HAL API handlers.
     ***************************************************************************/

    /*
     * Opens (connects to) a camera device.
     *
     * This method is called in response to hw_module_methods_t::open callback.
     */
    int cameraDeviceOpen(int camera_id, hw_device_t **device);

    /*
     * Gets virtual camera information.
     *
     * This method is called in response to camera_module_t::get_camera_info
     * callback.
     */
    int getCameraInfo(int camera_id, struct camera_info *info);

    /*
     * Sets virtual camera callbacks.
     *
     * This method is called in response to camera_module_t::set_callbacks
     * callback.
     */
    int setCallbacks(const camera_module_callbacks_t *callbacks);

    /*
     * Fill in vendor tags for the module.
     *
     * This method is called in response to camera_module_t::get_vendor_tag_ops
     * callback.
     */
    void getVendorTagOps(vendor_tag_ops_t *ops);

public:
    /****************************************************************************
     * Camera HAL API callbacks.
     ***************************************************************************/

    /*
     * camera_module_t::get_number_of_cameras callback entry point.
     */
    static int get_number_of_cameras(void);

    /*
     * camera_module_t::get_camera_info callback entry point.
     */
    static int get_camera_info(int camera_id, struct camera_info *info);

    /*
     * camera_module_t::set_callbacks callback entry point.
     */
    static int set_callbacks(const camera_module_callbacks_t *callbacks);

    /*
     * camera_module_t::get_vendor_tag_ops callback entry point.
     */
    static void get_vendor_tag_ops(vendor_tag_ops_t *ops);

    /*
     * camera_module_t::open_legacy callback entry point.
     */
    static int open_legacy(const struct hw_module_t *module, const char *id, uint32_t halVersion,
                           struct hw_device_t **device);

private:
    /*
     * hw_module_methods_t::open callback entry point.
     */
    static int device_open(const hw_module_t *module, const char *name, hw_device_t **device);

public:
    /****************************************************************************
     * Public API.
     ***************************************************************************/

    /*
     * Gets fake camera orientation.
     */
    int getFakeCameraOrientation() {
        const char *key = "remote.camera.fake.orientation";
        int degree = property_get_int32(key, 90);
        return degree;
    }

    /*
     * Gets number of virtual cameras.
     */
    int getVirtualCameraNum() const { return mVirtualCameraNum; }

    /*
     * Checks whether or not the constructor has succeeded.
     */
    bool isConstructedOK() const { return mConstructedOK; }

private:
    /****************************************************************************
     * Private API
     ***************************************************************************/

    /*
     * Creates a fake camera and adds it to mVirtualCameras. If backCamera is
     * true, it will be created as if it were a camera on the back of the phone.
     * Otherwise, it will be front-facing.
     */
    void createFakeCamera(std::shared_ptr<CameraSocketServerThread> socket_server,
                          std::shared_ptr<CGVideoDecoder> decoder, bool backCamera);
    /*
     * Waits till remote-props has done setup, timeout after 500ms.
     */
    void waitForRemoteSfFakeCameraPropertyAvailable();

    /*
     * Checks if fake camera emulation is on for the camera facing back.
     */
    bool isFakeCameraEmulationOn(bool backCamera);

    /*
     * Gets camera device version number to use for back camera emulation.
     */
    int getCameraHalVersion(bool backCamera);

    void readSystemProperties();

private:
    /****************************************************************************
     * Data members.
     ***************************************************************************/
    std::atomic<socket::CameraSessionState> mCameraSessionState;

    // Array of cameras available for the emulation.
    VirtualBaseCamera **mVirtualCameras;

    // Number of virtual cameras (including the fake ones).
    int mVirtualCameraNum;

    // Number of virtual fake cameras.
    int mFakeCameraNum;

    // Flags whether or not constructor has succeeded.
    bool mConstructedOK;

    // Camera callbacks (for status changing).
    const camera_module_callbacks_t *mCallbacks;

public:
    // Contains device open entry point, as required by HAL API.
    static struct hw_module_methods_t mCameraModuleMethods;

private:
    // NV12 Decoder
    std::shared_ptr<CGVideoDecoder> mDecoder;

    // Socket server
    std::shared_ptr<CameraSocketServerThread> mSocketServer;

    bool createSocketServer(std::shared_ptr<CGVideoDecoder> decoder);
};

};  // end of namespace android

// References the global VirtualCameraFactory instance.
extern android::VirtualCameraFactory gVirtualCameraFactory;

#endif  // HW_EMULATOR_CAMERA_VIRTUALD_CAMERA_FACTORY_H
