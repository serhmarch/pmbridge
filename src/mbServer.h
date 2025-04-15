#ifndef MB_SERVER_H
#define MB_SERVER_H

#include "mb_core.h"

#include <ModbusServerPort.h>

class mbMemory;

class mbServer
{
public:
    mbServer(mbMemory *memory);
    ~mbServer();

public:
    inline const mb::String &name() const { return m_name; }
    inline void setName(const mb::String &name) { m_name = name; } 
    void setSettings(Modbus::ProtocolType type, const void *settings);
    
public:
    void run();

private:
    mb::String m_name;
    mbMemory *m_memory;
    ModbusServerPort *m_port;
};

#endif // MB_SERVER_H