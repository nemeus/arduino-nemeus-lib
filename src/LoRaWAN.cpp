/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * LoRaWAN.cpp - LoRaWAN singleton to process LoRa AT command
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

#include <NemeusLib.h>
#include "NemeusUART.h"
#include "LoRaWAN.h"
#include "Utils/Utils.h"

/**
 * Constructor
 */
LoRaWAN::LoRaWAN()
{
  devPerso_ = new DevPerso();
  dataContext_ = new DataContext();
  macDataRate_ = new MacDataRate();
  macChannel_ = new MacChannel();
  otaa_ = false;
  loraWANstate_ = false;
  /* Automatic register LoRaWAN intern callback to filter some received at response */
  NemeusUART::getInstance()->addCallback(onReceiveFromUART);
}

/**
 * Destructor
 */
LoRaWAN::~LoRaWAN()
{
  NemeusUART::getInstance()->delCallback(onReceiveFromUART);
}

boolean LoRaWAN::readMacStatus()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_STATUS);
  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_STATUS, NULL, 2000);

  if (ErrorCode != NEMEUS_SUCCESS)
  {
    /* The MAC status has been read and stored in treatAtResponse() function */
    this->loraWANstate_ = false;
    this->otaa_ = false;
  }

  return this->loraWANstate_;
}

DevPerso_t* LoRaWAN::readDevPerso()
{
  this->readMacStatus();
  this->readDevUID();
  this->readAppUID();
  this->readAppKey();

  if(!this->otaa_)
  {
    this->readDevAddr();
    this->readNwkSKey();
    this->readAppSKey();
  }

  return this->devPerso_->getDevPerso();
}

DevPerso_t* LoRaWAN::readAbpPerso()
{
  this->readDevAddr();
  this->readNwkSKey();
  this->readAppSKey();

  return this->devPerso_->getDevPerso();
}

boolean LoRaWAN::isOtaa()
{
  return this->otaa_;
}
#define MAXIMUM_MACON_SIZE 8
/**
 * Enable MAC
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::ON(char loraClass, boolean otaa)
{
  uint8_t ErrorCode = NEMEUS_ERROR;
  uint8_t MacStatus = NEMEUS_ERROR;
  char arguments[MAXIMUM_MACON_SIZE];
  char *pt_arguments; 
  pt_arguments = &arguments[0];

  char devAddr[8];

  this->otaa_ = otaa;
  strcpy(pt_arguments, SEPARATOR);
  pt_arguments++;
  strncat(pt_arguments, &loraClass, 1);
  pt_arguments++;
  strncat(pt_arguments, SEPARATOR, 1);
  pt_arguments++;

  if (otaa == true)
  {
    strncat(pt_arguments, (char*)String(1, DEC).c_str(), 1);
    pt_arguments++;
  }
  else
  {
    strncat(pt_arguments, (char*)String(0, DEC).c_str(), 1);
    pt_arguments++;
  }
  strncat(pt_arguments, (char*)"\r\n", 2);
  pt_arguments+= 2;

  /* Read Encryption */
  ErrorCode = readEncryption();

  /* Enable Unsollicited */
  if (ErrorCode == NEMEUS_SUCCESS)
  {
    ErrorCode = enableUnsollicited();
  }

  /* Read ADR */
  if (ErrorCode == NEMEUS_SUCCESS)
  {
    ErrorCode = readAdr();
  }

  /* Read Data Rate */
  if (ErrorCode == NEMEUS_SUCCESS)
  {
    ErrorCode = readDataRate();
  }

  /* Reset sending delay for Join request */
  this->sendingDelay_ = 0;

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    dataContext_->setOngoingAtCommand(MAC_ON);

    /* Enable MAC */
    ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_ON, arguments, 2000);
    ErrorCode = readDataRate();
  }

  /* Ask for MAC status */
  //  MacStatus = NemeusUART:: getInstance()->sendATCommand(MAC_STATUS, NULL, 5000);

  /* if OTAA, wait for device Addr */
  if ((ErrorCode == NEMEUS_SUCCESS) && (otaa == true))
  {
    /* OTAA is finished when Device Address is not null */
    boolean continueLoop = true;
    const int COUNTER_MAX = 60;
    uint32_t  delay;


    for (int i=0; (i<COUNTER_MAX) && (continueLoop == true); i++)
    {
      /* First, wait for Join request sending*/
      while (this->sendingDelay_ != 0)
      {
        delay = this->sendingDelay_;
        this->sendingDelay_ = 0;
        /* Poll device for traces buffer evacuation */
        NemeusUART::getInstance()->pollDevice(delay);
      }
      /* Poll device for traces buffer evacuation */
      NemeusUART::getInstance()->pollDevice(8000);

      /* Test if deviceAddr has been received in unsollicited response */
      long longDevAddr = deviceAddressToLong(this->devPerso_->getDevPerso()->devAddr);

      if ( (sizeof(this->devPerso_->getDevPerso()->devAddr)!= 0) && (longDevAddr != 0) )
      {
        /* Device Address is present */
        continueLoop = false;
      }
    }

    /* Set the OTAA error code procedure */
    if (continueLoop == true)
    {
      ErrorCode = NEMEUS_ERROR;
    }
    else
    {
      ErrorCode = NEMEUS_SUCCESS;
    }
  }

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    loraWANstate_ = true;
  }

  //macChannel_->isFrequencyPresent_  = true;
  return ErrorCode;
}

#define DEVICE_ADDRESS_SIZE 8

/**
 * Convert device Address to long
 * @return  the device address in long
 */
long LoRaWAN::deviceAddressToLong(String deviceAddr)
{
  char buffer[DEVICE_ADDRESS_SIZE+1];
  char *end;
  long result;

  deviceAddr.toCharArray(buffer, DEVICE_ADDRESS_SIZE+1);

  result = strtol(buffer, &end, 16);

  return result;
}

/**
 * Read Dev Addr
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
char* LoRaWAN::readDevAddr()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_READ_DEVADDR);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_DEVADDR, NULL, 2000);

  if (ErrorCode != NEMEUS_SUCCESS)
  {
    /* The device UID has been read and stored in treatAtResponse() function */
    this->devPerso_->setAbpPerso((char*)"", this->devPerso_->getDevPerso()->nwkSKey, this->devPerso_->getDevPerso()->appSKey);
  }

  return this->devPerso_->getDevPerso()->devAddr;
}

#define DEVADDR_SIZE 8
/**
 * Set the device address
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::setDevAddr(char DevADDR[DEVADDR_SIZE])
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  char arguments[10];
  char* pt_arguments;
  pt_arguments  = &arguments[0];
  strcpy(pt_arguments, DevADDR);
  pt_arguments+=8;
  strncat(pt_arguments, CRLF, 2);

  dataContext_->setOngoingAtCommand(MAC_SET_DEVADDR);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_SET_DEVADDR, arguments, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    this->devPerso_->setAbpPerso(DevADDR,this->devPerso_->getDevPerso()->nwkSKey, this->devPerso_->getDevPerso()->appSKey);
  }

  return ErrorCode;
}

/**
 * Read App Skey
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
char* LoRaWAN::readAppSKey()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_READ_APPSKEY);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_APPSKEY, NULL, 2000);

  if (ErrorCode != NEMEUS_SUCCESS)
  {
    /* The device UID has been read and stored in treatAtResponse() function */
    this->devPerso_->setAbpPerso(this->devPerso_->getDevPerso()->devAddr, this->devPerso_->getDevPerso()->nwkSKey, (char*)"");
  }

  return this->devPerso_->getDevPerso()->appSKey;
}

char* LoRaWAN::readNwkSKey()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_READ_NWKSKEY);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_NWKSKEY, NULL, 2000);

  if (ErrorCode != NEMEUS_SUCCESS)
  {
    /* The device UID has been read and stored in treatAtResponse() function */
    this->devPerso_->setAbpPerso(this->devPerso_->getDevPerso()->devAddr, (char*)"", this->devPerso_->getDevPerso()->appSKey);
  }

  return this->devPerso_->getDevPerso()->nwkSKey;
}

/**
 * Disable MAC
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::OFF()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_OFF);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_OFF, NULL, 2000);

  /* Reset internal state */
  if (ErrorCode == NEMEUS_SUCCESS)
  {
    loraWANstate_ = false;
    if(this->otaa_)
    {
      this->devPerso_->setAbpPerso((char*)"", this->devPerso_->getDevPerso()->nwkSKey, this->devPerso_->getDevPerso()->appSKey);
    }
  }

  return ErrorCode;
}

#define MINIMUM_SND_SIZE 12
#define MAXIMUM_ARGUMENT_SIZE 512
/**
 * Send a frame through LoRaWAN layer
 * @param mode  0 for Binary mode or 1 for Text mode
 * @param repetition  Number of repetition
 * @param macPort  MAC port
 * @param payload  null terminated payload buffer
 * @param ack  Ask for Acknowledgement or not
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 *               NEMEUS_ARGUMENT_ERROR if argument format error
 */
uint8_t LoRaWAN::sendFrame(uint8_t mode, uint8_t repetition, uint8_t macPort, const char* payload, boolean ack, boolean encrypt)
{
  uint8_t ErrorCode = NEMEUS_SUCCESS;
  static char arguments[MAXIMUM_ARGUMENT_SIZE];
  char *pt_arguments; 
  char *pt_arguments_end;
  uint8_t size_command_argument;
  int index = 0;
  boolean sizeTooBig = false;
  pt_arguments = &arguments[4];
  pt_arguments_end = &arguments[511];

  if (this->encryption_ != encrypt)
  {
    ErrorCode = setEncryption(encrypt);
    if (ErrorCode != NEMEUS_SUCCESS)
    {
      return ErrorCode;
    }
  }

  /* Test limitation */
  if ( (repetition>99) || (macPort>99) )
  {
    return NEMEUS_ARGUMENT_ERROR;
  }

  if (mode == BINARY_MODE)
  {
    strcpy(arguments, "BIN");

    /* Calculate the size of comamnd */
    if (strlen(payload) > 2*getMaximumPayloadSize())
    {
      sizeTooBig = true;
      size_command_argument = MINIMUM_SND_SIZE+(2*getMaximumPayloadSize());   
    }
    else
    {
      size_command_argument = MINIMUM_SND_SIZE+strlen(payload);
    }

  }
  else if (mode == TEXT_MODE)
  {
    strcpy(arguments, "TXT");

    /* Calculate the size of comamnd */
    if (strlen(payload) > getMaximumPayloadSize())
    {
      sizeTooBig = true;
      size_command_argument = MINIMUM_SND_SIZE+getMaximumPayloadSize();  
    }
    else
    {
      size_command_argument = MINIMUM_SND_SIZE+strlen(payload);
    }
  }
  else
  {
    return NEMEUS_ARGUMENT_ERROR;
  }
  index+=3;


  if (repetition>=10)
  {
    size_command_argument++;
  }
  if (macPort>=10)
  {
    size_command_argument++;
  }

  strcat(arguments, ",");
  index++;
  if (sizeTooBig)
  {
    if (mode == BINARY_MODE )
    {
      strncat(pt_arguments, payload, (2*getMaximumPayloadSize()));
      pt_arguments += (2*getMaximumPayloadSize());
      index+=(2*getMaximumPayloadSize());
    }
    else if (mode == TEXT_MODE)
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
  index+=sprintf(arguments+index, "%d", repetition);
  strncat(pt_arguments, ",", 1);
  pt_arguments++;
  index++;
  index+=sprintf(arguments+index, "%d", macPort);
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
  pt_arguments+=2;
  index+=2;

  dataContext_->setOngoingAtCommand(MAC_SEND);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_SEND, arguments, 20000);

  if ((ErrorCode == NEMEUS_SUCCESS) && (sizeTooBig == true))
  {
    ErrorCode = NEMEUS_WARNING_PAYLOAD_TRUNACTED;
  }

  return ErrorCode;
}

/**
 * Get Maximum payload size (WARINNG: without FOpt in payload)
 * @param  MacDataRate structure
 * @return  the maximum payload size. 0 if error
 */
uint8_t LoRaWAN::getMaximumPayloadSize()
{
  uint8_t maximumPayloadSize = 0;
  char * buffer;
  uint8_t ErrorCode = NEMEUS_SUCCESS;

  if (macDataRate_->getDataRate().length() == 0)
  {
    /* Data Rate not present, read the Mac Data Rate */
    ErrorCode = readDataRate();
  }

  if ( (ErrorCode == NEMEUS_SUCCESS) && (macDataRate_->getDataRate().length() != 0) )
  {

    buffer = (char*) malloc(macDataRate_->getDataRate().length()+1);
    macDataRate_->getDataRate().toCharArray(buffer, macDataRate_->getDataRate().length()+1);

    if (strcmp(buffer, "SF12BW125") == 0)
    {
      maximumPayloadSize = MAX_LORAWAN_PAYLOAD_1;
    }
    else if (strcmp(buffer, "SF11BW125") == 0)
    {
      maximumPayloadSize = MAX_LORAWAN_PAYLOAD_1;
    }
    else if (strcmp(buffer, "SF10BW125") == 0)
    {
      maximumPayloadSize = MAX_LORAWAN_PAYLOAD_1;
    }
    else if (strcmp(buffer, "SF9BW125") == 0)
    {
      maximumPayloadSize = MAX_LORAWAN_PAYLOAD_2;
    }
    else if (strcmp(buffer, "SF8BW125") == 0)
    {
      maximumPayloadSize = MAX_LORAWAN_PAYLOAD_3;
    }
    else if (strcmp(buffer, "SF7BW125") == 0)
    {
      maximumPayloadSize = MAX_LORAWAN_PAYLOAD_3;
    }
    else if (strcmp(buffer, "SF7BW250") == 0)
    {
      maximumPayloadSize = MAX_LORAWAN_PAYLOAD_3;
    }
    else if (strcmp(buffer, "FSK50KBPS") == 0)
    {
      maximumPayloadSize = MAX_LORAWAN_PAYLOAD_3;
    }

    free(buffer);
  }

  return maximumPayloadSize;

}

/**
 * Set MAC data rate
 * @param  MacDataRate structure
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::setDataRate(MacDataRate_t* macDataRate)
{
  static char buffer[512];
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_SET_DATA_RATE);
  SerialUSB.println("debug");
  SerialUSB.println(this->macDataRate_->generateArguments(buffer));
  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_SET_DATA_RATE, this->macDataRate_->generateArguments(buffer), 2000);


  return ErrorCode;
}


/**
 * Read MAC data rate
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::readDataRate()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_READ_DATA_RATE);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_DATA_RATE, NULL, 2000);

  return ErrorCode;
}

/**
 * Set MAC channel
 * @param  MacChannel structure
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::setChannel(MacChannel macChannel)
{
  static char buffer[512];
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_SET_CHANNEL);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_SET_CHANNEL, macChannel.generateArguments(buffer), 2000);

  return ErrorCode;
}

/**
 * Read MAC channel
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::readChannel(uint8_t channelNumber, uint8_t pageNumber, bool unsolEvent)
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  char arguments[11];
  char* pt_arguments;
  pt_arguments  = &arguments[0];

  strcpy(pt_arguments, (char*)String(channelNumber, DEC).c_str());
  pt_arguments+= String(channelNumber, DEC).length();
  strncat(pt_arguments, SEPARATOR, 1);
  pt_arguments++;
  strncat(pt_arguments, (char*)String(pageNumber, DEC).c_str(), 3);
  pt_arguments+= String(pageNumber, DEC).length();
  strncat(pt_arguments, SEPARATOR,1);
  pt_arguments++;
  if (unsolEvent == true)
  {
    strncat(pt_arguments, (char*)String(1, DEC).c_str(), 1);
  }
  else
  {
    strncat(pt_arguments, (char*)String(0, DEC).c_str(), 1);
  }
  pt_arguments++;
  strncat(pt_arguments, CRLF, 2);
  pt_arguments+=2;

  dataContext_->setOngoingAtCommand(MAC_READ_CHANNEL);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_CHANNEL, arguments, 2000);

  return ErrorCode;
}

/**
 * Enable unsollicited events
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::enableUnsollicited()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  char arguments[10];
  char* pt_arguments;
  pt_arguments  = &arguments[0];

  strcpy(pt_arguments, SEPARATOR);
  pt_arguments++;
  strncat(arguments, SEPARATOR, 1);
  pt_arguments++;
  strncat(arguments, (char*)String(1, DEC).c_str(), 1);
  pt_arguments++;
  strncat(arguments, CRLF, 2);
  pt_arguments+=2;

  dataContext_->setOngoingAtCommand(MAC_READ_CHANNEL);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_CHANNEL, arguments, 2000);

  return ErrorCode;
}

/**
 * Read ADR values
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::readAdr()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_READ_ADR);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_ADR, NULL, 2000);

  return ErrorCode;
}

/**
 * Set ADR value
 * @param  MacChannel structure
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::setAdr(bool adr)
{
  uint8_t ErrorCode = NEMEUS_ERROR;
  char arguments[8];
  char* pt_arguments;
  pt_arguments  = &arguments[0];

  strcpy(pt_arguments, (char*)boolToString(adr).c_str());
  if (boolToString(adr) == "true")
  {
    pt_arguments+=4;
  }
  else 
  {
    pt_arguments+=5;
  }
  strncat(pt_arguments, CRLF, 2);
  pt_arguments++;

  dataContext_->setOngoingAtCommand(MAC_SET_ADR);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_SET_ADR, arguments, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    this->adr_ = adr;
  }

  return ErrorCode;
}

/**
 * Set ADR and piggyback
 * @param  ADR and piggyback booleans
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::setAdr(bool adr, bool piggyback)
{
  uint8_t ErrorCode = NEMEUS_ERROR;
  char arguments[14];
  char* pt_arguments;
  pt_arguments  = &arguments[0];

  strcpy(pt_arguments, (char*)boolToString(adr).c_str());
  if (boolToString(adr) == "true")
  {
    pt_arguments+=4;
  }
  else 
  {
    pt_arguments+=5;
  }

  strncat(pt_arguments, SEPARATOR, 1);
  pt_arguments++;
  strncat(pt_arguments, (char*)boolToString(piggyback).c_str(), 5);
  if (boolToString(adr) == "true")
  {
    pt_arguments+=4;
  }
  else 
  {
    pt_arguments+=5;
  }
  strncat(arguments, CRLF,2);
  pt_arguments+=2;

  dataContext_->setOngoingAtCommand(MAC_SET_ADR);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_SET_ADR, arguments, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    this->adr_ = adr;
    this->piggyback_ = piggyback;
  }

  return ErrorCode;
}


/**
 * Set encryption
 * @param  Encryption boolean
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::setEncryption(boolean encrypt)
{
  uint8_t ErrorCode = NEMEUS_ERROR;
  char arguments[6];
  char* pt_arguments;
  pt_arguments  = &arguments[0];
  strcpy(pt_arguments, SEPARATOR);
  pt_arguments++;
  strncat(pt_arguments, SEPARATOR, 1);
  pt_arguments++;
  if (encrypt == true)
  {
    strncat(pt_arguments, (char*)String(1).c_str(), 1);
  }
  else
  {
    strncat(pt_arguments, (char*)String(0).c_str(), 1);
  }
  pt_arguments++;
  strncat(pt_arguments, CRLF, 2);
  pt_arguments+=2;

  dataContext_->setOngoingAtCommand(MAC_SET_VAR);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_SET_VAR, arguments, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    this->encryption_ = encrypt;
  }

  return ErrorCode;
}

/**
 * Read encryption value
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::readEncryption()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_READ_VAR);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_VAR, NULL, 2000);

  return ErrorCode;
}

/**
 * Read device UID
 * @return  the device UID as a string
 *         the string is empty if error occured
 */
char* LoRaWAN::readDevUID()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_READ_DEVUID);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_DEVUID, NULL, 2000);

  if (ErrorCode != NEMEUS_SUCCESS)
  {
    /* The device UID has been read and stored in treatAtResponse() function */
    this->devPerso_->setOtaaPerso((char*)"", this->devPerso_->getDevPerso()->appUID, this->devPerso_->getDevPerso()->appKey);
  }
  return this->devPerso_->getDevPerso()->devUID;

}

/**
 * Read app UID
 * @return  the app UID as a string
 *         the string is empty if error occured
 */
char* LoRaWAN::readAppUID()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_READ_APPUID);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_APPUID, NULL, 2000);

  if (ErrorCode != NEMEUS_SUCCESS)
  {
    /* The app UID has been read and stored in treatAtResponse() function */
    this->devPerso_->setOtaaPerso(this->devPerso_->getDevPerso()->devUID, (char*)"", this->devPerso_->getDevPerso()->appKey);
  }
  return this->devPerso_->getDevPerso()->appUID;
}

#define APPUID_SIZE 16
/**
 * Set the app UID value
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::setAppUID(char appUID[APPUID_SIZE])
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  char arguments[18];
  char* pt_arguments;
  pt_arguments  = &arguments[0];

  strcpy(pt_arguments, (char*)appUID);
  pt_arguments+=8;
  strncat(pt_arguments, CRLF, 2);

  dataContext_->setOngoingAtCommand(MAC_SET_APPUID);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_SET_APPUID, arguments, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    this->devPerso_->setOtaaPerso(this->devPerso_->getDevPerso()->devUID, appUID, this->devPerso_->getDevPerso()->appKey);
  }

  return ErrorCode;
}

/**
 * Read app Key
 * @return  the app Key as a string
 *         the string is empty if error occured
 */
char* LoRaWAN::readAppKey()
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  dataContext_->setOngoingAtCommand(MAC_READ_APPKEY);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_READ_APPKEY, NULL, 2000);

  if (ErrorCode != NEMEUS_SUCCESS)
  {
    /* The app key has been read and stored in treatAtResponse() function */
    this->devPerso_->setOtaaPerso(this->devPerso_->getDevPerso()->devUID, this->devPerso_->getDevPerso()->appUID, (char*)"");
  }

  return this->devPerso_->getDevPerso()->appKey;
}

#define APPKEY_SIZE 32
/**
 * Set the app Key value
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t LoRaWAN::setAppKey(char appKey[APPKEY_SIZE])
{
  uint8_t ErrorCode = NEMEUS_ERROR;

  char arguments[34];
  char* pt_arguments;
  pt_arguments  = &arguments[0];
  strcpy(pt_arguments, appKey);
  pt_arguments+=32;
  strncat(pt_arguments, CRLF, 2);

  dataContext_->setOngoingAtCommand(MAC_SET_APPKEY);

  ErrorCode = NemeusUART::getInstance()->sendATCommand(MAC_SET_APPKEY, arguments, 2000);

  if (ErrorCode == NEMEUS_SUCCESS)
  {
    this->devPerso_->setOtaaPerso(this->devPerso_->getDevPerso()->devUID, this->devPerso_->getDevPerso()->appUID, appKey);
  }

  return ErrorCode;
}

/**
 * onReceive callback registered to UART to get back AT response (OK, ERROR, +... and so on)
 * @param  buffer  a pointer to the buffer to fill
 */
void LoRaWAN::onReceiveFromUART(const char * buffer)
{
  getInstance()->treatAtResponse(buffer);
}

#define PREFIX_MAC_RESPONSE "+MAC:"
/**
 * Treat AT response received
 * @param  buffer  a pointer to the buffer to fill
 */
void LoRaWAN::treatAtResponse(const char * buffer)
{
  uint8_t index;
  String stringBuffer = String(buffer);


  /* Filter AT response to get some parameters before forwarding to sketch */
  if (strncmp(buffer,  PREFIX_MAC_RESPONSE, strlen(PREFIX_MAC_RESPONSE)) == 0)
  {

    if (!unsollicitedResponse(buffer))
    {
      /* Do some work */
      if (dataContext_->getOngoingAtCommand() == MAC_ON)
      {

      }
      else if (dataContext_->getOngoingAtCommand() == MAC_OFF)
      {

      }
      else if (dataContext_->getOngoingAtCommand() == MAC_READ_ADR)
      {
        index = stringBuffer.indexOf(COLON)+2;
        this->adr_ = stringToBoolean(getParameterAsString(stringBuffer, index));

        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        this->piggyback_ = stringToBoolean(getParameterAsString(stringBuffer, index));
      }
      else if (dataContext_->getOngoingAtCommand() == MAC_READ_CHANNEL)
      {

      }
      else if (dataContext_->getOngoingAtCommand() == MAC_READ_DATA_RATE)
      {
        parseMacReadDataRate(stringBuffer);
      }
      else if (dataContext_->getOngoingAtCommand() == MAC_SEND)
      {

      }
      else if (dataContext_->getOngoingAtCommand() == MAC_SET_ADR)
      {

      }
      else if (dataContext_->getOngoingAtCommand() == MAC_SET_CHANNEL)
      {

      }
      else if (dataContext_->getOngoingAtCommand() == MAC_SET_DATA_RATE)
      {

      }
      else if (dataContext_->getOngoingAtCommand() == MAC_STATUS)
      {
        /* Position index at begin of parameters */
        index = stringBuffer.indexOf(COLON)+2;
        if ((getParameterAsString(stringBuffer, index).equals("ON") )
            || (getParameterAsString(stringBuffer, index).equals("DUAL") ))
        {
          loraWANstate_ = true;
        }
        else
        {
          loraWANstate_ = false;
        }
        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        /* skip version */
        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        /* skip class */
        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        /* skip nb pages */
        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        /* skip ISM band */
        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        if (getParameterAsString(stringBuffer, index).toInt() == 1)
        {
          this->otaa_ = true;
        }
        else
        {
          this->otaa_ = false;
        }
      }
      else if (dataContext_->getOngoingAtCommand() == MAC_READ_VAR)
      {
        /* Position index at begin of parameters */
        index = stringBuffer.indexOf(COLON)+2;
        /* Skip the first 3 parameters */
        /* txcounter */
        /* Read tx counter */
        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        /* rxcounter */
        /* Read rx counter */
        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        /* aggregateddc */
        /* Read aggr */
        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        /* encryption */
        if (getParameterAsString(stringBuffer, index).toInt() == 1)
        {
          this->encryption_ = true;
        } 
        else
        {
          this->encryption_ = false;
        }

      }
      else if (dataContext_->getOngoingAtCommand() == MAC_READ_DEVUID)
      {
        static char parameterDEVUID[17];

        /* Position index at begin of parameters */
        index = stringBuffer.indexOf(COLON)+2;

        strcpy(parameterDEVUID , (char*)getParameterAsString(stringBuffer, index).c_str());  

        this->devPerso_->setOtaaPerso(parameterDEVUID, this->devPerso_->getDevPerso()->appUID, this->devPerso_->getDevPerso()->appKey);

      }

      else if (dataContext_->getOngoingAtCommand() == MAC_READ_APPUID)
      {
        static char parameterAPPUID[17];

        /* Position index at begin of parameters */
        index = stringBuffer.indexOf(COLON)+2;

        strcpy(parameterAPPUID, (char*)getParameterAsString(stringBuffer, index).c_str());

        this->devPerso_->setOtaaPerso(this->devPerso_->getDevPerso()->devUID,parameterAPPUID, this->devPerso_->getDevPerso()->appKey);

      }
      else if (dataContext_->getOngoingAtCommand() == MAC_READ_APPKEY)
      {
        static char parameterAPPKEY[33];

        /* Position index at begin of parameters */
        index = stringBuffer.indexOf(COLON)+2;

        strcpy(parameterAPPKEY,(char*)getParameterAsString(stringBuffer, index).c_str());

        this->devPerso_->setOtaaPerso(this->devPerso_->getDevPerso()->devUID, this->devPerso_->getDevPerso()->appUID, parameterAPPKEY);
      }
      else if (dataContext_->getOngoingAtCommand() == MAC_READ_DEVADDR)
      {
        static char parameterDEVADDR[9];
        /* Position index at begin of parameters */
        index = stringBuffer.indexOf(COLON)+2;

        strcpy(parameterDEVADDR, (char*)getParameterAsString(stringBuffer, index).c_str());

        this->devPerso_->setAbpPerso(parameterDEVADDR, this->devPerso_->getDevPerso()->nwkSKey, this->devPerso_->getDevPerso()->appSKey);
        index = stringBuffer.indexOf(SEPARATOR, index)+1;
        /* Network ID */
        getParameterAsString(stringBuffer, index);
      }
      else if (dataContext_->getOngoingAtCommand() == MAC_READ_APPSKEY)
      {
        static char parameterAPPSKEY[33];
        /* Position index at begin of parameters */
        index = stringBuffer.indexOf(COLON)+2;

        strcpy(parameterAPPSKEY, (char*)getParameterAsString(stringBuffer, index).c_str());

        this->devPerso_->setAbpPerso(this->devPerso_->getDevPerso()->devAddr, this->devPerso_->getDevPerso()->nwkSKey, parameterAPPSKEY);
      }
      else if (dataContext_->getOngoingAtCommand() == MAC_READ_NWKSKEY)
      {
        static char parameterNWKSKEY[33];
        /* Position index at begin of parameters */
        index = stringBuffer.indexOf(COLON)+2;

        strcpy(parameterNWKSKEY, (char*)getParameterAsString(stringBuffer, index).c_str());

        this->devPerso_->setAbpPerso(this->devPerso_->getDevPerso()->devAddr, parameterNWKSKEY, this->devPerso_->getDevPerso()->appSKey);
      }

    }
  }
  else if (strncmp(buffer, RSP_AT_OK, strlen(RSP_AT_OK)) == 0)
  {
    /* OK */
    dataContext_->resetOngoingAtCommand();
  }
  else if (strncmp(buffer, RSP_AT_ERR, strlen(RSP_AT_ERR)) == 0)
  {
    dataContext_->resetOngoingAtCommand();
  }

}

/**
 * Check if this is an unsollicited response
 * @param  buffer  a pointer to the buffer to fill
 */
boolean LoRaWAN::unsollicitedResponse(const char * buffer)
{
  int index = 0, i;
  boolean unsollicitedReturn = false;
  String stringBuffer = String(buffer);

  for (i=0; i< sizeof(table_LORAWAN_UNSOLLICITED)/sizeof(table_LORAWAN_UNSOLLICITED[0]); i++)
  {
    if (strncmp(buffer, table_LORAWAN_UNSOLLICITED[i], strlen(table_LORAWAN_UNSOLLICITED[i])) == 0 )
    {
      unsollicitedReturn = true;
      break;
    }
  }

  if (strncmp(buffer, LORAWAN_RDEVADDR_UNSOL, strlen(LORAWAN_RDEVADDR_UNSOL)) == 0 )
  {
    /* +MAC: RDEVADDR,0870C367,010203 */
    /* Position index at begin of parameters */
    index = stringBuffer.indexOf(SEPARATOR)+1;
    static char parameterDEVADDR_UNSOL[9];
    strcpy(parameterDEVADDR_UNSOL, getParameterAsString(stringBuffer, index).c_str());
    this->devPerso_->setAbpPerso(parameterDEVADDR_UNSOL, this->devPerso_->getDevPerso()->nwkSKey, this->devPerso_->getDevPerso()->appSKey);
    index = stringBuffer.indexOf(SEPARATOR, index)+1;
    /* Network ID */
    getParameterAsString(stringBuffer, index);
  }
  else if (strncmp(buffer, LORAWAN_RDR_UNSOL, strlen(LORAWAN_RDR_UNSOL)) == 0 )
  {
    parseMacReadDataRate(stringBuffer);
  }
  else if (strncmp(buffer, LORAWAN_SEND_UNSOL, strlen(LORAWAN_SEND_UNSOL)) == 0 )
  {
    if (dataContext_->getOngoingAtCommand() == MAC_ON)
    {
      /* Manage extra time for send */
      index = stringBuffer.indexOf(SEPARATOR)+1;
      this->sendingDelay_ = getParameterAsString(stringBuffer, index).toInt();

      if (SerialUSB)
      {
        SerialUSB.print("NemeusLib(LoRaWAN)>>Sending Join Request delayed of: ");
        SerialUSB.println(this->sendingDelay_, DEC);
      }
    }
  }

  return unsollicitedReturn;
}


/**
 * Parse the AT+MAC= RDR response
 * @param  buffer  a pointer to the buffer to fill
 */
void LoRaWAN::parseMacReadDataRate(String stringBuffer)
{
  int index = 0;
  uint8_t ErrorCode = NEMEUS_ERROR;

  /* Get index (first parameter) */
  if (stringBuffer.startsWith(LORAWAN_RDR_UNSOL)) 
  {
    index = stringBuffer.indexOf(SEPARATOR)+1;
  } 
  else
  {
    index = stringBuffer.indexOf(COLON)+2;
  }

  macDataRate_->setDataRate(getParameterAsString(stringBuffer, index));
  /* Power */
  index = stringBuffer.indexOf(SEPARATOR, index)+1;
  macDataRate_->setTxPower((uint8_t)getParameterAsString(stringBuffer, index).toInt());

  /* Channel Mask */
  index = stringBuffer.indexOf(SEPARATOR, index)+1;
  macDataRate_->setChannelMask(getParameterAsString(stringBuffer, index));

  /* Channel Mask Ctrl */
  index = stringBuffer.indexOf(SEPARATOR, index)+1;
  macDataRate_->setChannelMaskCtrl(getParameterAsString(stringBuffer, index));

  /* Nb Repetition */
  index = stringBuffer.indexOf(SEPARATOR, index)+1;
  macDataRate_->setNbRepetition((uint8_t)getParameterAsString(stringBuffer, index).toInt());


}
