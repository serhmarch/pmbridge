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

const Char* toConstCharPtr(Format fmt)
{
    switch (fmt)
    {
    case Format_Bin16 : return mbSTR("Bin16" );
    case Format_Oct16 : return mbSTR("Oct16" );
    case Format_Dec16 : return mbSTR("Dec16" );
    case Format_UDec16: return mbSTR("UDec16");
    case Format_Hex16 : return mbSTR("Hex16" );
    case Format_Bin32 : return mbSTR("Bin32" );
    case Format_Oct32 : return mbSTR("Oct32" );
    case Format_Dec32 : return mbSTR("Dec32" );
    case Format_UDec32: return mbSTR("UDec32");
    case Format_Hex32 : return mbSTR("Hex32" );
    case Format_Bin64 : return mbSTR("Bin64" );
    case Format_Oct64 : return mbSTR("Oct64" );
    case Format_Dec64 : return mbSTR("Dec64" );
    case Format_UDec64: return mbSTR("UDec64");
    case Format_Hex64 : return mbSTR("Hex64" );
    case Format_Float : return mbSTR("Float" );
    case Format_Double: return mbSTR("Double");
    }
    return nullptr;
}

} // namespace mb