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

class Address
{
public:
    static Address fromString(const String &s);

public:
    /// \details Defauilt constructor ot the class. Creates invalid Modbus Data Address
    Address();

    /// \details Constructor ot the class. E.g. `Address(Modbus::Memory_4x, 0)` creates `400001` standard address. 
    Address(Modbus::MemoryType, uint16_t offset);

    /// \details Constructor ot the class. E.g. `Address(400001)` creates `Address` with type `Modbus::Memory_4x`
    /// and offset `0`, and `Address(1)` creates `Address` with type `Modbus::Memory_0x` and offset `0`. 
    Address(uint32_t adr);

public:
    /// \details Returns `true` if memory type is `Modbus::Memory_Unknown`, `false` otherwise
    inline bool isValid() const { return m_type != Modbus::Memory_Unknown; }

    /// \details Returns memory type of Modbus Data Address
    inline Modbus::MemoryType type() const { return static_cast<Modbus::MemoryType>(m_type); }
 
    /// \details Returns memory offset of Modbus Data Address
    inline uint16_t offset() const { return m_offset; }
 
    /// \details Returns memory number (offset+1) of Modbus Data Address
    inline uint32_t number() const { return m_offset+1; }
 
    /// \details Returns string repr of Modbus Data Address
    /// e.g. `Address(Modbus::Memory_4x, 0)` will be converted to `QString("400001")`.
    String toString() const;

    /// \details Converts current Modbus Data Address to `quint32`,
    /// e.g. `Address(Modbus::Memory_4x, 0)` will be converted to `400001`.
    inline operator uint32_t () const { return number() + (m_type*100000);  }

    /// \details Assigment operator definition.
    Address &operator= (uint32_t v);

private:
    uint16_t m_type;
    uint16_t m_offset;
};

} // namespace mb

#endif // MBP_GLOBAL_H