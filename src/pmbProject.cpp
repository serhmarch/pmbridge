#include "pmbProject.h"

#include "pmbMemory.h"
#include "pmbClient.h"
#include "pmbServer.h"
#include "pmbCommand.h"

pmbProject::pmbProject()
{
    m_memory = new pmbMemory();
}

pmbProject::~pmbProject()
{
    delete m_memory;
    for (auto server : m_servers)
        delete server;
    for (auto client : m_clients)
        delete client;
    for (auto command : m_commands)
        delete command;
}

pmbServer *pmbProject::server(const pmb::String &name) const
{
    auto it = m_hashServers.find(name);
    if (it != m_hashServers.end())
        return it->second;
    return nullptr;
}

void pmbProject::addServer(pmbServer *server)
{
    m_servers.push_back(server);
    m_hashServers[server->name()] = server;
}

pmbClient *pmbProject::client(const pmb::String &name) const
{
    auto it = m_hashClients.find(name);
    if (it != m_hashClients.end())
        return it->second;
    return nullptr;
}

void pmbProject::addClient(pmbClient *client)
{
    m_clients.push_back(client);
    m_hashClients[client->name()] = client;
}
