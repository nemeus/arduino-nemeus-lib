/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * RadioParam.cpp - Radio  Param class to manage RF parameters structure and format.
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

#include "RadioParam.h"

/**
 * Constructor. Init boolean to false
 */
RadioParam::RadioParam() {
  isModePresent_ = false;
  isFreqPresent_ = false;
  isBandwidthPresent_ = false;
  isDataRatePresent_ = false;
  isCodeRatePresent_ = false;
}

/**
 * Destructor
 */
RadioParam::~RadioParam() {

}

/**
 * Set the radio mode (LORA or FSK)
 * @param  the radio mode
 */
void RadioParam::setMode(String mode)
{
  this->isModePresent_ = true;
  this->mode_ = mode;
}

/**
 * Set the frequency i Hz
 * @param  frequency in Hz
 */
void RadioParam::setFrequency(uint32_t frequency)
{
  this->isFreqPresent_ = true;
  this->frequency_ = frequency;
}

/**
 * Set the bandwidth i Hz
 * @param  bandwidth in Hz
 */
void RadioParam::setBandwidth(uint32_t bandwidth)
{
  this->isBandwidthPresent_ = true;
  this->bandwidth_ = bandwidth;
}

/**
 * Set the data rate Hz
 * @param  data rate
 */
void RadioParam::setDataRate(uint8_t dataRate)
{
  this->isDataRatePresent_ = true;
  this->dataRate_ = dataRate;
}

/**
 * Set the coding rate
 * @param  coding rate
 */
void RadioParam::setCodeRate(uint8_t codeRate)
{
  this->isCodeRatePresent_ = true;
  this->codeRate_ = codeRate;
}



