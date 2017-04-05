/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * Sigfox.h - Sigfox class definition
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
 
#ifndef SIGFOX_H
#define SIGFOX_H

#include <stdint.h>

#include "Singleton.h"


/**
 * Enumeration for send Mode (Binary or Text)
 */
enum SIGFOX_SEND_MODE
{
  SIGFOX_BINARY_MODE = 0,
  SIGFOX_BIT_MODE   = 1, 
  SIGFOX_OOB_MODE   = 2
};

#define MAXIMUM_SIGFOX_PAYLOAD 12
#define SF_SEND_UNSOL "+SF: SND,"

class Sigfox : public Singleton<Sigfox>
{
  friend class Singleton<Sigfox>;

  public:
    /* Start sigfox */
    uint8_t ON(char * disDutyCycle);
    /* Stop sigfox */
    uint8_t OFF();
    /* Send a sigfox frame */
    uint8_t sendFrame(uint8_t mode, char* payload, boolean ack);
    /* Get the maximum payload size for sigfox frame */
    uint8_t getMaximumPayloadSize();
  protected:
    void treatAtResponse(const char * buffer);
  private:
    //static Sigfox m_instance;
    Sigfox();
    ~Sigfox();
    DataContext * dataContext_;
    static void onReceiveFromUART(const char * buffer);
};

#endif // SIGFOX_H
