#include <string.h>

const char* help_options =
"Usage: pmbridge [options]\n"
"\n"
"Options:\n"
"  --version (-v)         - show program version.\n"
"  --help (-?)[all|<cmd>] - show this help, 'all' commands or specific command help.\n"
"  --file (-f)            - path to *.conf file (pmbridge.conf by default)\n"
"  --log-flags (-lc)      - list of log flags (categories); `,`, `;` or `|` separated\n"
"  --log-format (-lf)   - format of each message to output\n"
"  --log-time (-lt)       - format of time of each message to output\n";


#define CMD_MEMORY " MEMORY={<0x>,<1x>,<3x>,<4x>}\n"
#define CMD_SERVER " SERVER={<type>,<name>,...}\n"
#define CMD_CLIENT " CLIENT={<type>,<name>,...}\n"
#define CMD_QUERY " QUERY={<client>,<unit>,<func>,<devadr>,<count>,<memadr>,<execpatt>,<succadr>,<errcadr>,<errvadr>}\n"
#define CMD_COPY " COPY={<srcadr>,<count>,<destadr>}\n"
#define CMD_DUMP " DUMP={<memadr>,<count>,<format>}\n"
#define CMD_DELAY " DELAY={<msec>}\n"

#define CMD_MEMORY_DESCR "   Command for inner memory configuration.\n"
#define CMD_SERVER_DESCR "   Command to create server.\n"
#define CMD_CLIENT_DESCR "   Command to create client.\n"
#define CMD_QUERY_DESCR "   Command for remote request for previously configured client port.\n"
#define CMD_COPY_DESCR "   Command to copy data within inner memory.\n"
#define CMD_DUMP_DESCR "   Command to print current inner memory data with defined format.\n"
#define CMD_DELAY_DESCR "   Command to delay execution (wait) for defined milliseconds.\n"

const char* help_params =
CMD_MEMORY
CMD_SERVER
CMD_CLIENT
CMD_QUERY
CMD_COPY
CMD_DELAY
CMD_DUMP;

#define CMD_PARAM_SERIAL \
"    devname     - device system name or port name. For example: COM13, /dev/ttyM0, /dev/ttyUSB0 etc\n"                                          \
"    baudrate    - unnecessary parameter, baud rate, use from serie: 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 (9600 by default)\n"    \
"    databits    - unnecessary parameter, number of databits, use from serie: 5, 6, 7, 8  (8 by default)\n"                                      \
"    parity      - unnecessary parameter, parity: None,N - none; Even,E - even; Odd,O - odd (None by default)\n"                                 \
"    stopbits    - unnecessary parameter, stop bits: may be 1, 1.5 or 2 (1 by default)\n"                                                        \
"    flowcontrol - unnecessary parameter, flow control: No, Hard, Soft (No by default)\n"                                                        \
"    timeoutfb   - unnecessary parameter, timeout for read first byte of the input packet in milliseconds (1000 by default)\n"                   \
"    timeoutib   - unnecessary parameter, timeout for read next bytes of the input packet in milliseconds (50 by default)\n"

#define CMD_PARAM_SERVER_TCP \
"    tcpport - unnecessary parameter, server ModbusTCP port (502 by default)\n"              \
"    timeout - unnecessary parameter, timeout for read in milliseconds (3000 by default)\n"  \
"    maxconn - unnecessary parameter, maximum TCP connection for server (10 by default)\n"

#define CMD_PARAM_CLIENT_TCP \
"    host    - remote host to connect\n"                                                     \
"    tcpport - unnecessary parameter, remote port to connect (502 by default)\n"             \
"    timeout - unnecessary parameter, timeout for read in milliseconds (3000 by default)\n"

#define CMD_SERVER_SERIAL \
" SERVER={RTU,<name>,<devname>,<baudrate>,<databits>,<parity>,<stopbits>,<flowcontrol>,<timeoutfb>,<timeoutib>}\n" \
" SERVER={ASC,<name>,<devname>,<baudrate>,<databits>,<parity>,<stopbits>,<flowcontrol>,<timeoutfb>,<timeoutib>}\n"

#define CMD_SERVER_TCP \
" SERVER={TCP,<name>,<tcpport>,<timeout>,<maxconn>}\n"

#define CMD_CLIENT_SERIAL \
" CLIENT={RTU,<name>,<devname>,<baudrate>,<databits>,<parity>,<stopbits>,<flowcontrol>,<timeoutfb>,<timeoutib>}\n" \
" CLIENT={ASC,<name>,<devname>,<baudrate>,<databits>,<parity>,<stopbits>,<flowcontrol>,<timeoutfb>,<timeoutib>}\n"

#define CMD_CLIENT_TCP \
" CLIENT={TCP,<name>,<host>,<tcpport>,<timeout>}\n"

const char* help_CMD_MEMORY = CMD_MEMORY
CMD_MEMORY_DESCR
"    0x - count of coils (0x)-memory\n" 
"    1x - count of input discretes (1x)-memory\n"
"    3x - count of input registers (3x)-memory\n"
"    4x - count of holding registers (4x)-memory\n";

const char* help_CMD_SERVER = CMD_SERVER
CMD_SERVER_DESCR
CMD_SERVER_SERIAL
CMD_PARAM_SERIAL
CMD_SERVER_TCP
CMD_PARAM_SERVER_TCP;

const char* help_CMD_CLIENT = CMD_CLIENT
CMD_CLIENT_DESCR
CMD_CLIENT_SERIAL
CMD_PARAM_SERIAL
CMD_CLIENT_TCP
CMD_PARAM_CLIENT_TCP;


const char* help_CMD_QUERY = CMD_QUERY
CMD_QUERY_DESCR
"    client   - name of client port previously defined in `CLIENT` command\n"
"    unit     - modbus unit/address slave\n"
"    func     - name of function. Can be {RD,WR}. What to read/write defined in the next `devadr` parameter\n"
"    devadr   - address of first item of the remote device to read/write\n"
"    count    - count of elements (discrete or register)\n"
"    memadr   - address within inner memory to get/set\n"
"    execpatt - execution pattern. Specifies the query will be executed once at execpatt-cycle\n"
"    succadr  - address of success counter within inner memory\n"
"    errcadr  - address of error counter within inner memory\n"
"    errvadr  - address of last error within inner memory\n";

const char* help_CMD_COPY = CMD_COPY
CMD_COPY_DESCR
"    srcadr   - memory address to copy from\n"
"    count    - count of elements to copy (discret or register)\n"
"    destadr  - memory address to copy to\n";

const char* help_CMD_DELAY = CMD_DELAY
CMD_DELAY_DESCR
"    msec - time to delay in milliseconds\n";

const char* help_CMD_DUMP = CMD_DUMP
CMD_DUMP_DESCR
"    memadr  - memory address to print\n"
"    count   - count of elements to print (discrete or register)\n"
"    format  - format of element. Can be:\n"
"                Bin16   Bin32   Bin64 \n"
"                Oct16   Oct32   Oct64 \n"
"                Dec16   Dec32   Dec64 \n"
"                UDec16  UDec32  UDec64\n"
"                Hex16   Hex32   Hex64 \n"
"                        Float   Double\n";

const char* help(int argc, char** argv)
{
    if (argc == 0)
        return help_options;
        
    if (strcmp("all", argv[0]) == 0)
        return help_params;
    if (strcmp("MEMORY", argv[0]) == 0)
        return help_CMD_MEMORY;
    if (strcmp("SERVER", argv[0]) == 0)
        return help_CMD_SERVER;
    if (strcmp("CLIENT", argv[0]) == 0)
        return help_CMD_CLIENT;
    if (strcmp("QUERY", argv[0]) == 0)
        return help_CMD_QUERY;
    if (strcmp("COPY", argv[0]) == 0)
        return help_CMD_COPY;
    if (strcmp("DELAY", argv[0]) == 0)
        return help_CMD_DELAY;
    if (strcmp("DUMP", argv[0]) == 0)
        return help_CMD_DUMP;
    return "Unknown help option param\n";
}
