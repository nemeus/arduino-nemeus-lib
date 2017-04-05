/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * MacChannel.cpp - Mac Channel class to manage Mas Channel structure and format.
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

#include "MacChannel.h"
#include "Arduino.h"


MacChannel::MacChannel()
{
  isFrequencyPresent_ = false;
  isMinDrPresent_ = false;
  isMaxDrPresent_ = false;
  isDutyCyclePresent_ = false;
  isChannelNumberPresent_ = false;
}

/**
 * Set the channel and number for the modified channel
 * @param channelNumber  Channel number of the page to modify
 * @param pageNumber  Page number of the channel to modify
 */
void MacChannel::setChannelNumber(uint8_t channelNumber, uint8_t pageNumber)
{
  isChannelNumberPresent_ = true;

  this->channelNumber_ = channelNumber;
  this->pageNumber_ = pageNumber;
}

/**
 * Set the frequency
 * @param frequency  new frequency
 */
void MacChannel::setFrequency(uint32_t frequency)
{
  isFrequencyPresent_ = true;
  this->frequency_ = frequency;
}

/**
 * Set the Min Data Rate as a string
 * @param minDataRate  minimum Data Rate
 */
void MacChannel::setMinDataRate(String minDataRate)
{
  isMinDrPresent_ = true;
  this->minDr_ = minDataRate;
}

/**
 * Set the Max Data Rate as a string
 * @param maxDataRate  minimum Data Rate
 */
void MacChannel::setMaxDataRate(String maxDataRate)
{
  isMaxDrPresent_ = true;
  this->maxDr_ = maxDataRate;
}

/**
 * Set the duty cycle = 100%/(parameter in argument)
 * @param maxDataRate  minimum Data Rate
 */
void MacChannel::setDutyCycle(uint8_t dutyCycle)
{
  isDutyCyclePresent_ = true;
  this->dutyCycle_ = dutyCycle;
}

/**
 * Generate the argument for AT command in string representation
 * @return  the formatted argument with COMMAs & CRLF end of line
 */
char* MacChannel::generateArguments(char* arguments)
{

  if (isChannelNumberPresent_ != true)
  {
    strncat(arguments, (char*)"\r\n", 2);
    goto error;
  }

  strncat(arguments, (char*)String(channelNumber_, DEC).c_str(),3) ;
  strncat(arguments, SEPARATOR, 1);

  if (isFrequencyPresent_)
  {
    strncat(arguments, (char*)String(frequency_, DEC).c_str(), 10);
  }
  strncat(arguments, SEPARATOR, 1);

  if(isMinDrPresent_)
  {
    strncat(arguments, (char*)minDr_.c_str(), minDr_.length());
  }
  strncat(arguments, SEPARATOR, 1);

  if (isMaxDrPresent_)
  {
    strncat(arguments, (char*)maxDr_.c_str(), maxDr_.length());
  }
  strncat(arguments, SEPARATOR, 1);

  if (isDutyCyclePresent_)
  {
    strncat(arguments, (char*)String(dutyCycle_, DEC).c_str(), 3);
  }
  strncat(arguments, SEPARATOR, 1);

  strncat(arguments, (char*)String(pageNumber_, DEC).c_str(), 3);
  strncat(arguments, (char*)"\r\n", 2);

error:

  return arguments;
}


