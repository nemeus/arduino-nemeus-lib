/**
/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * RadioTxParam.cpp - Radio  Param class to manage RF parameters structure and format.
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

#include "RadioTxParam.h"

RadioTxParam::RadioTxParam() : RadioParam(), isTxPowerPresent_(false)
{

}

void RadioTxParam::setTxPower(uint8_t txPower)
{
  this->isTxPowerPresent_ = true;
  this->txPower_ = txPower;
}

char* RadioTxParam::generateArguments(char* arguments)
{
  if (isModePresent_)
  {
    strncat(arguments, (char*)mode_.c_str(), mode_.length());
  }
  strncat(arguments, SEPARATOR, 1);

  if(isFreqPresent_)
  {
    strncat(arguments, (char*)String(frequency_, DEC).c_str(), 10);
  }
  strncat(arguments, SEPARATOR, 1);

  if(isTxPowerPresent_)
  {
    strncat(arguments, (char*)String(txPower_, DEC).c_str(), 3);
  }
  strncat(arguments, SEPARATOR, 1);

  if (isBandwidthPresent_)
  {
    strncat(arguments, (char*)String(bandwidth_, DEC).c_str(), 10);
  }
  strncat(arguments, SEPARATOR, 1);

  if (isDataRatePresent_)
  {
    strncat(arguments, (char*)String(dataRate_, DEC).c_str(), 10);
  }
  strncat(arguments, SEPARATOR, 1);

  if (isCodeRatePresent_)
  {
    strncat(arguments, (char*)String(codeRate_, DEC).c_str(), 3);
  }
  strncat(arguments, (char*)"\r\n", 2);

  return arguments;
}
