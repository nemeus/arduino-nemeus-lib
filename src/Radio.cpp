/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * Radio.cpp - Radio singleton to process Radio AT command
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
#include "Radio.h"


/**
 * Constructor
 */
Radio::Radio()
{
  dataContext_ = new DataContext();
  isContinuousRx_ = false;
  isContinuousTx_ = false;
  /* register LoRaWAN intern callback to filter some received at response */
  NemeusUART::getInstance()->addCallback(onReceiveFromUART);
}

/**
 * Destructor
 */
Radio::~Radio()
{
  //(NemeusUART::getInstance()->delCallback(onReceiveFromUART);
}

/**
 * Enable Radio
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t Radio::ON()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RADIO_ON, NULL, 2000);

  return ErrorCode;
}

/**
 * Disable Radio
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t Radio::OFF()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RADIO_OFF, NULL, 2000);

  return ErrorCode;
}

/**
 * Get size of maximum payload in Sigfox
 * @return  the maximum payload size
 */
uint8_t Radio::getMaximumPayloadSize()
{
  return MAXIMUM_RADIO_PAYLOAD;
}

#define RADIO_SEND_SIZE 14
#define MAXIMUM_ARGUMENT_SIZE 512
/**
 * Send a Radio frame
 * @param payload  null terminated payload buffer
 * @param nbRepeat  Number of repetition
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 *               NEMEUS_ARGUMENT_ERROR if argument format error
 */
uint8_t Radio::sendFrame(uint8_t mode,char* payload, int nbRepeat)
{
  uint8_t ErrorCode = NEMEUS_SUCCESS;
  static char arguments[MAXIMUM_ARGUMENT_SIZE];
  char *pt_arguments; 
  char *pt_arguments_end;
  int index = 0;
  int size_command_argument = 0;
  boolean sizeTooBig = false;
  pt_arguments = &arguments[4];
  pt_arguments_end = &arguments[511];

  if (mode == RADIO_BINARY_MODE)
  {
    strcpy(arguments, "BIN");

    /* Calculate the size of comamnd */
    if (strlen(payload) > 2*getMaximumPayloadSize())
    {
      sizeTooBig = true;
      size_command_argument = String(nbRepeat, DEC).length() + 3 + 2*getMaximumPayloadSize();
    }
    else
    {
      size_command_argument = String(nbRepeat, DEC).length() + 3 + strlen(payload);
    }

  }
  else if (mode == RADIO_TEXT_MODE)
  {
    strcpy(arguments, "TXT");

    /* Calculate the size of comamnd */
    if (strlen(payload) > getMaximumPayloadSize())
    {
      sizeTooBig = true;
      size_command_argument = String(nbRepeat, DEC).length() + 3 + getMaximumPayloadSize();
    }
    else
    {
      size_command_argument = String(nbRepeat, DEC).length() + 3 + strlen(payload);
    }
  }
  else
  {
    return NEMEUS_ARGUMENT_ERROR;
  }
  index+=3;

  strcat(arguments, ",");
  index++;    

  if (sizeTooBig)
  {
    if (mode == RADIO_BINARY_MODE )
    {
      strncat(pt_arguments, payload, (2*getMaximumPayloadSize()));
      pt_arguments += (2*getMaximumPayloadSize());
      index+=(2*getMaximumPayloadSize());
    }
    else if (mode == RADIO_TEXT_MODE)
    {
      strncat(pt_arguments, payload, (getMaximumPayloadSize()));
      pt_arguments += (getMaximumPayloadSize());
      index+=(getMaximumPayloadSize());
    }

  } 
  else
  {
    strncat(pt_arguments, payload, strlen(payload));
    index+=strlen(payload);
    pt_arguments += strlen(payload);
  }
  strncat(pt_arguments, ",", 1);
  pt_arguments++;
  index++;

  index+=sprintf(arguments+index, "%d", nbRepeat);

  strncat(pt_arguments, "\r\n", 2);
  pt_arguments+=2;
  index+=2;

  dataContext_->setOngoingAtCommand(RADIO_SEND_FRAME);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RADIO_SEND_FRAME, arguments, 20000);

  if ( (ErrorCode == NEMEUS_SUCCESS) && (sizeTooBig == true))
  {
    ErrorCode = NEMEUS_WARNING_PAYLOAD_TRUNACTED;
  }

  return ErrorCode;

}

/**
 * Continuous Rx mode
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 *               NEMEUS_ARGUMENT_ERROR if argument format error
 */
uint8_t Radio::continuousRx()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(RADIO_CONTINUOUS_RX);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RADIO_CONTINUOUS_RX, NULL, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    isContinuousRx_ = true;
  }

  return ErrorCode;
}

/**
 * Stop Rx mode
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 *               NEMEUS_ARGUMENT_ERROR if argument format error
 */
uint8_t Radio::stopRx()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(RADIO_STOP_RX);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RADIO_STOP_RX, NULL, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    isContinuousRx_ = false;
  }

  return ErrorCode;
}

/**
 * Continuous Tx mode
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 *               NEMEUS_ARGUMENT_ERROR if argument format error
 */
uint8_t Radio::continuousTx()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(RADIO_CONTINUOUS_TX);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RADIO_CONTINUOUS_TX, NULL, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    isContinuousTx_ = true;
  }

  return ErrorCode;
}

/**
 * Stop Tx mode
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 *               NEMEUS_ARGUMENT_ERROR if argument format error
 */
uint8_t Radio::stopTx()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(RADIO_STOP_TX);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RADIO_STOP_TX, NULL, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    isContinuousTx_ = false;
  }

  return ErrorCode;
}

/**
 * Set radio Tx paramaters
 * @param params  Radio Tx parameters
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 *               NEMEUS_ARGUMENT_ERROR if argument format error
 */
uint8_t Radio::setRadioTxParam(RadioTxParam radioTxParams)
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  char buffer[512];

  dataContext_->setOngoingAtCommand(RADIO_SET_TX_PARAM);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RADIO_SET_TX_PARAM, radioTxParams.generateArguments(buffer), 2000);

  return ErrorCode;
}

/**
 * Set radio Rx paramaters
 * @param params  Radio Rx parameters
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 *               NEMEUS_ARGUMENT_ERROR if argument format error
 */
uint8_t Radio::setRadioRxParam(RadioRxParam radioRxParams)
{
  uint8_t ErrorCode = NEMEUS_ERROR;
  char buffer[512];

  dataContext_->setOngoingAtCommand(RADIO_SET_RX_PARAM);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(RADIO_SET_RX_PARAM, radioRxParams.generateArguments(buffer), 2000);

  return ErrorCode;
}

/**
 * onReceive callback registered to UART to get back AT response (OK, ERROR, +... and so on)
 * @param  buffer  a pointer to the buffer to fill
 */
void Radio::onReceiveFromUART(const char * buffer)
{
  getInstance()->treatAtResponse(buffer);
}

#define PREFIX_RFTX_RESPONSE "+RFTX:"
#define PREFIX_RFRX_RESPONSE "+RFRX:"
/**
 * Treat AT response received
 * @param  buffer  a pointer to the buffer to fill
 */
void Radio::treatAtResponse(const char * buffer)
{
  uint8_t index;
  String stringBuffer = String(buffer);

  /* Filter AT response to get some parameters before forwarding to sketch */
  if (strncmp(buffer,  PREFIX_RFTX_RESPONSE, sizeof(PREFIX_RFTX_RESPONSE)) == 0)
  {
    if (isContinuousRx_ == true)
    {
      // Add some code if needed
    }
  }
  else if (strncmp(buffer,  PREFIX_RFRX_RESPONSE, sizeof(PREFIX_RFRX_RESPONSE)) == 0)
  {
    if (isContinuousRx_ == true)
    {
      // Add some code if needed
    }

  }

}
