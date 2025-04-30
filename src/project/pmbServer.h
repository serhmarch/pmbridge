#ifndef PMB_SERVER_H
#define PMB_SERVER_H

#include <ModbusServerPort.h>
#include <pmb_core.h>

class pmbMemory;

class pmbServer
{
public:
    pmbServer(pmbMemory *memory);
    ~pmbServer();

public:
    inline ModbusServerPort *port() const { return m_port; }
    inline const pmb::String &name() const { return m_name; }
    inline void setName(const pmb::String &name) { m_name = name; } 
    void setSettings(Modbus::ProtocolType type, const void *settings);
    
public:
    void run();

private:
    pmb::String m_name;
    pmbMemory *m_memory;
    ModbusServerPort *m_port;
};

#endif // PMB_SERVER_H