/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * DataContext.cpp - Data Context for library
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
 
#include "DataContext.h"
#include "AtCommand.h"

/**
 * Constructor
 */
DataContext::DataContext()
{
  resetOngoingAtCommand();
}

/**
 * Reset the on going AT command
 */
void DataContext::resetOngoingAtCommand()
{
  this->ongoingAtCommand_ = NO_CMD;
}

/**
 * Set the on going AT command to the specified one
 * @param newValue  at command to set
 */
void DataContext::setOngoingAtCommand(AtCommand newValue)
{
  this->ongoingAtCommand_ = newValue;
}

/**
 * Get the on going AT command
 * @return  the at command on process (waiting for response)
 */
AtCommand DataContext::getOngoingAtCommand()
{
  return this->ongoingAtCommand_;
}

