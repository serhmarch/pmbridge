/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmbClient.h"

pmbClient::pmbClient()
{
    m_port = nullptr;
}

pmbClient::~pmbClient()
{
    delete m_port;
}

void pmbClient::setSettings(Modbus::ProtocolType type, const void *settings)
{
    if (m_port)
        delete m_port;
    m_port = Modbus::createClientPort(type, settings, false);
    m_port->setObjectName(m_name.data());
}
