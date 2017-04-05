
/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * MacDataRate.h - MacDataRate class definition
 *                  Manage the MacDataRate structure for AT+MAC= RDR and SDR commands
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
 
#ifndef MACDATARATE_H
#define MACDATARATE_H

#include <stdint.h>
#include <Arduino.h>
#include "AtCommand.h"

typedef struct
{
  bool isDataRatePresent;
  bool isTxPowerPresent;
  bool isChannelMaskPresent;
  bool isChannelMaskCtrlPresent;
  bool isNbRepetitionPresent;
}MacDataRate_t;

class MacDataRate
{
  public:
    MacDataRate();
    //~MacDataRate();
    void setDataRate(String dataRate);
    void setTxPower(uint8_t txPOwer);
    void setChannelMask(String channelMask);
    void setChannelMaskCtrl(String channelMaskCtrl);
    void setNbRepetition(uint8_t nbRepetition);
    String getDataRate();
    char* generateArguments(char* arguments);
    void setMacDataRate(bool isDataRatePresent, bool isTxPowerPresent, bool isChannelMaskPresent, bool isChannelMaskCtrlPresent, bool isNbRepetitionPresent);
    MacDataRate_t* getMacDataRate();
  private:
    bool isDataRatePresent_;
    String dataRate_;
    bool isTxPowerPresent_;
    uint8_t txPower_;
    bool isChannelMaskPresent_;
    String channelMask_;
    bool isChannelMaskCtrlPresent_;
    String channelMaskCtrl_;
    bool isNbRepetitionPresent_;
    uint8_t nbRepetition_;
    MacDataRate_t  macDataRate_;
};

#endif /* MACDATARATE_H */
