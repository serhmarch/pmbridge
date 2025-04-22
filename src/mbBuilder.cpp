#include "mbBuilder.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <ModbusSerialPort.h>
#include <ModbusTcpPort.h>
#include <ModbusTcpServer.h>

#include "mb_print.h"
#include "mb_log.h"
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

bool mbBuilder::readNext()
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
                m_lastError = mbSTR("Error: Missed key-value pair separator '='");
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
bool mbBuilder::passLine()
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
            m_lastError = mbSTR("Error: Not finished quotes");
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
                if (multiline)
                {
                    passLine();
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

bool mbBuilder::parseArgs(std::list<std::string> &args)
{
    if (isEOF())
    {
        m_lastError = mbSTR("Error: End of file while parsing arguments");
        return false;
    }
    if (m_ch == '{')
    {
        nextChar();
        while (m_ch != '}')
        {
            passSpace();
            std::string arg;
            if (!parseString(arg, ",}", true))
            {
                m_lastError = mbSTR("Error: Parsing argument string");
                return false;
            }
            args.push_back(std::move(arg));
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
                m_lastError = mbSTR("Error: Parsing argument string");
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

mbCommand* mbBuilder::parseCommand(const std::string &command, const std::list<std::string> &args)
{
    if (command == "LOG")
    {
        return parseLog(args);
    }
    else if (command == "MEMORY")
    {
        return parseMemory(args);
    }
    else if (command == "SERVER")
    {
        return parseServer(args);
    }
    else if (command == "CLIENT")
    {
        return parseClient(args);
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

mbCommand *mbBuilder::parseLog(const std::list<std::string> &args)
{
    if (args.size() < 2)
    {
        m_lastError = "LOG-command must have at least 2 params";
        return nullptr;
    }
    auto it = args.begin();
    auto end = args.end();
    const std::string &format = *it; ++it;
    const std::string &timeformat = *it;
    mb::setLogFormat(format);
    mb::setLogTimeFormat(timeformat);
    mb::LogFlags flags = 0;
    for (; it != end; ++it)
    {
        const std::string &arg = *it;
        if (arg == "ALL")
            flags |= (mb::Log_All);
        else if (arg == "ERROR")
            flags |= (mb::Log_Error);
        else if (arg == "WARN")
            flags |= (mb::Log_Warning);
        else if (arg == "INFO")
            flags |= (mb::Log_Info);
        else if (arg == "DEBUG")
            flags |= (mb::Log_Debug);
        else if (arg == "CONN")
            flags |= (mb::Log_Connection);
        else if (arg == "RX")
            flags |= (mb::Log_Rx);
        else if (arg == "TX")
            flags |= (mb::Log_Tx);
        else
        {
            m_lastError = "Unknown log category: " + arg;
            return nullptr;
        }
    }
    mb::setLogFlags(flags);
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
    int sz0x = std::atoi((*it).data()); ++it;
    int sz1x = std::atoi((*it).data()); ++it;
    int sz3x = std::atoi((*it).data()); ++it;
    int sz4x = std::atoi((*it).data());

    auto memory = m_project->memory();
    memory->realloc_0x(sz0x);
    memory->realloc_1x(sz1x);
    memory->realloc_3x(sz3x);
    memory->realloc_4x(sz4x);

    return nullptr;
}

mbCommand *mbBuilder::parseServer(const std::list<std::string> &args)
{
    if (args.size() < 2)
    {
        m_lastError = "SERVER-command must have at least 2 params";
        return nullptr;
    }

    auto it = args.begin();
    auto end = args.end();

    bool ok;
    const std::string &stype = *it;
    ++it;
    Modbus::ProtocolType type = mb::toProtocolType(stype, &ok);
    if (!ok)
    {
        m_lastError = "Unknown port type for SERVER-port: " + stype;
        return nullptr;
    }

    // TODO: check name correctness (not empty, etc)
    const std::string &name  = *it;
    ++it;
    if (m_project->server(name))
    {
        m_lastError = "Server with this name already exists: " + name;
        return nullptr;
    }
    mbServer *server = new mbServer(m_project->memory());
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
            m_lastError = "SERVER-command for RTU and ASCII must have at least 3 params";
            return nullptr;
        }
        Modbus::SerialSettings settings;
        if (parseSerialSettings(it, end, settings))
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

mbCommand *mbBuilder::parseClient(const std::list<std::string> &args)
{
    if (args.size() < 3)
    {
        m_lastError = "CLIENT-command must have at least 3 params";
        return nullptr;
    }

    auto it = args.begin();
    auto end = args.end();

    bool ok;
    const std::string &stype = *it;
    ++it;
    Modbus::ProtocolType type = mb::toProtocolType(stype, &ok);
    if (!ok)
    {
        m_lastError = "Unknown port type for CLIENT-port: " + stype;
        return nullptr;
    }

    // TODO: check name correctness (not empty, etc)
    const std::string &name  = *it;
    ++it;
    if (m_project->client(name))
    {
        m_lastError = "Client with this name already exists: " + name;
        return nullptr;
    }
    mbClient *client = new mbClient();
    client->setName(name);
    m_project->addClient(client);
    ModbusClientPort *cli;
    switch (type)
    {
    case Modbus::RTU:
    case Modbus::ASC:
    {
        Modbus::SerialSettings settings;
        if (!parseSerialSettings(it, end, settings))
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

    uint8_t unit = static_cast<uint8_t>(std::atoi((*it).data()));               ++it;
    const std::string &func = *it;                                              ++it;                       
    Modbus::Address devAdr   = Modbus::Address::fromString(*it);                ++it;
    uint16_t        count    = static_cast<uint16_t>(std::atoi((*it).data()));  ++it;
    Modbus::Address memAdr   = Modbus::Address::fromString(*it);                ++it;
    uint16_t        execPatt = static_cast<uint16_t>(std::atoi((*it).data()));  ++it;
    Modbus::Address succAdr  = Modbus::Address::fromString(*it);                ++it;
    Modbus::Address errcAdr  = Modbus::Address::fromString(*it);                ++it;
    Modbus::Address errvAdr  = Modbus::Address::fromString(*it);          

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
    Modbus::Address srcAdr  = Modbus::Address::fromString(*it);         ++it;
    uint16_t     count   = static_cast<uint16_t>(std::atoi((*it).data())); ++it;
    Modbus::Address destAdr = Modbus::Address::fromString(*it);

    mbCommandCopy *cmd = new mbCommandCopy(m_project->memory());
    cmd->setParams(srcAdr, destAdr, count);
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
    uint32_t msec = static_cast<uint32_t>(std::atoi((*it).data()));

    mbCommandDelay *cmd = new mbCommandDelay();
    cmd->setMilliseconds(msec);
    return cmd;
}

mbCommand* mbBuilder::parseDump(const std::list<std::string> &args)
{
    if (args.size() != 3)
    {
        m_lastError = "DUMP-command must have 3 params";
        return nullptr;
    }

    auto it = args.begin();
    Modbus::Address srcAdr  = Modbus::Address::fromString(*it);         ++it;
    uint16_t count       = static_cast<uint16_t>(std::atoi((*it).data())); ++it;
    mb::Format format   = mb::toFormat(*it);

    if (format == mb::Format_Unknown)
    {
        m_lastError = "Unknown format: " + *it;
        return nullptr;
    }

    mbCommandDump *cmd = new mbCommandDump(m_project->memory());
    cmd->setParams(srcAdr, format, count);  
    return cmd;
}

bool mbBuilder::parseSerialSettings(std::list<std::string>::const_iterator &it, const std::list<std::string>::const_iterator &end, Modbus::SerialSettings &settings)
{
    const ModbusSerialPort::Defaults &d = ModbusSerialPort::Defaults::instance();

    settings.baudRate         = d.baudRate        ;
    settings.dataBits         = d.dataBits        ;
    settings.parity           = d.parity          ;
    settings.stopBits         = d.stopBits        ;
    settings.flowControl      = d.flowControl     ;
    settings.timeoutFirstByte = d.timeoutFirstByte;
    settings.timeoutInterByte = d.timeoutInterByte;

    std::string portName = *it;
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
