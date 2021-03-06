/****************************************************************************************************************************
   Argument_None.ino
   For ESP32 boards
   Written by Khoi Hoang

   Built by Khoi Hoang https://github.com/khoih-prog/ESP8266TimerInterrupt
   Licensed under MIT license
   Version: 1.0.3

   The ESP32 has two timer groups, each one with two general purpose hardware timers. All the timers are based on 64 bits
   counters and 16 bit prescalers. The timer counters can be configured to count up or down and support automatic reload
   and software reload. They can also generate alarms when they reach a specific value, defined by the software. The value
   of the counter can be read by the software program.

   Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
   unsigned long miliseconds), you just consume only one ESP32 timer and avoid conflicting with other cores' tasks.
   The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
   Therefore, their executions are not blocked by bad-behaving functions / tasks.
   This important feature is absolutely necessary for mission-critical tasks.

   Based on SimpleTimer - A timer library for Arduino.
   Author: mromani@ottotecnica.com
   Copyright (c) 2010 OTTOTECNICA Italy

   Based on BlynkTimer.h
   Author: Volodymyr Shymanskyy

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
    1.0.0   K Hoang      23/11/2019 Initial coding
    1.0.1   K Hoang      27/11/2019 No v1.0.1. Bump up to 1.0.2 to match ESP8266_ISR_TimerInterupt library
    1.0.2   K.Hoang      03/12/2019 Permit up to 16 super-long-time, super-accurate ISR-based timers to avoid being blocked
    1.0.3   K.Hoang      17/05/2020 Restructure code. Add examples. Enhance README.
*****************************************************************************************************************************/

/*
   Notes:
   Special design is necessary to share data between interrupt code and the rest of your program.
   Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
   variable can not spontaneously change. Because your function may change variables while your program is using them,
   the compiler needs this hint. But volatile alone is often not enough.
   When accessing shared variables, usually interrupts must be disabled. Even with volatile,
   if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
   If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
   or the entire sequence of your code which accesses the data.
*/

#ifndef ESP32
#error This code is designed to run on ESP32 platform, not Arduino nor ESP8266! Please check your Tools->Board setting.
#endif

//These define's must be placed at the beginning before #include "ESP32TimerInterrupt.h"
#define TIMER_INTERRUPT_DEBUG      1

#include "ESP32TimerInterrupt.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
#endif

#define PIN_D23           23        // Pin D23 mapped to pin GPIO23/VSPI_MOSI of ESP32

void IRAM_ATTR TimerHandler0(void)
{
  static bool toggle0 = false;
  static bool started = false;

  if (!started)
  {
    started = true;
    pinMode(LED_BUILTIN, OUTPUT);
  }

#if (TIMER_INTERRUPT_DEBUG > 0)
  Serial.println("ITimer0: millis() = " + String(millis()));
#endif

  //timer interrupt toggles pin LED_BUILTIN
  digitalWrite(LED_BUILTIN, toggle0);
  toggle0 = !toggle0;
}

void IRAM_ATTR TimerHandler1(void)
{
  static bool toggle1 = false;
  static bool started = false;

  if (!started)
  {
    started = true;
    pinMode(PIN_D23, OUTPUT);
  }

#if (TIMER_INTERRUPT_DEBUG > 0)
  Serial.println("ITimer1: millis() = " + String(millis()));
#endif

  //timer interrupt toggles outputPin
  digitalWrite(PIN_D23, toggle1);
  toggle1 = !toggle1;
}

#define TIMER0_INTERVAL_MS        1000

#define TIMER1_INTERVAL_MS        5000

// Init ESP32 timer 0
ESP32Timer ITimer0(0);
ESP32Timer ITimer1(1);

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\nStarting Argument_None");

  // Using ESP32  => 80 / 160 / 240MHz CPU clock ,
  // For 64-bit timer counter
  // For 16-bit timer prescaler up to 1024

  // Interval in microsecs
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0))
    Serial.println("Starting  ITimer0 OK, millis() = " + String(millis()));
  else
    Serial.println("Can't set ITimer0. Select another freq. or timer");

  // Interval in microsecs
  if (ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS * 1000, TimerHandler1))
    Serial.println("Starting  ITimer1 OK, millis() = " + String(millis()));
  else
    Serial.println("Can't set ITimer1. Select another freq. or timer");
}

void loop()
{

}
