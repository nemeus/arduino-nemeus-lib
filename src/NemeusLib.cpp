/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * NemeusLib.cpp - Main nemeus library. 
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
 
#include "NemeusLib.h"

/**
 * Constructor NemeusLib
 */
NemeusLib::NemeusLib()
{
  onReceiveSketchCbk = NULL;
}

/**
 * Get access to LoRaWAN instance
 * @return  LoRaWAN object unique instance
 */
LoRaWAN* NemeusLib::loraWan()
{
  return LoRaWAN::getInstance();
}

/**
 * Get access to Sigfox instance
 * @return  Sigfox object unique instance
 */
Sigfox* NemeusLib::sigfox()
{
  return Sigfox::getInstance();
}

/**
 * Get access to radio instance
 * @return  Sigfox object unique instance
 */
Radio* NemeusLib::radio()
{
  return Radio::getInstance();
}

/**
 * Init the UART port
 */
uint8_t NemeusLib::init()
{
  return NemeusUART::getInstance()->begin();
}

/**
 * Reset the modem
 */
uint8_t NemeusLib::resetModem()
{
  return NemeusUART::getInstance()->reset();
}

/**
 * Close UART port
 */
void NemeusLib::close()
{
  NemeusUART::getInstance()->end();
}

/**
 * Print traces buffer on SerialUSB
 */
void NemeusLib::printTraces()
{
  const int BUFFER_SIZE = 256;
  char buffer[BUFFER_SIZE];
  int nb_bytes;

  if (SerialUSB)
  {
    while (availableTraces() >0)
    {
      SerialUSB.print("mm002 >> .");
      nb_bytes = nemeusLib.readLine(buffer, BUFFER_SIZE);
      SerialUSB.write(buffer, nb_bytes);
    }
  }
}

/**
 * Register a callback for traces and AT responses
 */
void NemeusLib::register_at_response_callback(void (*onReceive)(const char *))
{
  NemeusUART::getInstance()->addCallback(onReceive);
}

/**
 * Unregister a callback for traces and AT responses
 */
void NemeusLib::unregister_at_response_callback(void (*onReceive)(const char *))
{
  NemeusUART::getInstance()->delCallback(onReceive);
}

/**
 * Set module powersaving to ON or OFF
 * @parameter isOn  to enable or disable powersaving
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusLib::setPowersaving(bool isOn)
{
  uint8_t ErrorCode = NEMEUS_ERROR;
  const char* argument;
  if (isOn == true)
  {
    argument = "01\r\n";
  }
  else
  {
    argument = "00\r\n";
  }
  ErrorCode = NemeusUART::getInstance()->sendATCommand(AT_POWER_SET, argument, 2000);

  return ErrorCode;
}

/**
 * Set module trace to ON or OFF
 * @parameter isOn  to enable or disable traces
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusLib::setVerbose(bool isOn)
{
  uint8_t ErrorCode = NEMEUS_ERROR;
  const char* argument;
  if (isOn == true)
  {  
    argument = "ON\r\n";
  }
  else
  {
    argument = "OFF\r\n";
  }
  ErrorCode = NemeusUART::getInstance()->sendATCommand(AT_TRACE, argument, 2000);

  return ErrorCode;
}

/**
 * Reset device by AT command
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusLib::resetDevice()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RESET_COLD, NULL, 5000);

  return ErrorCode;
}

/**
 * Get the version by AT command
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusLib::debugMver()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  ErrorCode = NemeusUART::getInstance()->sendATCommand(DEBUG_MVER, NULL, 5000);

  return ErrorCode;
}

/**
 * Check if traces are available on UART
 * @return  the number of bytes available
 */
uint8_t NemeusLib::availableTraces()
{
  return NemeusUART::getInstance()->availableTraces();
}

/**
 * Read the first byte of traces buffer
 * @return  the first character
 */
uint8_t NemeusLib::readTracesByte()
{
  return NemeusUART::getInstance()->readTracesByte();
}

/**
 * Read a buffer of specified size in buffer
 * @param  buffer  a pointer to the buffer to fill
 * @param size  the maximum size of buffer
 * @return  the number of bytes read
 */
uint8_t NemeusLib::readTracesBuffer(char* buffer, int size)
{
  return NemeusUART::getInstance()->readTracesBuffer(buffer, size);
}

/**
 * Read a buffer of maximum size in buffer until the '\n' LF character
 * @param  buffer  a pointer to the buffer to fill
 * @param size  the maximum size of buffer
 * @return  the number of bytes read
 */
uint8_t NemeusLib::readLine(char* buffer, int size)
{
  int nbBytes;
  nbBytes = NemeusUART::getInstance()->readLine(buffer, size);
  if (buffer[0] == '+')
  {
    /* This is AT response */
    /* Forward to receive callback if it exists */
    if (onReceiveSketchCbk != NULL)
    {
      onReceiveSketchCbk(buffer);
    }
  }
  return nbBytes;
}

/**
 * Poll device to ask for traces during a specified time
 * @param  timeout  the time to read traces on device
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusLib::pollDevice(uint32_t timeout)
{
  return NemeusUART::getInstance()->pollDevice(timeout);
}

NemeusLib nemeusLib = NemeusLib();
