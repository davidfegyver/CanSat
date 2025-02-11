#include "camera.h"

#include "globals.h"

Camera::Camera() {}

void Camera::init()
{
    SystemLog.addEvent(F("Initializing camera library"));

    applyCameraConfig();
    initCameraModule();
}

void Camera::reinitialize()
{
    SystemLog.addEvent(F("Reinitializing camera module"));

    if (esp_camera_deinit() != ESP_OK)
    {
        SystemLog.addEvent(F("Error while deinitializing camera module"));
    }

    delay(100);
    applyCameraConfig();
    initCameraModule();

    SystemLog.addEvent(F("Camera module reinitialized"));
}

void Camera::applyCameraConfig()
{
    SystemLog.addEvent(F("Applying camera configuration"));

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
#ifdef BOARD_HAS_PSRAM
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.fb_count = 2;
#else
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.fb_count = 1;
#endif

    config.frame_size = TFrameSize;
    config.jpeg_quality = PhotoQuality;
}

void Camera::initCameraModule()
{
    SystemLog.addEvent(F("Initializing camera module"));

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        SystemLog.addEvent(PSTR("Camera initialization failed. Error: ") + String(err, HEX));
        SystemLog.addEvent(F("Resetting ESP32-CAM!"));
        ESP.restart();
    }

    sensor = esp_camera_sensor_get();
    fetchCameraModuleInfo();
}

void Camera::fetchCameraModuleInfo()
{
    if (!sensor)
    {
        SystemLog.addEvent(F("Camera sensor is NULL"));
        return;
    }

    auto *info = esp_camera_sensor_get_info(&sensor->id);
    if (!info)
    {
        SystemLog.addEvent(F("Camera sensor info is NULL"));
        return;
    }

    CameraType = static_cast<camera_pid_t>(sensor->id.PID);
    CameraName = info->name;

    SystemLog.addEvent(PSTR("Camera type: ") + String(CameraType));
    SystemLog.addEvent(PSTR("Camera name: ") + String(CameraName));
    SystemLog.addEvent(PSTR("Camera model: ") + String(info->model));
    SystemLog.addEvent(PSTR("Camera PID: ") + String(info->pid));
    SystemLog.addEvent(PSTR("Camera MAX framesize: ") + String(info->max_size));
    SystemLog.addEvent(PSTR("Camera supports JPEG: ") + String(info->support_jpeg));
}

void Camera::capturePhoto()
{
    CameraCaptureSuccess = false;

    if (!xSemaphoreTake(frameBufferSemaphore, portMAX_DELAY))
    {
        SystemLog.addEvent(F("Failed to take frame buffer semaphore"));
        return;
    }

    if (FrameBuffer)
    {
        esp_camera_fb_return(FrameBuffer);
        // without this, a previous photo is returned
        FrameBuffer = esp_camera_fb_get();
        esp_camera_fb_return(FrameBuffer);
    }

    int attempts = 0;
    const int maxAttempts = 5;

    do
    {
        SystemLog.addEvent(F("Taking photo..."));

        FrameBuffer = esp_camera_fb_get();
        if (!FrameBuffer)
        {
            CameraFailCounter++;
            SystemLog.addEvent(PSTR("Camera capture failed! Attempt: ") + String(CameraFailCounter));
            xSemaphoreGive(frameBufferSemaphore);
            return;
        }

        if (FrameBuffer->len > 100)
        {
            CameraFailCounter = 0;
            CameraCaptureSuccess = true;
        }

        attempts++;
        if (attempts >= maxAttempts)
        {
            SystemLog.addEvent(F("Failed to capture a valid photo after max attempts"));
            reinitialize();
            break;
        }
    } while (!CameraCaptureSuccess);

    xSemaphoreGive(frameBufferSemaphore);
}

void Camera::takePhotoToSdCard()
{
    capturePhoto();

    if (isCaptureSuccessful() && sd_card.getCardHealthy())
    {
        String FileName = String(PHOTO_FOLDER) + String("/") + String(PHOTO_PREFIX) + String(++photoCount) + String(PHOTO_SUFFIX);

        if (sd_card.WritePicture(FileName, FrameBuffer->buf, FrameBuffer->len))
        {
            SystemLog.addEvent(PSTR("Photo saved to SD card: ") + FileName);
        }
        else
        {
            SystemLog.addEvent(F("Failed to save photo to SD card"));
        }
    }
}

void Camera::timelapseTask(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true)
    {
        esp_task_wdt_reset();

        if (sd_card.getCardHealthy())
        {
            takePhotoToSdCard();
        }
        else
        {
            capturePhoto();
        }

        std::vector<uint8_t> hash = crypto.sha256(FrameBuffer->buf, FrameBuffer->len);

        String hashString = crypto.uint8_vector_to_string(hash);

        SystemLog.addEvent(PSTR("R: ") + hashString);

        vTaskDelayUntil(&xLastWakeTime, TASK_TIMELAPSE / portTICK_PERIOD_MS);
    }
}

void Camera::setPhotoQuality(uint8_t quality)
{
    PhotoQuality = quality;
    reinitialize();
}

void Camera::setFrameSize(uint8_t frameSize)
{
    FrameSize = frameSize;
    TFrameSize = convertToFrameSize(frameSize);
    reinitialize();
}

uint8_t Camera::getPhotoQuality() const { return PhotoQuality; }
uint8_t Camera::getFrameSize() const { return FrameSize; }

uint16_t Camera::getFrameWidth() const
{
    switch (FrameSize)
    {
    case 0:
        return 320;
    case 1:
        return 352;
    case 2:
        return 640;
    case 3:
        return 800;
    case 4:
        return 1024;
    case 5:
        return 1280;
    case 6:
        return 1600;
    default:
        return 320;
    }
}

uint16_t Camera::getFrameHeight() const
{
    switch (FrameSize)
    {
    case 0:
        return 240;
    case 1:
        return 288;
    case 2:
        return 480;
    case 3:
        return 600;
    case 4:
        return 768;
    case 5:
        return 1024;
    case 6:
        return 1200;
    default:
        return 240;
    }
}

void Camera::connectSdCard()
{
    if (sd_card.getCardHealthy())
    {
        sd_card.createFileIfNotExists(PHOTO_FOLDER);
        photoCount = sd_card.countFilesInDir(PHOTO_FOLDER);
        SystemLog.addEvent(PSTR("SD card connected. Photo count: ") + String(photoCount));
    }
}

camera_fb_t *Camera::getPhotoFrameBuffer() const { return FrameBuffer; }

camera_fb_t *Camera::getDuplicatePhotoFrameBuffer() const { return FrameBufferDuplicate; }

bool Camera::isCaptureSuccessful() const { return CameraCaptureSuccess; }

framesize_t Camera::convertToFrameSize(uint8_t data)
{
    switch (data)
    {
    case 0:
        return FRAMESIZE_QVGA;
    case 1:
        return FRAMESIZE_CIF;
    case 2:
        return FRAMESIZE_VGA;
    case 3:
        return FRAMESIZE_SVGA;
    case 4:
        return FRAMESIZE_XGA;
    case 5:
        return FRAMESIZE_SXGA;
    case 6:
        return FRAMESIZE_UXGA;
    default:
        SystemLog.addEvent(F("Invalid frame size input. Defaulting to FRAMESIZE_QVGA."));
        return FRAMESIZE_QVGA;
    }
}

void Camera::returnCapturedFrameBuffer()
{
    esp_camera_fb_return(FrameBuffer);
}
