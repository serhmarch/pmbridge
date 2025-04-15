#include "mbProject.h"

#include "mbMemory.h"
#include "mbClient.h"
#include "mbServer.h"

mbProject::mbProject()
{
    m_memory = new mbMemory();
}

mbProject::~mbProject()
{
    delete m_memory;
    for (auto server : m_servers)
        delete server;
    for (auto client : m_clients)
        delete client;
    for (auto command : m_commands)
        delete command;
}

mbServer *mbProject::server(const mb::String &name) const
{
    auto it = m_hashServers.find(name);
    if (it != m_hashServers.end())
        return it->second;
    return nullptr;
}

void mbProject::addServer(mbServer *server)
{
    m_servers.push_back(server);
    m_hashServers[server->name()] = server;
}

mbClient *mbProject::client(const mb::String &name) const
{
    auto it = m_hashClients.find(name);
    if (it != m_hashClients.end())
        return it->second;
    return nullptr;
}

void mbProject::addClient(mbClient *client)
{
    m_clients.push_back(client);
    m_hashClients[client->name()] = client;
}
