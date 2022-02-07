# Portenta_H7_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/Portenta_H7_Slow_PWM.svg?)](https://www.ardu-badge.com/Portenta_H7_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/Portenta_H7_Slow_PWM.svg)](https://github.com/khoih-prog/Portenta_H7_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/Portenta_H7_Slow_PWM/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/Portenta_H7_Slow_PWM.svg)](http://github.com/khoih-prog/Portenta_H7_Slow_PWM/issues)

<a href="https://www.buymeacoffee.com/khoihprog6" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

---
---

## Table of Contents

* [Important Change from v1.2.0](#Important-Change-from-v120)
* [Why do we need this Portenta_H7_Slow_PWM library](#why-do-we-need-this-Portenta_H7_Slow_PWM-library)
  * [Features](#features)
  * [Why using ISR-based PWM is better](#why-using-isr-based-pwm-is-better)
  * [Currently supported Boards](#currently-supported-boards)
  * [Important Notes about ISR](#important-notes-about-isr)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [Packages' Patches](#packages-patches)
  * [1. For Portenta_H7 boards using Arduino IDE in Linux](#1-for-portenta_h7-boards-using-arduino-ide-in-linux)
* [More useful Information about STM32 Timers](#more-useful-information-about-stm32-timers)
* [Available Timers for Portenta_H7](#available-timers-for-portenta_h7)
* [Usage](#usage)
  * [1. Using only Hardware Timer directly](#1-using-only-hardware-timer-directly)
    * [1.1 Init Hardware Timer](#11-init-hardware-timer)
    * [1.2 Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions](#12-Set-PWM-Frequency-dutycycle-attach-irqCallbackStartFunc-and-irqCallbackStopFunc-functions)
* [Examples](#examples)
  * [ 1. ISR_16_PWMs_Array](examples/ISR_16_PWMs_Array)
  * [ 2. ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)
  * [ 3. ISR_16_PWMs_Array_Simple](examples/ISR_16_PWMs_Array_Simple)
  * [ 4. ISR_Changing_PWM](examples/ISR_Changing_PWM)
  * [ 5. ISR_Modify_PWM](examples/ISR_Modify_PWM)
  * [ 6. multiFileProject](examples/multiFileProject). **New**
* [Example ISR_16_PWMs_Array_Complex](#Example-ISR_16_PWMs_Array_Complex)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ISR_16_PWMs_Array_Complex on PORTENTA_H7_M7](#1-ISR_16_PWMs_Array_Complex-on-PORTENTA_H7_M7)
  * [2. ISR_16_PWMs_Array on PORTENTA_H7_M7](#2-ISR_16_PWMs_Array-on-PORTENTA_H7_M7)
  * [3. ISR_16_PWMs_Array_Simple on PORTENTA_H7_M7](#3-ISR_16_PWMs_Array_Simple-on-PORTENTA_H7_M7)
  * [4. ISR_Modify_PWM on PORTENTA_H7_M7](#4-ISR_Modify_PWM-on-PORTENTA_H7_M7)
  * [5. ISR_Changing_PWM on PORTENTA_H7_M7](#5-ISR_Changing_PWM-on-PORTENTA_H7_M7)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---


### Important Change from v1.2.0

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)

As more complex calculation and check **inside ISR** are introduced from v1.2.0, there is possibly some crash depending on use-case.

You can modify to use larger `HW_TIMER_INTERVAL_US`, (from current 10uS), according to your board and use-case if crash happens.


```
// Don't change these numbers to make higher Timer freq. System can hang
#define HW_TIMER_INTERVAL_US      10L
```

---


### Why do we need this [Portenta_H7_Slow_PWM library](https://github.com/khoih-prog/Portenta_H7_Slow_PWM)

### Features

This library enables you to use Hardware Timers on an STM32H7-based Portenta_H7 board to create and output PWM to pins. Because this library doesn't use the powerful hardware-controlled PWM with limitations, the maximum PWM frequency is currently limited at **1000Hz**, which is suitable for many real-life applications. Now you can also modify PWM settings on-the-fly.

---

This library enables you to use Interrupt from Hardware Timers on Portenta_H7 boards to create and output PWM to pins. It now supports 16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer. PWM interval can be very long (uint32_t millisecs). The most important feature is they're ISR-based PWM channels. Therefore, their executions are not blocked by bad-behaving functions or tasks. This important feature is absolutely necessary for mission-critical tasks. These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more precise (certainly depending on clock frequency accuracy) than other software PWM using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

As **Hardware Timers are rare, and very precious assets** of any board, this library now enables you to use up to **16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer**. Timers' interval is very long (**ulong millisecs**).

Now with these new **16 ISR-based PWM-channels**, the maximum interval is **practically unlimited** (limited only by unsigned long miliseconds) while **the accuracy is nearly perfect** compared to software timers. 

The most important feature is they're ISR-based PWM channels. Therefore, their executions are **not blocked by bad-behaving functions / tasks**. This important feature is absolutely necessary for mission-critical tasks. 

The [**PWMs_Array_Complex**](examples/PWMs_Array_Complex) example will demonstrate the nearly perfect accuracy, compared to software PWM, by printing the actual period / duty-cycle in `microsecs` of each of PWM-channels.

Being ISR-based PWM, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet or Blynk services. You can also have many `(up to 16)` timers to use.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks.

You'll see `software-based` SimpleTimer is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---

#### Why using ISR-based PWM is better

Imagine you have a system with a **mission-critical** function, measuring water level and control the sump pump or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is **blocking** the loop() or setup().

So your function **might not be executed, and the result would be disastrous.**

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a Hardware Timer with **Interrupt** to call your function.

These hardware timers, using interrupt, still work even if other functions are blocking. Moreover, they are much more **precise** (certainly depending on clock frequency accuracy) than other software timers using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software timers, relying on loop() and calling millis(), won't work if the loop() or setup() is blocked by certain operation. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is **your function is now part of an ISR (Interrupt Service Routine), and must be lean / mean, and follow certain rules.** More to read on:

[**HOWTO Attach Interrupt**](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

### Currently supported Boards

1. **Portenta_H7 boards** such as Portenta_H7 Rev2 ABX00042, etc., using [**ArduinoCore-mbed mbed_portenta** core](https://github.com/arduino/ArduinoCore-mbed)

---

#### Important Notes about ISR

1. Inside the attached function, **delay() won’t work and the value returned by millis() will not increment.** Serial data received while in the function may be lost. You should declare as **volatile any variables that you modify within the attached function.**

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisites

 1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
 2. [`ArduinoCore-mbed mbed_portenta core 2.6.1+`](https://github.com/arduino/ArduinoCore-mbed) for Arduino **Portenta_H7** boards, such as **Portenta_H7 Rev2 ABX00042, etc.**. [![GitHub release](https://img.shields.io/github/release/arduino/ArduinoCore-mbed.svg)](https://github.com/arduino/ArduinoCore-mbed/releases/latest)

 3. To use with certain example
   - [`SimpleTimer library`](https://github.com/jfturcot/SimpleTimer) for [ISR_16_Timers_Array example](examples/ISR_16_Timers_Array).
---
---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for [**Portenta_H7_Slow_PWM**](https://github.com/khoih-prog/Portenta_H7_Slow_PWM), then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/Portenta_H7_Slow_PWM.svg?)](https://www.ardu-badge.com/Portenta_H7_Slow_PWM) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [**Portenta_H7_Slow_PWM**](https://github.com/khoih-prog/Portenta_H7_Slow_PWM) page.
2. Download the latest release `Portenta_H7_Slow_PWM-master.zip`.
3. Extract the zip file to `Portenta_H7_Slow_PWM-master` directory 
4. Copy whole `Portenta_H7_Slow_PWM-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**Portenta_H7_Slow_PWM** library](https://platformio.org/lib/show/12854/Portenta_H7_Slow_PWM) by using [Library Manager](https://platformio.org/lib/show/12854/Portenta_H7_Slow_PWM/installation). Search for **Portenta_H7_Slow_PWM** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---

### Packages' Patches

#### 1. For Portenta_H7 boards using Arduino IDE in Linux

  **To be able to upload firmware to Portenta_H7 using Arduino IDE in Linux (Ubuntu, etc.)**, you have to copy the file [portenta_post_install.sh](Packages_Patches/arduino/hardware/mbed_portenta/2.6.1/portenta_post_install.sh) into mbed_portenta directory (~/.arduino15/packages/arduino/hardware/mbed_portenta/2.6.1/portenta_post_install.sh). 
  
  Then run the following command using `sudo`
  
```
$ cd ~/.arduino15/packages/arduino/hardware/mbed_portenta/2.6.1
$ chmod 755 portenta_post_install.sh
$ sudo ./portenta_post_install.sh
```

This will create the file `/etc/udev/rules.d/49-portenta_h7.rules` as follows:

```
# Portenta H7 bootloader mode UDEV rules

SUBSYSTEMS=="usb", ATTRS{idVendor}=="2341", ATTRS{idProduct}=="035b", GROUP="plugdev", MODE="0666"
```

Supposing the ArduinoCore-mbed core version is 2.6.1. Now only one file must be copied into the directory:

- `~/.arduino15/packages/arduino/hardware/mbed_portenta/2.6.1/portenta_post_install.sh`

Whenever a new version is installed, remember to copy this files into the new version directory. For example, new version is x.yy.zz

This file must be copied into the directory:

- `~/.arduino15/packages/arduino/hardware/mbed_portenta/x.yy.zz/portenta_post_install.sh`


---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "Portenta_H7_Slow_PWM.hpp"     //https://github.com/khoih-prog/Portenta_H7_Slow_PWM
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "Portenta_H7_Slow_PWM.h"           //https://github.com/khoih-prog/Portenta_H7_Slow_PWM
```

Check the new [**multiFileProject** example](examples/multiFileProject) for a `HOWTO` demo.

Have a look at the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)

---
---


## More useful Information about STM32 Timers

The Timers of STM32s are numerous, yet very sophisticated and powerful.

In general, across the STM32 microcontrollers families, the timer peripherals that have the same name also have the same features set, but there are a few exceptions. 

The general purpose timers embedded by the STM32 microcontrollers share the same backbone structure; they differ only on the level of features embedded by a given timer peripheral. 

The level of features integration for a given timer peripheral is decided based on the applications field that it targets.

The timer peripherals can be classified as:

• Advanced-configuration timers like TIM1 and TIM8 among others.
• General-purpose configuration timers like TIM2 and TIM3 among others
• Lite-configuration timers like TIM9, TIM10, TIM12 and TIM16 among others
• Basic-configuration timers like TIM6 and TIM7 among others.


More information can be found at [**Embedded-Lab STM32 TIMERS**](http://embedded-lab.com/blog/stm32-timers/)

To be sure which Timer is available for the board you're using, check the Core Package's related files. For example, for **Portenta_H7 using STM32H747XI**, check this file:

1. `~/.arduino15/packages/STM32/hardware/stm32/2.0.0/system/Drivers/CMSIS/Device/ST/STM32H7xx/Include/stm32h7xx.h`


The information will be as follows:

```
typedef struct
{
  __IO uint32_t CR1;         /*!< TIM control register 1,                   Address offset: 0x00 */
  __IO uint32_t CR2;         /*!< TIM control register 2,                   Address offset: 0x04 */
  __IO uint32_t SMCR;        /*!< TIM slave mode control register,          Address offset: 0x08 */
  __IO uint32_t DIER;        /*!< TIM DMA/interrupt enable register,        Address offset: 0x0C */
  __IO uint32_t SR;          /*!< TIM status register,                      Address offset: 0x10 */
  __IO uint32_t EGR;         /*!< TIM event generation register,            Address offset: 0x14 */
  __IO uint32_t CCMR1;       /*!< TIM capture/compare mode register 1,      Address offset: 0x18 */
  __IO uint32_t CCMR2;       /*!< TIM capture/compare mode register 2,      Address offset: 0x1C */
  __IO uint32_t CCER;        /*!< TIM capture/compare enable register,      Address offset: 0x20 */
  __IO uint32_t CNT;         /*!< TIM counter register,                     Address offset: 0x24 */
  __IO uint32_t PSC;         /*!< TIM prescaler,                            Address offset: 0x28 */
  __IO uint32_t ARR;         /*!< TIM auto-reload register,                 Address offset: 0x2C */
  __IO uint32_t RCR;         /*!< TIM repetition counter register,          Address offset: 0x30 */
  __IO uint32_t CCR1;        /*!< TIM capture/compare register 1,           Address offset: 0x34 */
  __IO uint32_t CCR2;        /*!< TIM capture/compare register 2,           Address offset: 0x38 */
  __IO uint32_t CCR3;        /*!< TIM capture/compare register 3,           Address offset: 0x3C */
  __IO uint32_t CCR4;        /*!< TIM capture/compare register 4,           Address offset: 0x40 */
  __IO uint32_t BDTR;        /*!< TIM break and dead-time register,         Address offset: 0x44 */
  __IO uint32_t DCR;         /*!< TIM DMA control register,                 Address offset: 0x48 */
  __IO uint32_t DMAR;        /*!< TIM DMA address for full transfer,        Address offset: 0x4C */
  uint32_t      RESERVED1;   /*!< Reserved, 0x50                                                 */
  __IO uint32_t CCMR3;       /*!< TIM capture/compare mode register 3,      Address offset: 0x54 */
  __IO uint32_t CCR5;        /*!< TIM capture/compare register5,            Address offset: 0x58 */
  __IO uint32_t CCR6;        /*!< TIM capture/compare register6,            Address offset: 0x5C */
  __IO uint32_t AF1;         /*!< TIM alternate function option register 1, Address offset: 0x60 */
  __IO uint32_t AF2;         /*!< TIM alternate function option register 2, Address offset: 0x64 */
  __IO uint32_t TISEL;       /*!< TIM Input Selection register,             Address offset: 0x68 */
} TIM_TypeDef;
```

and

```
#define PERIPH_BASE            0x40000000UL /*!< Base address of : AHB/ABP Peripherals   
/*!< Peripheral memory map */
#define APB1PERIPH_BASE        PERIPH_BASE

/*!< APB1 peripherals */
/*!< D2_APB1PERIPH peripherals */
#define TIM2_BASE             (D2_APB1PERIPH_BASE + 0x0000UL)
#define TIM3_BASE             (D2_APB1PERIPH_BASE + 0x0400UL)
#define TIM4_BASE             (D2_APB1PERIPH_BASE + 0x0800UL)
#define TIM5_BASE             (D2_APB1PERIPH_BASE + 0x0C00UL)
#define TIM6_BASE             (D2_APB1PERIPH_BASE + 0x1000UL)
#define TIM7_BASE             (D2_APB1PERIPH_BASE + 0x1400UL)
#define TIM12_BASE            (D2_APB1PERIPH_BASE + 0x1800UL)
#define TIM13_BASE            (D2_APB1PERIPH_BASE + 0x1C00UL)
#define TIM14_BASE            (D2_APB1PERIPH_BASE + 0x2000UL)

/*!< APB2 peripherals */
#define TIM1_BASE             (D2_APB2PERIPH_BASE + 0x0000UL)
#define TIM8_BASE             (D2_APB2PERIPH_BASE + 0x0400UL)
...
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4000UL)
#define TIM10_BASE            (APB2PERIPH_BASE + 0x4400UL)
#define TIM11_BASE            (APB2PERIPH_BASE + 0x4800UL)
...
#define TI15_BASE            (D2_APB2PERIPH_BASE + 0x4000UL)
#define TIM16_BASE            (D2_APB2PERIPH_BASE + 0x4400UL)
#define TIM17_BASE            (D2_APB2PERIPH_BASE + 0x4800UL)
...
#define HRTIM1_BASE           (D2_APB2PERIPH_BASE + 0x7400UL)
#define HRTIM1_TIMA_BASE      (HRTIM1_BASE + 0x00000080UL)
#define HRTIM1_TIMB_BASE      (HRTIM1_BASE + 0x00000100UL)
#define HRTIM1_TIMC_BASE      (HRTIM1_BASE + 0x00000180UL)
#define HRTIM1_TIMD_BASE      (HRTIM1_BASE + 0x00000200UL)
#define HRTIM1_TIME_BASE      (HRTIM1_BASE + 0x00000280UL)
#define HRTIM1_COMMON_BASE    (HRTIM1_BASE + 0x00000380UL)
...
#define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#define TIM3                ((TIM_TypeDef *) TIM3_BASE)
#define TIM4                ((TIM_TypeDef *) TIM4_BASE)
#define TIM5                ((TIM_TypeDef *) TIM5_BASE)
#define TIM6                ((TIM_TypeDef *) TIM6_BASE)
#define TIM7                ((TIM_TypeDef *) TIM7_BASE)
#define TIM13               ((TIM_TypeDef *) TIM13_BASE)
#define TIM14               ((TIM_TypeDef *) TIM14_BASE)
...
#define TIM1                ((TIM_TypeDef *) TIM1_BASE)
#define TIM8                ((TIM_TypeDef *) TIM8_BASE)
...
#define TIM12               ((TIM_TypeDef *) TIM12_BASE)
#define TIM15               ((TIM_TypeDef *) TIM15_BASE)
#define TIM16               ((TIM_TypeDef *) TIM16_BASE)
#define TIM17               ((TIM_TypeDef *) TIM17_BASE)
...
#define HRTIM1              ((HRTIM_TypeDef *) HRTIM1_BASE)
#define HRTIM1_TIMA         ((HRTIM_Timerx_TypeDef *) HRTIM1_TIMA_BASE)
#define HRTIM1_TIMB         ((HRTIM_Timerx_TypeDef *) HRTIM1_TIMB_BASE)
#define HRTIM1_TIMC         ((HRTIM_Timerx_TypeDef *) HRTIM1_TIMC_BASE)
#define HRTIM1_TIMD         ((HRTIM_Timerx_TypeDef *) HRTIM1_TIMD_BASE)
#define HRTIM1_TIME         ((HRTIM_Timerx_TypeDef *) HRTIM1_TIME_BASE)
#define HRTIM1_COMMON       ((HRTIM_Common_TypeDef *) HRTIM1_COMMON_BASE)
```

---

## Available Timers for Portenta_H7

This is the temporary list for Portenta_H7 Timers which can be used. The available Timers certainly depends on they are being used for other purpose (core, application, libraries, etc.) or not. You have to exhausively test yourself to be sure.

#### 1. OK to use

**TIM1, TIM4, TIM7, TIM8, TIM12, TIM13, TIM14, TIM15, TIM16, TIM17**

#### 2. Not exist

**TIM9, TIM10, TIM11. Only for STM32F2, STM32F4 or STM32L1**

#### 3.Not declared

**TIM18, TIM19, TIM20, TIM21, TIM22**

#### 3. Not OK => conflict or crash

**TIM2, TIM3, TIM5, TIM6**


---
---

## Usage

Before using any Timer for a PWM channel, you have to make sure the Timer has not been used by any other purpose.

### 1. Using only Hardware Timer directly

#### 1.1 Init Hardware Timer

```
// Depending on the board, you can select STM32H7 Hardware Timer from TIM1-TIM22
// If you select a Timer not correctly, you'll get a message from compiler
// 'TIMxx' was not declared in this scope; did you mean 'TIMyy'? 

// Portenta_H7 OK       : TIM1, TIM4, TIM7, TIM8, TIM12, TIM13, TIM14, TIM15, TIM16, TIM17
// Portenta_H7 Not OK   : TIM2, TIM3, TIM5, TIM6, TIM18, TIM19, TIM20, TIM21, TIM22
// Portenta_H7 No timer : TIM9, TIM10, TIM11. Only for STM32F2, STM32F4 and STM32L1 
// Portenta_H7 No timer : TIM18, TIM19, TIM20, TIM21, TIM22

// Init timer TIM16
Portenta_H7_Timer ITimer(TIM16);

// Init Portenta_H7_Slow_PWM
Portenta_H7_Slow_PWM ISR_PWM;
```

#### 1.2 Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions

```
void irqCallbackStartFunc()
{

}

void irqCallbackStopFunc()
{

}

void setup()
{
  ....
  
  // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(PWM_Pin, PWM_Freq, PWM_DutyCycle, irqCallbackStartFunc, irqCallbackStopFunc);
                   
  ....                 
}  
```

---
---

### Examples: 

 1. [ISR_16_PWMs_Array](examples/ISR_16_PWMs_Array)
 2. [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)
 3. [ISR_16_PWMs_Array_Simple](examples/ISR_16_PWMs_Array_Simple) 
 4. [ISR_Changing_PWM](examples/ISR_Changing_PWM)
 5. [ISR_Modify_PWM](examples/ISR_Modify_PWM)
 6. [**multiFileProject**](examples/multiFileProject) **New**

 
---
---

### Example [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)

```
#if !( ( defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4) ) && defined(ARDUINO_ARCH_MBED) )
  #error This code is intended to run on the MBED ARDUINO_PORTENTA_H7 platform! Please check your Tools->Board setting. 
#endif

// These define's must be placed at the beginning before #include "ESP32_PWM.h"
// _PWM_LOGLEVEL_ from 0 to 4
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_      4

#define USING_MICROS_RESOLUTION       true    //false 

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "Portenta_H7_Slow_PWM.h"

#include <SimpleTimer.h>              // https://github.com/jfturcot/SimpleTimer

#define LED_OFF             HIGH
#define LED_ON              LOW

#ifndef LED_BUILTIN
  #define LED_BUILTIN       24    //LEDG               // Pin 24 control on-board LED_GREEN on Portenta_H7
#endif

#ifndef LED_BLUE
  #define LED_BLUE          25    //LEDB               // Pin 25 control on-board LED_BLUE on Portenta_H7
#endif

#ifndef LED_RED
  #define LED_RED           23   // LEDR              // Pin 23 control on-board LED_RED on Portenta_H7
#endif

#define HW_TIMER_INTERVAL_US      10L

volatile uint64_t startMicros = 0;

// Depending on the board, you can select STM32H7 Hardware Timer from TIM1-TIM22
// If you select a Timer not correctly, you'll get a message from compiler
// 'TIMxx' was not declared in this scope; did you mean 'TIMyy'? 

// Portenta_H7 OK       : TIM1, TIM4, TIM7, TIM8, TIM12, TIM13, TIM14, TIM15, TIM16, TIM17
// Portenta_H7 Not OK   : TIM2, TIM3, TIM5, TIM6, TIM18, TIM19, TIM20, TIM21, TIM22
// Portenta_H7 No timer : TIM9, TIM10, TIM11. Only for STM32F2, STM32F4 and STM32L1 
// Portenta_H7 No timer : TIM18, TIM19, TIM20, TIM21, TIM22

// Init timer TIM16
Portenta_H7_Timer ITimer(TIM16);

// Init Portenta_H7_Slow_PWM
Portenta_H7_Slow_PWM ISR_PWM;

//////////////////////////////////////////////////////

void TimerHandler()
{
  ISR_PWM.run();
}

/////////////////////////////////////////////////

#define NUMBER_ISR_PWMS         16

#define PIN_D0      D0
#define PIN_D1      D1
#define PIN_D2      D2
#define PIN_D3      D3
#define PIN_D4      D4
#define PIN_D5      D5
#define PIN_D6      D6
#define PIN_D7      D7
#define PIN_D8      D8
#define PIN_D9      D9
#define PIN_D10     D10
#define PIN_D11     D11
#define PIN_D12     D12

typedef void (*irqCallback)  ();

//////////////////////////////////////////////////////

#define USE_COMPLEX_STRUCT      true

#define USING_PWM_FREQUENCY     true

//////////////////////////////////////////////////////

#if USE_COMPLEX_STRUCT

typedef struct
{
  uint32_t      PWM_Pin;
  irqCallback   irqCallbackStartFunc;
  irqCallback   irqCallbackStopFunc;

#if USING_PWM_FREQUENCY  
  float         PWM_Freq;
#else  
  uint32_t      PWM_Period;
#endif
  
  float         PWM_DutyCycle;
  
  uint64_t      deltaMicrosStart;
  uint64_t      previousMicrosStart;

  uint64_t      deltaMicrosStop;
  uint64_t      previousMicrosStop;
  
} ISR_PWM_Data;

// In NRF52, avoid doing something fancy in ISR, for example Serial.print()
// The pure simple Serial.prints here are just for demonstration and testing. Must be eliminate in working environment
// Or you can get this run-time error / crash

void doingSomethingStart(int index);

void doingSomethingStop(int index);

#else   // #if USE_COMPLEX_STRUCT

volatile unsigned long deltaMicrosStart    [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
volatile unsigned long previousMicrosStart [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

volatile unsigned long deltaMicrosStop     [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
volatile unsigned long previousMicrosStop  [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// You can assign pins here. Be carefull to select good pin to use or crash, e.g pin 6-11
uint32_t PWM_Pin[] =
{
      LEDG,   LEDB,   LEDR, PIN_D0, PIN_D1,  PIN_D2,  PIN_D3,  PIN_D4,
   PIN_D5,  PIN_D6, PIN_D7, PIN_D8, PIN_D9, PIN_D10, PIN_D11, PIN_D12
};

// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period[] =
{
  1000000L,   500000L,   333333L,   250000L,   200000L,   166667L,   142857L,   125000L,
   111111L,   100000L,    66667L,    50000L,    40000L,   33333L,     25000L,    20000L
};

// You can assign any interval for any timer here, in Hz
float PWM_Freq[] =
{
  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,
  9.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 40.0f, 50.0f
};

// You can assign any interval for any timer here, in milliseconds
float PWM_DutyCycle[] =
{
   5.0, 10.0, 20.0, 30.0, 40.0, 45.0, 50.0, 55.0,
  60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0, 95.0
};

void doingSomethingStart(int index)
{
  unsigned long currentMicros  = micros();

  deltaMicrosStart[index]    = currentMicros - previousMicrosStart[index];
  previousMicrosStart[index] = currentMicros;
}

void doingSomethingStop(int index)
{
  unsigned long currentMicros  = micros();

  // Count from start to stop PWM pulse
  deltaMicrosStop[index]    = currentMicros - previousMicrosStart[index];
  previousMicrosStop[index] = currentMicros;
}

#endif    // #if USE_COMPLEX_STRUCT

////////////////////////////////////
// Shared
////////////////////////////////////

void doingSomethingStart0()
{
  doingSomethingStart(0);
}

void doingSomethingStart1()
{
  doingSomethingStart(1);
}

void doingSomethingStart2()
{
  doingSomethingStart(2);
}

void doingSomethingStart3()
{
  doingSomethingStart(3);
}

void doingSomethingStart4()
{
  doingSomethingStart(4);
}

void doingSomethingStart5()
{
  doingSomethingStart(5);
}

void doingSomethingStart6()
{
  doingSomethingStart(6);
}

void doingSomethingStart7()
{
  doingSomethingStart(7);
}

void doingSomethingStart8()
{
  doingSomethingStart(8);
}

void doingSomethingStart9()
{
  doingSomethingStart(9);
}

void doingSomethingStart10()
{
  doingSomethingStart(10);
}

void doingSomethingStart11()
{
  doingSomethingStart(11);
}

void doingSomethingStart12()
{
  doingSomethingStart(12);
}

void doingSomethingStart13()
{
  doingSomethingStart(13);
}

void doingSomethingStart14()
{
  doingSomethingStart(14);
}

void doingSomethingStart15()
{
  doingSomethingStart(15);
}

//////////////////////////////////////////////////////

void doingSomethingStop0()
{
  doingSomethingStop(0);
}

void doingSomethingStop1()
{
  doingSomethingStop(1);
}

void doingSomethingStop2()
{
  doingSomethingStop(2);
}

void doingSomethingStop3()
{
  doingSomethingStop(3);
}

void doingSomethingStop4()
{
  doingSomethingStop(4);
}

void doingSomethingStop5()
{
  doingSomethingStop(5);
}

void doingSomethingStop6()
{
  doingSomethingStop(6);
}

void doingSomethingStop7()
{
  doingSomethingStop(7);
}

void doingSomethingStop8()
{
  doingSomethingStop(8);
}

void doingSomethingStop9()
{
  doingSomethingStop(9);
}

void doingSomethingStop10()
{
  doingSomethingStop(10);
}

void doingSomethingStop11()
{
  doingSomethingStop(11);
}

void doingSomethingStop12()
{
  doingSomethingStop(12);
}

void doingSomethingStop13()
{
  doingSomethingStop(13);
}

void doingSomethingStop14()
{
  doingSomethingStop(14);
}

void doingSomethingStop15()
{
  doingSomethingStop(15);
}

//////////////////////////////////////////////////////

#if USE_COMPLEX_STRUCT

  #if USING_PWM_FREQUENCY
  
  ISR_PWM_Data curISR_PWM_Data[] =
  {
    // pin, irqCallbackStartFunc, irqCallbackStopFunc, PWM_Freq, PWM_DutyCycle, deltaMicrosStart, previousMicrosStart, deltaMicrosStop, previousMicrosStop
    { LEDG,         doingSomethingStart0,    doingSomethingStop0,    1,   5, 0, 0, 0, 0 },
    { LEDB,         doingSomethingStart1,    doingSomethingStop1,    2,  10, 0, 0, 0, 0 },
    { LEDR,         doingSomethingStart2,    doingSomethingStop2,    3,  20, 0, 0, 0, 0 },
    { PIN_D0,       doingSomethingStart3,    doingSomethingStop3,    4,  30, 0, 0, 0, 0 },
    { PIN_D1,       doingSomethingStart4,    doingSomethingStop4,    5,  40, 0, 0, 0, 0 },
    { PIN_D2,       doingSomethingStart5,    doingSomethingStop5,    6,  45, 0, 0, 0, 0 },
    { PIN_D3,       doingSomethingStart6,    doingSomethingStop6,    7,  50, 0, 0, 0, 0 },
    { PIN_D4,       doingSomethingStart7,    doingSomethingStop7,    8,  55, 0, 0, 0, 0 },
    { PIN_D5,       doingSomethingStart8,    doingSomethingStop8,    9,  60, 0, 0, 0, 0 },
    { PIN_D6,       doingSomethingStart9,    doingSomethingStop9,   10,  65, 0, 0, 0, 0 },
    { PIN_D7,       doingSomethingStart10,   doingSomethingStop10,  15,  70, 0, 0, 0, 0 },
    { PIN_D8,       doingSomethingStart11,   doingSomethingStop11,  20,  75, 0, 0, 0, 0 },
    { PIN_D9,       doingSomethingStart12,   doingSomethingStop12,  25,  80, 0, 0, 0, 0 },
    { PIN_D10,      doingSomethingStart13,   doingSomethingStop13,  30,  85, 0, 0, 0, 0 },
    { PIN_D11,      doingSomethingStart14,   doingSomethingStop14,  40,  90, 0, 0, 0, 0 },
    { PIN_D12,      doingSomethingStart15,   doingSomethingStop15,  50,  95, 0, 0, 0, 0 }
  };
  
  #else   // #if USING_PWM_FREQUENCY
  
  ISR_PWM_Data curISR_PWM_Data[] =
  {
    // pin, irqCallbackStartFunc, irqCallbackStopFunc, PWM_Period, PWM_DutyCycle, deltaMicrosStart, previousMicrosStart, deltaMicrosStop, previousMicrosStop
    { LEDG,         doingSomethingStart0,     doingSomethingStop0,   1000000L,  5, 0, 0, 0, 0 },
    { LEDB,         doingSomethingStart1,     doingSomethingStop1,    500000L, 10, 0, 0, 0, 0 },
    { LEDR,         doingSomethingStart2,     doingSomethingStop2,    333333L, 20, 0, 0, 0, 0 },
    { PIN_D0,       doingSomethingStart3,     doingSomethingStop3,    250000L, 30, 0, 0, 0, 0 },
    { PIN_D1,       doingSomethingStart4,     doingSomethingStop4,    200000L, 40, 0, 0, 0, 0 },
    { PIN_D2,       doingSomethingStart5,     doingSomethingStop5,    166667L, 45, 0, 0, 0, 0 },
    { PIN_D3,       doingSomethingStart6,     doingSomethingStop6,    142857L, 50, 0, 0, 0, 0 },
    { PIN_D4,       doingSomethingStart7,     doingSomethingStop7,    125000L, 55, 0, 0, 0, 0 },
    { PIN_D5,       doingSomethingStart8,     doingSomethingStop8,    111111L, 60, 0, 0, 0, 0 },
    { PIN_D6,       doingSomethingStart9,     doingSomethingStop9,    100000L, 65, 0, 0, 0, 0 },
    { PIN_D7,       doingSomethingStart10,    doingSomethingStop10,    66667L, 70, 0, 0, 0, 0 },
    { PIN_D8,       doingSomethingStart11,    doingSomethingStop11,    50000L, 75, 0, 0, 0, 0 },
    { PIN_D9,       doingSomethingStart12,    doingSomethingStop12,    40000L, 80, 0, 0, 0, 0 },
    { PIN_D10,      doingSomethingStart13,    doingSomethingStop13,    33333L, 85, 0, 0, 0, 0 },
    { PIN_D11,      doingSomethingStart14,    doingSomethingStop14,    25000L, 90, 0, 0, 0, 0 },
    { PIN_D12,      doingSomethingStart15,    doingSomethingStop15,    20000L, 95, 0, 0, 0, 0 }
  };
  
  #endif  // #if USING_PWM_FREQUENCY

void doingSomethingStart(int index)
{
  unsigned long currentMicros  = micros();

  curISR_PWM_Data[index].deltaMicrosStart    = currentMicros - curISR_PWM_Data[index].previousMicrosStart;
  curISR_PWM_Data[index].previousMicrosStart = currentMicros;
}

void doingSomethingStop(int index)
{
  unsigned long currentMicros  = micros();

  //curISR_PWM_Data[index].deltaMicrosStop     = currentMicros - curISR_PWM_Data[index].previousMicrosStop;
  // Count from start to stop PWM pulse
  curISR_PWM_Data[index].deltaMicrosStop     = currentMicros - curISR_PWM_Data[index].previousMicrosStart;
  curISR_PWM_Data[index].previousMicrosStop  = currentMicros;
}

#else   // #if USE_COMPLEX_STRUCT

irqCallback irqCallbackStartFunc[] =
{
  doingSomethingStart0,  doingSomethingStart1,  doingSomethingStart2,  doingSomethingStart3,
  doingSomethingStart4,  doingSomethingStart5,  doingSomethingStart6,  doingSomethingStart7,
  doingSomethingStart8,  doingSomethingStart9,  doingSomethingStart10, doingSomethingStart11,
  doingSomethingStart12, doingSomethingStart13, doingSomethingStart14, doingSomethingStart15
};

irqCallback irqCallbackStopFunc[] =
{
  doingSomethingStop0,  doingSomethingStop1,  doingSomethingStop2,  doingSomethingStop3,
  doingSomethingStop4,  doingSomethingStop5,  doingSomethingStop6,  doingSomethingStop7,
  doingSomethingStop8,  doingSomethingStop9,  doingSomethingStop10, doingSomethingStop11,
  doingSomethingStop12, doingSomethingStop13, doingSomethingStop14, doingSomethingStop15
};

#endif    // #if USE_COMPLEX_STRUCT

//////////////////////////////////////////////////////

#define SIMPLE_TIMER_MS        2000L

// Init SimpleTimer
SimpleTimer simpleTimer;

// Here is software Timer, you can do somewhat fancy stuffs without many issues.
// But always avoid
// 1. Long delay() it just doing nothing and pain-without-gain wasting CPU power.Plan and design your code / strategy ahead
// 2. Very long "do", "while", "for" loops without predetermined exit time.
void simpleTimerDoingSomething2s()
{
  static unsigned long previousMicrosStart = startMicros;

  unsigned long currMicros = micros();

  Serial.print(F("SimpleTimer (ms): ")); Serial.print(SIMPLE_TIMER_MS);
  Serial.print(F(", us : ")); Serial.print(currMicros);
  Serial.print(F(", Dus : ")); Serial.println(currMicros - previousMicrosStart);

  for (uint16_t i = 0; i < NUMBER_ISR_PWMS; i++)
  {
#if USE_COMPLEX_STRUCT
    Serial.print(F("PWM Channel : ")); Serial.print(i);
    Serial.print(F(", programmed Period (us): "));

  #if USING_PWM_FREQUENCY
    Serial.print(1000000 / curISR_PWM_Data[i].PWM_Freq);
  #else
    Serial.print(curISR_PWM_Data[i].PWM_Period);
  #endif
    
    Serial.print(F(", actual : ")); Serial.print(curISR_PWM_Data[i].deltaMicrosStart);
    
    Serial.print(F(", programmed DutyCycle : ")); 

    Serial.print(curISR_PWM_Data[i].PWM_DutyCycle);
    
    Serial.print(F(", actual : ")); Serial.println((float) curISR_PWM_Data[i].deltaMicrosStop * 100.0f / curISR_PWM_Data[i].deltaMicrosStart);
    
#else

    Serial.print(F("PWM Channel : ")); Serial.print(i);
    
  #if USING_PWM_FREQUENCY
    Serial.print(1000000 / PWM_Freq[i]);
  #else
    Serial.print(PWM_Period[i]);
  #endif
  
    Serial.print(F(", programmed Period (us): ")); Serial.print(PWM_Period[i]);
    Serial.print(F(", actual : ")); Serial.print(deltaMicrosStart[i]);

    Serial.print(F(", programmed DutyCycle : ")); 
  
    Serial.print(PWM_DutyCycle[i]);
      
    Serial.print(F(", actual : ")); Serial.println( (float) deltaMicrosStop[i] * 100.0f / deltaMicrosStart[i]);
#endif
  }

  previousMicrosStart = currMicros;
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  delay(2000);

  Serial.print(F("\nStarting ISR_16_PWMs_Array_Complex on ")); Serial.println(BOARD_NAME);
  Serial.println(PORTENTA_H7_SLOW_PWM_VERSION);

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_US, TimerHandler))
  {
    startMicros = micros();
    Serial.print(F("Starting ITimer OK, micros() = ")); Serial.println(startMicros);
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));

  startMicros = micros();

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_PWM
  
  for (uint16_t i = 0; i < NUMBER_ISR_PWMS; i++)
  {
#if USE_COMPLEX_STRUCT
    curISR_PWM_Data[i].previousMicrosStart = startMicros;
    //ISR_PWM.setInterval(curISR_PWM_Data[i].PWM_Period, curISR_PWM_Data[i].irqCallbackStartFunc);

    //void setPWM(uint32_t pin, float frequency, float dutycycle
    // , timer_callback_p StartCallback = nullptr, timer_callback_p StopCallback = nullptr)

  #if USING_PWM_FREQUENCY
    // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(curISR_PWM_Data[i].PWM_Pin, curISR_PWM_Data[i].PWM_Freq, curISR_PWM_Data[i].PWM_DutyCycle, 
                   curISR_PWM_Data[i].irqCallbackStartFunc, curISR_PWM_Data[i].irqCallbackStopFunc);
  #else
    // Or You can use this with PWM_Period in us
    ISR_PWM.setPWM_Period(curISR_PWM_Data[i].PWM_Pin, curISR_PWM_Data[i].PWM_Period, curISR_PWM_Data[i].PWM_DutyCycle, 
                          curISR_PWM_Data[i].irqCallbackStartFunc, curISR_PWM_Data[i].irqCallbackStopFunc);
  #endif
  
#else
    previousMicrosStart[i] = micros();
    
  #if USING_PWM_FREQUENCY
    // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(PWM_Pin[i], PWM_Freq[i], PWM_DutyCycle[i], irqCallbackStartFunc[i], irqCallbackStopFunc[i]);
  #else
    // Or You can use this with PWM_Period in us
    ISR_PWM.setPWM_Period(PWM_Pin[i], PWM_Period[i], PWM_DutyCycle[i], irqCallbackStartFunc[i], irqCallbackStopFunc[i]);
  #endif 
   
#endif
  }

  // You need this timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary.
  simpleTimer.setInterval(SIMPLE_TIMER_MS, simpleTimerDoingSomething2s);
}

#define BLOCKING_TIME_MS      10000L

void loop()
{
  // This unadvised blocking task is used to demonstrate the blocking effects onto the execution and accuracy to Software timer
  // You see the time elapse of ISR_PWM still accurate, whereas very unaccurate for Software Timer
  // The time elapse for 2000ms software timer now becomes 3000ms (BLOCKING_TIME_MS)
  // While that of ISR_PWM is still prefect.
  delay(BLOCKING_TIME_MS);

  // You need this Software timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary
  // You don't need to and never call ISR_PWM.run() here in the loop(). It's already handled by ISR timer.
  simpleTimer.run();
}
```
---
---

### Debug Terminal Output Samples

### 1. ISR_16_PWMs_Array_Complex on PORTENTA_H7_M7

The following is the sample terminal output when running example [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex) to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```
Starting ISR_16_PWMs_Array_Complex on PORTENTA_H7_M7
Portenta_H7_Slow_PWM v1.2.0
[PWM] Portenta_H7_TimerInterrupt: Timer Input Freq (Hz) = 200000000
[PWM] Frequency = 100000.00 , _count = 10
Starting ITimer OK, micros() = 2892390
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2892614
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2892992
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2893361
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 2893750
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 2894152
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2894543
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2894923
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2895323
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 2895725
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 2896121
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 2896514
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 2896889
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 2897263
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 2897642
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 2898031
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 2898420
SimpleTimer (ms): 2000, us : 12822562, Dus : 9929972
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 500000, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333340, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250000, programmed DutyCycle : 30.00, actual : 30.00
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200000, programmed DutyCycle : 40.00, actual : 40.00
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166670, programmed DutyCycle : 45.00, actual : 44.99
PWM Channel : 6, programmed Period (us): 142857.14, actual : 142860, programmed DutyCycle : 50.00, actual : 49.99
PWM Channel : 7, programmed Period (us): 125000.00, actual : 125000, programmed DutyCycle : 55.00, actual : 55.00
PWM Channel : 8, programmed Period (us): 111111.11, actual : 111119, programmed DutyCycle : 60.00, actual : 59.99
PWM Channel : 9, programmed Period (us): 100000.00, actual : 100001, programmed DutyCycle : 65.00, actual : 65.00
PWM Channel : 10, programmed Period (us): 66666.66, actual : 66670, programmed DutyCycle : 70.00, actual : 69.99
PWM Channel : 11, programmed Period (us): 50000.00, actual : 50000, programmed DutyCycle : 75.00, actual : 75.00
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40000, programmed DutyCycle : 80.00, actual : 80.00
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33340, programmed DutyCycle : 85.00, actual : 84.96
PWM Channel : 14, programmed Period (us): 25000.00, actual : 25002, programmed DutyCycle : 90.00, actual : 89.99
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20000, programmed DutyCycle : 95.00, actual : 95.00
SimpleTimer (ms): 2000, us : 22756001, Dus : 9933439
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 500000, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333340, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250000, programmed DutyCycle : 30.00, actual : 30.00
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200000, programmed DutyCycle : 40.00, actual : 40.00
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166670, programmed DutyCycle : 45.00, actual : 44.99
PWM Channel : 6, programmed Period (us): 142857.14, actual : 142860, programmed DutyCycle : 50.00, actual : 49.99
PWM Channel : 7, programmed Period (us): 125000.00, actual : 124999, programmed DutyCycle : 55.00, actual : 55.00
PWM Channel : 8, programmed Period (us): 111111.11, actual : 111120, programmed DutyCycle : 60.00, actual : 59.99
PWM Channel : 9, programmed Period (us): 100000.00, actual : 100000, programmed DutyCycle : 65.00, actual : 65.00
PWM Channel : 10, programmed Period (us): 66666.66, actual : 66670, programmed DutyCycle : 70.00, actual : 69.99
PWM Channel : 11, programmed Period (us): 50000.00, actual : 50000, programmed DutyCycle : 75.00, actual : 75.00
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40000, programmed DutyCycle : 80.00, actual : 80.00
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33340, programmed DutyCycle : 85.00, actual : 85.00
PWM Channel : 14, programmed Period (us): 25000.00, actual : 25000, programmed DutyCycle : 90.00, actual : 90.00
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20000, programmed DutyCycle : 95.00, actual : 94.95
```

---

### 2. ISR_16_PWMs_Array on PORTENTA_H7_M7

The following is the sample terminal output when running example [**ISR_16_PWMs_Array**](examples/ISR_16_PWMs_Array) on **Portenta_H7** to demonstrate how to use multiple PWM channels with simple callback functions.

```
Starting ISR_16_PWMs_Array on PORTENTA_H7_M7
Portenta_H7_Slow_PWM v1.2.0
[PWM] Portenta_H7_TimerInterrupt: Timer Input Freq (Hz) = 200000000
[PWM] Frequency = 100000.00 , _count = 10
Starting ITimer OK, micros() = 3295139
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 3295369
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 3295769
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 3296160
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 3296542
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 3296938
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 3297311
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 3297695
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 3298082
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 3298463
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 3298860
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 3299234
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 3299617
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 3300015
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 3300381
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 3300761
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 3301134
```

---

### 3. ISR_16_PWMs_Array_Simple on PORTENTA_H7_M7

The following is the sample terminal output when running example [**ISR_16_PWMs_Array_Simple**](examples/ISR_16_PWMs_Array_Simple) on **Portenta_H7** to demonstrate how to use multiple PWM channels.

```
Starting ISR_16_PWMs_Array_Simple on PORTENTA_H7_M7
Portenta_H7_Slow_PWM v1.2.0
[PWM] Portenta_H7_TimerInterrupt: Timer Input Freq (Hz) = 200000000
[PWM] Frequency = 50000.00 , _count = 20
Starting ITimer OK, micros() = 2812174
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2812393
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2812767
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2813140
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 2813523
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 2813904
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2814306
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2814706
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2815105
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 2815490
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 2815875
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 2816269
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 2816645
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 2817017
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 2817400
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 2817805
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 2818203
```

---

### 4. ISR_Modify_PWM on PORTENTA_H7_M7

The following is the sample terminal output when running example [ISR_Modify_PWM](examples/ISR_Modify_PWM) on **PORTENTA_H7_M7** to demonstrate how to modify PWM settings on-the-fly without deleting the PWM channel

```
Starting ISR_Modify_PWM on PORTENTA_H7_M7
Portenta_H7_Slow_PWM v1.2.0
[PWM] Portenta_H7_TimerInterrupt: Timer Input Freq (Hz) = 200000000
[PWM] Frequency = 100000.00 , _count = 10
Starting ITimer OK, micros() = 3493413
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 3493789
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 12493810
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 22993850
```

---

### 5. ISR_Changing_PWM on PORTENTA_H7_M7

The following is the sample terminal output when running example [ISR_Changing_PWM](examples/ISR_Changing_PWM) on **PORTENTA_H7_M7** to demonstrate how to modify PWM settings on-the-fly by deleting the PWM channel and reinit the PWM channel

```
Starting ISR_Changing_PWM on PORTENTA_H7_M7
Portenta_H7_Slow_PWM v1.2.0
[PWM] Portenta_H7_TimerInterrupt: Timer Input Freq (Hz) = 200000000
[PWM] Frequency = 100000.00 , _count = 10
Starting ITimer OK, micros() = 2709405
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 2709786
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 12619281
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 22529414
```

---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level `_PWM_LOGLEVEL_` from 0 to 4

```cpp
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_     0
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.


---
---

### Issues

Submit issues to: [Portenta_H7_Slow_PWM issues](https://github.com/khoih-prog/Portenta_H7_Slow_PWM/issues)

---

## TO DO

1. Search for bug and improvement.

---

## DONE

1. Basic hardware multi-channel PWM for **Portenta_H7**.
2. Add Table of Contents
3. Add functions to modify PWM settings on-the-fly
4. Fix `multiple-definitions` linker error
5. Optimize library code by using `reference-passing` instead of `value-passing`
6. Improve accuracy by using `float`, instead of `uint32_t` for `dutycycle`
7. DutyCycle to be optionally updated at the end current PWM period instead of immediately.

---
---

### Contributions and Thanks

Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this library.


---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/Portenta_H7_Slow_PWM/blob/master/LICENSE)

---

## Copyright

Copyright 2021- Khoi Hoang


