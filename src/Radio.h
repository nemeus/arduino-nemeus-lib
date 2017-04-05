/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * Radio.h - Radio class definition
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
 
#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>

#include "Singleton.h"
#include "Data/RadioTxParam.h"
#include "Data/RadioRxParam.h"

#define MAXIMUM_RADIO_PAYLOAD 248
//#define SF_SEND_RESPONSE "+SF: SND,"

/**
 * Enumeration for send Mode (Binary or Text)
 */
enum RADIO_SEND_MODE
{
  RADIO_BINARY_MODE = 0,
  RADIO_TEXT_MODE   = 1
};


class Radio : public Singleton<Radio>
{
  friend class Singleton<Radio>;

  public:
    /* Start RF */
    uint8_t ON();
    /* Stop RF */
    uint8_t OFF();
    /* Send a RF frame */
    uint8_t sendFrame(uint8_t mode, char* payload, int nbRepeat);
    /* Set device on continuous Rx mode (need to poll device to get traces */
    uint8_t continuousRx();
    /* Stop continuous Rx */
    uint8_t stopRx();
    /* Set device on continuous TX mode (need to poll device to get traces */
    uint8_t continuousTx();
    /* Stop continuous Tx */
    uint8_t stopTx();
    /* Get the maximum payload size for RF frame */
    uint8_t getMaximumPayloadSize();
    /* Set Tx radio parameters */
    uint8_t setRadioTxParam(RadioTxParam params);
    /* Set Rx radio parameters */
    uint8_t setRadioRxParam(RadioRxParam params);
  protected:
    void treatAtResponse(const char * buffer);
  private:
    //static Sigfox m_instance;
    Radio();
    ~Radio();
    DataContext * dataContext_;
    boolean isContinuousRx_;
    boolean isContinuousTx_;
    static void onReceiveFromUART(const char * buffer);
};

#endif // RADIO_H
