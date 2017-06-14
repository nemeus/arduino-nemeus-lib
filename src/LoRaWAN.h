/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * LoRaWAN.h - LoRaWAN class definition
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
 
#ifndef LORAWAN_H
#define LORAWAN_H

#include <stdint.h>

#include "Arduino.h"
#include "Singleton.h"
#include "Data/DevPerso.h"
#include "Data/MacDataRate.h"
#include "Data/MacChannel.h"

/**
 * Enumeration for send Mode (Binary or Text)
 */
enum MAC_SEND_MODE
{
  BINARY_MODE = 0,
  TEXT_MODE   = 1
};

/**
 * Maximum LoRaWAN payload size (from LoRaWAN specification). Depends on Data Rate.
 */
enum MAXIMUM_LORAWAN_PAYLOAD
{
  MAX_LORAWAN_PAYLOAD_1 = 51,
  MAX_LORAWAN_PAYLOAD_2   = 115,
  MAX_LORAWAN_PAYLOAD_3 = 242
};

/**
 * Prefix of LoRaWAN unsollicited AT response. Used to parse incoming data.
 */
const char LORAWAN_SEND_UNSOL[] =			"+MAC: SND,";
const char LORAWAN_RCH_UNSOL[] = 			"+MAC: RCH,";
const char LORAWAN_RCVBIN_UNSOL[] = 		"+MAC: RCVBIN,";
const char LORAWAN_RCVTXT_UNSOL[] = 		"+MAC: RCVTXT,";
const char LORAWAN_SCH_UNSOL[] = 			"+MAC: SCH,";
const char LORAWAN_RDR_UNSOL[] = 			"+MAC: RDR,";
const char LORAWAN_SDR_UNSOL[] = 			"+MAC: SDR,";
const char LORAWAN_RTI_UNSOL[] = 			"+MAC: RTI,";
const char LORAWAN_STI_UNSOL[] = 			"+MAC: STI,";
const char LORAWAN_RRX_UNSOL[] = 			"+MAC: RRX,";
const char LORAWAN_RVAR_UNSOL[] = 			"+MAC: RVAR,";
const char LORAWAN_RDEVADDR_UNSOL[] =	 	"+MAC: RDEVADDR,";

/**
 * Table of unsollicited for LoRaWAN
 */
const char* const table_LORAWAN_UNSOLLICITED[] =
{
  LORAWAN_SEND_UNSOL,
  LORAWAN_RCH_UNSOL,
  LORAWAN_RCVBIN_UNSOL,
  LORAWAN_RCVTXT_UNSOL,
  LORAWAN_SCH_UNSOL,
  LORAWAN_RDR_UNSOL,
  LORAWAN_SDR_UNSOL,
  LORAWAN_RTI_UNSOL,
  LORAWAN_STI_UNSOL,
  LORAWAN_RRX_UNSOL,
  LORAWAN_RVAR_UNSOL,
  LORAWAN_RDEVADDR_UNSOL,
};


/**
 * LoRaWAN class
 */
class LoRaWAN : public Singleton<LoRaWAN>
{
  friend class Singleton<LoRaWAN>;

  public:
  /* Start LoRaWAN */
  uint8_t ON(char loraClass, boolean otaa);
  /* Stop LoRaWAN */
  uint8_t OFF();
  /* Send a LoRaWAN frame */
  uint8_t sendFrame(uint8_t mode, uint8_t repetition, uint8_t macPort, const char* payload, boolean ack, boolean encrypt);
  /* Get the maximum payload size according to Data Rate */
  uint8_t getMaximumPayloadSize();
  /* Read OTAA status */
  boolean isOtaa();
  /* Read the device UID */
  char* readDevUID();
  /* Read the App UID */
  char* readAppUID();
  /* Set the App UID */
  uint8_t setAppUID(char appUID[16]);
  /* Read the App Key */
  char* readAppKey();
  /* Set the App Key */
  uint8_t setAppKey(char appKey[32]);
  /* Read the App Security Key */
  char* readAppSKey();
  /* Read the Network Security Key */
  char* readNwkSKey();
  /* Read the device address*/
  char* readDevAddr();
  /* Set the device Address*/
  uint8_t setDevAddr(char DevADDR[8]);
  /* Read device perso */
  DevPerso_t* readDevPerso();
  /* Get ABP perso */
  DevPerso_t* readAbpPerso();
  // Register a callback for downlink frame
  void register_downlink_callback(void (*onReceiveDownlink)(uint8_t port , boolean more, const char * hexaPayload, int rssi, int snr));
  protected:
  void treatAtResponse(const char * buffer);
  private:
  /* Constructor */
  LoRaWAN();
  /* Destructor */
  ~LoRaWAN();
  /* Data context */
  DataContext * dataContext_;
  boolean otaa_;
  boolean adr_;
  boolean piggyback_;
  boolean encryption_;
  uint8_t macPort_;
  boolean loraWANstate_;
  MacDataRate* macDataRate_;
  DevPerso* devPerso_;
  MacChannel* macChannel_;
  NemeusTimer* otaaTimer_;
  uint32_t sendingDelay_;
  static void onReceiveFromUART(const char * buffer);
  boolean readMacStatus();
  uint8_t readAdr();
  uint8_t setAdr(bool adr);
  uint8_t setAdr(bool adr, bool piggyback);
  uint8_t readDataRate();
  uint8_t setDataRate(MacDataRate_t* macDataRate);
  uint8_t readChannel();
  uint8_t readChannel(uint8_t channelNumber, uint8_t pageNumber, bool unsolEvent);
  uint8_t enableUnsollicited();
  uint8_t setChannel(MacChannel macChannel);
  uint8_t setEncryption(boolean encrypt);
  uint8_t readEncryption();
  void parseMacReadDataRate(String buffer);
  long deviceAddressToLong(String deviceAddr);
  boolean unsollicitedResponse(const char * buffer);
  void (*onReceiveDownlink)(uint8_t port , boolean more, const char * hexaPayload, int rssi, int snr);

};

#endif // LORAWAN_H
