/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * RadioParam.h - Radio parameters class definition
 *                  Manage the Radio parameters structure for RF parameetrs commands
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
 
#ifndef RADIOPARAM_H
#define RADIOPARAM_H

#include <stdint.h>
#include <Arduino.h>
#include "AtCommand.h"


/* ----- List of radio mode ----- */
#define RADIO_LORA_MODE             "LORA"
#define RADIO_FSK_MODE	            "FSK"

class RadioParam
{
  public:
    RadioParam();
    virtual ~RadioParam();
    void setMode(String mode);
    void setFrequency(uint32_t frequency);
    void setBandwidth(uint32_t bandwidth);
    void setDataRate(uint8_t dataRate);
    void setCodeRate(uint8_t codeRate);
  protected:
    bool isModePresent_;
    String mode_;
    bool isFreqPresent_;
    uint32_t frequency_;
    bool isBandwidthPresent_;
    uint32_t bandwidth_;
    bool isDataRatePresent_;
    uint8_t dataRate_;
    bool isCodeRatePresent_;
    uint8_t codeRate_;
};

#endif /* RADIOPARAM_H */
