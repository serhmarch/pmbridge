#ifndef MB_CLIENT_H
#define MB_CLIENT_H

#include "mb_core.h"

#include <ModbusClientPort.h>

class mbClient
{
public:
    mbClient();
    ~mbClient();
 
public:
    inline ModbusClientPort *port() const { return m_port; }
    inline const mb::String &name() const { return m_name; }
    inline void setName(const mb::String &name) { m_name = name; } 
    void setSettings(Modbus::ProtocolType type, const void *settings);
    
public:
    inline Modbus::StatusCode readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values) { return m_port->readCoils(unit, offset, count, values); }
    inline Modbus::StatusCode readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values) { return m_port->readDiscreteInputs(unit, offset, count, values); }
    inline Modbus::StatusCode readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) { return m_port->readHoldingRegisters(unit, offset, count, values); }
    inline Modbus::StatusCode readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) { return m_port->readInputRegisters(unit, offset, count, values); }
    inline Modbus::StatusCode writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values) { return m_port->writeMultipleCoils(unit, offset, count, values); }
    inline Modbus::StatusCode writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values) { return m_port->writeMultipleRegisters(unit, offset, count, values); }

private:
    mb::String m_name;
    ModbusClientPort *m_port;
};

#endif // MB_CLIENT_H