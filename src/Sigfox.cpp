/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * Sigfox.cpp - Sigfox singleton to process Sigfox AT command
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

#include "NemeusUART.h"
#include "Sigfox.h"


/**
 * Constructor
 */
Sigfox::Sigfox()
{
  dataContext_ = new DataContext();
  /* register LoRaWAN intern callback to filter some received at response */
  NemeusUART::getInstance()->addCallback(onReceiveFromUART);
}

/**
 * Destructor
 */
Sigfox::~Sigfox()
{
  //(NemeusUART::getInstance()->delCallback(onReceiveFromUART);
}

/**
 * Enable Sigfox
 * @param disDutyCycle if 0, disables the duty cycle for sigfox
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t Sigfox::ON(char * disDutyCycle)
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  if (disDutyCycle != NULL)
  {
    ErrorCode = NemeusUART::getInstance()->sendATCommand(SIGFOX_ON, disDutyCycle, 2000);
  }
  else
  {
    ErrorCode = NemeusUART::getInstance()->sendATCommand(SIGFOX_ON, (char*)"\r\n", 2000);
  }

  return ErrorCode;
}

/**
 * Disable Sigfox
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t Sigfox::OFF()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  ErrorCode = NemeusUART::getInstance()->sendATCommand(SIGFOX_OFF, NULL, 2000);

  return ErrorCode;
}

/**
 * Get size of maximum payload in Sigfox
 * @return  the maximum payload size
 */
uint8_t Sigfox::getMaximumPayloadSize()
{
  return MAXIMUM_SIGFOX_PAYLOAD;
}

#define SIGFOX_SEND_SIZE 14
#define MAXIMUM_ARGUMENT_SIZE 512
/**
 * Send a SIGFOX frame
 * @param payload  null terminated payload buffer
 * @param ack  Ask for Acknowledgement or not
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 *               NEMEUS_ARGUMENT_ERROR if argument format error
 */
uint8_t Sigfox::sendFrame(uint8_t mode, char* payload, boolean ack)
{
  uint8_t ErrorCode = NEMEUS_SUCCESS;
  static char arguments[MAXIMUM_ARGUMENT_SIZE];
  char *pt_arguments; 
  char *pt_arguments_end;
  int index = 0;
  boolean sizeTooBig = false;
  pt_arguments = &arguments[3];
  pt_arguments_end = &arguments[511];

  if (mode == SIGFOX_BINARY_MODE)
  {
    strcpy(arguments, "BIN");
    /* Calculate the size of comamnd */
    if (strlen(payload) > (2*getMaximumPayloadSize()))
    {
      sizeTooBig = true;
    }
  }
  else if (mode == SIGFOX_BIT_MODE)
  {
    strcpy(arguments, "BIT");
    if (strlen(payload) > 1)
    {
      sizeTooBig = true;
    }
  }
  else if ( mode == SIGFOX_OOB_MODE)
  {
    strcpy(arguments, "OOB");
    strncat(pt_arguments, "\r\n", 2);
    index+=2;
  }
  else
  {
    return NEMEUS_ARGUMENT_ERROR;
  }
  index+=3;
  pt_arguments++;


  if (sizeTooBig)
  {

    if (mode == SIGFOX_BINARY_MODE )
    {
      strcat(arguments, ",");
      index++;
      strncat(pt_arguments, payload, (2*getMaximumPayloadSize()));
      pt_arguments += (2*getMaximumPayloadSize());
      index+=(2*getMaximumPayloadSize());
    }
    else if (mode == SIGFOX_BIT_MODE)
    {
      strcat(arguments, ",");
      index++;
      strncat(pt_arguments, payload, 1);
      pt_arguments++;
      index++;
    }
  } 
  else
  {
    if (mode == SIGFOX_BINARY_MODE ||  mode == SIGFOX_BIT_MODE)
    {
      strcat(arguments, ",");
      index++;
      strncat(pt_arguments, payload, strlen(payload));
      index+=strlen(payload);
      pt_arguments += strlen(payload);
    }
  }

  if (mode == SIGFOX_BINARY_MODE ||  mode == SIGFOX_BIT_MODE)
  {
    strncat(pt_arguments, ",", 1);
    pt_arguments++;
    index++;
    if (ack == true)
    {
      strncat(pt_arguments, "1", 1);
    }
    else
    {
      strncat(pt_arguments, "0", 1);
    }
    index++;
    pt_arguments++;
    strncat(pt_arguments, "\r\n", 2);
    index+=2;
    pt_arguments+=2;
  }


  dataContext_->setOngoingAtCommand(SIGFOX_SEND_BINARY);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(SIGFOX_SEND_BINARY, arguments, 20000);

  if ( (ErrorCode == NEMEUS_SUCCESS) && (sizeTooBig == true))
  {
    ErrorCode = NEMEUS_WARNING_PAYLOAD_TRUNACTED;
  }

  return ErrorCode;

}

/**
 * onReceive callback registered to UART to get back AT response (OK, ERROR, +... and so on)
 * @param  buffer  a pointer to the buffer to fill
 */
void Sigfox::onReceiveFromUART(const char * buffer)
{
  getInstance()->treatAtResponse(buffer);
}

#define PREFIX_SIGFOX_RESPONSE "+SF"
/**
 * Treat AT response received
 * @param  buffer  a pointer to the buffer to fill
 */
void Sigfox::treatAtResponse(const char * buffer)
{
  uint8_t index;
  String stringBuffer = String(buffer);

  /* Filter AT response to get some parameters before forwarding to sketch */
  if (strncmp(buffer,  PREFIX_SIGFOX_RESPONSE, sizeof(PREFIX_SIGFOX_RESPONSE)) == 0)
  {
    /* Do some work */
    if (dataContext_->getOngoingAtCommand() == SIGFOX_ON)
    {

    }
  }

}

