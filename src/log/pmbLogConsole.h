#ifndef PMB_LOGCONSOLE_H
#define PMB_LOGCONSOLE_H

#include <pmb_log.h>

#define ccTOKEN_TIME "%time"
#define ccTOKEN_TEXT "%text"
#define ccTOKEN_CAT "%cat"

class pmbLogConsole
{
public:
    struct Token
    {
        enum Type { Null, Literal, Time, Category, Text };
        Token(Type type = Null) : type(type) {}
        Type type;
        pmb::String str;
    };

    struct TimeToken
    {
        enum Type { Null, Literal, Year, Month, Day, Hour, Minute, Second, Millisecond };
        TimeToken(Type type = Null) : type(type) {}
        Type type;
        pmb::String str;
    };

    static pmb::List<Token> parseFormat(const pmb::String &str);    
    static pmb::List<TimeToken> parseTimeformat(const pmb::String &str);

public:
    pmbLogConsole();

public:
    inline pmb::List<Token> formatTokens() const { return m_formatTokens; }
    inline void setLogFormat(const pmb::String &fmt) { m_formatTokens = parseFormat(fmt); }

    inline pmb::List<TimeToken> timeFormatTokens() const { return m_timeformatTokens; }
    inline void setLogTimeFormat(const pmb::String &fmt) { m_timeformatTokens = parseTimeformat(fmt); }

    pmb::String getTimeString();
    pmb::String toString(const Token &token, pmb::LogFlag category, const pmb::Char *text);
    void logMessage(pmb::LogFlag category, const pmb::Char *text);

private:
    pmb::List<Token> m_formatTokens;
    pmb::List<TimeToken> m_timeformatTokens;
};

#endif // PMB_LOGCONSOLE_H