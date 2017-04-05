/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * NemeusUART.h - Nemeus UART class definition
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
 
#ifndef NEMEUS_UART_H
#define NEMEUS_UART_H

#include <stdint.h>

#include "Arduino.h"
#include "Singleton.h"
#include "AtCommand.h"
#include "Data/DataContext.h"
#include "Utils/CircBuffer.h"
#include "Utils/NemeusTimer.h"

//------------------------------------------
// Use Serial2 for MM002
#define PIN_SERIAL2_RX       (34ul)               // Pin description number for PIO_SERCOM on D12
#define PIN_SERIAL2_TX       (36ul)               // Pin description number for PIO_SERCOM on D10
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)      // SERCOM pad 2
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)    // SERCOM pad 3

// wakeup pin
#define WAKEUP_PIN A3

#define UART_SPEED 38400

enum ERROR_CODE
{
  NEMEUS_SUCCESS = 0,
  NEMEUS_NO_ANSWER = 2,
  NEMEUS_ERROR_NOACK = 3,
  NEMEUS_ARGUMENT_ERROR = 4,
  NEMEUS_WARNING_PAYLOAD_TRUNACTED = 5,
  NEMEUS_ERROR   = 255
};

#define TRACE_BUF_SZ 256


class NemeusUART : public Singleton<NemeusUART>
{
  friend class Singleton<NemeusUART>;

  typedef void (*onReceive)(const char *);
  struct CallbackElement {
    onReceive callbackPtr;
    CallbackElement* next;
  };

  public:
  //static NemeusUART& getInstance();
  uint8_t begin();
  uint8_t reset();
  void end();
  uint8_t sendATCommand(AtCommand atCommand, const char* arguments, uint32_t timeout);
  uint8_t manageAtCommandResponse(char* traceBuffer, uint8_t nbCharacterRead);
  int availableTraces();
  int readTracesByte();
  int readTracesBuffer(char* buffer, int size);
  int readLine(char* buffer, int size);
  void addCallback(onReceive onReceiveFunction);
  void delCallback(onReceive onReceiveFunction);
  uint8_t waitForAtResponse(uint32_t timeout);
  uint8_t pollDevice(uint32_t timeout);
  private:
  //static NemeusUART m_instance;
  NemeusUART();
  ~NemeusUART();
  DataContext * dataContext_;
  CallbackElement* m_onReceiveFunctionList_;
  NemeusTimer* atTimer_;

  /* Methods */
  uint8_t nbCallbacks();
  void wakeUp();
  int readLineInCircularBuffer(char* buffer, int buffer_length);
  void notifyCallbacks(const char* buffer);

};

#endif // NEMEUS_UART_H

