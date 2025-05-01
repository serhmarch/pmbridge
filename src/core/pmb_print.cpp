/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmb_print.h"

void printTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbLogTx("'%s' Tx: %s", source, Modbus::bytesToString(buff, size).data());
}

void printRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbLogRx("'%s' Rx: %s", source, Modbus::bytesToString(buff, size).data());
}

void printTxAsc(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbLogTx("'%s' Tx: %s", source, Modbus::asciiToString(buff, size).data());
}

void printRxAsc(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbLogRx("'%s' Rx: %s", source, Modbus::asciiToString(buff, size).data());
}

void printOpened(const Modbus::Char *source)
{
    mbLogConnection("'%s' opened", source);
}

void printClosed(const Modbus::Char *source)
{
    mbLogConnection("'%s' closed", source);
}

void printError(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text)
{
    mbLogWarning("Error (0x%X): %s", status, text);
}

void printErrorSerialServer(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text)
{
    if (status != Modbus::Status_BadSerialReadTimeout)
        mbLogWarning("Error (0x%X): %s", status, text);
}

void printNewConnection(const Modbus::Char *source)
{
    mbLogConnection("New connection: '%s'", source);
}

void printCloseConnection(const Modbus::Char *source)
{
    mbLogConnection("Close connection: '%s'", source);
}
