#include "blinkingLed.h"

#include "globals.h"

BlinkingLed::BlinkingLed(uint8_t i_pin, uint8_t i_on_state, uint32_t i_off_time, uint32_t i_on_time)
    : Led(i_pin, i_on_state), offTime(i_off_time), onTime(i_on_time)
{
  SystemLog.addEvent(PSTR("BlinkingLed initialized with pin: ") + String(i_pin) +
                     PSTR(", on state: ") + String(i_on_state) +
                     PSTR(", off time: ") + String(i_off_time) +
                     PSTR(", on time: ") + String(i_on_time));
}

void BlinkingLed::setTimes(uint32_t i_offTime, uint32_t i_onTime)
{
  offTime = i_offTime;
  onTime = i_onTime;
  SystemLog.addEvent(PSTR("BlinkingLed times updated - off time: ") + String(i_offTime) +
                  PSTR(", on time: ") + String(i_onTime));
}

uint32_t BlinkingLed::getOffTime()
{
  return offTime;
}

uint32_t BlinkingLed::getOnTime()
{
  return onTime;
}

uint32_t BlinkingLed::getTimer()
{
  if (get() == onState)
  {
    return onTime;
  }
  else
  {
    return offTime;
  }
}

void BlinkingLed::blinkingTask(void *pvParameters)
{
  SystemLog.addEvent(PSTR("Starting Blinking Led task on core: ") + String(xPortGetCoreID()));

  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (true)
  {
    toggle();
    esp_task_wdt_reset();
    vTaskDelayUntil(&xLastWakeTime, getTimer() / portTICK_PERIOD_MS);
  }
}

void BlinkingLed::createTask()
{
  SystemLog.addEvent(F("Creating LED Blink Task"));

  xTaskCreatePinnedToCore(
      [](void *pvParameters)
      {
        BlinkingLed *flash_led = static_cast<BlinkingLed *>(pvParameters);
        flash_led->blinkingTask(pvParameters);
      },
      "LedBlinkTask",
      2048,
      this,
      1,
      &taskHandle,
      0);

  ESP_ERROR_CHECK(esp_task_wdt_add(taskHandle));
}

void BlinkingLed::suspendTask()
{
  vTaskSuspend(taskHandle);
  turnOff();
  SystemLog.addEvent(F("LED Blink Task suspended"));
}

void BlinkingLed::resumeTask()
{
  vTaskResume(taskHandle);
  SystemLog.addEvent(F("LED Blink Task resumed"));
}
