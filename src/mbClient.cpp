#include "mbClient.h"

mbClient::mbClient()
{
    m_port = nullptr;
}

mbClient::~mbClient()
{
    delete m_port;
}

void mbClient::setSettings(Modbus::ProtocolType type, const void *settings)
{
    if (m_port)
        delete m_port;
    m_port = Modbus::createClientPort(type, settings, false);
}
