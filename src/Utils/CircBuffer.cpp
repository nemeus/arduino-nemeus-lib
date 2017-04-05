/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * CircBuffer.cpp - Circular Buffer class
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

#include "CircBuffer.h"



CircBuffer::CircBuffer()
{
  buffer_ = (char*) malloc(CIRCULAR_BUFFER_SIZE*sizeof(char));
  bufferLen_ = CIRCULAR_BUFFER_SIZE;
  writePtr_ = buffer_;
  readPtr_ = buffer_;
  size_ = 0;
}

int CircBuffer::write(const char* src, int srcLen)
{
  for (int i = 0; i < srcLen; ++i) 
  {
    *writePtr_++ = *src++; // bufferLen MUST be >= 1
    if (writePtr_ >= getEndOfBuffer())
    {
      writePtr_ = buffer_;
    }
  }
  size_ += srcLen;
  if (size_ > bufferLen_) 
  {
    // Overwrote existing data, adjust readPtr
    size_ = bufferLen_;
    readPtr_ = writePtr_;
  }
  return srcLen;
}

int CircBuffer::write(char val)
{
  *writePtr_++ = val;

  if (writePtr_ >= getEndOfBuffer())
  {
    writePtr_ = buffer_;
  }
  
  size_ ++;
  if (size_ > bufferLen_) 
  {
    // Overwrote existing data, adjust readPtr
    size_ = bufferLen_;
    readPtr_ = writePtr_;
  }
  //	printFunction("CircBuffer::write");
  //	printFunction(size);
  return 1;
}


int CircBuffer::read(char* dest, int destLen)
{
  if (destLen > size_)
  {
    destLen = size_;
  }
  
  for (int i = 0; i < destLen; ++i) 
  {
    *dest++ = *readPtr_++;
    if (readPtr_ >= getEndOfBuffer())
    {
      readPtr_ = buffer_;
    }
  }

  size_ -= destLen;
  return destLen;
}

int CircBuffer::readLine(char* dest, int destLen)
{
  int i = 0;
  char *p = readPtr_;
  char byteRead;
  int tempLength;

  tempLength = destLen;

  if (size_ != 0)
  {
    if (tempLength > size_)
    {
      tempLength = size_;
    }
    
    do
    {
      byteRead = *readPtr_++;
      *dest++ = byteRead;
      i++;
      if (readPtr_ >= getEndOfBuffer())
        readPtr_ = buffer_;
    } 
    while ( (byteRead != '\n') && (i<tempLength) );

    /* If last character \n is not present, do not return a chain and restore readPtr and size */
    if ( (i != destLen) && (byteRead != '\n') )
    {
      readPtr_ = p;
      i = 0;
    }
    else
    {
      size_ -= i;
    }
  }


  return i;
}

int CircBuffer::read()
{
  int character = 0;

  if (size_ > 0)
  {
    character = *readPtr_++;
    if (readPtr_ >= getEndOfBuffer())
    {
      readPtr_ = buffer_;
    }
    
    size_--;
  }

  return character;
}


int CircBuffer::peek(char* dest, int destLen)
{
  int s = size_;
  char *p = readPtr_;
  int r = read(dest, destLen);
  // Restore to original state
  size_ = s;
  readPtr_ = p;
  return r;
}


int CircBuffer::skip(int len)
{
  if (len > size_)
  {
    len = size_;
  }
  
  readPtr_ += len;
  if (readPtr_ >= getEndOfBuffer())
  {
    readPtr_ -= bufferLen_;
  }
  
  size_ -= len;
  return len;
}


void CircBuffer::clear(void)
{
  writePtr_ = buffer_;
  readPtr_ = buffer_;
  size_ = 0;
}

