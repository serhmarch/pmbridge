#ifndef MB_PROJECT_H
#define MB_PROJECT_H

#include "mb_core.h"

class mbMemory;
class mbServer;
class mbClient;
class mbCommand;

class mbProject
{
public:
	mbProject();
	~mbProject();

public:
	inline mbMemory *memory() const { return m_memory; }

public:
	inline const mb::List<mbServer*> &servers() const { return m_servers; }
	mbServer *server(const mb::String &name) const;
	void addServer(mbServer *server);

public:
	inline const mb::List<mbClient*> &clients() const { return m_clients; }
	mbClient *client(const mb::String &name) const;
	void addClient(mbClient *client);

public:
	inline const mb::List<mbCommand*> &commands() const { return m_commands; }
	inline void addCommand(mbCommand *command) { m_commands.push_back(command); }

private:
	mbMemory *m_memory;

private:
	mb::List<mbServer*> m_servers;
	mb::Hash<mb::String, mbServer*> m_hashServers;

private:
	mb::List<mbClient*> m_clients;
	mb::Hash<mb::String, mbClient*> m_hashClients;

private:
	mb::List<mbCommand*> m_commands;
};

#endif // MB_PROJECT_H
