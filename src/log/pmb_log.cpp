/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmb_log.h"

#include <cstdarg>

#include "pmbLogConsole.h"

namespace pmb {

#define ccTOKEN_TIME "%time"
#define ccTOKEN_TEXT "%text"
#define ccTOKEN_CAT "%cat"

static pmbLogConsole s_logConsole;

static LogFlags s_logFlags = Log_All;

Modbus::Color toColor(LogFlag flag)
{
    switch (flag)
    {
    case Log_Error  : return Modbus::Color_Red    ;
    case Log_Warning: return Modbus::Color_Yellow ;
    case Log_Dump   : return Modbus::Color_Green  ;
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
    case Log_Dump      : return pmbSTR("DUMP");
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
    if (strcmp(slogFlag, pmbSTR("DUMP")) == 0) return Log_Dump      ;
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