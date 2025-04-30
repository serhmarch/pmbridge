# pmbridge

## Overview

`pmbridge` (Programmable Modbus Bridge) is a simple cross-platform (Windows, Linux) Modbus converter application.
It supports different types of Modbus protocol: `TCP`, `RTU`, `ASC`.

'Programmable' means user can configure every Modbus function to read/write from remote device(s)
and where to put/get data within internal Modbus memory and 
have some other simple commands described below.
In this case `pmbridge` acts like a client (master).

At the same time `pmbridge` can acts like a server (slave) and provide access to the internal memory.

It's free and open source software based on `ModbusLib` project:

https://github.com/serhmarch/ModbusLib

Clients can use next list of functions:

* `1`  (`0x01`) - `READ_COILS`
* `2`  (`0x02`) - `READ_DISCRETE_INPUTS`
* `3`  (`0x03`) - `READ_HOLDING_REGISTERS`
* `4`  (`0x04`) - `READ_INPUT_REGISTERS`
* `15` (`0x0F`) - `WRITE_MULTIPLE_COILS`
* `16` (`0x10`) - `WRITE_MULTIPLE_REGISTERS`

Server can use next list of functions:

* `1`  (`0x01`) - `READ_COILS`
* `2`  (`0x02`) - `READ_DISCRETE_INPUTS`
* `3`  (`0x03`) - `READ_HOLDING_REGISTERS`
* `4`  (`0x04`) - `READ_INPUT_REGISTERS`
* `5`  (`0x05`) - `WRITE_SINGLE_COIL`
* `6`  (`0x06`) - `WRITE_SINGLE_REGISTER`
* `7`  (`0x07`) - `READ_EXCEPTION_STATUS`
* `15` (`0x0F`) - `WRITE_MULTIPLE_COILS`
* `16` (`0x10`) - `WRITE_MULTIPLE_REGISTERS`
* `17` (`0x11`) - `REPORT_SERVER_ID`
* `22` (`0x16`) - `MASK_WRITE_REGISTER`
* `23` (`0x17`) - `READ_WRITE_MULTIPLE_REGISTERS`

## Using pmbridge

To use `pmbridge` user need to make configuration file: `pmbridge.conf`.
There is a program (configuration) defined in this file.

### Configuration file

Configuration file contains list of commands (program).
There can be declaration and execution commands.

#### Declaration commands

* `MEMORY={<0x>,<1x>,<3x>,<4x>}`

  Command for inner memory configuration.
  * `0x` - quantity of coils (0x)-memory 
  * `1x` - quantity of input discretes (1x)-memory 
  * `3x` - quantity of input registers (3x)-memory 
  * `4x` - quantity of holding registers (4x)-memory
 
* `SERVER={<type>,<name>,...}`
* `CLIENT={<type>,<name>,...}`

  Command to create server and client port respectively.
  * `type` - type of Modbus protocol of the port. Can be {RTU,ASC,TCP}
  * `name` - name/id of the port. It is used for QUERY commands (client) and log

  Types of port and parameters:

  * `SERVER={TCP,<name>,<tcpport>,<timeout>,<maxconn>}`

    * `tcpport` - unnecessary parameter, server ModbusTCP port (502 by default)
    * `timeout` - unnecessary parameter, timeout for read in milliseconds (3000 by default)
    * `maxconn` - unnecessary parameter, maximum TCP connection for server (10 by default)

  * `CLIENT={TCP,<name>,<host>,<tcpport>,<timeout>}`

	  * `host`    - remote host to connect
	  * `tcpport` - unnecessary parameter, remote port to connect (502 by default)
    * `timeout` - unnecessary parameter, timeout for read in milliseconds (3000 by default)

  * `SERVER={RTU,<name>,<devname>,<baudrate>,<databits>,<parity>,<stopbits>,<flowcontrol>,<timeoutfb>,<timeoutib>}`

    `SERVER={ASC,<name>,<devname>,<baudrate>,<databits>,<parity>,<stopbits>,<flowcontrol>,<timeoutfb>,<timeoutib>}`

    `CLIENT={RTU,<name>,<devname>,<baudrate>,<databits>,<parity>,<stopbits>,<flowcontrol>,<timeoutfb>,<timeoutib>}`

    `CLIENT={ASC,<name>,<devname>,<baudrate>,<databits>,<parity>,<stopbits>,<flowcontrol>,<timeoutfb>,<timeoutib>}`

	  * `devname`     - device system name or port name. For example: COM13, /dev/ttyM0, /dev/ttyUSB0 etc
	  * `baudrate`    - unnecessary parameter, baud rate, use from serie: 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 (9600 by default)
	  * `databits`    - unnecessary parameter, number of databits, use from serie: 5, 6, 7, 8  (8 by default)
	  * `parity`      - unnecessary parameter, parity: None,N - none; Even,E - even; Odd,O - odd (None by default)
	  * `stopbits`    - unnecessary parameter, stop bits: may be 1, 1.5 or 2 (1 by default)
	  * `flowcontrol` - unnecessary parameter, flow control: No, Hard, Soft (No by default)
    * `timeoutfb`   - unnecessary parameter, timeout for read first byte of the input packet in milliseconds (1000 by default)
    * `timeoutib`   - unnecessary parameter, timeout for read next bytes of the input packet in milliseconds (50 by default)

#### Execution commands

* `QUERY={<client>,<unit>,<func>,<devadr>,<count>,<memadr>,<execpatt>,<succadr>,<errcadr>,<errvadr>}`

  Command for remote request for previously configured client port.

  * `client`   - name of client port previously defined in `CLIENT` command
  * `unit`     - modbus unit/address slave
  * `func`     - name of function. Can be {RD,WR}. What to read/write defined in the next `devadr` parameter.
  * `devadr`   - address of first item of the remote device to read/write
  * `count`    - count of elements (discrete or register)
  * `memadr`   - address within inner memory to get/set
  * `execpatt` - execution pattern. Specifies the query will be executed once at execpatt-cycle
  * `succadr`  - address of success counter within inner memory
  * `errcadr`  - address of error counter within inner memory
  * `errvadr`  - address of last error within inner memory

* `COPY={<srcadr>,<count>,<destadr>}`

  Copy data from one part of memory to another
  * `srcadr`  - memory address to copy from
  * `count`   - count of elements to copy (discret or register)
  * `destadr` - memory address to copy to

* `DELAY={<msec>}`

  Delay execution of next command.

  * `msec`    - time to delay in milliseconds

* `DUMP={<memadr>,<count>,<format>}`

  Print memory dump into console.

  * `memadr`  - memory address to print
  * `count`   - count of elements to print (discret or register)
  * `format`  - format of element. Must be:
    * `Bin16` 
    * `Oct16` 
    * `Dec16` 
    * `UDec16`
    * `Hex16` 
    * `Bin32` 
    * `Oct32` 
    * `Dec32` 
    * `UDec32`
    * `Hex32` 
    * `Bin64` 
    * `Oct64` 
    * `Dec64` 
    * `UDec64`
    * `Hex64` 
    * `Float` 
    * `Double`

#### Memory addressing

`pmbridge` support 3 types of memory addressing format:

| Memory type       | Standard (1 based) | IEC 61131-3 (0 based)| IEC 61131-3 Hex (0 based)
|-------------------|--------------------|----------------------|---------------------------
| Coils             | `000001`           | `%Q0`                | `%Q0000h`                 
| Discrete inputs   | `100016`           | `%I15`               | `%I000Fh`                
| Input registers   | `300017`           | `%IW16`              | `%IW0010h`               
| Holding registers | `406658`           | `%MW6657`            | `%MW1A01h`

#### Example of the program

```conf
# Declaration. Memory with 1000 coils, 1000 input discretes, 1000 input registers and 5000 holding registers
MEMORY={1000,1000,1000,5000}

# Declaration. Port as TCP server with port 502, timeout 5000ms and max 10 connections
SERVER={TCP,serv,502,5000,10}

# Declaration. Port as RTU client with portname "/dev/ttyM0", baudrate 19200, 8 databits, even parity, 1.5 stopbits
CLIENT={RTU,client1,"/dev/ttyM0",
                    19200,
                    8,
                    E,
                    1.5,
                    Hard,
                    1000,
                    50}

# Declaration. Port as TCP client for 'localhost'
CLIENT={TCP,client2,"127.0.0.1"}

# Execution. Write 10 holding register into remote device (from 400001 to 400010) 
# with values from inner memory of 300001 to 300010.
# Success counter is 300901, error counter is 300902 and last error code is 300903 within inner memory.
QUERY={client1,1,WR,400001,10,300001,1,300901,300902,300903}

# Execution. Print inner memory dump into console: 3 registers (from 300901 to 300903) in hexadecimal format
DUMP={300901,3,Hex16}

# Execution. Wait 200 milliseconds
DELAY={200}

# Execution. Read 10 holding register from remote device (from 400001 to 400010)
# and put values into inner memory 400101 to 400110.
# Success counter is 300904, error counter is 300905 and last error code is 300906 within inner memory.
QUERY={client2,1,RD,%MW0,3,%MW100,1,300904,300905,300906}

# Execution. Print inner memory dump into console: 3 registers
# (from 300904 to 300906) in decimal format
DUMP={300904,3,Dec16}

# Execution. Copy 6 registers (previously defined counters)
# from inner memory 300901 to 300906 into inner memory 400001 to 400006
COPY={300901,6,%MW0000h}

# Execution. Wait 2 seconds (2000 milliseconds)
DELAY={2000}
```

### Command line options

To show list of available parameters print:
```console
$ pmbridge --help
Usage: pmbridge [options]

Options:
  --version (-v)       - show program version.
  --help (-?)          - show this help.
  --file (-f)          - path to *.conf file (pmbridge.conf by default)
  --log-flags (-lc)    - list of log flags (categories); `,`, `;` or `|` separated
  --log-format (-lfmt) - format of each message to output
  --log-time (-lt)     - format of time of each message to output

Format can contain following special symbols:
  `%time` - timestamp of the message, for which the format is also specified (`--log-time`)
  `%cat`  - text representation of the message category
  `%text` - text of the message
  
Time format of the message (`%time`) has the following special symbols:          
  `%Y` - year (4 characters)
  `%M` - month (2 characters `01`-`12`)
  `%D` - day (2 characters `01`-`31`)
  `%h` - hour (2 characters `00`-`23`)
  `%m` - minute (2 characters `00`-`59`)
  `%s` - second (2 characters `00`-`59`)
  `%f` - millisecond (3 characters `000`-`999`)

Category of the message. Can be:
  `ERR`   - error messages
  `WARN`  - warning messages 
  `INFO`  - information messages
  `DBG`   - debug messages
  `CONN`  - connect/disconnect messages
  `TX`    - transmited Modbus messages
  `RX`    - received Modbus messages
  `ALL`   - all categories included
```

## Build using CMake

1.  Build Tools

    Previously you need to install c++ compiler kit, git and cmake itself.
    Then set PATH env variable to find compliler, cmake, git etc.

2.  Create project directory, move to it and clone repository:
    ```console
    $ cd ~
    $ mkdir src
    $ cd src
    $ git clone --recursive https://github.com/serhmarch/pmbridge.git
    ```

3.  Create and/or move to directory for build output, e.g. `~/bin/pmbridge`:
    ```console
    $ cd ~
    $ mkdir -p bin/pmbridge
    $ cd bin/pmbridge
    ```

4.  Run cmake to generate project (make) files.
    ```console
    $ cmake -S ~/src/pmbridge -B .
    ```

5.  Make binaries (+ debug|release config):
    ```console
    $ cmake --build .
    $ cmake --build . --config Debug
    $ cmake --build . --config Release
    ```    
    
6.  Resulting bin files is located in `./bin` directory.
