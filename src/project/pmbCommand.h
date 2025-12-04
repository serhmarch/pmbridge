/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#ifndef PMB_COMMAND_H
#define PMB_COMMAND_H

#include <pmbMemory.h>

class pmbMemory;
class pmbClient;

class pmbCommand
{
public:
    virtual ~pmbCommand();
    virtual bool run() = 0;
};


/************************************************************************
 ********************************* QUERY ********************************
 ************************************************************************/

class pmbCommandQuery : public pmbCommand
{
public:
    pmbCommandQuery(pmbMemory *memory, pmbClient *client);
    ~pmbCommandQuery() override;

public:
    inline pmbMemory *memory() const { return m_memory; }
    inline pmbClient *client() const { return m_client; }

    inline uint8_t unit() const { return m_unit; }
    inline void setUnit(uint8_t unit) { m_unit = unit; }

    inline uint16_t execPattern() const { return m_execPattern; }
    void setExecPattern(uint16_t exec);

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
    pmbMemory *m_memory;
    pmbClient *m_client;
    uint8_t m_unit;
    uint16_t m_execPattern;
    Modbus::Address m_succAdr;
    Modbus::Address m_errcAdr;
    Modbus::Address m_errvAdr;
    pmb::ByteArray m_buffer;
    bool m_isBegin;
    uint16_t m_exec;
};

class pmbCommandQueryBase : public pmbCommandQuery
{
public:
    using pmbCommandQuery::pmbCommandQuery;

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

class pmbCommandQueryReadCoils : public pmbCommandQueryBase
{   
public:
    using pmbCommandQueryBase::pmbCommandQueryBase;
    Modbus::StatusCode runQuery() override;
};

class pmbCommandQueryReadDiscreteInputs : public pmbCommandQueryBase
{   
public:
    using pmbCommandQueryBase::pmbCommandQueryBase;
    Modbus::StatusCode runQuery() override;
};

class pmbCommandQueryReadHoldingRegisters : public pmbCommandQueryBase
{   
public:
    using pmbCommandQueryBase::pmbCommandQueryBase;
    Modbus::StatusCode runQuery() override;
};

class pmbCommandQueryReadInputRegisters : public pmbCommandQueryBase
{   
public:
    using pmbCommandQueryBase::pmbCommandQueryBase;
    Modbus::StatusCode runQuery() override;
};

class pmbCommandQueryWriteMultipleCoils : public pmbCommandQueryBase
{   
public:
    using pmbCommandQueryBase::pmbCommandQueryBase;
    Modbus::StatusCode beginQuery() override;
    Modbus::StatusCode runQuery() override;
};

class pmbCommandQueryWriteMultipleRegisters : public pmbCommandQueryBase
{   
public:
    using pmbCommandQueryBase::pmbCommandQueryBase;
    Modbus::StatusCode beginQuery() override;
    Modbus::StatusCode runQuery() override;
};


/************************************************************************
 ********************************* COPY *********************************
 ************************************************************************/

class pmbCommandCopy : public pmbCommand
{
public:
    pmbCommandCopy(pmbMemory *memory);

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
    pmbMemory *m_memory;
    pmbMemory::Block *m_readblock;
    pmbMemory::Block *m_writeblock;
    Modbus::Address m_srcAdr;
    Modbus::Address m_dstAdr;
    uint16_t m_count;
    pmb::ByteArray m_buff;
    uint16_t m_readOffset;
    uint16_t m_readCount;
    uint16_t m_writeOffset;
    uint16_t m_writeCount;

    typedef void (pmbCommandCopy::*pmethod)();
    pmethod m_readmethod;
    pmethod m_writemethod;
};


/************************************************************************
 ********************************* DUMP *********************************
 ************************************************************************/

class pmbCommandDump : public pmbCommand
{
public:
    pmbCommandDump(pmbMemory *memory);

public:
    inline Modbus::Address memAddress() const { return m_memAdr; }
    inline pmb::Format format() const { return m_format; }
    inline uint16_t count() const { return m_count; }
    void setParams(Modbus::Address memAddress, pmb::Format fmt, uint16_t count);

public:
    bool run() override;

protected:
    void calcbits();
    void calcregs();
    void printbits();
    void printregs();
    void printformat(pmb::Format fmt, const void *mem, uint16_t count);

protected:
    pmbMemory *m_memory;
    pmbMemory::Block *m_block;
    Modbus::Address m_memAdr;
    pmb::Format m_format;
    uint16_t m_count;
    uint16_t m_elemCount;
    pmb::ByteArray m_buff;

    typedef void (pmbCommandDump::*pprintmethod)();
    pprintmethod m_printmethod;
    std::string m_prefix;
    std::vector<char> m_printbuff;
    int m_printindex;
};


/************************************************************************
 ********************************* DELAY ********************************
 ************************************************************************/

class pmbCommandDelay : public pmbCommand
{
public:
    pmbCommandDelay();
    bool run() override;

public:
    inline uint32_t milliseconds() const { return m_millis; }
    inline void setMilliseconds(uint32_t msec) { m_millis = msec; }    

protected:
    Modbus::Timer m_timer;
    uint32_t m_millis;
    bool m_isBegin;
};

#endif // PMB_COMMAND_H
