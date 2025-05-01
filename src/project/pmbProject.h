/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#ifndef PMB_PROJECT_H
#define PMB_PROJECT_H

#include <pmb_core.h>

class pmbServer;
class pmbClient;
class pmbCommand;

class pmbProject
{
public:
	pmbProject();
	~pmbProject();

public:
	inline const pmb::List<pmbServer*> &servers() const { return m_servers; }
	pmbServer *server(const pmb::String &name) const;
	void addServer(pmbServer *server);

public:
	inline const pmb::List<pmbClient*> &clients() const { return m_clients; }
	pmbClient *client(const pmb::String &name) const;
	void addClient(pmbClient *client);

public:
	inline const pmb::List<pmbCommand*> &commands() const { return m_commands; }
	inline void addCommand(pmbCommand *command) { m_commands.push_back(command); }

private:
	pmb::List<pmbServer*> m_servers;
	pmb::Hash<pmb::String, pmbServer*> m_hashServers;

private:
	pmb::List<pmbClient*> m_clients;
	pmb::Hash<pmb::String, pmbClient*> m_hashClients;

private:
	pmb::List<pmbCommand*> m_commands;
};

#endif // PMB_PROJECT_H
