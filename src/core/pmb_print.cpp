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
    pmbLogTx("'%s' Tx: %s", source, Modbus::bytesToString(buff, size).data());
}

void printRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    pmbLogRx("'%s' Rx: %s", source, Modbus::bytesToString(buff, size).data());
}

void printTxAsc(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    pmbLogTx("'%s' Tx: %s", source, Modbus::asciiToString(buff, size).data());
}

void printRxAsc(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    pmbLogRx("'%s' Rx: %s", source, Modbus::asciiToString(buff, size).data());
}

void printOpened(const Modbus::Char *source)
{
    pmbLogConnection("'%s' opened", source);
}

void printClosed(const Modbus::Char *source)
{
    pmbLogConnection("'%s' closed", source);
}

void printError(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text)
{
    pmbLogWarning("Error (0x%X): %s", status, text);
}

void printErrorSerialServer(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text)
{
    if (status != Modbus::Status_BadSerialReadTimeout)
        pmbLogWarning("Error (0x%X): %s", status, text);
}

void printNewConnection(const Modbus::Char *source)
{
    pmbLogConnection("New connection: '%s'", source);
}

void printCloseConnection(const Modbus::Char *source)
{
    pmbLogConnection("Close connection: '%s'", source);
}
