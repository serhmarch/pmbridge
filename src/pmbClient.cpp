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
}
