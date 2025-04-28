#include "pmb_core.h"

namespace pmb {

Modbus::ProtocolType toProtocolType(const String &stype, bool *ok)
{
    bool okInner = true;
    Modbus::ProtocolType res = static_cast<Modbus::ProtocolType>(-1);
    if      (stype == pmbSTR("RTU")) res = Modbus::RTU;
    else if (stype == pmbSTR("ASC")) res = Modbus::ASC;
    else if (stype == pmbSTR("TCP")) res = Modbus::TCP;
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
    if (s == pmbSTR("Bin16" )) return Format_Bin16 ;
    if (s == pmbSTR("Oct16" )) return Format_Oct16 ;
    if (s == pmbSTR("Dec16" )) return Format_Dec16 ;
    if (s == pmbSTR("UDec16")) return Format_UDec16;
    if (s == pmbSTR("Hex16" )) return Format_Hex16 ;
    if (s == pmbSTR("Bin32" )) return Format_Bin32 ;
    if (s == pmbSTR("Oct32" )) return Format_Oct32 ;
    if (s == pmbSTR("Dec32" )) return Format_Dec32 ;
    if (s == pmbSTR("UDec32")) return Format_UDec32;
    if (s == pmbSTR("Hex32" )) return Format_Hex32 ;
    if (s == pmbSTR("Bin64" )) return Format_Bin64 ;
    if (s == pmbSTR("Oct64" )) return Format_Oct64 ;
    if (s == pmbSTR("Dec64" )) return Format_Dec64 ;
    if (s == pmbSTR("UDec64")) return Format_UDec64;
    if (s == pmbSTR("Hex64" )) return Format_Hex64 ;
    if (s == pmbSTR("Float" )) return Format_Float ;
    if (s == pmbSTR("Double")) return Format_Double;
    return Format_Unknown;
}

const Char* toConstCharPtr(Format fmt)
{
    switch (fmt)
    {
    case Format_Bin16 : return pmbSTR("Bin16" );
    case Format_Oct16 : return pmbSTR("Oct16" );
    case Format_Dec16 : return pmbSTR("Dec16" );
    case Format_UDec16: return pmbSTR("UDec16");
    case Format_Hex16 : return pmbSTR("Hex16" );
    case Format_Bin32 : return pmbSTR("Bin32" );
    case Format_Oct32 : return pmbSTR("Oct32" );
    case Format_Dec32 : return pmbSTR("Dec32" );
    case Format_UDec32: return pmbSTR("UDec32");
    case Format_Hex32 : return pmbSTR("Hex32" );
    case Format_Bin64 : return pmbSTR("Bin64" );
    case Format_Oct64 : return pmbSTR("Oct64" );
    case Format_Dec64 : return pmbSTR("Dec64" );
    case Format_UDec64: return pmbSTR("UDec64");
    case Format_Hex64 : return pmbSTR("Hex64" );
    case Format_Float : return pmbSTR("Float" );
    case Format_Double: return pmbSTR("Double");
    }
    return nullptr;
}

} // namespace pmb