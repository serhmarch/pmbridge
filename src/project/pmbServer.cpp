/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmbServer.h"

#include <pmbMemory.h>

pmbServer::pmbServer(pmbMemory *memory) : 
    m_memory(memory),
    m_port(nullptr)
{
}

pmbServer::~pmbServer()
{
    delete m_port;
}

void pmbServer::setSettings(Modbus::ProtocolType type, const void *settings)
{
    if (m_port)
        delete m_port;
    m_port = Modbus::createServerPort(m_memory, type, settings, false);
    m_port->setObjectName(m_name.data());
}

void pmbServer::run()
{
    m_port->process();
}