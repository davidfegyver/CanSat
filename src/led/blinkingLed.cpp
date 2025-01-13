#include "blinkingLed.h"

BlinkingLed::BlinkingLed(Logger &i_logger, uint8_t i_pin, uint8_t i_on_state, uint32_t i_off_time = 500, uint32_t i_on_time = 500) : Led(i_logger, i_pin, i_on_state), offTime(i_off_time), onTime(i_on_time)
{
}

void BlinkingLed::setTimes(uint32_t i_offTime, uint32_t i_onTime)
{
  offTime = i_offTime;
  onTime = i_onTime;
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
  logger.AddEvent(PSTR("BlinkingLed task. core: ") + String(xPortGetCoreID()));

  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1)
  {
    toggle();
    esp_task_wdt_reset();
    vTaskDelayUntil(&xLastWakeTime, getTimer() / portTICK_PERIOD_MS);
  }
}
