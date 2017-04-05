/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * CircBuffer.h - Circular Buffer class definition
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

#ifndef CIRCBUFFER_H
#define CIRCBUFFER_H

#include <Arduino.h>

// Allow debugging/regression testing under normal g++ environment.
#ifdef CB_DEBUG
#include <iostream>
using namespace std;
#endif

#define CIRCULAR_BUFFER_SIZE 4096

class CircBuffer {
  public:
    CircBuffer();

    inline int getCapacity(void) const;
    inline int getSize(void) const;
    inline int getSizeRemaining(void) const;

    inline int available() const;

    // @return number of bytes written
    int write(const char* src, int srcLen);
    int write(char val);

    // @return number of bytes actually read
    int read(char* dest, int destLen);
    int readLine(char* dest, int destLen);
    int read();

    // @return number of bytes copied
    int peek(char* dest, int destLen);
    int skip(int len);

    void clear(void);

#if 0
    inline void status(const char *file = NULL, int line = 0) const;
#endif

  private:
    char* buffer_;
    int bufferLen_;
    char*  writePtr_;
    char* readPtr_;
    int size_;

    inline const char* getEndOfBuffer(void) const;
    // Declare but do not define, copying not permitted
    CircBuffer(CircBuffer const &a);
    const CircBuffer& operator=(CircBuffer const &a);
};


inline int CircBuffer::getCapacity(void) const
{
  return bufferLen_;
}


inline int CircBuffer::getSize(void) const
{
  return size_;
}

inline int CircBuffer::available(void) const
{
  return getSize();
}

inline int CircBuffer::getSizeRemaining(void) const
{
  return bufferLen_ - size_;
}

inline const char* CircBuffer::getEndOfBuffer(void) const
{
  return buffer_ + bufferLen_;
}


#if 0
inline void CircBuffer::status(const char *file, int line) const
{
  cout << "--------------- ";
  if (file != NULL) {
    cout << "File: " << file;
    if (line)
      cout << ':' << line;
  }
  cout << endl;

  cout << "Capacity:        " << bufferLen << endl
    << "read ptr: " << (readPtr-buffer) << endl
    << "write ptr: " << (writePtr-buffer) << endl;
}
#endif


#endif
