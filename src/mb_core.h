#ifndef MBP_GLOBAL_H
#define MBP_GLOBAL_H

#include "mb_config.h"

#define MBRIDGE_VERSION ((MBRIDGE_VERSION_MAJOR<<16)|(MBRIDGE_VERSION_MINOR<<8)|(MBRIDGE_VERSION_PATCH))

#define MBRIDGE_VERSION_STR_HELPER(major,minor,patch) #major"."#minor"."#patch

#define MBRIDGE_VERSION_STR_MAKE(major,minor,patch) MBRIDGE_VERSION_STR_HELPER(major,minor,patch)

#define MBRIDGE_VERSION_STR MBRIDGE_VERSION_STR_MAKE(MBRIDGE_VERSION_MAJOR,MBRIDGE_VERSION_MINOR,MBRIDGE_VERSION_PATCH)

#include <string>
#include <vector>
#include <list>
#include <unordered_map>

#include <Modbus.h>

#define mbCHR(x) x
#define mbSTR(x) x

typedef unsigned int uint;

namespace mb {

typedef char Char;

typedef std::string String;

typedef std::vector<uint8_t> ByteArray;

template <class T>
using List = std::list<T>;

typedef List<String> StringList;

template <class Key, class Value>
using Hash = std::unordered_map<Key, Value>;

enum Format
{
    Format_Unknown = 0,
    Format_Bin16      ,
    Format_Oct16      ,
    Format_Dec16      ,
    Format_UDec16     ,
    Format_Hex16      ,
    Format_Bin32      ,
    Format_Oct32      ,
    Format_Dec32      ,
    Format_UDec32     ,
    Format_Hex32      ,
    Format_Bin64      ,
    Format_Oct64      ,
    Format_Dec64      ,
    Format_UDec64     ,
    Format_Hex64      ,
    Format_Float      ,
    Format_Double
};

Modbus::ProtocolType toProtocolType(const String &stype, bool *ok = nullptr);

size_t sizeofFormat(Format fmt);

Format toFormat(const String &s);
const Char* toConstCharPtr(Format fmt);

template<class T>
String toBinString(T value)
{
    size_t c = sizeof(value) * MB_BYTE_SZ_BITES;
    String res(c, '0');
    while (value)
    {
        res[c-1] = '0' + static_cast<char>(value & 1);
        value >>= 1;
        c--;
    }
    return res;
}

} // namespace mb

#endif // MBP_GLOBAL_H