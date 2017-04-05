/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * NemeusTimer.cpp - UART class to manage UART operation
 * 
 * Copyright (C) 2017 Nemeus - All Rights Reserved
 *
 * This file is part of Nemeus Smart IoT Sensor (Tm) SDK.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 * http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
 
#include "NemeusTimer.h"


NemeusTimer::NemeusTimer() {

  timerOverflow_ = false;
}

/**
 * Set a timeout value
 * @param timeout  timeout in ms to
 */
void NemeusTimer::setTimeout(uint32_t timeout)
{
  uint32_t currentTime;

  currentTime = millis();
  timeoutValue_ = currentTime+timeout;

  if (timeoutValue_>=currentTime)
  {
    timerOverflow_ = false;
  }
  else
  {
    timerOverflow_ = true;
  }
}

/**
 * Has timer elapsed?
 * @return  true if timer has elapsed
 *          false otherwise
 */
bool NemeusTimer::isTimeout()
{
  bool returnTimeout;
  uint32_t currentTime;

  currentTime = millis();
  if (timerOverflow_)
  {
    if (currentTime < timeoutValue_)
    {
      timerOverflow_ = false;
    }
    returnTimeout = false;
  }
  else
  {
    if (currentTime < timeoutValue_)
    {
      returnTimeout = false;
    }
    else
      returnTimeout = true;
  }

  return returnTimeout;
}
