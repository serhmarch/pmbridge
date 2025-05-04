/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmbLogConsole.h"

#include <chrono>
#include <ctime>
#include <iostream>

pmb::List<pmbLogConsole::Token> pmbLogConsole::parseFormat(const pmb::String &str)
{
    size_t pos = 0, len = str.length();
    Token token;
    pmb::List<Token> res;
    while (pos < len) 
    {
        pmb::Char c = str[pos];
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

pmb::List<pmbLogConsole::TimeToken> pmbLogConsole::parseTimeformat(const pmb::String &str)
{
    size_t pos = 0, len = str.length();
    TimeToken token;
    pmb::List<TimeToken> res;
    while (pos < len) 
    {
        pmb::Char c = str[pos];
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
                    pmb::Char buff[3];
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
    if (token.type == TimeToken::Literal) 
        res.push_back(token);
    return res;
}

pmbLogConsole::pmbLogConsole()
{
}

pmb::String pmbLogConsole::getTimeString()
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

    pmb::String res;
    for (auto it = m_timeformatTokens.begin(); it != m_timeformatTokens.end(); ++it) 
    {
        switch ((*it).type)
        {
        case TimeToken::Year       : res += pmb::toDecString(year       , 4); break;
        case TimeToken::Month      : res += pmb::toDecString(month      , 2); break;
        case TimeToken::Day        : res += pmb::toDecString(day        , 2); break;
        case TimeToken::Hour       : res += pmb::toDecString(hour       , 2); break;
        case TimeToken::Minute     : res += pmb::toDecString(minute     , 2); break;
        case TimeToken::Second     : res += pmb::toDecString(second     , 2); break;
        case TimeToken::Millisecond: res += pmb::toDecString(millisecond, 3); break;
        case TimeToken::Literal    : res += (*it).str;                   break;
        }
    }
    return res;
}

pmb::String pmbLogConsole::toString(const pmbLogConsole::Token &token, pmb::LogFlag category, const pmb::Char *text)
{
    switch (token.type)
    {
    case Token::Literal : return token.str;
    case Token::Time    : return getTimeString();
    case Token::Category: return pmb::String(toConstCharPtr(category));
    case Token::Text    : return pmb::String(text);
    }
    return pmb::String();
}

void pmbLogConsole::logMessage(pmb::LogFlag category, const pmb::Char *text)
{
    pmb::String res;
    for (auto it = m_formatTokens.begin(); it != m_formatTokens.end(); ++it) 
        res += toString(*it, category, text);
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
