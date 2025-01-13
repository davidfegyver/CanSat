#pragma once

#include "esp_camera.h"
#include "img_converters.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "micro_sd/micro_sd.h"
#include "logging/logger.h"

#include "cfg.h"

class Camera {
public:
    explicit Camera(Logger &logger);

    void init();
    void reinitialize();

    void setPhotoQuality(uint8_t quality);
    void setFrameSize(uint8_t frameSize);

    uint8_t getPhotoQuality() const;
    uint8_t getFrameSize() const;
    uint16_t getFrameWidth() const;
    uint16_t getFrameHeight() const;

    bool isCaptureSuccessful() const;
    camera_fb_t* getPhotoFrameBuffer() const;
    camera_fb_t* getDuplicatePhotoFrameBuffer() const;
    int getPhotoSize() const;

    void returnCapturedFrameBuffer();

private:
    Logger &logger;

    framesize_t convertToFrameSize(uint8_t data);
    void initCameraModule();
    void applyCameraConfig();
    void fetchCameraModelInfo();

    uint8_t PhotoQuality = PHOTO_QUALITY;
    uint8_t FrameSize = FRAME_SIZE;
    framesize_t TFrameSize = convertToFrameSize(FrameSize);

    camera_fb_t *FrameBuffer = nullptr;
    camera_fb_t *FrameBufferDuplicate = (camera_fb_t *)heap_caps_malloc(sizeof(camera_fb_t), MALLOC_CAP_SPIRAM);
    bool CameraCaptureSuccess = false;

    camera_config_t config = {};
    sensor_t *sensor = nullptr;

    String CameraName;
    camera_pid_t CameraType;
};