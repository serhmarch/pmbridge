#include "pmb_log.h"

#include <cstdarg>
#include <iostream>
#include <chrono>
#include <ctime>

namespace pmb {

#define ccTOKEN_TIME "%time"
#define ccTOKEN_TEXT "%text"
#define ccTOKEN_CAT "%cat"

class LogConsole
{
public:
    struct Token
    {
        enum Type { Null, Literal, Time, Category, Text };
        Token(Type type = Null) : type(type) {}
        Type type;
        String str;
    };

    struct TimeToken
    {
        enum Type { Null, Literal, Year, Month, Day, Hour, Minute, Second, Millisecond };
        TimeToken(Type type = Null) : type(type) {}
        Type type;
        String str;
    };

    static List<Token> parseFormat(const String &str)
    {
        size_t pos = 0, len = str.length();
        Token token;
        List<Token> res;
        while (pos < len) 
        {
            Char c = str[pos];
            if (c == pmbCHR('%'))
            {
                if (str.substr(pos, sizeof(ccTOKEN_TIME)-1) == pmbSTR(ccTOKEN_TIME)) 
                {
                    if (token.type == Token::Literal) 
                        res.push_back(token);
                    token = Token(Token::Time);
                    res.push_back(token);
                    pos += sizeof(ccTOKEN_TIME)-1;
                    continue;
                } 
                else if (str.substr(pos, sizeof(ccTOKEN_CAT)-1) == pmbSTR(ccTOKEN_CAT)) 
                {
                    if (token.type == Token::Literal) 
                        res.push_back(token);
                    token = Token(Token::Category);
                    res.push_back(token);
                    pos += sizeof(ccTOKEN_CAT)-1;
                    continue;
                } 
                else if (str.substr(pos, sizeof(ccTOKEN_TEXT)-1) == pmbSTR(ccTOKEN_TEXT)) 
                {
                    if (token.type == Token::Literal) 
                        res.push_back(token);
                    token = Token(Token::Text);
                    res.push_back(token);
                    pos += sizeof(ccTOKEN_TEXT)-1;
                    continue;
                }
            }
            if (token.type == Token::Literal) 
                token.str += c;
            else 
            {
                token = Token(Token::Literal);
                token.str = c;
            }
            ++pos;
        }
        if (token.type == Token::Literal) 
            res.push_back(token);
        return res;
    }
    
    static List<TimeToken> parseTimeformat(const String &str)
    {
        size_t pos = 0, len = str.length();
        TimeToken token;
        List<TimeToken> res;
        while (pos < len) 
        {
            Char c = str[pos];
            if (c == pmbCHR('%'))
            {
                if (pos + 1 < len) 
                {
                    c = str[++pos];
                    TimeToken::Type type = TimeToken::Null;
                    switch (c) 
                    {
                    case pmbCHR('Y'): type = TimeToken::Year       ; break;
                    case pmbCHR('M'): type = TimeToken::Month      ; break;
                    case pmbCHR('D'): type = TimeToken::Day        ; break;
                    case pmbCHR('h'): type = TimeToken::Hour       ; break;
                    case pmbCHR('m'): type = TimeToken::Minute     ; break;
                    case pmbCHR('s'): type = TimeToken::Second     ; break;
                    case pmbCHR('f'): type = TimeToken::Millisecond; break;
                    default:
                    {
                        Char buff[3];
                        buff[0] = pmbCHR('%');
                        buff[1] = c;
                        buff[2] = 0;
                        if (token.type == TimeToken::Literal)
                            token.str += buff;
                        else 
                        {
                            token = TimeToken(TimeToken::Literal);
                            token.str = buff;
                        }
                    }
                        ++pos;
                        continue;
                    }
                    if (token.type == TimeToken::Literal) 
                        res.push_back(token);
                    token.type = type;
                    token.str.clear();
                    res.push_back(token);
                    ++pos;
                    continue;
                }
            }
            if (token.type == TimeToken::Literal) 
                token.str += c;
            else 
            {
                token = TimeToken(TimeToken::Literal);
                token.str = c;
            }
            ++pos;
        }
        if (token.type == Token::Literal) 
            res.push_back(token);
        return res;
    }

public:
    LogConsole() {}

public:
    void setLogFormat(const String &fmt)
    {
        m_formatTokens = parseFormat(fmt);
    }

    void setLogTimeFormat(const String &fmt)
    {
        m_timeformatTokens = parseTimeformat(fmt);
    }

    String getTimeString()
    {
        using namespace std::chrono;

        // Get current system time
        auto now = system_clock::now();

        // Convert to time_t
        std::time_t now_c = system_clock::to_time_t(now);

        // Convert to broken-down time
        std::tm *now_tm = std::localtime(&now_c);

        // Get milliseconds part
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        // Extract parts
        int year        = now_tm->tm_year + 1900;
        int month       = now_tm->tm_mon + 1;
        int day         = now_tm->tm_mday;
        int hour        = now_tm->tm_hour;
        int minute      = now_tm->tm_min;
        int second      = now_tm->tm_sec;
        int millisecond = static_cast<int>(ms.count());

        String res;
        for (auto it = m_timeformatTokens.begin(); it != m_timeformatTokens.end(); ++it) 
        {
            switch ((*it).type)
            {
            case TimeToken::Year       : res += toDecString(year       , 4); break;
            case TimeToken::Month      : res += toDecString(month      , 2); break;
            case TimeToken::Day        : res += toDecString(day        , 2); break;
            case TimeToken::Hour       : res += toDecString(hour       , 2); break;
            case TimeToken::Minute     : res += toDecString(minute     , 2); break;
            case TimeToken::Second     : res += toDecString(second     , 2); break;
            case TimeToken::Millisecond: res += toDecString(millisecond, 3); break;
            case TimeToken::Literal    : res += (*it).str;                   break;
            }
        }
        return res;
    }

    String toString(const Token &token, LogFlag category, const Char *text)
    {
        switch (token.type)
        {
        case Token::Literal : return token.str;
        case Token::Time    : return getTimeString();
        case Token::Category: return String(toConstCharPtr(category));
        case Token::Text    : return String(text);
        }
        return String();
    }

    void logMessage(LogFlag category, const Char *text)
    {
        String res;
        for (auto it = m_formatTokens.begin(); it != m_formatTokens.end(); ++it) 
            res += toString(*it, category, text);
        std::cout << res << std::endl;
        Modbus::Color color = pmb::toColor(category);
        if (color == Modbus::Color_Default)
            std::cout << res << std::endl;
        else
        {
            Modbus::setConsoleColor(color);
            std::cout << res << std::endl;
            Modbus::setConsoleColor(Modbus::Color_Default);
        }
    }

private:
    List<Token> m_formatTokens;
    List<TimeToken> m_timeformatTokens;
};

static LogConsole s_logConsole;

static LogFlags s_logFlags = Log_All;

Modbus::Color toColor(LogFlag flag)
{
    switch (flag)
    {
    case Log_Error  : return Modbus::Color_Red;
    case Log_Warning: return Modbus::Color_Yellow;
    default:
        return Modbus::Color_Default;
    }
}

const Char *toConstCharPtr(LogFlag logFlag)
{
    switch (logFlag)
    {
    case Log_Error     : return pmbSTR("ERR" );
    case Log_Warning   : return pmbSTR("WARN");
    case Log_Info      : return pmbSTR("INFO");
    case Log_Debug     : return pmbSTR("DBG" );
    case Log_Connection: return pmbSTR("CONN");
    case Log_Tx        : return pmbSTR("TX"  );
    case Log_Rx        : return pmbSTR("RX"  );
    }
    return nullptr;
}

LogFlag toLogFlag(const Char *slogFlag)
{
    if (strcmp(slogFlag, pmbSTR("ERR" )) == 0) return Log_Error     ;
    if (strcmp(slogFlag, pmbSTR("WARN")) == 0) return Log_Warning   ;
    if (strcmp(slogFlag, pmbSTR("INFO")) == 0) return Log_Info      ;
    if (strcmp(slogFlag, pmbSTR("DBG" )) == 0) return Log_Debug     ;
    if (strcmp(slogFlag, pmbSTR("CONN")) == 0) return Log_Connection;
    if (strcmp(slogFlag, pmbSTR("TX"  )) == 0) return Log_Tx        ;
    if (strcmp(slogFlag, pmbSTR("RX"  )) == 0) return Log_Rx        ;
    return static_cast<LogFlag>(0);
}

LogFlags toLogFlags(const String &slogFlags)
{
    LogFlags res = 0;
    // TODO: trim and upper case every item
    StringList ls = toStringList(slogFlags);
    for (const auto &s : ls)
    {
        res |= toLogFlag(s);
    }
    return res;
}

LogFlags logFlags()
{
    return s_logFlags;
}

void setLogFlags(LogFlags flags)
{
    s_logFlags = flags;
}

void setLogFormat(const String &fmt)
{
    s_logConsole.setLogFormat(fmt);
}

void setLogTimeFormat(const String &fmt)
{
    s_logConsole.setLogTimeFormat(fmt);
}

void logMessage(LogFlag category, const Char *format, ...)
{
    Char buffer[PMB_LOGMESSAGE_MAXLEN];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, PMB_LOGMESSAGE_MAXLEN, format, args);
    va_end(args);
    s_logConsole.logMessage(category, buffer);
}

} // namespace pmb