/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * MacChannel.h - MacChannel class definition
 *                  Manage the MacChannel structure for AT+MAC= RCH and SCH commands 
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

#ifndef MACCHANNEL_H
#define MACCHANNEL_H

#include <stdint.h>
#include <Arduino.h>
#include "AtCommand.h"

/**
 * Mac Channel class to format Mac Channel command with the parameters
 */
class MacChannel
{
  public:
    MacChannel();
    void setChannelNumber(uint8_t channelNumber, uint8_t pageNumber);
    void setFrequency(uint32_t frequency);
    void setMinDataRate(String minDataRate);
    void setMaxDataRate(String maxDataRate);
    void setDutyCycle(uint8_t dutyCycle);
    char* generateArguments(char* argumennts);
  private:
    bool isChannelNumberPresent_;
    uint8_t channelNumber_;
    bool isFrequencyPresent_;
    uint32_t frequency_;
    bool isMinDrPresent_;
    String minDr_;
    bool isMaxDrPresent_;
    String maxDr_;
    bool isDutyCyclePresent_;
    uint8_t dutyCycle_;
    uint8_t pageNumber_;
};

#endif /* MACCHANNEL_H */
