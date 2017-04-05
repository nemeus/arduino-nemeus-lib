/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * Utils.cpp - Utils functions
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

#include "Utils.h"
#include "AtCommand.h"


String getParameterAsString(const char * buffer, int fromIndex)
{
  String stringBuffer = String(buffer);

  return getParameterAsString(stringBuffer, fromIndex);
}

String getParameterAsString(String stringBuffer, int fromIndex)
{
  int nextIndex;

  if (fromIndex == -1)
  {
    return "";
  }

  nextIndex = stringBuffer.indexOf(SEPARATOR, fromIndex);
  if (nextIndex == -1) 
  {
    /* End of String */
    nextIndex = stringBuffer.length();
  }

  return stringBuffer.substring(fromIndex, nextIndex);
}

String boolToString(bool value)
{
  if (value)
  {
    return "true";
  }
  else
  {
    return "false";
  }
}

bool stringToBoolean(String value)
{
  if (value.equals("true"))
  {  
    return true;
  }
  else
  {
    return false;
  }
}

