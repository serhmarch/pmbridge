/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmbBuilder.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

#include <ModbusSerialPort.h>
#include <ModbusTcpPort.h>
#include <ModbusTcpServer.h>

#include <pmb_print.h>
#include <pmb_log.h>
#include <pmbMemory.h>

#include "pmbProject.h"
#include "pmbClient.h"
#include "pmbServer.h"
#include "pmbCommand.h"

#define CHAIN_CONFREADER_EOF (std::char_traits<char>::eof())

static inline bool containsChar(const char* str, char ch)
{
    return std::strchr(str, ch) != nullptr;
}

pmbBuilder::pmbBuilder() :
    m_project(nullptr),
    m_ch(mbEMPTY_CHAR)
{

}

pmbBuilder::~pmbBuilder()
{
    delete m_project;
}

pmbProject *pmbBuilder::load(const pmb::String &filePath)
{
    m_file.open(filePath.c_str(), std::ios::in);
    if (!m_file.is_open())
    {
        m_lastError =  pmbSTR("Error opening file: ") + filePath;
        return nullptr;
    }
    m_project = new pmbProject();
    pmbProject *res = nullptr;
    nextChar();
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

bool pmbBuilder::readNext()
{
    std::string buffer;
    passSpace();
    if (isEOF())
        return false;
    else if (isComment())
    {
        // Comment line, skip it
        return passLine();
    }
    else if (isEndOfLine())
    {
        // Empty line, skip it
        return passLine();
    }
    else
    {
        if (!parseString(buffer, "="))
        {
            if (m_lastError.empty())
                m_lastError = pmbSTR("Error: Missed key-value pair separator '='");
            return false;
        }
        std::string command;
        command.swap(buffer);
        if (m_ch == '=')
            nextChar();
        passSpace();
        std::list<std::string> args;
        if (!parseArgs(args))
        {
            if (m_lastError.empty())
                m_lastError = pmbSTR("Error: Parsing command arguments");
            return false;
        }
        pmbCommand *cmd = parseCommand(command, args);
        if (cmd)
            m_project->addCommand(cmd);
        else if (hasError())
            return false;
        passLine();
    }
    return true;
}

int pmbBuilder::nextChar()
{
    m_ch = m_file.get(); 
    return m_ch;
}

// Helper to skip over whitespace
void pmbBuilder::passSpace()
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
bool pmbBuilder::passLine()
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
    return !isEOF();
}

bool pmbBuilder::parseString(std::string &buffer, const char *endchars, bool multiline)
{
    bool notfound = true;
    if (m_ch == '"' || m_ch == '\'')
    {
        char quoteChar = m_ch;
        while (nextChar() != CHAIN_CONFREADER_EOF && m_ch != quoteChar && !isEndOfLine())
            buffer += m_ch;
        if (m_ch != quoteChar)
        {
            m_lastError = pmbSTR("Error: Not finished quotes");
            return false;
        }
        nextChar();
        passSpace();
        if (multiline)
        {
            while (isEndOfLine() || isComment())
            {
                passLine();
                passSpace();
            }
        }
        if (endchars && containsChar(endchars, m_ch))
            notfound = false;
    }
    else
    {
        while (m_ch != CHAIN_CONFREADER_EOF)
        {
            if (isEndOfLine())
            {
                if (multiline)
                {
                    passLine();
                    passSpace();
                    continue;
                }
                else
                {
                    if (containsChar(endchars, m_ch))
                        notfound = false;
                    break;
                }
            }
            if (endchars && containsChar(endchars, m_ch))
            {
                notfound = false;
                break;
            }
            if (isComment())
            {
                // Comment line, skip it
                if (multiline)
                {
                    passLine();
                    passSpace();
                    continue;
                }
                else
                    break;
            }
            buffer += m_ch;
            nextChar();
        }
        while (buffer.size() && std::isspace(buffer.back()))
            buffer.pop_back(); // Remove trailing spaces
    }
    return !(endchars && notfound);  
}

bool pmbBuilder::parseArgs(std::list<std::string> &args)
{
    if (isEOF())
    {
        m_lastError = pmbSTR("Error: End of file while parsing arguments");
        return false;
    }
    if (m_ch == '{')
    {
        nextChar();
        while (m_ch != '}')
        {
            passSpace();
            while (isComment())
            {
                passLine();
                passSpace();
            }
            std::string arg;
            if (!parseString(arg, ",}", true))
            {
                m_lastError = pmbSTR("Error: Parsing argument string");
                return false;
            }
            args.push_back(std::move(arg));
            if (isEOF())
            {
                m_lastError = pmbSTR("Error: Ending '}' not found");
                return false;
            }
            if (m_ch == ',')
            {
                nextChar();
                //passSpace();
            }
        }
        nextChar();
        passSpace();
    }
    else
    {
        while (!isEndOfLine() && !isEOF())
        {
            passSpace();
            std::string arg;
            if (!parseString(arg, ",\r\n"))
            {
                m_lastError = pmbSTR("Error: Parsing argument string");
                return false;
            }
            args.push_back(std::move(arg));
            if (m_ch == ',')
            {
                nextChar();
                //passSpace();
            }
        }
    }
    return true;
}

pmbCommand* pmbBuilder::parseCommand(const std::string &command, const std::list<std::string> &args)
{
    if (command == pmbSTR("MEMORY"))
    {
        return parseMemory(args);
    }
    else if (command == pmbSTR("SERVER"))
    {
        return parseServer(args);
    }
    else if (command == pmbSTR("CLIENT"))
    {
        return parseClient(args);
    }
    else if (command == pmbSTR("QUERY"))
    {
        return parseQuery(args);
    }
    else if (command == pmbSTR("COPY"))
    {
        return parseCopy(args);
    }
    else if (command == pmbSTR("DELAY"))
    {
        return parseDelay(args);
    }
    else if (command == pmbSTR("DUMP"))
    {
        return parseDump(args);
    }
    return nullptr;
}

pmbCommand* pmbBuilder::parseMemory(const std::list<std::string> &args)
{
    if (args.size() != 4)
    {
        m_lastError = pmbSTR("MEMORY-command must have 4 params");
        return nullptr;
    }

    auto it = args.begin();
    size_t size0x = std::atoi((*it).data()); ++it;
    size_t size1x = std::atoi((*it).data()); ++it;
    size_t size3x = std::atoi((*it).data()); ++it;
    size_t size4x = std::atoi((*it).data());

    pmbMemory *mem = pmbMemory::global();
    mem->realloc_0x(size0x);
    mem->realloc_1x(size1x);
    mem->realloc_3x(size3x);
    mem->realloc_4x(size4x);

    return nullptr;
}

pmbCommand *pmbBuilder::parseServer(const std::list<std::string> &args)
{
    if (args.size() < 2)
    {
        m_lastError = pmbSTR("SERVER-command must have at least 2 params");
        return nullptr;
    }

    auto it = args.begin();
    auto end = args.end();

    bool ok;
    const std::string &stype = *it;
    ++it;
    Modbus::ProtocolType type = pmb::toProtocolType(stype, &ok);
    if (!ok)
    {
        m_lastError = pmbSTR("Unknown port type for SERVER-port: ") + stype;
        return nullptr;
    }

    // TODO: check name correctness (not empty, etc)
    const std::string &name  = *it;
    ++it;
    if (m_project->server(name))
    {
        m_lastError = pmbSTR("Server with this name already exists: ") + name;
        return nullptr;
    }
    pmbServer *server = new pmbServer(pmbMemory::global());
    server->setName(name);
    m_project->addServer(server);
    ModbusServerPort *srv;
    switch (type)
    {
    case Modbus::RTU:
    case Modbus::ASC:
    {
        if (args.size() < 3)
        {
            m_lastError = pmbSTR("SERVER-command for RTU and ASCII must have at least 3 params");
            return nullptr;
        }
        pmb::String portName;
        Modbus::SerialSettings settings;
        if (!parseSerialSettings(it, end, portName, settings))
            return nullptr;
        server->setSettings(type, &settings);
        srv = server->port();
        srv->connect(&ModbusServerPort::signalError, printErrorSerialServer);
        if (type == Modbus::RTU)
        {
            srv->connect(&ModbusServerPort::signalTx, printTx);
            srv->connect(&ModbusServerPort::signalRx, printRx);
        }
        else
        {
            srv->connect(&ModbusServerPort::signalTx, printTxAsc);
            srv->connect(&ModbusServerPort::signalRx, printRxAsc);
        }
    }
        break;
    default:
    {
        const ModbusTcpServer::Defaults &d = ModbusTcpServer::Defaults::instance();
        Modbus::TcpSettings settings;
        settings.host    = "";
        settings.port    = d.port;
        settings.timeout = d.timeout;
        settings.maxconn = d.maxconn;
        if (it != end)
        {     
            settings.port = static_cast<uint16_t>(std::atoi((*it).data()));
            ++it;
            if (it != end)
            {
                settings.timeout = static_cast<uint16_t>(std::atoi((*it).data()));
                ++it;
                if (it != end)
                    settings.maxconn = static_cast<uint16_t>(std::atoi((*it).data()));
            }
        }
        server->setSettings(type, &settings);
        ModbusTcpServer *tcpsrv = static_cast<ModbusTcpServer*>(server->port());
        tcpsrv->connect(&ModbusServerPort::signalTx, printTx);
        tcpsrv->connect(&ModbusServerPort::signalRx, printRx);
        tcpsrv->connect(&ModbusTcpServer::signalNewConnection, printNewConnection);
        tcpsrv->connect(&ModbusTcpServer::signalCloseConnection, printCloseConnection);
        tcpsrv->connect(&ModbusServerPort::signalError, printError);
        srv = tcpsrv;
    }
        break;
    }
    srv->connect(&ModbusServerPort::signalOpened, printOpened);
    srv->connect(&ModbusServerPort::signalClosed, printClosed);
    return nullptr;
}

pmbCommand *pmbBuilder::parseClient(const std::list<std::string> &args)
{
    if (args.size() < 3)
    {
        m_lastError = pmbSTR("CLIENT-command must have at least 3 params");
        return nullptr;
    }

    auto it = args.begin();
    auto end = args.end();

    bool ok;
    const std::string &stype = *it;
    ++it;
    Modbus::ProtocolType type = pmb::toProtocolType(stype, &ok);
    if (!ok)
    {
        m_lastError = pmbSTR("Unknown port type for CLIENT-port: ") + stype;
        return nullptr;
    }

    // TODO: check name correctness (not empty, etc)
    const std::string &name  = *it;
    ++it;
    if (m_project->client(name))
    {
        m_lastError = pmbSTR("Client with this name already exists: ") + name;
        return nullptr;
    }
    pmbClient *client = new pmbClient();
    client->setName(name);
    m_project->addClient(client);
    ModbusClientPort *cli;
    switch (type)
    {
    case Modbus::RTU:
    case Modbus::ASC:
    {
        pmb::String portName;
        Modbus::SerialSettings settings;
        if (!parseSerialSettings(it, end, portName, settings))
            return nullptr;
        client->setSettings(type, &settings);
        cli = client->port();
        if (type == Modbus::RTU)
        {
            cli->connect(&ModbusClientPort::signalTx, printTx);
            cli->connect(&ModbusClientPort::signalRx, printRx);
        }
        else
        {
            cli->connect(&ModbusClientPort::signalTx, printTxAsc);
            cli->connect(&ModbusClientPort::signalRx, printRxAsc);
        }
        }
        break;
    default:
    {
        const ModbusTcpPort::Defaults &d = ModbusTcpPort::Defaults::instance();
        Modbus::TcpSettings settings;
        settings.host    = (*it).data();
        settings.port    = d.port;
        settings.timeout = d.timeout;
        
        ++it;
        if (it != end)
        {     
            settings.port = static_cast<uint16_t>(std::atoi((*it).data()));
            ++it;
            if (it != end)
                settings.timeout = static_cast<uint16_t>(std::atoi((*it).data()));
        }
        client->setSettings(Modbus::TCP, &settings);
        cli = client->port();
        cli->connect(&ModbusClientPort::signalTx, printTx);
        cli->connect(&ModbusClientPort::signalRx, printRx);
    }
        break;
    }
    cli->connect(&ModbusClientPort::signalOpened, printOpened);
    cli->connect(&ModbusClientPort::signalClosed, printClosed);
    cli->connect(&ModbusClientPort::signalError , printError );
    return nullptr;
}

pmbCommand* pmbBuilder::parseQuery(const std::list<std::string> &args)
{
    if (args.size() != 10)
    {
        m_lastError = pmbSTR("QUERY-command must have 10 params");
        return nullptr;
    }

    auto it = args.begin();
    const std::string &clientName = *it; ++it;
    
    auto memory = pmbMemory::global();
    // Create and configure the query command
    auto client = m_project->client(clientName);
    if (!client)
    {
        m_lastError = pmbSTR("Client not found: ") + clientName;
        return nullptr;
    }

    uint8_t unit = static_cast<uint8_t>(std::atoi((*it).data()));               ++it;
    const std::string &func = *it;                                              ++it;                       
    Modbus::Address devAdr   = Modbus::Address::fromString(*it);                ++it;
    uint16_t        count    = static_cast<uint16_t>(std::atoi((*it).data()));  ++it;
    Modbus::Address memAdr   = Modbus::Address::fromString(*it);                ++it;
    uint16_t        execPatt = static_cast<uint16_t>(std::atoi((*it).data()));  ++it;
    Modbus::Address succAdr  = Modbus::Address::fromString(*it);                ++it;
    Modbus::Address errcAdr  = Modbus::Address::fromString(*it);                ++it;
    Modbus::Address errvAdr  = Modbus::Address::fromString(*it);          

    pmbCommandQueryBase *cmd = nullptr;
    if (func == pmbSTR("RD"))
    {
        switch (devAdr.type())
        {
        case Modbus::Memory_0x:
            cmd = new pmbCommandQueryReadCoils(memory, client);
            break;
        case Modbus::Memory_1x:
            cmd = new pmbCommandQueryReadDiscreteInputs(memory, client);
            break;
        case Modbus::Memory_3x:
            cmd = new pmbCommandQueryReadInputRegisters(memory, client);
            break;
        case Modbus::Memory_4x:
            cmd = new pmbCommandQueryReadHoldingRegisters(memory, client);
            break;
        default:
            m_lastError = pmbSTR("Unknown memory type for RD function");
            return nullptr;
        }
    }
    else if (func == pmbSTR("WR"))
    {
        switch (devAdr.type())
        {
        case Modbus::Memory_0x:
            cmd = new pmbCommandQueryWriteMultipleCoils(memory, client);
            break;
        case Modbus::Memory_4x:
            cmd = new pmbCommandQueryWriteMultipleRegisters(memory, client);
            break;
        default:
            m_lastError = pmbSTR("Unknown memory type for WR function");
            return nullptr;
        }
    }
    else
    {
        m_lastError = pmbSTR("Unknown function: ") + func;
        return nullptr;
    }
    cmd->setUnit(unit);
    cmd->setOffset(devAdr.offset());
    cmd->setCount(count);
    cmd->setMemAddress(memAdr);
    cmd->setExecPattern(execPatt);
    cmd->setSuccAddress(succAdr);
    cmd->setErrcAddress(errcAdr);
    cmd->setErrvAddress(errvAdr);
    return cmd;
}

pmbCommand* pmbBuilder::parseCopy(const std::list<std::string> &args)
{
    if (args.size() != 3)
    {
        m_lastError = pmbSTR("COPY-command must have 3 params");
        return nullptr;
    }

    auto it = args.begin();
    Modbus::Address srcAdr  = Modbus::Address::fromString(*it);         ++it;
    uint16_t     count   = static_cast<uint16_t>(std::atoi((*it).data())); ++it;
    Modbus::Address destAdr = Modbus::Address::fromString(*it);

    pmbCommandCopy *cmd = new pmbCommandCopy(pmbMemory::global());
    cmd->setParams(srcAdr, destAdr, count);
    return cmd;
}

pmbCommand* pmbBuilder::parseDelay(const std::list<std::string> &args)
{
    if (args.size() != 1)
    {
        m_lastError = pmbSTR("DELAY-command must have 1 param");
        return nullptr;
    }

    auto it = args.begin();
    uint32_t msec = static_cast<uint32_t>(std::atoi((*it).data()));

    pmbCommandDelay *cmd = new pmbCommandDelay();
    cmd->setMilliseconds(msec);
    return cmd;
}

pmbCommand* pmbBuilder::parseDump(const std::list<std::string> &args)
{
    if (args.size() != 3)
    {
        m_lastError = pmbSTR("DUMP-command must have 3 params");
        return nullptr;
    }

    auto it = args.begin();
    Modbus::Address srcAdr  = Modbus::Address::fromString(*it);         ++it;
    uint16_t count       = static_cast<uint16_t>(std::atoi((*it).data())); ++it;
    pmb::Format format   = pmb::toFormat(*it);

    if (format == pmb::Format_Unknown)
    {
        m_lastError = pmbSTR("Unknown format: ") + *it;
        return nullptr;
    }

    pmbCommandDump *cmd = new pmbCommandDump(pmbMemory::global());
    cmd->setParams(srcAdr, format, count);  
    return cmd;
}

bool pmbBuilder::parseSerialSettings(std::list<std::string>::const_iterator &it, const std::list<std::string>::const_iterator &end, pmb::String &portName, Modbus::SerialSettings &settings)
{
    const ModbusSerialPort::Defaults &d = ModbusSerialPort::Defaults::instance();

    settings.baudRate         = d.baudRate        ;
    settings.dataBits         = d.dataBits        ;
    settings.parity           = d.parity          ;
    settings.stopBits         = d.stopBits        ;
    settings.flowControl      = d.flowControl     ;
    settings.timeoutFirstByte = d.timeoutFirstByte;
    settings.timeoutInterByte = d.timeoutInterByte;

    portName = *it;
    settings.portName = portName.data();
    ++it;
    if (it != end)
    {
        auto baudRate = Modbus::tobaudRate((*it).data());
        if (baudRate >= 0)
            settings.baudRate = baudRate;
        ++it;
        if (it != end)
        {
            auto dataBits = Modbus::todataBits((*it).data());
            if (dataBits >= 0)
                settings.dataBits = dataBits;
            ++it;
            if (it != end)
            {
                auto parity = Modbus::toparity((*it).data());
                if (parity >= 0)
                    settings.parity = parity;
                ++it;
                if (it != end)
                {
                    auto stopBits = Modbus::tostopBits((*it).data());
                    if (stopBits >= 0)
                        settings.stopBits = stopBits;
                    ++it;
                    if (it != end)
                    {
                        auto flowControl = Modbus::toflowControl((*it).data());
                        if (flowControl >= 0)
                            settings.flowControl = flowControl;
                        ++it;
                        if (it != end)
                        {
                            settings.timeoutFirstByte = static_cast<uint32_t>(std::atoi((*it).data()));
                            ++it;
                            if (it != end)
                                settings.timeoutInterByte = static_cast<uint32_t>(std::atoi((*it).data()));
                        }
                    }
                }
            }
        }
    }
    return true;
}
