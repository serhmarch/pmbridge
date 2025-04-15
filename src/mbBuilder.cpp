#include "mbBuilder.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "mbProject.h"
#include "mbMemory.h"
#include "mbClient.h"
#include "mbServer.h"
#include "mbCommand.h"

#define CHAIN_CONFREADER_EOF (std::char_traits<char>::eof())

static inline bool containsChar(const char* str, char ch)
{
    return std::strchr(str, ch) != nullptr;
}

mbBuilder::mbBuilder() :
    m_project(nullptr),
    m_ch(mbEMPTY_CHAR)
{

}

mbBuilder::~mbBuilder()
{
    delete m_project;
}

mbProject *mbBuilder::load(const mb::String &filePath)
{
    m_file.open(filePath.c_str(), std::ios::in);
    if (!m_file.is_open())
    {
        m_lastError =  mbSTR("Error opening file: ") + filePath;
        return nullptr;
    }
    m_project = new mbProject();
    mbProject *res = nullptr;
    while (readNext())
        ;
    if (hasError())
        delete m_project;
    else
        res = m_project;
    m_project = nullptr;
    m_file.close();
    return res;
}

bool mbBuilder::readNext()
{
    std::string buffer;
    passSpace();
    if (isEOF())
        return false;
    else if (isComment())
    {
        // Comment line, skip it
        passLine();
        return readNext(); // Recursively read the next line
    }
    else if (isEndOfLine())
    {
        // Empty line, skip it
        passLine();
        return readNext(); // Recursively read the next line
    }
    else
    {
        if (!parseString(buffer, "="))
        {
            if (m_lastError.empty())
                m_lastError = mbSTR("Error: Missed key-value pair separator '='");
            return false;
        }
        std::string command;
        command.swap(buffer);
        passSpace();
        std::list<std::string> args;
        if (!parseArgs(args))
        {
            if (m_lastError.empty())
                m_lastError = mbSTR("Error: Parsing command arguments");
            return false;
        }
        mbCommand *cmd = parseCommand(command, args);
        if (cmd)
            m_project->addCommand(cmd);
        else if (hasError())
            return false;
        passLine();
    }
    return true;
}

int mbBuilder::nextChar()
{
    m_ch = m_file.get(); 
    return m_ch;
}

// Helper to skip over whitespace
void mbBuilder::passSpace()
{
    while (!isEOF() && (m_ch == (' ') || m_ch == ('\t') || m_ch == ('\v')))
    {
        if (isComment())
        {
            while (nextChar() != CHAIN_CONFREADER_EOF && !isEndOfLine())
                ; // goto end of current line
            break;
        }
        nextChar();
    }
}

// Helper to skip the rest of the line (used for comments or after parsing a line)
void mbBuilder::passLine()
{
    while (m_ch != CHAIN_CONFREADER_EOF)
    {  
        if (m_ch == ('\r'))
        {
            if (nextChar() == ('\n'))
                nextChar();
            break;
        }
        if (m_ch == ('\n'))
        {
            nextChar();
            break;
        }
        nextChar();
    }
}

bool mbBuilder::parseString(std::string &buffer, const char *endchars, bool multiline)
{
    bool notfound = true;
    if (m_ch == '"' || m_ch == '\'')
    {
        char quoteChar = m_ch;
        while (nextChar() != CHAIN_CONFREADER_EOF && m_ch != quoteChar && !isEndOfLine())
            buffer += m_ch;
        if (m_ch != quoteChar)
        {
            m_lastError = mbSTR("Error: Not finished quotes '\"'");
            return false;
        }
        passSpace();
    }
    else
    {
        buffer += m_ch;
        while (nextChar() != CHAIN_CONFREADER_EOF)
        {
            if (isEndOfLine())
            {
                if (multiline)
                {
                    passLine();
                    continue;
                }
                else
                    break;
            }
            if (endchars && containsChar(endchars, m_ch))
            {
                notfound = false;
                break;
            }
            if (isComment())
            {
                // Comment line, skip it
                passSpace();
                break;
            }
            buffer += m_ch;
        }
    }
    if (endchars) 
    {
        if (notfound)
            return false;
        else
            nextChar();
    }
    return true;
}

bool mbBuilder::parseArgs(std::list<std::string> &args)
{
    if (isEOF())
    {
        m_lastError = mbSTR("Error: End of file while parsing arguments");
        return false;
    }
    if (m_ch == '{')
    {
        while (nextChar() != '}')
        {
            std::string arg;
            if (!parseString(arg, ",}", true))
            {
                m_lastError = mbSTR("Error: Parsing argument string");
                return false;
            }
            args.push_back(arg);
            if (isEOF())
            {
                m_lastError = mbSTR("Error: Ending '}' not found");
                return false;
            }
            if (m_ch == ',')
            {
                nextChar();
                //passSpace();
            }
            else // if (m_ch == '}')
                break;
        }
    }
    else
    {
        while (!isEndOfLine())
        {
            std::string arg;
            if (!parseString(arg, ",\r\n"))
            {
                m_lastError = mbSTR("Error: Parsing argument string");
                return false;
            }
            args.push_back(arg);
            if (m_ch == ',')
            {
                nextChar();
                //passSpace();
            }
            if (isEOF())
                break;
        }
    }
    return true;
}

mbCommand* mbBuilder::parseCommand(const std::string &command, const std::list<std::string> &args)
{
    if (command == "MEMORY")
    {
        return parseMemory(args);
    }
    else if (command == "PORT")
    {
        return parsePort(args);
    }
    else if (command == "QUERY")
    {
        return parseQuery(args);
    }
    else if (command == "COPY")
    {
        return parseCopy(args);
    }
    else if (command == "DELAY")
    {
        return parseDelay(args);
    }
    else if (command == "DUMP")
    {
        return parseDump(args);
    }
    return nullptr;
}

mbCommand* mbBuilder::parseMemory(const std::list<std::string> &args)
{
    if (args.size() != 4)
    {
        m_lastError = "MEMORY-command must have 4 params";
        return nullptr;
    }

    auto it = args.begin();
    int sz0x = std::stoi(*it); ++it;
    int sz1x = std::stoi(*it); ++it;
    int sz3x = std::stoi(*it); ++it;
    int sz4x = std::stoi(*it);

    auto memory = m_project->memory();
    memory->realloc_0x(sz0x);
    memory->realloc_1x(sz1x);
    memory->realloc_3x(sz3x);
    memory->realloc_4x(sz4x);

    return nullptr;
}

mbCommand* mbBuilder::parsePort(const std::list<std::string> &args)
{
    if (args.size() < 3)
    {
        m_lastError = "PORT-command must have at least 3 params";
        return nullptr;
    }

    auto it = args.begin();
    const std::string &name = *it; ++it;
    const std::string &side = *it; ++it;
    const std::string &stype = *it; ++it;

    mbServer *server = nullptr;
    mbClient *client = nullptr;
    if (side == "SERVER")
    {
        if (m_project->server(name))
        {
            m_lastError = "Server with this name already exists: " + name;
            return nullptr;
        }
        server = new mbServer(m_project->memory());
        server->setName(name);
        m_project->addServer(server);
    }
    else if (side == "CLIENT")
    {
        if (m_project->client(name))
        {
            m_lastError = "Client with this name already exists: " + name;
            return nullptr;
        }
        client = new mbClient();
        client->setName(name);
        m_project->addClient(client);
    }
    else
    {
        m_lastError = "PORT-command must have SERVER or CLIENT as second param";
        return nullptr;
    }

    if (server && stype == "TCP")
    {
        Modbus::TcpSettings settings;        
        settings.port    = static_cast<uint16_t>(std::stoi(*it)); ++it;
        settings.timeout = static_cast<uint16_t>(std::stoi(*it)); ++it;
        uint16_t maxconn = static_cast<uint16_t>(std::stoi(*it)); ++it; // TODO:
        server->setSettings(Modbus::TCP, &settings);
    }
    else if (stype == "TCP")
    {
        Modbus::TcpSettings settings;
        
        std::string host = *it;
        settings.host    = host.data();                           ++it;
        settings.port    = static_cast<uint16_t>(std::stoi(*it)); ++it;
        settings.timeout = static_cast<uint16_t>(std::stoi(*it)); ++it;
        client->setSettings(Modbus::TCP, &settings);
    }
    else if (stype == "RTU" || stype == "ASC")
    {
        Modbus::ProtocolType type = (stype == "RTU") ? Modbus::RTU : Modbus::ASC;
        Modbus::SerialSettings settings;
        std::string portName = *it;
        settings.portName         = portName.data();                                  ++it;
        settings.baudRate         = static_cast<uint32_t>           (std::stoi(*it)); ++it;
        settings.dataBits         = static_cast<uint8_t>            (std::stoi(*it)); ++it;
        settings.parity           = static_cast<Modbus::Parity>     (std::stoi(*it)); ++it;
        settings.stopBits         = static_cast<Modbus::StopBits>   (std::stoi(*it)); ++it;
        settings.flowControl      = static_cast<Modbus::FlowControl>(std::stoi(*it)); ++it;
        settings.timeoutFirstByte = static_cast<uint32_t>           (std::stoi(*it)); ++it;
        settings.timeoutInterByte = static_cast<uint32_t>           (std::stoi(*it)); ++it;
        if (server)
            server->setSettings(Modbus::RTU, &settings);
        else
            client->setSettings(Modbus::RTU, &settings);
    }
    else
    {
        m_lastError = "Unknown port type: " + stype;
        return nullptr;
    }
    return nullptr;
}

mbCommand* mbBuilder::parseQuery(const std::list<std::string> &args)
{
    if (args.size() != 10)
    {
        m_lastError = "QUERY-command must have 10 params";
        return nullptr;
    }

    auto it = args.begin();
    const std::string &clientName = *it; ++it;
    
    auto memory = m_project->memory();
    // Create and configure the query command
    auto client = m_project->client(clientName);
    if (!client)
    {
        std::cerr << "Client not found: " + clientName;
        return nullptr;
    }

    uint8_t unit = static_cast<uint8_t>(std::stoi(*it));           ++it;
    const std::string &func = *it;                                 ++it;                       
    mb::Address devAdr   = mb::Address::fromString(*it);         ++it;
    uint16_t     count    = static_cast<uint16_t>(std::stoi(*it)); ++it;
    mb::Address memAdr   = mb::Address::fromString(*it);         ++it;
    uint16_t     execPatt = static_cast<uint16_t>(std::stoi(*it)); ++it;
    mb::Address succAdr  = mb::Address::fromString(*it);         ++it;
    mb::Address errcAdr  = mb::Address::fromString(*it);         ++it;
    mb::Address errvAdr  = mb::Address::fromString(*it);          

    mbCommandQueryBase *cmd = nullptr;
    if (func == "RD")
    {
        switch (devAdr.type())
        {
        case Modbus::Memory_0x:
            cmd = new mbCommandQueryReadCoils(memory, client);
            break;
        case Modbus::Memory_1x:
            cmd = new mbCommandQueryReadDiscreteInputs(memory, client);
            break;
        case Modbus::Memory_3x:
            cmd = new mbCommandQueryReadInputRegisters(memory, client);
            break;
        case Modbus::Memory_4x:
            cmd = new mbCommandQueryReadHoldingRegisters(memory, client);
            break;
        default:
            m_lastError = "Unknown memory type for RD function";
            return nullptr;
        }
    }
    else if (func == "WR")
    {
        switch (devAdr.type())
        {
        case Modbus::Memory_0x:
            cmd = new mbCommandQueryWriteMultipleCoils(memory, client);
            break;
        case Modbus::Memory_4x:
            cmd = new mbCommandQueryWriteMultipleRegisters(memory, client);
            break;
        default:
            m_lastError = "Unknown memory type for WR function";
            return nullptr;
        }
    }
    else
    {
        m_lastError = "Unknown function: " + func;
        return nullptr;
    }
    cmd->setUnit(unit);
    cmd->setOffset(devAdr.offset());
    cmd->setCount(count);
    cmd->setMemAddress(memAdr);
    cmd->setExec(execPatt);
    cmd->setSuccAddress(succAdr);
    cmd->setErrcAddress(errcAdr);
    cmd->setErrvAddress(errvAdr);
    return cmd;
}

mbCommand* mbBuilder::parseCopy(const std::list<std::string> &args)
{
    if (args.size() != 3)
    {
        m_lastError = "COPY-command must have 3 params";
        return nullptr;
    }

    auto it = args.begin();
    mb::Address srcAdr  = mb::Address::fromString(*it);         ++it;
    uint16_t     count   = static_cast<uint16_t>(std::stoi(*it)); ++it;
    mb::Address destAdr = mb::Address::fromString(*it);

    mbCommandCopy *cmd = new mbCommandCopy(m_project->memory());
    cmd->setSrcAddress(srcAdr);
    cmd->setDstAddress(destAdr);
    cmd->setCount(count);
    return cmd;
}

mbCommand* mbBuilder::parseDelay(const std::list<std::string> &args)
{
    if (args.size() != 1)
    {
        m_lastError = "DELAY-command must have 1 param";
        return nullptr;
    }

    auto it = args.begin();
    uint32_t msec = static_cast<uint32_t>(std::stoi(*it));

    mbCommandDelay *cmd = new mbCommandDelay();
    cmd->setMilliseconds(msec);
    return cmd;
}

mbCommand* mbBuilder::parseDump(const std::list<std::string> &args)
{
    if (args.size() != 3)
    {
        m_lastError = "COPY-command must have 3 params";
        return nullptr;
    }

    auto it = args.begin();
    mb::Address srcAdr  = mb::Address::fromString(*it);         ++it;
    uint16_t count       = static_cast<uint16_t>(std::stoi(*it)); ++it;
    mb::Format format   = mb::toFormat(*it);

    if (format == mb::Format_Unknown)
    {
        m_lastError = "Unknown format: " + *it;
        return nullptr;
    }

    mbCommandDump *cmd = new mbCommandDump(m_project->memory());
    cmd->setMemAddress(srcAdr);
    cmd->setFormat(format);
    cmd->setCount(count);  
    return cmd;
}

