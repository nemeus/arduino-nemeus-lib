/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * NemeusUART.cpp - UART class to manage UART operation
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
 
#include "NemeusUART.h"
#include "Utils/Utils.h"
#include "LoRaWAN.h"
#include "Sigfox.h"

// Instantiate the Serial2 class
Uart Serial2(&sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);
/* The circular buffer of incoming traces */
CircBuffer circularBuffer;
/* The circular buffer where traces are stored */
CircBuffer circularTraceBuffer;

// Interrupt handler for SERCOM1
void SERCOM1_Handler()
{
  /* Copy SERCOM1 buffer (UART) in Serial2 buffer */
  Serial2.IrqHandler();
  while(Serial2.available())
  {
    circularBuffer.write((char)Serial2.read());
  }
}

/**
 * Private constructor (Singleton concept)
 */
NemeusUART::NemeusUART() {
  dataContext_ = new DataContext();
  atTimer_ = new NemeusTimer();
  m_onReceiveFunctionList_ = NULL;
}

/**
 * Private destructor (probably never called)
 */
NemeusUART::~NemeusUART() {
  end();

  /* Remove all calbacks */
  while (m_onReceiveFunctionList_ != NULL)
  {
    CallbackElement* temp = m_onReceiveFunctionList_;
    m_onReceiveFunctionList_ = m_onReceiveFunctionList_->next;
    free(temp);
  }
}

/**
 * Open Serial2 (UART) and test with an AT command
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusUART::begin()
{
  Serial2.begin(UART_SPEED);

  while(!Serial2)
  {
    /*Serial2 not ready*/
    ;
  }

  /* Set a global timeout to 1 seconds */
  Serial2.setTimeout(1000);

  Serial2.write("\r\n", 2);
  delay(2); 

  /* Test modem response to a status AT Command */
  return sendATCommand(RF_STATUS, NULL, 5000);

}

/**
 * Reset the modem with an AT command
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusUART::reset()
{
  uint8_t ret;

  Serial2.begin(UART_SPEED);

  while(!Serial2)
  {
    /*Serial2 not ready*/
    ;
  }

  /* Set a global timeout to 1 second */
  Serial2.setTimeout(1000);

  Serial2.write("\r\n", 2);
  delay(2); 

  /* Send AT command for cold reset then wait some delay (minimum ~ 500)
     to prevent others commands before reset*/
  ret = sendATCommand(RESET_COLD, NULL, 5000);
  delay(1000);
  return (ret);
}

/**
 * Close the Serial2 (UART)
 */
void NemeusUART::end()
{
  Serial2.end();
}

/**
 * Add a callback function to the list of callback function
 * @param  pointer on callback function
 */
void NemeusUART::addCallback(onReceive onReceiveFunction)
{
  CallbackElement* element = (CallbackElement*) malloc(sizeof(CallbackElement));

  /* Create element */
  element->callbackPtr = onReceiveFunction;
  element->next = NULL;

  /* If list doesn't exist */
  if (m_onReceiveFunctionList_ == NULL)
  {
    /* First element */
    m_onReceiveFunctionList_ = element;
  }
  else
  {
    CallbackElement* temp = m_onReceiveFunctionList_;
    while (temp->next != NULL)
    {
      temp = temp->next;
    }
    temp->next = element;
  }

}

/**
 * Remove a callback function from the list of callback function
 * @param  pointer on callback function to remove
 */
void NemeusUART::delCallback(onReceive onReceiveFunction)
{

  if (m_onReceiveFunctionList_ == NULL)
    return;

  CallbackElement* temp = m_onReceiveFunctionList_;
  if (temp->callbackPtr == onReceiveFunction)
  {
    m_onReceiveFunctionList_ = temp->next;
    free(temp);
    return;
  }

  CallbackElement* previousElement = NULL;
  while ( (temp->next != NULL) )
  {
    previousElement = temp;
    temp = temp->next;
    if (temp->callbackPtr == onReceiveFunction)
    {
      previousElement->next = temp->next;
      free(temp);
      temp = previousElement;
    }
  }

}


/**
 * Wake up MM002 with wake up pin
 */
void NemeusUART::wakeUp()
{
#ifdef NEMEUS_LIB_DEBUG
  SerialUSB.println("mm002 >>>> WAKE UP!");
#endif
  pinMode(WAKEUP_PIN, OUTPUT);
  digitalWrite(WAKEUP_PIN, HIGH);
  delay(10);
  digitalWrite(WAKEUP_PIN, LOW);
  pinMode(WAKEUP_PIN, INPUT);
  delay(100);
}

/**
 * Number of callback function in list
 * @return  the number of callbacks function
 */
uint8_t NemeusUART::nbCallbacks() {
  uint8_t nbElement =0;

  CallbackElement* temp = m_onReceiveFunctionList_;

  while(temp!=NULL)
  {
    nbElement++;
    temp = temp->next;
  }

  return nbElement;
}

/**
 * Notify all callbacks from a string
 * @param  the string to notify to all callbacks
 */
void NemeusUART::notifyCallbacks(const char* traces)
{
  CallbackElement* element = m_onReceiveFunctionList_;
  while(element != NULL)
  {
    element->callbackPtr(traces);
    element = element->next;
  }
}

/**
 * Get Serial2 buffer until LF
 */
int NemeusUART::readLineInCircularBuffer(char* serial_buffer, int serial_buffer_length)
{
  int nbBytesRead = 0;

  if (circularBuffer.available() > 0)
  {
    nbBytesRead = circularBuffer.readLine(serial_buffer, serial_buffer_length);
  }

  return nbBytesRead;

}

/**
 * Decode AT response to the ongoing AT command
 * @param traceBuffer  pointer on data
 * @param nbCharacterRead  number of read character
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusUART::manageAtCommandResponse(char* traceBuffer, uint8_t nbCharacterRead)
{
  uint8_t ret = NEMEUS_NO_ANSWER;
  int index;
  uint32_t extraTime;
  String stringBuffer;

  /* Response received */
  if (dataContext_->getOngoingAtCommand() != NO_CMD)
  {
    if (strncmp(traceBuffer, RSP_AT_OK, strlen(RSP_AT_OK)) == 0)
    {
      /* OK */
      dataContext_->resetOngoingAtCommand();
      ret = NEMEUS_SUCCESS;

      notifyCallbacks("OK");
    }
    else if (strncmp(traceBuffer, RSP_AT_ERR_NOACK, strlen(RSP_AT_ERR_NOACK)) == 0)
    {
      /* ERROR NO ACK */
      dataContext_->resetOngoingAtCommand();
      ret = NEMEUS_ERROR_NOACK;
      notifyCallbacks("ERROR NOT ACK");
    }
    else if (strncmp(traceBuffer, RSP_AT_ERR, strlen(RSP_AT_ERR)) == 0)
    {
      /* ERROR */
      dataContext_->resetOngoingAtCommand();
      ret = NEMEUS_ERROR;
      notifyCallbacks("ERROR");
    }
    else if (traceBuffer[0] == '+')
    {
      if ( (strncmp(traceBuffer, SF_SEND_UNSOL, strlen(SF_SEND_UNSOL)) == 0)
          || (strncmp(traceBuffer, LORAWAN_SEND_UNSOL, strlen(LORAWAN_SEND_UNSOL)) == 0) )
      {
        /* Manage extra time for send */
        stringBuffer = String(traceBuffer);

        index = stringBuffer.indexOf(SEPARATOR)+1;
        extraTime = getParameterAsString(stringBuffer, index).toInt();

        /* New Timeout with 4000 ms of margin */
        if (extraTime != 0)
        {
          atTimer_->setTimeout(extraTime+4000);
        }
      }

      notifyCallbacks(traceBuffer);
    }
  }

  return ret;

}

/**
 * Send AT response and wait for response during a specified time
 * @param atCommand  At command
 * @param arguments  extra argument if needed (NULL otherwise)
 * @param timeout  timeout in ms to consider module doesn't answer
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusUART::sendATCommand(AtCommand atCommand, const char* arguments, uint32_t timeout)
{
  char* formattedCommand;
  uint8_t returnValue = NEMEUS_NO_ANSWER;
  int commandSize = 0;

  /* Register command */
  dataContext_->setOngoingAtCommand(atCommand);

  if (atCommand.getStringCommand() != NULL)
  {
    commandSize += strlen(atCommand.getStringCommand());
  }
  else
  {
    goto error_send;
  }

  if (arguments != NULL)
  {
    commandSize += strlen(arguments);
  }

  if (commandSize != 0)
  {
    formattedCommand = (char*)malloc(commandSize);
    strcpy(formattedCommand, atCommand.getStringCommand());

    if (arguments != NULL)
    {
      strcat(formattedCommand, arguments);
    }

    /* wakeup MM002 if powersaving is enabled */
    wakeUp();

    /* Send AT command */
    {
      int remaining = commandSize;
      char* idx = formattedCommand;
      while(remaining--)
      {
        /* add delay between chars when traces are enabled */
        Serial2.write(idx++, 1);
        delay(1);
      }
    }

    if (SerialUSB)
    {
#ifdef NEMEUSLIB_DEBUG
      SerialUSB.print("NemeusLib(UART)>>>> Send AT command (size = ");
      SerialUSB.print(commandSize);
      SerialUSB.print("): ");
      SerialUSB.print(formattedCommand);
#else
      SerialUSB.println("");
      SerialUSB.print("mm002 << ");
      SerialUSB.print(formattedCommand);
#endif
    }

    returnValue = waitForAtResponse(timeout);

    free(formattedCommand);
  }
  else
  {
    returnValue = NEMEUS_ERROR;
    goto error_send;
  }

error_send:
  dataContext_->resetOngoingAtCommand();

  return returnValue;

}

/**
 * Wait for AT response during timeout ms
 * @param timeout  timeout in ms to consider module doesn't answer
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusUART::waitForAtResponse(uint32_t timeout)
{
  char serial_buffer[TRACE_BUF_SZ];
  int serial_buffer_length;
  bool stop = false;
  int returnValue = NEMEUS_NO_ANSWER;

  memset(serial_buffer, 0, TRACE_BUF_SZ);

  /* Set the timer */
  atTimer_->setTimeout(timeout);

  while( (atTimer_->isTimeout() == false) && (stop == false) )
  {
    /* Read a Serial line */
    serial_buffer_length = readLineInCircularBuffer(serial_buffer, TRACE_BUF_SZ);

    if ( (serial_buffer_length > 0) && (serial_buffer[serial_buffer_length-1] == '\n') )
    {
      /* End of line */
      if ( (serial_buffer[0] == '+')
          || ( (serial_buffer[0] == 'O') && (serial_buffer[1] == 'K') )
          || ( (serial_buffer[0] == 'E') && (serial_buffer[1] == 'R') ) )
      {

        /*Remove the carriage return */
        if (serial_buffer[serial_buffer_length-2] == '\r')
        {
          serial_buffer[serial_buffer_length-2] = '\0'; 
          serial_buffer_length= serial_buffer_length -2;
        }
        else // only remove the line feed
        {
          serial_buffer[serial_buffer_length-1] = '\0'; 
          serial_buffer_length= serial_buffer_length -1;
        }
        returnValue = manageAtCommandResponse(serial_buffer, serial_buffer_length);
        /* Break on OK & ERROR */
        if (( (serial_buffer[0] == 'O') && (serial_buffer[1] == 'K') )
            || ( (serial_buffer[0] == 'E') && (serial_buffer[1] == 'R') ) )
        {
          stop = true;
        }
      }
      else
      {
        /* Simple trace */
        circularTraceBuffer.write(serial_buffer, serial_buffer_length);
      }
      /* Reset buffer */
      memset(serial_buffer, 0, serial_buffer_length);
      serial_buffer_length = 0;
    }
  }

  return returnValue;
}

/**
 * Poll device to get unsollicited or downlink payload
 * @param timeout  timeout in ms to consider module doesn't answer
 * @return  the error code
 *               NEMEUS_OK if response is OK
 *               NEMEUS_ERROR if response is ERROR
 *               NEMEUS_NO_ANSWER if no response from module
 */
uint8_t NemeusUART::pollDevice(uint32_t timeout)
{
  char serial_buffer[TRACE_BUF_SZ];
  int serial_buffer_length;
  bool stop = false;
  int returnValue = NEMEUS_SUCCESS;

  memset(serial_buffer, 0, TRACE_BUF_SZ);

  /* This is on 32 bits. The overflow is correctly managed? No!! */
  atTimer_->setTimeout(timeout);

  while(atTimer_->isTimeout() == false)
  {
    /* Read a Serial line */
    serial_buffer_length = readLineInCircularBuffer(serial_buffer, TRACE_BUF_SZ);

    if ( (serial_buffer_length > 0) && (serial_buffer[serial_buffer_length-1] == '\n') )
    {
      /* End of line */
      if ( (serial_buffer[0] == '+')
          || ( (serial_buffer[0] == 'O') && (serial_buffer[1] == 'K') )
          || ( (serial_buffer[0] == 'E') && (serial_buffer[1] == 'R') ) )
      {
        /*Remove the carriage return */
        if (serial_buffer[serial_buffer_length-2] == '\r')
        {
          serial_buffer[serial_buffer_length-2] = '\0'; 
          serial_buffer_length= serial_buffer_length -2;
        }
        else // only remove the line feed
        {
          serial_buffer[serial_buffer_length-1] = '\0'; 
          serial_buffer_length= serial_buffer_length -1;
        }
        notifyCallbacks(serial_buffer);
      }
      else
      {
        /* Simple trace */
        circularTraceBuffer.write(serial_buffer, serial_buffer_length);
      }
      /* Reset buffer */
      memset(serial_buffer, 0, serial_buffer_length);
      serial_buffer_length = 0;
    }
  }
}

/**
 * Check if traces are available on UART
 * @return  the number of bytes available
 */
int NemeusUART::availableTraces()
{
  return circularTraceBuffer.available();
}

/**
 * Read the first byte of traces buffer
 * @return  the first character
 */
int NemeusUART::readTracesByte()
{
  return circularTraceBuffer.read();
}

/**
 * Read a buffer of specified size in buffer
 * @param  buffer  a pointer to the buffer to fill
 * @param size  the maximum size of buffer
 * @return  the number of bytes read
 */
int NemeusUART::readTracesBuffer(char* buffer, int size)
{
  return circularTraceBuffer.read(buffer, size);
}

/**
 * Read a buffer of maximum size in buffer until the '\n' LF character
 * @param  buffer  a pointer to the buffer to fill
 * @param size  the maximum size of buffer
 * @return  the number of bytes read
 */
int NemeusUART::readLine(char* buffer, int size)
{
  return circularTraceBuffer.readLine(buffer, size);
}

