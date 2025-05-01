/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmbProject.h"

#include "pmbClient.h"
#include "pmbServer.h"
#include "pmbCommand.h"

pmbProject::pmbProject()
{
}

pmbProject::~pmbProject()
{
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
