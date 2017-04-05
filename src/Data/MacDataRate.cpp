/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * MacDataRate.cpp - Mac Data Rate class to manage Mas Data Rate structure and format.
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

#include "MacDataRate.h"

/**
 * Constructor. Init boolean to false
 */
MacDataRate::MacDataRate() {
  isDataRatePresent_ = false;
  isTxPowerPresent_ = false;
  isChannelMaskPresent_ = false;
  isChannelMaskCtrlPresent_ = false;
  isNbRepetitionPresent_ = false;
}

/**
 * Set the data rate as a string
 * @param dataRate  the string of data rate to set (i.e "SF7BW125")
 */
void MacDataRate::setDataRate(String dataRate)
{
  this->isDataRatePresent_ = true;
  this->dataRate_ = dataRate;

}

/**
 * Get the data rate as a string
 * @param dataRate  the string of data rate to set (i.e "SF7BW125")
 */
String MacDataRate::getDataRate()
{
  if (isDataRatePresent_)
  {
    return dataRate_;
  }
  else
  {
    return "";
  }
}

/**
 * Set the Tx Power
 * @param txPower  Tx Power to set
 */
void MacDataRate::setTxPower(uint8_t txPower)
{
  this->isTxPowerPresent_ = true;
  this->txPower_ = txPower;

}

/**
 * Set the channel mask
 * @param channelMask  Channel Mask to set
 */
void MacDataRate::setChannelMask(String channelMask)
{
  this->isChannelMaskPresent_ = true;
  this->channelMask_ = channelMask;
}

/**
 * Set the channel mask control
 * @param channelMaskCtrl  Channel Mask control to set
 */
void MacDataRate::setChannelMaskCtrl(String channelMaskCtrl)
{
  this->isChannelMaskCtrlPresent_ = true;
  this->channelMaskCtrl_ = channelMaskCtrl;
}

/**
 * Set the number of repetition
 * @param  nbRepetition  Number of repetition
 */
void MacDataRate::setNbRepetition(uint8_t nbRepetition)
{
  this->isNbRepetitionPresent_ = true;
  this->nbRepetition_ = nbRepetition;
}

/**
 * Generate the argument for AT command in string representation
 * @return  the formatted argument with COMMAs & CRLF end of line
 */
char* MacDataRate::generateArguments(char* arguments)
{
  if (isDataRatePresent_)
  {
    strncat(arguments, (char*)dataRate_.c_str(), dataRate_.length());
  }
  strncat(arguments, SEPARATOR, 1);

  if(isTxPowerPresent_)
  {
    strncat(arguments, (char*)String(txPower_, DEC).c_str(), 3);
  }
  strncat(arguments, SEPARATOR, 1);

  if (isChannelMaskPresent_)
  {
    strncat(arguments, (char*)channelMask_.c_str(), channelMask_.length());
  }
  strncat(arguments, SEPARATOR, 1);

  if (isChannelMaskCtrlPresent_)
  {
    strncat(arguments, (char*)channelMaskCtrl_.c_str(), channelMaskCtrl_.length());
  }
  strncat(arguments, SEPARATOR, 1);

  if (isNbRepetitionPresent_)
  {
    strncat(arguments, (char*)String(nbRepetition_, DEC).c_str(), 3);
  }
  strncat(arguments, (char*)"\r\n", 2);

  return arguments;
}


void MacDataRate::setMacDataRate(bool isDataRatePresent, bool isTxPowerPresent, bool isChannelMaskPresent, bool isChannelMaskCtrlPresent, bool isNbRepetitionPresent)
{
  this->isDataRatePresent_ = isDataRatePresent;
  this->isTxPowerPresent_= isTxPowerPresent;
  this->isChannelMaskPresent_= isChannelMaskPresent;
  this->isChannelMaskCtrlPresent_= isChannelMaskCtrlPresent;
  this->isNbRepetitionPresent_= isNbRepetitionPresent;
}

MacDataRate_t* MacDataRate::getMacDataRate()
{
  return &this->macDataRate_;
}
