/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * RadioRxParam.h - Radio parameters class definition
 *                  Manage the Radio parameters structure for RF parameetrs commands
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
 
#ifndef RADIORXPARAM_H
#define RADIORXPARAM_H

#include <stdint.h>
#include <Arduino.h>
#include "AtCommand.h"
#include "RadioParam.h"


class RadioRxParam : public RadioParam
{
  public:
    RadioRxParam();
    char* generateArguments(char* arguments);
};

#endif /* RADIORXPARAM_H */
