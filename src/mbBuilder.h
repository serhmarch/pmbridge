#ifndef MB_BUILDER_H
#define MB_BUILDER_H

#include <string>
#include <fstream>

#include "mb_core.h"

#define mbEMPTY_CHAR -2 // because -1 is EOF

class mbProject;
class mbCommand;

class mbBuilder
{
public:
    mbBuilder();
    ~mbBuilder();

public:
    mbProject *load(const mb::String &filePath);
    inline bool hasError() const { return !m_lastError.empty(); }
    const mb::String &lastError() const { return m_lastError; }

private:
    bool readNext();
    int nextChar();
    void passSpace();
    bool passLine();
    bool parseString(std::string &buffer, const char *endchars = nullptr, bool multiline = false);
    bool parseArgs(std::list<std::string> &args);
    inline bool isComment() const { return m_ch == mbCHR('#') || m_ch == mbCHR(';'); }
    inline bool isEndOfLine() const { return m_ch == mbCHR('\r') || m_ch == mbCHR('\n'); }
    inline bool isEOF() const { return m_file.eof(); }
    
private:
    // Helper methods for parsing specific commands    // Parses the configuration file and builds the mbProject
    mbCommand *parseCommand(const std::string &command, const std::list<std::string> &args);
    mbCommand *parseLog(const std::list<std::string> &args);
    mbCommand *parseMemory(const std::list<std::string> &args);
    mbCommand *parseServer(const std::list<std::string> &args);
    mbCommand *parseClient(const std::list<std::string> &args);
    mbCommand *parseQuery(const std::list<std::string> &args);
    mbCommand *parseCopy(const std::list<std::string> &args);
    mbCommand *parseDelay(const std::list<std::string> &args);
    mbCommand *parseDump(const std::list<std::string> &args);
    bool parseSerialSettings(std::list<std::string>::const_iterator &it, const std::list<std::string>::const_iterator &end, Modbus::SerialSettings &settings);

private:
    std::ifstream m_file;
    mbProject *m_project;
    mb::String m_command;
    char m_ch;
    mb::String m_lastError;
};

#endif // MB_BUILDER_H
