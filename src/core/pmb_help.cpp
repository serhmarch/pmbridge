#include <string.h>

const char* help_options =
"  -help (-h)               - Show help of all available options.\n"
"  -help (-h) params        - Show help of all available params.\n"
"  -help (-h) param NAME    - Show help for specific param.\n"
"  -conf (-c)               - Specifies path to daemon config-file.\n"
"  -verbose (-v)            - Enable verbose option.\n"
"                             It's useful when debug needed.\n"
"                             It's prints debug info.\n"
"  -print-conf (-p)         - Print configuration of daemon to standart output.\n"
"  -disable (-dis) ARG      - available arguments for disable:\n"
"                             * server (srv) - disable server part of daemon\n"
"                             * daemon (dmn) - disable 'daemon'-mode\n"
"  -enable (-en) ARG    - available arguments for enable:\n"
"                             * server (srv) - enable server part of daemon\n"
"                             * daemon (dmn) - enable 'daemon'-mode\n";

#define PARAM_MEMORY " MEMORY={sz0x,sz1x,sz3x,sz4x}\n"
#define PARAM_SERVER " SERVER={port,max_con}\n"
#define PARAM_COM " COM[N]={devname,type,mode,baudrate,databits,parity,stopbits[,timeout]}\n"
#define PARAM_TCP " TCP[N]={host,port[,timeout]}\n"
#define PARAM_QUERY " QUERY={devname,slave,function,from_adr,count,to_adr,ic,sc_adr,fc_adr,err_adr}\n"
#define PARAM_COPY " COPY={from_memtype,from_adr,count,to_memtype,to_adr}\n"
#define PARAM_DUMP " DUMP={memtype,adr,count,numsys}\n"
#define PARAM_DELAY " DELAY={msec}\n"

const char* help_params =
PARAM_MEMORY
PARAM_SERVER
PARAM_COM
PARAM_TCP
PARAM_QUERY
PARAM_COPY
PARAM_DELAY
PARAM_DUMP;

const char* help_MEMORY = PARAM_MEMORY
"       sz0x - size of coils (0x)-memory\n" 
"       sz1x - size of input discretes (1x)-memory\n"
"       sz3x - size of input registers (3x)-memory\n"
"       sz4x - size of holding registers (4x)-memory\n";

const char* help_SERVER = PARAM_SERVER
"       port - server ModbusTCP port (502 by default)\n"
"       max_con - maximum TCP connection for server (10 by default)\n";

const char* help_COM = PARAM_COM
"	    N - interface number\n"
"	    devname - device system name\n"
"	    type - device type. Must be RTU or ASCII\n"
"	    mode - interface mode: 0 - RS232, 1 - RS485_2WIRE, 2 - RS422, 3 - RS485_4WIRE\n"
"	    baudrate - baud rate, use from serie: 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200\n"
"	    databits - number of databits, use from serie: 5, 6, 7, 8\n"
"	    parity - parity: 0 - none, 1 - even, 2 - odd\n"
"	    stopbits - stop bits: may be 1 or 2\n"
"       timeout - unnecessary parameter, timeout for read in milliseconds (1000 by default)\n";

const char* help_TCP = PARAM_TCP
"	    N - interface number\n"
"	    host - remote host to connect\n"
"	    port - remote port to connect\n"
"       timeout - unnecessary parameter, timeout for read in milliseconds (5000 by default)\n";

const char* help_QUERY = PARAM_QUERY
"       devname - name of device\n"
"       slave - modbus address slave\n"
"       function - name of function. Must be:\n"
"                   * RD0x - read coils (0x)\n"
"                   * RD1x - read input discretes (1x)\n"
"                   * RD3x - read input registers (3x)\n"
"                   * RD4x - read holding registers (4x)\n"
"                   * WR0x - write coils (0x)\n"
"                   * WR4x - write holding registers (4x)\n"
"       from_adr - address of memory from\n"
"       count - count of elements (discret or register)\n"
"       to_adr - address of memory to\n"
"       ic - execution pattern. Specifies the query will be executed once at ic-cycle\n"
"       sc_adr - address of success counter (in bridge 3x-memory)\n"
"       fc_adr - address of fail counter (in bridge 3x-memory)\n"
"       err_adr - address of last error (in bridge 3x-memory)\n";

const char* help_COPY = PARAM_COPY
"       from_memtype - memory type to copy from. Must be:\n"
"                   * 0x - coils (0x)\n"
"                   * 1x - input discretes (1x)\n"
"                   * 3x - input registers (3x)\n"
"                   * 4x - holding registers (4x)\n"
"       from_adr - memory address to copy from\n"
"       count - count of elements to copy (discret or register)\n"
"       to_memtype - memory type to copy to. Must be:\n"
"                   * 0x - coils (0x)\n"
"                   * 1x - input discretes (1x)\n"
"                   * 3x - input registers (3x)\n"
"                   * 4x - holding registers (4x)\n"
"       to_adr - memory address to copy to\n";

const char* help_DELAY = PARAM_DELAY
"       msec - time to delay in milliseconds\n";

const char* help_DUMP = PARAM_DUMP
"       memtype - memory type to print. Must be:\n"
"                   * 0x - coils (0x)\n"
"                   * 1x - input discretes (1x)\n"
"                   * 3x - input registers (3x)\n"
"                   * 4x - holding registers (4x)\n"
"       adr - memory address to print\n"
"       count - count of elements to print (discret or register)\n"
"       numsys - number system. Must be:\n"
"                   * oct - octal\n"
"                   * dec - decimal\n"
"                   * hex - hexadecimal\n";

const char* help(int argc, char** argv)
{
    if (argc == 0)
        return help_options;
        
    if (strcmp("params", argv[0]) == 0)
        return help_params;

    if (strcmp("param", argv[0]) == 0)
    {
        if (strcmp("MEMORY", argv[1]) == 0)
            return help_MEMORY;
        if (strcmp("SERVER", argv[1]) == 0)
            return help_SERVER;
        if (strcmp("COM", argv[1]) == 0)
            return help_COM;
        if (strcmp("TCP", argv[1]) == 0)
            return help_TCP;
        if (strcmp("QUERY", argv[1]) == 0)
            return help_QUERY;
        if (strcmp("COPY", argv[1]) == 0)
            return help_COPY;
        if (strcmp("DELAY", argv[1]) == 0)
            return help_DELAY;
        if (strcmp("DUMP", argv[1]) == 0)
            return help_DUMP;
    }
    return "Unknown help option param\n";
}
