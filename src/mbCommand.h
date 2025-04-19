#ifndef MB_COMMAND_H
#define MB_COMMAND_H

#include "mb_core.h"
#include "mbMemory.h"

class mbMemory;
class mbClient;

class mbCommand
{
public:
    virtual ~mbCommand();
    virtual bool run() = 0;
};


/************************************************************************
 ********************************* QUERY ********************************
 ************************************************************************/

class mbCommandQuery : public mbCommand
{
public:
    mbCommandQuery(mbMemory *memory, mbClient *client);
    ~mbCommandQuery() override;

public:
    inline mbMemory *memory() const { return m_memory; }
    inline mbClient *client() const { return m_client; }

    inline uint8_t unit() const { return m_unit; }
    inline void setUnit(uint8_t unit) { m_unit = unit; }

    inline uint16_t exec() const { return m_exec; }
    inline void setExec(uint16_t exec) { m_exec = exec; }

    inline Modbus::Address succAddress() const { return m_succAdr; }
    inline void setSuccAddress(Modbus::Address adr) { m_succAdr = adr; }

    inline Modbus::Address errcAddress() const { return m_errcAdr; }
    inline void setErrcAddress(Modbus::Address adr) { m_errcAdr = adr; }

    inline Modbus::Address errvAddress() const { return m_errvAdr; }
    inline void setErrvAddress(Modbus::Address adr) { m_errvAdr = adr; }
    
public:
    bool run() override;

protected:
    virtual Modbus::StatusCode beginQuery();
    virtual Modbus::StatusCode runQuery() = 0;

protected:
    mbMemory *m_memory;
    mbClient *m_client;
    uint8_t m_unit;
    uint16_t m_exec;
    Modbus::Address m_succAdr;
    Modbus::Address m_errcAdr;
    Modbus::Address m_errvAdr;
    mb::ByteArray m_buffer;
    bool m_isBegin;
};

class mbCommandQueryBase : public mbCommandQuery
{
public:
    using mbCommandQuery::mbCommandQuery;

public:
    inline uint16_t offset() const { return m_offset; }
    inline void setOffset(uint16_t offset) { m_offset = offset; }

    inline uint16_t count() const { return m_count; }
    inline void setCount(uint16_t c) { m_count = c; }

    inline Modbus::Address memAddress() const { return m_memAdr; }
    inline void setMemAddress(Modbus::Address adr) { m_memAdr = adr; }

protected:
    uint16_t m_offset;
    uint16_t m_count;
    Modbus::Address m_memAdr;
};

class mbCommandQueryReadCoils : public mbCommandQueryBase
{   
public:
    using mbCommandQueryBase::mbCommandQueryBase;
    Modbus::StatusCode runQuery() override;
};

class mbCommandQueryReadDiscreteInputs : public mbCommandQueryBase
{   
public:
    using mbCommandQueryBase::mbCommandQueryBase;
    Modbus::StatusCode runQuery() override;
};

class mbCommandQueryReadHoldingRegisters : public mbCommandQueryBase
{   
public:
    using mbCommandQueryBase::mbCommandQueryBase;
    Modbus::StatusCode runQuery() override;
};

class mbCommandQueryReadInputRegisters : public mbCommandQueryBase
{   
public:
    using mbCommandQueryBase::mbCommandQueryBase;
    Modbus::StatusCode runQuery() override;
};

class mbCommandQueryWriteMultipleCoils : public mbCommandQueryBase
{   
public:
    using mbCommandQueryBase::mbCommandQueryBase;
    Modbus::StatusCode beginQuery() override;
    Modbus::StatusCode runQuery() override;
};

class mbCommandQueryWriteMultipleRegisters : public mbCommandQueryBase
{   
public:
    using mbCommandQueryBase::mbCommandQueryBase;
    Modbus::StatusCode beginQuery() override;
    Modbus::StatusCode runQuery() override;
};


/************************************************************************
 ********************************* COPY *********************************
 ************************************************************************/

class mbCommandCopy : public mbCommand
{
public:
    mbCommandCopy(mbMemory *memory);

public:
    inline Modbus::Address srcAddress() const { return m_srcAdr; }
    inline Modbus::Address dstAddress() const { return m_dstAdr; }
    inline uint16_t count() const { return m_count; }
    void setParams(Modbus::Address srcAddress, Modbus::Address dstAddress, uint16_t count);

public:
    bool run() override;

protected:
    void calcreadbits();
    void calcreadbytes();
    void calcwritebits(bool countIsBits);
    void calcwritebytes(bool countIsBits);
    void zeroCount();

protected:
    void readBits();
    void readBytes();
    void writeBits();
    void writeBytes();

protected:
    mbMemory *m_memory;
    mbMemory::Block *m_readblock;
    mbMemory::Block *m_writeblock;
    Modbus::Address m_srcAdr;
    Modbus::Address m_dstAdr;
    uint16_t m_count;
    mb::ByteArray m_buff;
    uint16_t m_readOffset;
    uint16_t m_readCount;
    uint16_t m_writeOffset;
    uint16_t m_writeCount;

    typedef void (mbCommandCopy::*pmethod)();
    pmethod m_readmethod;
    pmethod m_writemethod;
};


/************************************************************************
 ********************************* DUMP *********************************
 ************************************************************************/

class mbCommandDump : public mbCommand
{
public:
    mbCommandDump(mbMemory *memory);

public:
    inline Modbus::Address memAddress() const { return m_memAdr; }
    inline mb::Format format() const { return m_format; }
    inline uint16_t count() const { return m_count; }
    void setParams(Modbus::Address memAddress, mb::Format fmt, uint16_t count);

public:
    bool run() override;

protected:
    void calcbits();
    void calcregs();
    void printbits();
    void printregs();

protected:
    mbMemory *m_memory;
    mbMemory::Block *m_block;
    Modbus::Address m_memAdr;
    mb::Format m_format;
    uint16_t m_count;
    uint16_t m_elemCount;
    mb::ByteArray m_buff;

    typedef void (mbCommandDump::*pprintmethod)();
    pprintmethod m_printmethod;
    std::string m_prefix;
};


/************************************************************************
 ********************************* DELAY ********************************
 ************************************************************************/

class mbCommandDelay : public mbCommand
{
public:
    mbCommandDelay();
    bool run() override;

public:
    inline uint32_t milliseconds() const { return m_millis; }
    inline void setMilliseconds(uint32_t msec) { m_millis = msec; }    

protected:
    Modbus::Timer m_timer;
    uint32_t m_millis;
    bool m_isBegin;
};

#endif // MB_COMMAND_H
