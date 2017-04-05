/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * NemeusLib.h - Include for Nemeus main library. Class description for general operation (init/close/traces...). 
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

#ifndef NEMEUSLIB_H
#define NEMEUSLIB_H

#include <stdint.h>

#include "Arduino.h"
#include "NemeusUART.h"
#include "LoRaWAN.h"
#include "Sigfox.h"
#include "Radio.h"
#include <Data/RadioTxParam.h>
#include <Data/RadioRxParam.h>
#include <Data/MacDataRate.h>
#include <Data/MacDataRate.h>
#include <Data/MacChannel.h>
#include <Data/DevPerso.h>


class NemeusLib
{
  public:
    /* Constructor */
    NemeusLib();
    Sigfox* sigfox();     // Access to sigfox object (& methods)
    LoRaWAN* loraWan();   // Access to loraWan object (& methods)
    Radio* radio();     // Access to radio RF object (& methods)
    uint8_t init();     // Init the (UART)
    uint8_t resetModem();     // Init the (UART)
    void close();     // Close UART
    uint8_t setPowersaving(bool isOn);  // Enable/disable powersaving from device
    uint8_t setVerbose(bool isOn);  // Enable/disable verbose traces from device
    uint8_t debugMver();  // Get the version
    void printTraces();       // Print traces buffer on SerialUSB
    uint8_t resetDevice();      // Reset the nemeus device
    // Register a callback for unsollicited and AT response
    void register_at_response_callback(void (*onReceive)(const char *));
    // Unegister the callback for unsollicited and AT response
    void unregister_at_response_callback(void (*onReceive)(const char *));
    uint8_t availableTraces();    // Check if traces are available in buffer
    uint8_t readLine(char* buffer, int size); // Read a line (ends with '\n') in buffer
    // Poll device during a period to read UART and store in internal library buffer
    uint8_t pollDevice(uint32_t timeout);
  private:
    typedef void (*onReceive)(const char *);
    onReceive onReceiveSketchCbk;
    uint8_t readTracesByte();
    uint8_t readTracesBuffer(char* buffer, int size);

};

extern NemeusLib nemeusLib;

#endif /* NEMEUSLIB_H */
