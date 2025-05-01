/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#ifndef PMB_PRINT_H
#define PMB_PRINT_H

#include <pmb_log.h>

void printTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
void printRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
void printTxAsc(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
void printRxAsc(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
void printOpened(const Modbus::Char *source);
void printClosed(const Modbus::Char *source);
void printError(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text);
void printErrorSerialServer(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text);
void printNewConnection(const Modbus::Char *source);
void printCloseConnection(const Modbus::Char *source);

#endif // PMB_PRINT_H