/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include "pmbMemory.h"

pmbMemory::Block::Block()
{
    m_sizeBits = 0;
    m_changeCounter = 0;
}

void pmbMemory::Block::resize(size_t bytes)
{
    m_data.resize(bytes);
    memset(m_data.data(), 0, m_data.size());
    m_sizeBits = m_data.size() * MB_BYTE_SZ_BITES;
}

void pmbMemory::Block::resizeBits(size_t bits)
{
    m_data.resize((bits+7)/8);
    memset(m_data.data(), 0, m_data.size());
    m_sizeBits = bits;
}

void pmbMemory::Block::zerroAll()
{
    m_changeCounter++;
    memset(m_data.data(), 0, m_data.size());
}

Modbus::StatusCode pmbMemory::Block::read(uint offset, uint count, void *buff, uint *fact) const
{
    uint c;
    if (offset >= static_cast<uint>(static_cast<uint>(m_data.size())))
        return Modbus::Status_BadIllegalDataAddress;

    if ((offset+count) > static_cast<uint>(m_data.size()))
        c = static_cast<uint>(static_cast<uint>(m_data.size())) - offset;
    else
        c = count;
    memcpy(buff, m_data.data()+offset, c);
    if (fact)
        *fact = c;
    return Modbus::Status_Good;
}

Modbus::StatusCode pmbMemory::Block::write(uint offset, uint count, const void *buff, uint *fact)
{
    uint c;
    if (offset >= static_cast<uint>(m_data.size()))
        return Modbus::Status_BadIllegalDataAddress;

    if ((offset+count) > static_cast<uint>(m_data.size()))
        c = static_cast<uint>(m_data.size()) - offset;
    else
        c = count;
    if (c == 0)
        return Modbus::Status_BadIllegalDataAddress;
    memcpy(m_data.data()+offset, buff, c);
    m_changeCounter++;
    if (fact)
        *fact = c;
    return Modbus::Status_Good;
}

Modbus::StatusCode pmbMemory::Block::readBits(uint bitOffset, uint bitCount, void *buff, uint *fact) const
{
    return Modbus::readMemBits(bitOffset, bitCount, buff, m_data.data(), static_cast<uint32_t>(m_data.size()), fact);
}

Modbus::StatusCode pmbMemory::Block::writeBits(uint bitOffset, uint bitCount, const void *buff, uint *fact)
{
    return Modbus::writeMemBits(bitOffset, bitCount, buff, m_data.data(), static_cast<uint32_t>(m_data.size()), fact);
}

Modbus::StatusCode pmbMemory::Block::readRegs(uint regOffset, uint regCount, uint16_t *buff, uint *fact) const
{
    uint offset = regOffset * MB_REGE_SZ_BYTES;
    uint count = regCount * MB_REGE_SZ_BYTES;
    Modbus::StatusCode r = read(offset, count, buff, fact);
    if (Modbus::StatusIsGood(r))
    {
        if (fact)
            *fact /= MB_REGE_SZ_BYTES;
    }
    return r;
}

Modbus::StatusCode pmbMemory::Block::writeRegs(uint regOffset, uint regCount, const uint16_t *buff, uint *fact)
{
    uint offset = regOffset * MB_REGE_SZ_BYTES;
    uint count = regCount * MB_REGE_SZ_BYTES;
    Modbus::StatusCode r = write(offset, count, buff, fact);
    if (Modbus::StatusIsGood(r))
    {
        if (fact)
            *fact /= MB_REGE_SZ_BYTES;
    }
    return r;
}

pmbMemory *pmbMemory::global()
{
    static pmbMemory mem;
    return &mem;
}

pmbMemory::pmbMemory()
{
}

pmbMemory::~pmbMemory()
{
}

Modbus::StatusCode pmbMemory::readCoils(uint8_t /*unit*/, uint16_t offset, uint16_t count, void *values)
{
    return this->read_0x(offset, count, values);
}

Modbus::StatusCode pmbMemory::readDiscreteInputs(uint8_t /*unit*/, uint16_t offset, uint16_t count, void *values)
{
    return this->read_1x(offset, count, values);
}

Modbus::StatusCode pmbMemory::readHoldingRegisters(uint8_t /*unit*/, uint16_t offset, uint16_t count, uint16_t *values)
{
    return this->read_4x(offset, count, values);
}

Modbus::StatusCode pmbMemory::readInputRegisters(uint8_t /*unit*/, uint16_t offset, uint16_t count, uint16_t *values)
{
    return this->read_3x(offset, count, values);
}

Modbus::StatusCode pmbMemory::writeSingleCoil(uint8_t /*unit*/, uint16_t offset, bool value)
{
    this->setBool_0x(offset, value);
    return Modbus::Status_Good;
}

Modbus::StatusCode pmbMemory::writeSingleRegister(uint8_t /*unit*/, uint16_t offset, uint16_t value)
{
    this->setUInt16_4x(offset, value);
    return Modbus::Status_Good;
}

Modbus::StatusCode pmbMemory::readExceptionStatus(uint8_t /*unit*/, uint8_t *status)
{
    *status = this->exceptionStatus();
    return Modbus::Status_Good;
}

Modbus::StatusCode pmbMemory::writeMultipleCoils(uint8_t /*unit*/, uint16_t offset, uint16_t count, const void *values)
{
    return this->write_0x(offset, count, values);
}

Modbus::StatusCode pmbMemory::writeMultipleRegisters(uint8_t /*unit*/, uint16_t offset, uint16_t count, const uint16_t *values)
{
    return this->write_4x(offset, count, values);
}

Modbus::StatusCode pmbMemory::reportServerID(uint8_t /*unit*/, uint8_t *count, uint8_t *data)
{
    pmb::String utf8 = pmbSTR("pmbridge");
    if (utf8.size() > MB_MAX_BYTES)
        *count = MB_MAX_BYTES;
    else
        *count = static_cast<uint8_t>(utf8.size());
    memcpy(data, utf8.data(), *count);
    return Modbus::Status_Good;
}


Modbus::StatusCode pmbMemory::maskWriteRegister(uint8_t /*unit*/, uint16_t offset, uint16_t andMask, uint16_t orMask)
{
    uint16_t c = this->uint16_4x(offset);
    uint16_t r = (c & andMask) | (orMask & ~andMask);
    this->setUInt16_4x(offset, r);
    return Modbus::Status_Good;
}

Modbus::StatusCode pmbMemory::readWriteMultipleRegisters(uint8_t /*unit*/, uint16_t readOffset, uint16_t readCount, uint16_t *readValues, uint16_t writeOffset, uint16_t writeCount, const uint16_t *writeValues)
{
    Modbus::StatusCode s = this->write_4x(writeOffset, writeCount, writeValues);
    if (!Modbus::StatusIsGood(s))
        return s;
    return this->read_4x(readOffset, readCount, readValues);
}

void pmbMemory::realloc_0x(size_t count)
{
    if (count_0x() != count)
    {
        m_mem_0x.resizeBits(count);
    }
}

void pmbMemory::realloc_1x(size_t count)
{
    if (count_1x() != count)
    {
        m_mem_1x.resizeBits(count);
    }
}

void pmbMemory::realloc_3x(size_t count)
{
    if (count_3x() != count)
    {
        m_mem_3x.resizeRegs(count);
    }
}

void pmbMemory::realloc_4x(size_t count)
{
    if (count_4x() != count)
    {
        m_mem_4x.resizeRegs(count);
    }
}

Modbus::StatusCode pmbMemory::read(Modbus::Address address, uint count, void *buff, uint *fact) const
{
    switch (address.type())
    {
    case Modbus::Memory_0x: return this->read_0x(address.offset(), count, buff, fact);
    case Modbus::Memory_1x: return this->read_1x(address.offset(), count, buff, fact);
    case Modbus::Memory_3x: return this->read_3x(address.offset(), count, buff, fact);
    case Modbus::Memory_4x: return this->read_4x(address.offset(), count, buff, fact);
    default:
        return Modbus::Status_BadIllegalDataAddress;
    }
}

Modbus::StatusCode pmbMemory::write(Modbus::Address address, uint count, const void *buff, uint *fact)
{
    switch (address.type())
    {
    case Modbus::Memory_0x: return this->write_0x(address.offset(), count, buff, fact);
    case Modbus::Memory_1x: return this->write_1x(address.offset(), count, buff, fact);
    case Modbus::Memory_3x: return this->write_3x(address.offset(), count, buff, fact);
    case Modbus::Memory_4x: return this->write_4x(address.offset(), count, buff, fact);
    default:
        return Modbus::Status_BadIllegalDataAddress;
    }
}

uint16_t pmbMemory::getUInt16(Modbus::Address address) const
{
    switch (address.type())
    {
    case Modbus::Memory_0x: return this->uint16_0x(address.offset());
    case Modbus::Memory_1x: return this->uint16_1x(address.offset());
    case Modbus::Memory_3x: return this->uint16_3x(address.offset());
    case Modbus::Memory_4x: return this->uint16_4x(address.offset());
    default:
        return 0;
    }
}

void pmbMemory::setUInt16(Modbus::Address address, uint16_t value)
{
    switch (address.type())
    {
    case Modbus::Memory_0x: this->setUInt16_0x(address.offset(), value); break;
    case Modbus::Memory_1x: this->setUInt16_1x(address.offset(), value); break;
    case Modbus::Memory_3x: this->setUInt16_3x(address.offset(), value); break;
    case Modbus::Memory_4x: this->setUInt16_4x(address.offset(), value); break;
    default:
        break;
    }
}

uint8_t pmbMemory::exceptionStatus() const
{
    switch (m_exceptionStatusAddress.type())
    {
    case Modbus::Memory_0x: return uint8_0x(m_exceptionStatusAddress.offset());
    case Modbus::Memory_1x: return uint8_1x(m_exceptionStatusAddress.offset());
    case Modbus::Memory_3x: return uint8_3x(m_exceptionStatusAddress.offset()*MB_REGE_SZ_BYTES);
    case Modbus::Memory_4x: return uint8_4x(m_exceptionStatusAddress.offset()*MB_REGE_SZ_BYTES);
    default:
        break;
    }
    return 0;
}
