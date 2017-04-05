/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * DevPerso.cpp - Device perso class to manage device personalization
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

#include "DevPerso.h"

DevPerso::DevPerso()
{
  devPerso_.devUID = (char*)"";
  devPerso_.appUID = (char*)"";
  devPerso_.appKey = (char*)"";
  devPerso_.devAddr = (char*)"";
  devPerso_.nwkSKey = (char*)"";
  devPerso_.appSKey = (char*)"";
}

/**
 * Set the device UID as a string
 * @param devUID  the device UID
 */
void DevPerso::setOtaaPerso(char devUID[16], char appUID[16], char appKey[32])
{
  this->devPerso_.devUID  = devUID;
  this->devPerso_.appUID = appUID;
  this->devPerso_.appKey = appKey;
}

void DevPerso::setAbpPerso(char devAddr[8], char nwkSKey[32], char appSKey[32])
{
  this->devPerso_.devAddr = devAddr;
  this->devPerso_.nwkSKey = nwkSKey;
  this->devPerso_.appSKey= appSKey;
}

/**
 * Get ABP perso as a string
 * @return abpPerso  the structure containing strings of ABP perso
 */
DevPerso_t* DevPerso::getDevPerso()
{
  return &this->devPerso_;
}

