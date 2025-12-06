/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmbClient.h"

pmbClient::pmbClient(ModbusClientPort *port) :
    m_port(port)
{
}

pmbClient::~pmbClient()
{
    delete m_port;
}

void pmbClient::setName(const pmb::String &name)
{
    m_name = name;
    m_port->setObjectName(m_name.data());
}
