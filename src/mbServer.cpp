#include "mbServer.h"

#include "mbMemory.h"

mbServer::mbServer(mbMemory *memory) : 
    m_memory(memory),
    m_port(nullptr)
{
}

mbServer::~mbServer()
{
    delete m_port;
}

void mbServer::setSettings(Modbus::ProtocolType type, const void *settings)
{
    if (m_port)
        delete m_port;
    m_port = Modbus::createServerPort(m_memory, type, settings, false);
}

void mbServer::run()
{
    m_port->process();
}