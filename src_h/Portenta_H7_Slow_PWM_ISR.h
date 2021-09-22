/****************************************************************************************************************************
  Portenta_H7_Slow_ISR.h
  For Portenta_H7 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/Portenta_H7_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one Portenta_H7 STM32 timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.0.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      22/09/2021 Initial coding for Portenta_H7
*****************************************************************************************************************************/

#pragma once

#ifndef PORTENTA_H7_SLOW_PWM_ISR_GENERIC_H
#define PORTENTA_H7_SLOW_PWM_ISR_GENERIC_H

#if ( ( defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4) ) && defined(ARDUINO_ARCH_MBED) )
  #warning Use MBED ARDUINO_PORTENTA_H7 and LittleFS
  
  #if defined(BOARD_NAME)
    #undef BOARD_NAME
  #endif

  #if defined(CORE_CM7)
    #warning Using Portenta H7 M7 core
    #define BOARD_NAME              "PORTENTA_H7_M7"
  #else
    #warning Using Portenta H7 M4 core
    #define BOARD_NAME              "PORTENTA_H7_M4"
  #endif
  
#else
  #error This code is intended to run on the MBED ARDUINO_PORTENTA_H7 platform! Please check your Tools->Board setting. 
#endif

#ifndef PORTENTA_H7_SLOW_PWM_VERSION
  #define PORTENTA_H7_SLOW_PWM_VERSION       "PORTENTA_H7_SLOW_PWM v1.0.0"
#endif

#ifndef _PWM_LOGLEVEL_
  #define _PWM_LOGLEVEL_       1
#endif

#include "PWM_Generic_Debug.h"

#include <stddef.h>

#include <inttypes.h>

#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif
#endif

#define Portenta_H7_Slow_PWM_ISR Portenta_H7_Slow_PWM

typedef void (*timer_callback)();
typedef void (*timer_callback_p)(void *);

#if !defined(USING_MICROS_RESOLUTION)
  #warning Not USING_MICROS_RESOLUTION, using millis resolution
  #define USING_MICROS_RESOLUTION       false
#endif

#define INVALID_PORTENTA_H7_PIN         255


class Portenta_H7_Slow_PWM_ISR 
{

  public:
    // maximum number of PWM channels
#define MAX_NUMBER_CHANNELS        16

    // constructor
    Portenta_H7_Slow_PWM_ISR();

    void init();

    // this function must be called inside loop()
    void run();
    
    //////////////////////////////////////////////////////////////////
    // PWM
    void setPWM(uint32_t pin, uint32_t frequency, uint32_t dutycycle, timer_callback StartCallback = nullptr,
                timer_callback StopCallback = nullptr)
    {
      uint32_t period = 0;
      
      if ( ( frequency != 0 ) && ( frequency <= 1000 ) )
      {
#if USING_MICROS_RESOLUTION
      // period in us
      period = 1000000 / frequency;
#else    
      // period in ms
      period = 1000 / frequency;
#endif
      }
      else
      {       
        PWM_LOGERROR("Error: Invalid frequency, max is 500Hz");
      }
      
      setupPWMChannel(pin, period, dutycycle, (void *) StartCallback, (void *) StopCallback);     
    }

#if USING_MICROS_RESOLUTION
    //period in us
    void setPWM_Period(uint32_t pin, uint32_t period, uint32_t dutycycle, timer_callback StartCallback = nullptr,
                       timer_callback StopCallback = nullptr)
#else    
    // PWM
    //period in ms
    void setPWM_Period(uint32_t pin, uint32_t period, uint32_t dutycycle, timer_callback StartCallback = nullptr,
                       timer_callback StopCallback = nullptr)
#endif    
    {     
      setupPWMChannel(pin, period, dutycycle, (void *) StartCallback, (void *) StopCallback);       
    }    
    
    //////////////////////////////////////////////////////////////////

    // destroy the specified PWM channel
    void deleteChannel(unsigned channelNum);

    // restart the specified PWM channel
    void restartChannel(unsigned channelNum);

    // returns true if the specified PWM channel is enabled
    bool isEnabled(unsigned channelNum);

    // enables the specified PWM channel
    void enable(unsigned channelNum);

    // disables the specified PWM channel
    void disable(unsigned channelNum);

    // enables all PWM channels
    void enableAll();

    // disables all PWM channels
    void disableAll();

    // enables the specified PWM channel if it's currently disabled, and vice-versa
    void toggle(unsigned channelNum);

    // returns the number of used PWM channels
    unsigned getnumChannels();

    // returns the number of available PWM channels
    unsigned getNumAvailablePWMChannels() 
    {
      return MAX_NUMBER_CHANNELS - numChannels;
    };

  private:

    // low level function to initialize and enable a new PWM channel
    // returns the PWM channel number (channelNum) on success or
    // -1 on failure (f == NULL) or no free PWM channels 
    int setupPWMChannel(uint32_t pin, uint32_t period, uint32_t dutycycle, void* cbStartFunc = nullptr, void* cbStopFunc = nullptr);

    // find the first available slot
    int findFirstFreeSlot();

    typedef struct 
    {
      ///////////////////////////////////
      
      
      ///////////////////////////////////
      
      uint64_t      prevTime;           // value returned by the micros() or millis() function in the previous run() call
      uint32_t      period;             // period value, in us / ms
      uint32_t      onTime;             // onTime value, ( period * dutyCycle / 100 ) us  / ms
      
      void*         callbackStart;      // pointer to the callback function when PWM pulse starts (HIGH)
      void*         callbackStop;       // pointer to the callback function when PWM pulse stops (LOW)
      
      ////////////////////////////////////////////////////////////
      
      uint32_t      pin;                // PWM pin
      bool          pinHigh;            // true if PWM pin is HIGH
      ////////////////////////////////////////////////////////////
      
      bool          enabled;            // true if enabled
    } PWM_t;

    volatile PWM_t PWM[MAX_NUMBER_CHANNELS];

    // actual number of PWM channels in use (-1 means uninitialized)
    volatile int numChannels;
};


#include "Portenta_H7_Slow_PWM_ISR.hpp"

#endif    // PORTENTA_H7_SLOW_PWM_ISR_GENERIC_H


