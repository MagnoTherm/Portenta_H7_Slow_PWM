/****************************************************************************************************************************
  Portenta_H7_Slow_PWM_ISR_Impl.h
  For Portenta_H7 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/Portenta_H7_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one Portenta_H7 STM32 timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.2.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      22/09/2021 Initial coding for Portenta_H7
  1.1.0   K Hoang      10/11/2021 Add functions to modify PWM settings on-the-fly
  1.2.0   K.Hoang      22/01/2022 Fix `multiple-definitions` linker error. Improve accuracy. Optimize code. Fix bug
  1.2.1   K Hoang      04/03/2022 Fix `DutyCycle` and `New Period` display bugs. Display warning only when debug level > 3
*****************************************************************************************************************************/

#pragma once

#ifndef PORTENTA_H7_SLOW_PWM_ISR_GENERIC_IMPL_H
#define PORTENTA_H7_SLOW_PWM_ISR_GENERIC_IMPL_H

#include <string.h>
#include <Arduino_MachineControl.h>
///////////////////////////////////////////////////
// using namespace machineControl;

uint64_t timeNow()
{
#if USING_MICROS_RESOLUTION
  return ((uint64_t)micros());
#else
  return ((uint64_t)millis());
#endif
}

///////////////////////////////////////////////////

Portenta_H7_Slow_PWM_ISR::Portenta_H7_Slow_PWM_ISR()
    : numChannels(-1)
{
}

///////////////////////////////////////////////////

void Portenta_H7_Slow_PWM_ISR::init()
{
  uint64_t currentTime = timeNow();

  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++)
  {
    memset((void *)&PWM[channelNum], 0, sizeof(PWM_t));
    PWM[channelNum].prevTime = currentTime;
    PWM[channelNum].pin = INVALID_PORTENTA_H7_PIN;
  }

  numChannels = 0;
}

///////////////////////////////////////////////////

void Portenta_H7_Slow_PWM_ISR::run()
{
  uint64_t currentTime = timeNow();

  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++)
  {
    // If enabled => check
    // start period / dutyCycle => digitalWrite HIGH
    // end dutyCycle =>  digitalWrite LOW
    if (PWM[channelNum].enabled)
    {
      if ((uint32_t)(currentTime - PWM[channelNum].prevTime) <= PWM[channelNum].onTime)
      {
        if (!PWM[channelNum].pinHigh)
        {
          machinecontrol::digital_outputs.set(PWM[channelNum].pin, HIGH);
          PWM[channelNum].pinHigh = true;

          // callbackStart
          if (PWM[channelNum].callbackStart != nullptr)
          {
            (*(timer_callback)PWM[channelNum].callbackStart)();
          }
        }
      }
      else if ((uint32_t)(currentTime - PWM[channelNum].prevTime) < PWM[channelNum].period)
      {
        if (PWM[channelNum].pinHigh)
        {
          machinecontrol::digital_outputs.set(PWM[channelNum].pin, LOW);
          PWM[channelNum].pinHigh = false;

          // callback when PWM pulse stops (LOW)
          if (PWM[channelNum].callbackStop != nullptr)
          {
            (*(timer_callback)PWM[channelNum].callbackStop)();
          }
        }
      }
      // else
      else if ((uint32_t)(currentTime - PWM[channelNum].prevTime) >= PWM[channelNum].period)
      {
        PWM[channelNum].prevTime = currentTime;

#if CHANGING_PWM_END_OF_CYCLE
        // Only update whenever having newPeriod
        if (PWM[channelNum].newPeriod != 0)
        {
          PWM[channelNum].period = PWM[channelNum].newPeriod;
          PWM[channelNum].newPeriod = 0;

          PWM[channelNum].onTime = PWM[channelNum].newOnTime;
        }
#endif
      }
    }
  }
}

///////////////////////////////////////////////////

// find the first available slot
// return -1 if none found
int Portenta_H7_Slow_PWM_ISR::findFirstFreeSlot()
{
  // all slots are used
  if (numChannels >= MAX_NUMBER_CHANNELS)
  {
    return -1;
  }

  // return the first slot with no callbackStart (i.e. free)
  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++)
  {
    if ((PWM[channelNum].period == 0) && !PWM[channelNum].enabled)
    {
      return channelNum;
    }
  }

  // no free slots found
  return -1;
}

///////////////////////////////////////////////////

int Portenta_H7_Slow_PWM_ISR::setupPWMChannel(const uint32_t &pin, const uint32_t &period, const float &dutycycle, void *cbStartFunc, void *cbStopFunc)
{
  int channelNum;

  // Invalid input, such as period = 0, etc
  if ((period == 0) || (dutycycle < 0.0) || (dutycycle > 100.0))
  {
    PWM_LOGERROR("Error: Invalid period or dutycycle");
    return -1;
  }

  if (numChannels < 0)
  {
    init();
  }

  channelNum = findFirstFreeSlot();

  if (channelNum < 0)
  {
    return -1;
  }

  PWM[channelNum].pin = pin;
  PWM[channelNum].period = period;

  // Must be 0 for new PWM channel
  PWM[channelNum].newPeriod = 0;

  PWM[channelNum].onTime = (period * dutycycle) / 100;

  pinMode(pin, OUTPUT);
  machinecontrol::digital_outputs.set(pin, HIGH);
  PWM[channelNum].pinHigh = true;

  PWM[channelNum].prevTime = timeNow();

  PWM[channelNum].callbackStart = cbStartFunc;
  PWM[channelNum].callbackStop = cbStopFunc;

  PWM_LOGINFO0("Channel : ");
  PWM_LOGINFO0(channelNum);
  PWM_LOGINFO0("\t    Period : ");
  PWM_LOGINFO0(PWM[channelNum].period);
  PWM_LOGINFO0("\t\tOnTime : ");
  PWM_LOGINFO0(PWM[channelNum].onTime);
  PWM_LOGINFO0("\tStart_Time : ");
  PWM_LOGINFOLN0(PWM[channelNum].prevTime);

  numChannels++;

  PWM[channelNum].enabled = true;

  return channelNum;
}

///////////////////////////////////////////////////

bool Portenta_H7_Slow_PWM_ISR::modifyPWMChannel_Period(const uint8_t &channelNum, const uint32_t &pin, const uint32_t &period, const float &dutycycle)
{
  // Invalid input, such as period = 0, etc
  if ((period == 0) || (dutycycle < 0.0) || (dutycycle > 100.0))
  {
    PWM_LOGERROR("Error: Invalid period or dutycycle");
    return false;
  }

  if (channelNum > MAX_NUMBER_CHANNELS)
  {
    PWM_LOGERROR("Error: channelNum > MAX_NUMBER_CHANNELS");
    return false;
  }

  if (PWM[channelNum].pin != pin)
  {
    PWM_LOGERROR("Error: channelNum and pin mismatched");
    return false;
  }

#if CHANGING_PWM_END_OF_CYCLE

  PWM[channelNum].newPeriod = period;
  PWM[channelNum].newDutyCycle = dutycycle;
  PWM[channelNum].newOnTime = (period * dutycycle) / 100;

  PWM_LOGINFO0("Channel : ");
  PWM_LOGINFO0(channelNum);
  PWM_LOGINFO0("\t    Period : ");
  PWM_LOGINFO0(period);
  PWM_LOGINFO0("\t\tOnTime : ");
  PWM_LOGINFO0(PWM[channelNum].newOnTime);
  PWM_LOGINFO0("\tStart_Time : ");
  PWM_LOGINFOLN0(PWM[channelNum].prevTime);

#else

  PWM[channelNum].period = period;

  PWM[channelNum].onTime = (period * dutycycle) / 100;

  machinecontrol::digital_outputs.set(pin, HIGH);
  PWM[channelNum].pinHigh = true;

  PWM[channelNum].prevTime = timeNow();

  PWM_LOGINFO0("Channel : ");
  PWM_LOGINFO0(channelNum);
  PWM_LOGINFO0("\t    Period : ");
  PWM_LOGINFO0(PWM[channelNum].period);
  PWM_LOGINFO0("\t\tOnTime : ");
  PWM_LOGINFO0(PWM[channelNum].onTime);
  PWM_LOGINFO0("\tStart_Time : ");
  PWM_LOGINFOLN0(PWM[channelNum].prevTime);

#endif

  return true;
}

///////////////////////////////////////////////////

void Portenta_H7_Slow_PWM_ISR::deleteChannel(const uint8_t &channelNum)
{
  // nothing to delete if no timers are in use
  if ((channelNum >= MAX_NUMBER_CHANNELS) || (numChannels == 0))
  {
    return;
  }

  // don't decrease the number of timers if the specified slot is already empty (zero period, invalid)
  if ((PWM[channelNum].pin != INVALID_PORTENTA_H7_PIN) && (PWM[channelNum].period != 0))
  {
    memset((void *)&PWM[channelNum], 0, sizeof(PWM_t));

    PWM[channelNum].pin = INVALID_PORTENTA_H7_PIN;

    // update number of timers
    numChannels--;
  }
}

///////////////////////////////////////////////////

// function contributed by code@rowansimms.com
void Portenta_H7_Slow_PWM_ISR::restartChannel(const uint8_t &channelNum)
{
  if (channelNum >= MAX_NUMBER_CHANNELS)
  {
    return;
  }

  PWM[channelNum].prevTime = timeNow();
}

///////////////////////////////////////////////////

bool Portenta_H7_Slow_PWM_ISR::isEnabled(const uint8_t &channelNum)
{
  if (channelNum >= MAX_NUMBER_CHANNELS)
  {
    return false;
  }

  return PWM[channelNum].enabled;
}

///////////////////////////////////////////////////

void Portenta_H7_Slow_PWM_ISR::enable(const uint8_t &channelNum)
{
  if (channelNum >= MAX_NUMBER_CHANNELS)
  {
    return;
  }

  PWM[channelNum].enabled = true;
}

///////////////////////////////////////////////////

void Portenta_H7_Slow_PWM_ISR::disable(const uint8_t &channelNum)
{
  if (channelNum >= MAX_NUMBER_CHANNELS)
  {
    return;
  }

  PWM[channelNum].enabled = false;
}

///////////////////////////////////////////////////

void Portenta_H7_Slow_PWM_ISR::enableAll()
{
  // Enable all timers with a callbackStart assigned (used)

  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++)
  {
    if (PWM[channelNum].period != 0)
    {
      PWM[channelNum].enabled = true;
    }
  }
}

///////////////////////////////////////////////////

void Portenta_H7_Slow_PWM_ISR::disableAll()
{
  // Disable all timers with a callbackStart assigned (used)
  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++)
  {
    if (PWM[channelNum].period != 0)
    {
      PWM[channelNum].enabled = false;
    }
  }
}

///////////////////////////////////////////////////

void Portenta_H7_Slow_PWM_ISR::toggle(const uint8_t &channelNum)
{
  if (channelNum >= MAX_NUMBER_CHANNELS)
  {
    return;
  }

  PWM[channelNum].enabled = !PWM[channelNum].enabled;
}

///////////////////////////////////////////////////

int8_t Portenta_H7_Slow_PWM_ISR::getnumChannels()
{
  return numChannels;
}

#endif // PORTENTA_H7_SLOW_PWM_ISR_GENERIC_IMPL_H
