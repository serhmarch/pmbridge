/*!
 * \file   mb_print.h
 * \brief  
 * 
 * \author serhmarch
 * \date   April 2025
 */
#ifndef MB_PRINT_H
#define MB_PRINT_H

#include "mb_log.h"

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

#endif // MB_PRINT_H