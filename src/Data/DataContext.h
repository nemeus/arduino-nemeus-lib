/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * DataContext.h - Data Context include for library
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
 
#ifndef DATACONTEXT_H
#define DATACONTEXT_H

#include "AtCommand.h"

class DataContext
{
  public:
    DataContext();
    void resetOngoingAtCommand();
    void setOngoingAtCommand(AtCommand atCommandCode);
    AtCommand getOngoingAtCommand();

  private:
    AtCommand ongoingAtCommand_;
};

#endif /* DATACONTEXT_H */
