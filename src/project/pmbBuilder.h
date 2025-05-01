/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#ifndef PMB_BUILDER_H
#define PMB_BUILDER_H

#include <string>
#include <fstream>

#include <pmb_core.h>

#define mbEMPTY_CHAR -2 // because -1 is EOF

class pmbProject;
class pmbCommand;

class pmbBuilder
{
public:
    pmbBuilder();
    ~pmbBuilder();

public:
    pmbProject *load(const pmb::String &filePath);
    inline bool hasError() const { return !m_lastError.empty(); }
    const pmb::String &lastError() const { return m_lastError; }

private:
    bool readNext();
    int nextChar();
    void passSpace();
    bool passLine();
    bool parseString(std::string &buffer, const char *endchars = nullptr, bool multiline = false);
    bool parseArgs(std::list<std::string> &args);
    inline bool isComment() const { return m_ch == pmbCHR('#') || m_ch == pmbCHR(';'); }
    inline bool isEndOfLine() const { return m_ch == pmbCHR('\r') || m_ch == pmbCHR('\n'); }
    inline bool isEOF() const { return m_file.eof(); }
    
private:
    // Helper methods for parsing specific commands    // Parses the configuration file and builds the pmbProject
    pmbCommand *parseCommand(const std::string &command, const std::list<std::string> &args);
    pmbCommand *parseMemory(const std::list<std::string> &args);
    pmbCommand *parseServer(const std::list<std::string> &args);
    pmbCommand *parseClient(const std::list<std::string> &args);
    pmbCommand *parseQuery(const std::list<std::string> &args);
    pmbCommand *parseCopy(const std::list<std::string> &args);
    pmbCommand *parseDelay(const std::list<std::string> &args);
    pmbCommand *parseDump(const std::list<std::string> &args);
    bool parseSerialSettings(std::list<std::string>::const_iterator &it, const std::list<std::string>::const_iterator &end, pmb::String &portName, Modbus::SerialSettings &settings);

private:
    std::ifstream m_file;
    pmbProject *m_project;
    pmb::String m_command;
    char m_ch;
    pmb::String m_lastError;
};

#endif // PMB_BUILDER_H
