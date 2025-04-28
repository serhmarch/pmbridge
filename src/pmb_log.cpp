#include "pmb_log.h"

#include <cstdarg>
#include <iostream>

namespace pmb {

static LogFlags s_logFlags = Log_All;

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
}

void setLogTimeFormat(const String &fmt)
{
}

void logMessage(LogFlag category, const Char *format, ...)
{
    Char buffer[PMB_LOGMESSAGE_MAXLEN];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, PMB_LOGMESSAGE_MAXLEN, format, args);
    va_end(args);
    std::cout << buffer << std::endl;
}

} // namespace pmb