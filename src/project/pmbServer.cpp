/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmbServer.h"

#include <pmbMemory.h>

pmbServer::pmbServer(ModbusServerPort *port, pmbMemory *memory) : 
    m_port(port),
    m_memory(memory)
{
}

pmbServer::~pmbServer()
{
    delete m_port;
}

void pmbServer::setName(const pmb::String &name)
{
    m_name = name;
    m_port->setObjectName(m_name.data());
}

void pmbServer::run()
{
    m_port->process();
}