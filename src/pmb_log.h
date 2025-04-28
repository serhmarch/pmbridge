/*!
 * \file   pmb_log.h
 * \brief  
 * 
 * \author serhmarch
 * \date   April 2025
 */
#ifndef PMB_LOG_H
#define PMB_LOG_H

#include "pmb_core.h"

namespace pmb {

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
const Char* toConstCharPtr(LogFlag logFlag);

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

#define PMB_LOGMESSAGE_MAXLEN 1024

#ifdef __GNUC__

#define __LOG_MESSAGE(category, _MESSAGE, ...) if (pmb::logFlags() & category) pmb::logMessage(category, _MESSAGE, ##__VA_ARGS__ ); 

#define mbLogError(_MESSAGE, ...)      __LOG_MESSAGE(pmb::Log_Error     , _MESSAGE , ##__VA_ARGS__ )
#define mbLogWarning(_MESSAGE, ...)    __LOG_MESSAGE(pmb::Log_Warning   , _MESSAGE , ##__VA_ARGS__ )
#define mbLogInfo(_MESSAGE, ...)       __LOG_MESSAGE(pmb::Log_Info      , _MESSAGE , ##__VA_ARGS__ )
#define mbLogDebug(_MESSAGE, ...)      __LOG_MESSAGE(pmb::Log_Debug     , _MESSAGE , ##__VA_ARGS__ )
#define mbLogConnection(_MESSAGE, ...) __LOG_MESSAGE(pmb::Log_Connection, _MESSAGE , ##__VA_ARGS__ )
#define mbLogTx(_MESSAGE, ...)         __LOG_MESSAGE(pmb::Log_Tx        , _MESSAGE , ##__VA_ARGS__ )
#define mbLogRx(_MESSAGE, ...)         __LOG_MESSAGE(pmb::Log_Rx        , _MESSAGE , ##__VA_ARGS__ )

#else // __GNUC__

#define __LOG_MESSAGE(category, _MESSAGE, ...) if (pmb::logFlags() & category) pmb::logMessage(category, _MESSAGE, __VA_ARGS__ ); 

#define mbLogError(_MESSAGE, ...)      __LOG_MESSAGE(pmb::Log_Error     , _MESSAGE , __VA_ARGS__ )
#define mbLogWarning(_MESSAGE, ...)    __LOG_MESSAGE(pmb::Log_Warning   , _MESSAGE , __VA_ARGS__ )
#define mbLogInfo(_MESSAGE, ...)       __LOG_MESSAGE(pmb::Log_Info      , _MESSAGE , __VA_ARGS__ )
#define mbLogDebug(_MESSAGE, ...)      __LOG_MESSAGE(pmb::Log_Debug     , _MESSAGE , __VA_ARGS__ )
#define mbLogConnection(_MESSAGE, ...) __LOG_MESSAGE(pmb::Log_Connection, _MESSAGE , __VA_ARGS__ )
#define mbLogTx(_MESSAGE, ...)         __LOG_MESSAGE(pmb::Log_Tx        , _MESSAGE , __VA_ARGS__ )
#define mbLogRx(_MESSAGE, ...)         __LOG_MESSAGE(pmb::Log_Rx        , _MESSAGE , __VA_ARGS__ )

#endif // __GNUC__

#endif // PMB_LOG_H
