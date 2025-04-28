#include "pmbCommand.h"

#include <iostream>

#include "pmbMemory.h"
#include "pmbClient.h"

pmbCommand::~pmbCommand()
{
}

/************************************************************************
 ********************************* QUERY ********************************
 ************************************************************************/

 mbCommandQuery::mbCommandQuery(pmbMemory *memory, pmbClient *client) :
    m_memory(memory),
    m_client(client),
    m_unit(0),
    m_exec(0),
    m_succAdr(),
    m_errcAdr(),
    m_errvAdr(),
    m_isBegin(true)
{
    m_buffer.resize(MB_MAX_BYTES);
}

mbCommandQuery::~mbCommandQuery()
{
}

bool mbCommandQuery::run()
{
    if (m_isBegin)
    {
        Modbus::StatusCode status = beginQuery();
        if (Modbus::StatusIsBad(status))
        {
            m_memory->setUInt16(m_errcAdr, m_memory->getUInt16(m_errcAdr) + 1);
            m_memory->setUInt16(m_errvAdr, static_cast<uint16_t>(status));
            return true;
        }
        m_isBegin = false;
    }
    Modbus::StatusCode status = runQuery();
    if (Modbus::StatusIsProcessing(status))
        return false;
    if (Modbus::StatusIsGood(status))
    {
        m_memory->setUInt16(m_succAdr, m_memory->getUInt16(m_succAdr) + 1);
    }
    else
    {
        m_memory->setUInt16(m_errcAdr, m_memory->getUInt16(m_errcAdr) + 1);
        m_memory->setUInt16(m_errvAdr, static_cast<uint16_t>(status));
    }
    m_isBegin = true;
    return true;
}

Modbus::StatusCode mbCommandQuery::beginQuery()
{
    return Modbus::Status_Good;
}

Modbus::StatusCode mbCommandQueryReadCoils::runQuery()
{
    Modbus::StatusCode status = m_client->readCoils(m_unit, m_offset, m_count, m_buffer.data());
    if (Modbus::StatusIsGood(status))
    {
        m_memory->write(m_memAdr, m_count, m_buffer.data());
    }
    return status;
}

Modbus::StatusCode mbCommandQueryReadDiscreteInputs::runQuery()
{
    Modbus::StatusCode status = m_client->readDiscreteInputs(m_unit, m_offset, m_count, m_buffer.data());
    if (Modbus::StatusIsGood(status))
    {
        m_memory->write(m_memAdr, m_count, m_buffer.data());
    }
    return status;
}

Modbus::StatusCode mbCommandQueryReadHoldingRegisters::runQuery()
{
    Modbus::StatusCode status = m_client->readHoldingRegisters(m_unit, m_offset, m_count, reinterpret_cast<uint16_t*>(m_buffer.data()));
    if (Modbus::StatusIsGood(status))
    {
        m_memory->write(m_memAdr, m_count, m_buffer.data());
    }
    return status;
}

Modbus::StatusCode mbCommandQueryReadInputRegisters::runQuery()
{
    Modbus::StatusCode status = m_client->readHoldingRegisters(m_unit, m_offset, m_count, reinterpret_cast<uint16_t*>(m_buffer.data()));
    if (Modbus::StatusIsGood(status))
    {
        m_memory->write(m_memAdr, m_count, m_buffer.data());
    }
    return status;
}

Modbus::StatusCode mbCommandQueryWriteMultipleCoils::beginQuery()
{
    return m_memory->read(m_memAdr, m_count, m_buffer.data());
}

Modbus::StatusCode mbCommandQueryWriteMultipleCoils::runQuery()
{
    return m_client->writeMultipleCoils(m_unit, m_offset, m_count, m_buffer.data());;
}

Modbus::StatusCode mbCommandQueryWriteMultipleRegisters::beginQuery()
{
    return m_memory->read(m_memAdr, m_count, m_buffer.data());
}

Modbus::StatusCode mbCommandQueryWriteMultipleRegisters::runQuery()
{
    return m_client->writeMultipleCoils(m_unit, m_offset, m_count, m_buffer.data());;
}


/************************************************************************
 ********************************* COPY *********************************
 ************************************************************************/

 mbCommandCopy::mbCommandCopy(pmbMemory *memory) :
    m_memory(memory)
{
    m_readblock = &m_memory->memBlockRef_4x();
    m_writeblock = &m_memory->memBlockRef_4x();

    m_readOffset = 0;
    m_writeOffset = 0;
    zeroCount();

    m_readmethod = &mbCommandCopy::readBytes;
    m_writemethod = &mbCommandCopy::writeBytes;
}

void mbCommandCopy::setParams(Modbus::Address srcAddress, Modbus::Address dstAddress, uint16_t count)
{
    m_srcAdr = srcAddress;
    m_dstAdr = dstAddress;
    m_count = count;

    switch (m_srcAdr.type())
    {
    case Modbus::Memory_0x:
        m_readblock = &m_memory->memBlockRef_0x();
        break;
    case Modbus::Memory_1x:
        m_readblock = &m_memory->memBlockRef_1x();
        break;
    case Modbus::Memory_3x:
        m_readblock = &m_memory->memBlockRef_3x();
        break;
    case Modbus::Memory_4x:
        m_readblock = &m_memory->memBlockRef_4x();
        break;
    default:
        zeroCount();
        return;
    }

    switch (m_dstAdr.type())
    {
    case Modbus::Memory_0x:
        m_writeblock = &m_memory->memBlockRef_0x();
        break;
    case Modbus::Memory_1x:
        m_writeblock = &m_memory->memBlockRef_1x();
        break;
    case Modbus::Memory_3x:
        m_writeblock = &m_memory->memBlockRef_3x();
        break;
    case Modbus::Memory_4x:
        m_writeblock = &m_memory->memBlockRef_4x();
        break;
    default:
        zeroCount();
        return;
    }

    switch (m_srcAdr.type())
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
        if ((m_srcAdr.offset() % MB_BYTE_SZ_BITES == 0) && (m_count % MB_BYTE_SZ_BITES == 0))
        {
            calcreadbytes();
            switch (m_dstAdr.type())
            {
            case Modbus::Memory_0x:
            case Modbus::Memory_1x:
                if (m_dstAdr.offset() % MB_BYTE_SZ_BITES == 0)
                    calcwritebytes(true);
                else
                    calcwritebits(true);
                break;
            default:
                calcwritebytes(true);
                break;
            }    
        }
        else
        {
            calcreadbits();
            calcwritebits(true);
        }
        break;
    default:
        calcreadbytes();
        switch (m_dstAdr.type())
        {
        case Modbus::Memory_0x:
        case Modbus::Memory_1x:
            if (m_dstAdr.offset() % MB_BYTE_SZ_BITES == 0)
                calcwritebytes(false);
            else
                calcwritebits(false);
            break;
        default:
            calcwritebytes(false);
            break;
        }    
        break;
        }
}

bool mbCommandCopy::run()
{
    (this->*m_readmethod)();
    (this->*m_writemethod)();
    return true;
}

void mbCommandCopy::calcreadbits()
{
    switch (m_srcAdr.type())
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
        m_readOffset = m_srcAdr.offset();
        m_readCount = m_count;
        break;
    default:
        m_readOffset = m_srcAdr.offset() * MB_REGE_SZ_BITES;
        m_readCount = m_count * MB_REGE_SZ_BITES;
        break;
    }
    if (m_readOffset > m_readblock->sizeBits())
        m_readCount = 0;
    else if (m_readOffset+m_readCount > m_readblock->sizeBits())
        m_readCount = static_cast<uint16_t>(m_readblock->sizeBits() - m_readOffset);
    size_t bytecount = (m_readCount + 7) / 8;
    if (bytecount > m_buff.size())
        m_buff.resize(bytecount);
    m_readmethod = &mbCommandCopy::readBits;
}

void mbCommandCopy::calcreadbytes()
{
    switch (m_srcAdr.type())
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
        m_readOffset = m_srcAdr.offset() / MB_BYTE_SZ_BITES;
        m_readCount = (m_count + 7) / 8;
        break;
    default:
        m_readOffset = m_srcAdr.offset() * MB_REGE_SZ_BYTES;
        m_readCount = m_count * MB_REGE_SZ_BYTES;
        break;
    }
    if (m_readOffset > m_readblock->sizeBytes())
        m_readCount = 0;
    else if (m_readOffset+m_readCount > m_readblock->sizeBytes())
        m_readCount = static_cast<uint16_t>(m_readblock->sizeBytes() - m_readOffset);
    size_t bytecount = m_readCount;
    if (bytecount > m_buff.size())
        m_buff.resize(bytecount);
    m_readmethod = &mbCommandCopy::readBytes;
}

void mbCommandCopy::calcwritebits(bool countIsBits)
{
    if (countIsBits)
        m_writeCount = m_count;
    else
        m_writeCount = m_count * MB_REGE_SZ_BITES;

    switch (m_dstAdr.type())
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
        m_writeOffset = m_dstAdr.offset();
        break;
    default:
        m_writeOffset = m_dstAdr.offset() * MB_REGE_SZ_BITES;
        break;
    }
    if (m_writeOffset > m_writeblock->sizeBits())
        m_writeCount = 0;
    else if (m_writeOffset+m_writeCount > m_writeblock->sizeBits())
        m_writeCount = static_cast<uint16_t>(m_writeblock->sizeBits() - m_writeOffset);
    size_t bytecount = (m_writeCount + 7) / 8;
    if (bytecount > m_buff.size())
        m_buff.resize(bytecount);
    m_writemethod = &mbCommandCopy::writeBits;
}

void mbCommandCopy::calcwritebytes(bool countIsBits)
{
    if (countIsBits)
        m_writeCount = (m_count + 7) / 8;
    else
        m_writeCount = m_count * MB_REGE_SZ_BYTES;
    switch (m_dstAdr.type())
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
        m_writeOffset = m_dstAdr.offset() / MB_BYTE_SZ_BITES;
        break;
    default:
        m_writeOffset = m_dstAdr.offset() * MB_REGE_SZ_BYTES;
        break;
    }
    if (m_writeOffset > m_writeblock->sizeBytes())
        m_writeCount = 0;
    else if (m_writeOffset+m_writeCount > m_writeblock->sizeBytes())
        m_writeCount = static_cast<uint16_t>(m_writeblock->sizeBytes() - m_writeOffset);
    size_t bytecount = m_writeCount;
    if (bytecount > m_buff.size())
        m_buff.resize(bytecount);
    m_writemethod = &mbCommandCopy::writeBytes;
}

void mbCommandCopy::zeroCount()
{
    m_count = 0;
    m_readCount = 0;
    m_writeCount = 0;
}

void mbCommandCopy::readBits()
{
    m_readblock->readBits(m_readOffset, m_readCount, m_buff.data());
}

void mbCommandCopy::readBytes()
{
    m_readblock->read(m_readOffset, m_readCount, m_buff.data());
}

void mbCommandCopy::writeBits()
{
    m_writeblock->writeBits(m_writeOffset, m_writeCount, m_buff.data());
}

void mbCommandCopy::writeBytes()
{
    m_writeblock->write(m_writeOffset, m_writeCount, m_buff.data());
}


/************************************************************************
 ********************************* DUMP *********************************
 ************************************************************************/

mbCommandDump::mbCommandDump(pmbMemory *memory) :
    m_memory(memory),
    m_format(pmb::Format_Hex16),
    m_count(0),
    m_elemCount(0)
{
    m_printmethod = &mbCommandDump::printregs;
}

static void printformat(pmb::Format fmt, const void *mem, uint16_t count)
{
    const uint8_t *bytePtr = static_cast<const uint8_t *>(mem);
    switch (fmt)
    {
    case pmb::Format_Bin16:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%s ", pmb::toBinString(*reinterpret_cast<const uint16_t *>(bytePtr)).data());
            bytePtr += sizeof(uint16_t);
        }
        break;
    case pmb::Format_Oct16:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%06o ", *reinterpret_cast<const uint16_t *>(bytePtr));
            bytePtr += sizeof(uint16_t);
        }
        break;
    case pmb::Format_Dec16:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%d ", *reinterpret_cast<const int16_t *>(bytePtr));
            bytePtr += sizeof(uint16_t);
        }
        break;
    case pmb::Format_UDec16:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%u ", *reinterpret_cast<const uint16_t *>(bytePtr));
            bytePtr += sizeof(uint16_t);
        }
        break;
    case pmb::Format_Hex16:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%04X ", *reinterpret_cast<const uint16_t *>(bytePtr));
            bytePtr += sizeof(uint16_t);
        }
        break;
    case pmb::Format_Bin32:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%s ", pmb::toBinString(*reinterpret_cast<const uint32_t *>(bytePtr)).data());
            bytePtr += sizeof(uint32_t);
        }
        break;
    case pmb::Format_Oct32:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%011o ", *reinterpret_cast<const uint32_t *>(bytePtr));
            bytePtr += sizeof(uint32_t);
        }
        break;
    case pmb::Format_Dec32:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%d ", *reinterpret_cast<const int32_t *>(bytePtr));
            bytePtr += sizeof(uint32_t);
        }
        break;
    case pmb::Format_UDec32:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%u ", *reinterpret_cast<const uint32_t *>(bytePtr));
            bytePtr += sizeof(uint32_t);
        }
        break;
    case pmb::Format_Hex32:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%08X ", *reinterpret_cast<const uint32_t *>(bytePtr));
            bytePtr += sizeof(uint32_t);
        }
        break;
    case pmb::Format_Bin64:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%s ", pmb::toBinString(*reinterpret_cast<const uint64_t *>(bytePtr)).data());
            bytePtr += sizeof(uint64_t);
        }
        break;
    case pmb::Format_Oct64:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%022llo ", *reinterpret_cast<const uint64_t *>(bytePtr));
            bytePtr += sizeof(uint64_t);
        }
        break;
    case pmb::Format_Dec64:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%lld ", *reinterpret_cast<const int64_t *>(bytePtr));
            bytePtr += sizeof(uint64_t);
        }
        break;
    case pmb::Format_UDec64:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%llu ", *reinterpret_cast<const uint64_t *>(bytePtr));
            bytePtr += sizeof(uint64_t);
        }
        break;
    case pmb::Format_Hex64:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%016llX ", *reinterpret_cast<const uint64_t *>(bytePtr));
            bytePtr += sizeof(uint64_t);
        }
        break;
    case pmb::Format_Float:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%f ", *reinterpret_cast<const float *>(bytePtr));
            bytePtr += sizeof(float);
        }
        break;
    case pmb::Format_Double:
        for (uint16_t i = 0; i < count; ++i)
        {
            printf("%lf ", *reinterpret_cast<const double *>(bytePtr));
            bytePtr += sizeof(double);
        }
        break;
    default:
        printf("Unknown format ");
        break;
    }
}

void mbCommandDump::setParams(Modbus::Address memAddress, pmb::Format fmt, uint16_t count)
{
    m_memAdr = memAddress;
    m_format = fmt;
    m_count = count;
    switch (m_memAdr.type())
    {
    case Modbus::Memory_0x:
        m_block = &m_memory->memBlockRef_0x();
        calcbits();
        break;
    case Modbus::Memory_1x:
        m_block = &m_memory->memBlockRef_1x();
        calcbits();
        break;
    case Modbus::Memory_3x:
        m_block = &m_memory->memBlockRef_3x();
        calcregs();
        break;
    case Modbus::Memory_4x:
        m_block = &m_memory->memBlockRef_4x();
        calcregs();
        break;
    default:
        m_count = 0;
        m_elemCount = 0;
    }
    char buff[32];
    std::snprintf(buff, sizeof(buff), "%i%05hu:%i%05hu (%s): ", m_memAdr.type(), 
                                                                m_memAdr.offset()+1,
                                                                m_memAdr.type(),
                                                                m_memAdr.offset()+m_elemCount,
                                                                pmb::toConstCharPtr(m_format));
    m_prefix = buff;
}

bool mbCommandDump::run()
{
    std::cout << m_prefix;
    (this->*m_printmethod)();
    std::cout << std::endl;
    return true;
}

void mbCommandDump::calcbits()
{
    size_t count = m_count;
    size_t bytesz = pmb::sizeofFormat(m_format);
    size_t bitsz = bytesz * MB_BYTE_SZ_BITES;
    size_t bitcount = count * bitsz;
    if (m_memAdr.offset()+bitcount > m_block->sizeBits())
        bitcount = m_block->sizeBits() - m_memAdr.offset();
    count = (bitcount + bitsz - 1) / bitsz;
    size_t bytecount = count * bytesz;
    if (bytecount != m_buff.size())
        m_buff.resize(bytecount);
    m_count = static_cast<decltype(m_count)>(count);
    m_elemCount = static_cast<decltype(m_elemCount)>(bitcount);
    m_printmethod = &mbCommandDump::printbits;
}

void mbCommandDump::calcregs()
{
    size_t count = m_count;
    size_t bytesz = pmb::sizeofFormat(m_format);
    size_t regsz = bytesz / MB_REGE_SZ_BYTES;
    size_t regcount = count * regsz;
    if (m_memAdr.offset()+regcount > m_block->sizeRegs())
        regcount = m_block->sizeRegs() - m_memAdr.offset();
    count = (regcount + regsz - 1) / regsz;
    size_t bytecount = count * bytesz;
    if (bytecount != m_buff.size())
        m_buff.resize(bytecount);
    m_count = static_cast<decltype(m_count)>(count);
    m_elemCount = static_cast<decltype(m_elemCount)>(regcount);
    m_printmethod = &mbCommandDump::printregs;
}

void mbCommandDump::printbits()
{
    auto s = m_block->readBits(m_memAdr.offset(), m_elemCount, m_buff.data());
    if (Modbus::StatusIsGood(s))
    {
        printformat(m_format, m_buff.data(), m_count);
    }
}

void mbCommandDump::printregs()
{
    auto s = m_block->readRegs(m_memAdr.offset(), m_elemCount, reinterpret_cast<uint16_t*>(m_buff.data()));
    if (Modbus::StatusIsGood(s))
    {
        printformat(m_format, m_buff.data(), m_count);
    }
}

/************************************************************************
 ********************************* DELAY ********************************
 ************************************************************************/

 mbCommandDelay::mbCommandDelay() :
    m_timer(0),
    m_millis(0),
    m_isBegin(true)
{
}

bool mbCommandDelay::run()
{
    if (m_isBegin)
    {
        m_timer = Modbus::timer();
        m_isBegin = false;
    }
    Modbus::msleep(1);
    if (Modbus::timer() - m_timer >= m_millis)
    {
        m_isBegin = true;
        return true;
    }
    return false;
}
