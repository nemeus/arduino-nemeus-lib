/**       __         __         __
 * |\ |  |_   |\/|  |_   |  |  (_
 * | \|  |__  |  |  |__  |__|  __)
 *
 * AtCommand.h - AT Command class definition (enum)
 *                  Manage the At command definition
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
 
#ifndef AT_COMMAND_H
#define AT_COMMAND_H

#include <string.h>

#define SEPARATOR ","
#define COLON ":"
#define CRLF "\r\n"

/* ----- List of MM002 AT answers ----- */
#define RSP_AT_OK               "OK"
#define RSP_AT_ERR              "ERROR"
#define RSP_AT_ERR_NOACK        "ERROR NOACK"
#define RSP_AT_INIT             "+DEBUG"

/******************************************************************************
 * AT COMMANDS CONSTANTS
 ******************************************************************************/
class AtCommand
{
  public:
    int enumValue;
    const char* stringCommand;
    int getCode() const;
    const char* getStringCommand();
    friend bool operator== (const AtCommand &atCmd1, const AtCommand &atCmd2);
    friend bool operator!= (const AtCommand &atCmd1, const AtCommand &atCmd2);
    /* Constructor */
    AtCommand(int value, const char* stringCommand);
    AtCommand();
};

/**
 * AT Command list ( declare AtCommand(Unique ID, AT command string) )
 */
const static AtCommand NO_CMD = AtCommand(0xFF, NULL);
const static AtCommand MAC_ON = AtCommand(0, "AT+MAC=ON,");
const static AtCommand MAC_OFF = AtCommand(1, "AT+MAC=OFF\r\n");
const static AtCommand MAC_STATUS = AtCommand(2, "AT+MAC=?\r\n");
const static AtCommand RF_STATUS = AtCommand(3, "AT+RF=?\r\n");
const static AtCommand RESET_COLD = AtCommand(4, "~K\nAT+GA=DIND,1,8401\r\n"); // ~K:  command to stop any on going process before doing the cold reset
const static AtCommand AT_TRACE = AtCommand(5, "AT+DEBUG=MV");
const static AtCommand RF_TX_SNDBIN = AtCommand(6, "AT+RFTX=SNDBIN,");
const static AtCommand RF_TX_SET_STATE = AtCommand(7, "AT+RF=");
const static AtCommand MAC_SEND = AtCommand(8, "AT+MAC=SND");
const static AtCommand MAC_READ_DATA_RATE = AtCommand(9, "AT+MAC=RDR\r\n");
const static AtCommand MAC_SET_DATA_RATE = AtCommand(10, "AT+MAC=SDR,");
const static AtCommand MAC_SET_CHANNEL = AtCommand(11, "AT+MAC=SCH,");
const static AtCommand MAC_READ_CHANNEL = AtCommand(12, "AT+MAC=RCH,");
const static AtCommand MAC_READ_ADR = AtCommand(13, "AT+MAC=RADR\r\n");
const static AtCommand MAC_SET_ADR = AtCommand(14, "AT+MAC=SADR,");
const static AtCommand MAC_READ_VAR = AtCommand(15, "AT+MAC=RVAR\r\n");
const static AtCommand MAC_SET_VAR = AtCommand(16, "AT+MAC=SVAR,");
const static AtCommand MAC_READ_DEVUID = AtCommand(17, "AT+MAC=RDEVUID\r\n");
const static AtCommand MAC_READ_DEVADDR = AtCommand(18, "AT+MAC=RDEVADDR\r\n");
const static AtCommand MAC_SET_DEVADDR = AtCommand(19, "AT+MAC=SDEVADDR,");
const static AtCommand AT_POWER_SET = AtCommand(20, "AT+GA=DIND,1,8802");
const static AtCommand MAC_READ_APPKEY = AtCommand(21, "AT+MAC=RAPPKEY\r\n");
const static AtCommand MAC_SET_APPKEY = AtCommand(22, "AT+MAC=SAPPKEY,");
const static AtCommand MAC_READ_APPUID = AtCommand(23, "AT+MAC=RAPPUID\r\n");
const static AtCommand MAC_SET_APPUID = AtCommand(24, "AT+MAC=SAPPUID,");
const static AtCommand MAC_READ_APPSKEY = AtCommand(25, "AT+MAC=RAPPSKEY\r\n");
const static AtCommand MAC_READ_NWKSKEY = AtCommand(26, "AT+MAC=RNSKEY\r\n");

const static AtCommand SIGFOX_ON = AtCommand(30, "AT+SF=ON");
const static AtCommand SIGFOX_OFF = AtCommand(31, "AT+SF=OFF\r\n");
const static AtCommand SIGFOX_SEND_BINARY = AtCommand(32, "AT+SF=SND");


const static AtCommand RADIO_ON = AtCommand(50, "AT+RF=ON\r\n");
const static AtCommand RADIO_OFF = AtCommand(51, "AT+RF=OFF\r\n");
const static AtCommand RADIO_SET_RX_PARAM = AtCommand(52, "AT+RFRX=SET,");
const static AtCommand RADIO_SET_TX_PARAM = AtCommand(53, "AT+RFTX=SET,");
const static AtCommand RADIO_SEND_FRAME = AtCommand(54, "AT+RFTX=SND");
const static AtCommand RADIO_CONTINUOUS_RX = AtCommand(55, "AT+RFRX=CONTRX\r\n");
const static AtCommand RADIO_STOP_RX = AtCommand(56, "AT+RFRX=STOP\r\n");
const static AtCommand RADIO_CONTINUOUS_TX = AtCommand(57, "AT+RFTX=START\r\n");
const static AtCommand RADIO_STOP_TX = AtCommand(58, "AT+RFTX=STOP\r\n");

const static AtCommand DEBUG_MVER = AtCommand(59, "AT+DEBUG=MVER\r\n");

/*
   RFRX(0, "\r\nAT+RFRX= ?\r\n"),
   RFTX(1, "\r\nAT+RFTX= ?\r\n"),
   RFASK(4,"\r\nAT+RF= ?\r\n"),
 */

/*
   MACSTI(20, "\r\nAT+MAC= STI,"),
   MACRTI(21, "\r\nAT+MAC= RTI\r\n"),
   MACSRX(22, "\r\nAT+MAC= SRX,"),
   MACRRX(23, "\r\nAT+MAC= RRX\r\n"),
   MACLCR(27, "\r\nAT+MAC= SNDLCR,1\r\n"),
   MACVARASK(37, "\r\nAT+MAC= VAR?\r\n"),
   MACVARSET(38, "\r\nAT+MAC= VAR,"),
   MACRSW(39, "\r\nAT+MAC= RSW\r\n"),
   MACSSW(40, "\r\nAT+MAC= SSW,"),
   MACRKAP(41, "\r\nAT+MAC= RKAP\r\n"),
   MACSKAP(42, "\r\nAT+MAC= SKAP,"),*/
#endif

