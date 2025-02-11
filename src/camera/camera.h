#pragma once

#include "esp_camera.h"
#include "img_converters.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "micro_sd/micro_sd.h"
#include "logging/logger.h"
#include "cfg.h"
#include <esp_task_wdt.h>
#include "crypto/crypto.h"
#include <vector>

class Camera
{
public:
    Camera();

    void init();
    void reinitialize();

    void setPhotoQuality(uint8_t quality);
    void setFrameSize(uint8_t frameSize);

    uint8_t getPhotoQuality() const;
    uint8_t getFrameSize() const;
    uint16_t getFrameWidth() const;
    uint16_t getFrameHeight() const;

    void capturePhoto();
    bool isCaptureSuccessful() const;
    camera_fb_t *getPhotoFrameBuffer() const;
    camera_fb_t *getDuplicatePhotoFrameBuffer() const;

    void returnCapturedFrameBuffer();

    void takePhotoToSdCard();
    void connectSdCard();

    void timelapseTask(void *pvParameters);

private:
    void initCameraModule();
    void applyCameraConfig();
    void fetchCameraModuleInfo();
    framesize_t convertToFrameSize(uint8_t data);

    uint8_t PhotoQuality = PHOTO_QUALITY;
    uint8_t FrameSize = FRAME_SIZE;
    framesize_t TFrameSize = convertToFrameSize(FrameSize);
    int photoCount = 0;
    int serial = 0;
    SemaphoreHandle_t frameBufferSemaphore = xSemaphoreCreateMutex();
    camera_fb_t *FrameBuffer = nullptr;
    bool CameraCaptureSuccess = false;
    int CameraFailCounter = 0;

    camera_config_t config = {};
    sensor_t *sensor = nullptr;

    String CameraName;
    camera_pid_t CameraType;

    Crypto crypto;
};
