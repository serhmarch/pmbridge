/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#ifndef PMB_SERVER_H
#define PMB_SERVER_H

#include <ModbusServerPort.h>
#include <pmb_core.h>

class pmbMemory;

class pmbServer
{
public:
    pmbServer(ModbusServerPort *port, pmbMemory *memory);
    ~pmbServer();

public:
    inline ModbusServerPort *port() const { return m_port; }
    inline const pmb::String &name() const { return m_name; }
    void setName(const pmb::String &name);
    
public:
    void run();

private:
    pmb::String m_name;
    pmbMemory *m_memory;
    ModbusServerPort *m_port;
};

#endif // PMB_SERVER_H