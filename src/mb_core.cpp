#include "mb_core.h"

namespace mb {

Modbus::ProtocolType toProtocolType(const String &stype, bool *ok)
{
    bool okInner = true;
    Modbus::ProtocolType res = static_cast<Modbus::ProtocolType>(-1);
    if      (stype == mbSTR("RTU")) res = Modbus::RTU;
    else if (stype == mbSTR("ASC")) res = Modbus::ASC;
    else if (stype == mbSTR("TCP")) res = Modbus::TCP;
    else
        okInner = false;
    if (ok)
        *ok = okInner;
    return res;
}

size_t sizeofFormat(Format fmt)
{
    switch (fmt)
    {
    case Format_Bin16:
    case Format_Oct16:
    case Format_Dec16:
    case Format_UDec16:
    case Format_Hex16:
        return sizeof(uint16_t);
    case Format_Bin32:
    case Format_Oct32:
    case Format_Dec32:
    case Format_UDec32:
    case Format_Hex32:
        return sizeof(uint32_t);
    case Format_Bin64:
    case Format_Oct64:
    case Format_Dec64:
    case Format_UDec64:
    case Format_Hex64:
        return sizeof(uint64_t);
    case Format_Float:
        return sizeof(float);
    case Format_Double:
        return sizeof(double);
    default:
        return 0; // Unknown format
    }
}

Format toFormat(const String &s)
{
    if (s == mbSTR("Bin16" )) return Format_Bin16 ;
    if (s == mbSTR("Oct16" )) return Format_Oct16 ;
    if (s == mbSTR("Dec16" )) return Format_Dec16 ;
    if (s == mbSTR("UDec16")) return Format_UDec16;
    if (s == mbSTR("Hex16" )) return Format_Hex16 ;
    if (s == mbSTR("Bin32" )) return Format_Bin32 ;
    if (s == mbSTR("Oct32" )) return Format_Oct32 ;
    if (s == mbSTR("Dec32" )) return Format_Dec32 ;
    if (s == mbSTR("UDec32")) return Format_UDec32;
    if (s == mbSTR("Hex32" )) return Format_Hex32 ;
    if (s == mbSTR("Bin64" )) return Format_Bin64 ;
    if (s == mbSTR("Oct64" )) return Format_Oct64 ;
    if (s == mbSTR("Dec64" )) return Format_Dec64 ;
    if (s == mbSTR("UDec64")) return Format_UDec64;
    if (s == mbSTR("Hex64" )) return Format_Hex64 ;
    if (s == mbSTR("Float" )) return Format_Float ;
    if (s == mbSTR("Double")) return Format_Double;
    return Format_Unknown;
}

const Char *sIEC61131Prefix0x  = mbSTR("%Q") ;
const Char *sIEC61131Prefix1x  = mbSTR("%I") ;
const Char *sIEC61131Prefix3x  = mbSTR("%IW");
const Char *sIEC61131Prefix4x  = mbSTR("%MW");   
const Char  cIEC61131SuffixHex = mbCHR('h');

Address Address::fromString(const String &s)
{
    if (s.size() && s.at(0) == '%')
    {
        Address adr;
        size_t i;
        // Note: 3x (%IW) handled before 1x (%I)
        if (s.find(sIEC61131Prefix3x, 0) == 0) // Check if string starts with sIEC61131Prefix3x
        {
            adr.m_type = Modbus::Memory_3x;
            i = std::strlen(sIEC61131Prefix3x);
        }
        else if (s.find(sIEC61131Prefix4x, 0) == 0) // Check if string starts with sIEC61131Prefix4x
        {
            adr.m_type = Modbus::Memory_4x;
            i = std::strlen(sIEC61131Prefix4x);
        }
        else if (s.find(sIEC61131Prefix0x, 0) == 0) // Check if string starts with sIEC61131Prefix0x
        {
            adr.m_type = Modbus::Memory_0x;
            i = std::strlen(sIEC61131Prefix0x);
        }
        else if (s.find(sIEC61131Prefix1x, 0) == 0) // Check if string starts with sIEC61131Prefix1x
        {
            adr.m_type = Modbus::Memory_1x;
            i = std::strlen(sIEC61131Prefix1x);
        }
        else
            return Address();

        Char suffix = s.back();
        if (suffix == cIEC61131SuffixHex)
        {
            char* end = nullptr;
            adr.m_offset = static_cast<uint16_t>(std::strtol(s.substr(i, s.size() - i - 1).data(), &end, 16));
        }
        else
        {
            adr.m_offset = static_cast<uint16_t>(std::atoi(s.substr(i).data()));
        }
        return adr;
    }
    return Address(std::atoi(s.data()));
}

Address::Address()
{
    m_type = Modbus::Memory_Unknown;
    m_offset = 0;
}

Address::Address(Modbus::MemoryType type, uint16_t offset) :
    m_type(type),
    m_offset(offset)
{
}

Address::Address(uint32_t adr)
{
    this->operator=(adr);
}

String Address::toString() const
{
    if (isValid())
        return std::to_string(m_type) + std::to_string(number()).insert(0, 5 - std::to_string(number()).length(), '0');
    else
        return String();
}

Address &Address::operator=(uint32_t v)
{
    uint32_t number = v % 100000;
    if ((number < 1) || (number > 65536))
    {
        m_type = Modbus::Memory_Unknown;
        m_offset = 0;
        return *this;
    }
    uint16_t type = static_cast<uint16_t>(v/100000);
    switch(type)
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
    case Modbus::Memory_3x:
    case Modbus::Memory_4x:
        m_type = type;
        m_offset = static_cast<uint16_t>(number-1);
        break;
    default:
        m_type = Modbus::Memory_Unknown;
        m_offset = 0;
        break;
    }
    return *this;
}

} // namespace mb