/*!
 * \file   mb_log.h
 * \brief  
 * 
 * \author serhmarch
 * \date   April 2025
 */
#ifndef MB_LOG_H
#define MB_LOG_H

#include "mb_core.h"

namespace mb {

enum LogFlag
{
    Log_Error           = 0x0000000000000001,
    Log_Warning         = 0x0000000000000002,
    Log_Info            = 0x0000000000000004,
    Log_Debug           = 0x0000000000000008,
    Log_Connection      = 0x0000000000000010,
    Log_Tx              = 0x0000000000000020,
    Log_Rx              = 0x0000000000000040,
    Log_All             = 0xFFFFFFFFFFFFFFFF
};

typedef uint64_t LogFlags;

/// \details
LogFlags logFlags();

/// \details
void setLogFlags(LogFlags flags);

/// \details
void setLogFormat(const String &fmt);

/// \details
void setLogTimeFormat(const String &fmt);

/// \details
void logMessage(LogFlag category, const Char *format, ...);

} // namespace  mb

#define MB_LOGMESSAGE_MAXLEN 1024

#ifdef __GNUC__

#define __LOG_MESSAGE(category, _MESSAGE, ...) if (mb::logFlags() & category) mb::logMessage(category, _MESSAGE, ##__VA_ARGS__ ); 

#define mbLogError(_MESSAGE, ...)      __LOG_MESSAGE(mb::Log_Error     , _MESSAGE , ##__VA_ARGS__ )
#define mbLogWarning(_MESSAGE, ...)    __LOG_MESSAGE(mb::Log_Warning   , _MESSAGE , ##__VA_ARGS__ )
#define mbLogInfo(_MESSAGE, ...)       __LOG_MESSAGE(mb::Log_Info      , _MESSAGE , ##__VA_ARGS__ )
#define mbLogDebug(_MESSAGE, ...)      __LOG_MESSAGE(mb::Log_Debug     , _MESSAGE , ##__VA_ARGS__ )
#define mbLogConnection(_MESSAGE, ...) __LOG_MESSAGE(mb::Log_Connection, _MESSAGE , ##__VA_ARGS__ )
#define mbLogTx(_MESSAGE, ...)         __LOG_MESSAGE(mb::Log_Tx        , _MESSAGE , ##__VA_ARGS__ )
#define mbLogRx(_MESSAGE, ...)         __LOG_MESSAGE(mb::Log_Rx        , _MESSAGE , ##__VA_ARGS__ )

#else // __GNUC__

#define __LOG_MESSAGE(category, _MESSAGE, ...) if (mb::logFlags() & category) mb::logMessage(category, _MESSAGE, __VA_ARGS__ ); 

#define mbLogError(_MESSAGE, ...)      __LOG_MESSAGE(mb::Log_Error     , _MESSAGE , __VA_ARGS__ )
#define mbLogWarning(_MESSAGE, ...)    __LOG_MESSAGE(mb::Log_Warning   , _MESSAGE , __VA_ARGS__ )
#define mbLogInfo(_MESSAGE, ...)       __LOG_MESSAGE(mb::Log_Info      , _MESSAGE , __VA_ARGS__ )
#define mbLogDebug(_MESSAGE, ...)      __LOG_MESSAGE(mb::Log_Debug     , _MESSAGE , __VA_ARGS__ )
#define mbLogConnection(_MESSAGE, ...) __LOG_MESSAGE(mb::Log_Connection, _MESSAGE , __VA_ARGS__ )
#define mbLogTx(_MESSAGE, ...)         __LOG_MESSAGE(mb::Log_Tx        , _MESSAGE , __VA_ARGS__ )
#define mbLogRx(_MESSAGE, ...)         __LOG_MESSAGE(mb::Log_Rx        , _MESSAGE , __VA_ARGS__ )

#endif // __GNUC__

#endif // MB_LOG_H
