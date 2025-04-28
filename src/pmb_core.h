#ifndef PMB_GLOBAL_H
#define PMB_GLOBAL_H

#include "pmb_config.h"

#define PMBRIDGE_VERSION ((PMBRIDGE_VERSION_MAJOR<<16)|(PMBRIDGE_VERSION_MINOR<<8)|(PMBRIDGE_VERSION_PATCH))

#define PMBRIDGE_VERSION_STR_HELPER(major,minor,patch) #major"."#minor"."#patch

#define PMBRIDGE_VERSION_STR_MAKE(major,minor,patch) PMBRIDGE_VERSION_STR_HELPER(major,minor,patch)

#define PMBRIDGE_VERSION_STR PMBRIDGE_VERSION_STR_MAKE(PMBRIDGE_VERSION_MAJOR,PMBRIDGE_VERSION_MINOR,PMBRIDGE_VERSION_PATCH)

#include <string>
#include <vector>
#include <list>
#include <unordered_map>

#include <Modbus.h>

#define pmbCHR(x) x
#define pmbSTR(x) x

typedef unsigned int uint;

namespace pmb {

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

} // namespace pmb

#endif // PMB_GLOBAL_H