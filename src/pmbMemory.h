/*
 * memory - realizes functions for management
 * of daemon internal memory
 */
#ifndef PMB_MEMORY_H
#define PMB_MEMORY_H

#include <pmb_core.h>

class pmbMemory : public ModbusInterface
{
public:
    class Block
    {
    public:
        Block();

    public:
        inline size_t size() const { return m_data.size(); }
        inline size_t sizeBits() const { return m_sizeBits; }
        inline size_t sizeBytes() const { return size(); }
        inline size_t sizeRegs() const { return m_data.size() / MB_REGE_SZ_BYTES; }
        void resize(size_t bytes);
        void resizeBits(size_t bits);
        inline void resizeBytes(size_t bytes) { resize(bytes); }
        inline void resizeRegs(size_t regs) { resize(regs*MB_REGE_SZ_BYTES); }
        inline const void *data() const { return m_data.data(); }

    public:
        inline uint changeCounter() const { return m_changeCounter; }
        void zerroAll();
        Modbus::StatusCode read(uint offset, uint count, void *values, uint *fact = nullptr) const;
        Modbus::StatusCode write(uint offset, uint count, const void *values, uint *fact = nullptr);
        Modbus::StatusCode readBits(uint bitOffset, uint bitCount, void *values, uint *fact = nullptr) const;
        Modbus::StatusCode writeBits(uint bitOffset, uint bitCount, const void *values, uint *fact = nullptr);
        Modbus::StatusCode readRegs(uint regOffset, uint regCount, uint16_t *values, uint *fact = nullptr) const;
        Modbus::StatusCode writeRegs(uint regOffset, uint regCount, const uint16_t *values, uint *fact = nullptr);

    private:
        pmb::ByteArray m_data;
        size_t m_sizeBits;
        uint m_changeCounter;
    };

public:
    static pmbMemory* global();

public:
    pmbMemory();
    ~pmbMemory();

public: // 'ModbusInterface'
    Modbus::StatusCode readCoils                 (uint8_t unit, uint16_t offset, uint16_t count, void *values) override;
    Modbus::StatusCode readDiscreteInputs        (uint8_t unit, uint16_t offset, uint16_t count, void *values) override;
    Modbus::StatusCode readHoldingRegisters      (uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) override;
    Modbus::StatusCode readInputRegisters        (uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) override;
    Modbus::StatusCode writeSingleCoil           (uint8_t unit, uint16_t offset, bool value) override;
    Modbus::StatusCode writeSingleRegister       (uint8_t unit, uint16_t offset, uint16_t value) override;
    Modbus::StatusCode readExceptionStatus       (uint8_t unit, uint8_t *status) override;
    Modbus::StatusCode writeMultipleCoils        (uint8_t unit, uint16_t offset, uint16_t count, const void *values) override;
    Modbus::StatusCode writeMultipleRegisters    (uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values) override;
    Modbus::StatusCode reportServerID            (uint8_t unit, uint8_t *count, uint8_t *data) override;
    Modbus::StatusCode maskWriteRegister         (uint8_t unit, uint16_t offset, uint16_t andMask, uint16_t orMask) override;
    Modbus::StatusCode readWriteMultipleRegisters(uint8_t unit, uint16_t readOffset, uint16_t readCount, uint16_t *readValues, uint16_t writeOffset, uint16_t writeCount, const uint16_t *writeValues) override;

public: // memory-0x management functions
    void realloc_0x(size_t count);
    inline uint changeCounter_0x() const { return m_mem_0x.changeCounter(); }
    inline size_t count_0x() const { return m_mem_0x.sizeBits(); }
    inline size_t count_0x_bites() const { return m_mem_0x.sizeBits(); }
    inline size_t count_0x_bytes() const { return m_mem_0x.sizeBytes(); }
    inline size_t count_0x_reges() const { return m_mem_0x.sizeRegs(); }
    inline void zerroAll_0x() { m_mem_0x.zerroAll(); }
    inline Modbus::StatusCode read_0x (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return m_mem_0x.readBits (bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode write_0x(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return m_mem_0x.writeBits(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode read_0x_bit(uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const  { return read_0x(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode write_0x_bit(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return write_0x(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode read_0x_reg(uint bitOffset, uint regCount, void* buff, uint *fact = nullptr) const  { return read_0x(bitOffset, regCount*MB_REGE_SZ_BITES, buff, fact); if (fact) *fact /= MB_REGE_SZ_BITES; }
    inline Modbus::StatusCode write_0x_reg(uint bitOffset, uint regCount, const void* buff, uint *fact = nullptr) { return write_0x(bitOffset, regCount*MB_REGE_SZ_BITES, buff, fact); if (fact) *fact /= MB_REGE_SZ_BITES; }
    inline bool bool_0x(uint bitOffset) const               { bool v = false; m_mem_0x.readBits(bitOffset, 1, &v); return v; }
    inline void setBool_0x(uint bitOffset, bool v)          { m_mem_0x.writeBits(bitOffset, 1, &v); }
    inline int8_t int8_0x(uint bitOffset) const             { int8_t v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt8_0x(uint bitOffset, int8_t v)        { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline uint8_t uint8_0x(uint bitOffset) const           { uint8_t v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt8_0x(uint bitOffset, uint8_t v)      { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int16_t int16_0x(uint bitOffset) const           { int16_t v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt16_0x(uint bitOffset, int16_t v)      { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline uint16_t uint16_0x(uint bitOffset) const         { uint16_t v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt16_0x(uint bitOffset, uint16_t v)    { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int32_t int32_0x(uint bitOffset) const           { int32_t v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt32_0x(uint bitOffset, int32_t v)      { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline uint32_t uint32_0x(uint bitOffset) const         { uint32_t v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt32_0x(uint bitOffset, uint32_t v)    { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int64_t int64_0x(uint bitOffset) const           { int64_t v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt64_0x(uint bitOffset, int64_t v)      { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline uint64_t uint64_0x(uint bitOffset) const         { uint64_t v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt64_0x(uint bitOffset, uint64_t v)    { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int int_0x(uint bitOffset) const                 { int v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt_0x(uint bitOffset, int v)            { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline unsigned int uint_0x(uint bitOffset) const       { unsigned int v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt_0x(uint bitOffset, unsigned int v)  { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline float float_0x(uint bitOffset) const             { float v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setFloat_0x(uint bitOffset, float v)        { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline double double_0x(uint bitOffset) const           { double v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setDouble_0x(uint bitOffset, double v)      { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }

    Block &memBlockRef_0x() { return m_mem_0x; }
    const void *memptr_0x() const { return m_mem_0x.data(); }

public: // memory-1x management functions
    void realloc_1x(size_t count);
    inline uint changeCounter_1x() const { return m_mem_1x.changeCounter(); }
    inline size_t count_1x() const { return m_mem_1x.sizeBits(); }
    inline size_t count_1x_bites() const { return m_mem_1x.sizeBits(); }
    inline size_t count_1x_bytes() const { return m_mem_1x.sizeBytes(); }
    inline size_t count_1x_reges() const { return m_mem_1x.sizeRegs(); }
    inline void zerroAll_1x() { m_mem_1x.zerroAll(); }
    inline Modbus::StatusCode read_1x (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return m_mem_1x.readBits (bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode write_1x(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return m_mem_1x.writeBits(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode read_1x_bit (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return read_1x (bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode write_1x_bit(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return write_1x(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode read_1x_reg(uint bitOffset, uint regCount, void* buff, uint *fact = nullptr) const  { return read_1x(bitOffset, regCount*MB_REGE_SZ_BITES, buff, fact); if (fact) *fact /= MB_REGE_SZ_BITES; }
    inline Modbus::StatusCode write_1x_reg(uint bitOffset, uint regCount, const void* buff, uint *fact = nullptr) { return write_1x(bitOffset, regCount*MB_REGE_SZ_BITES, buff, fact); if (fact) *fact /= MB_REGE_SZ_BITES; }
    inline bool bool_1x(uint bitOffset) const               { bool v = false; m_mem_1x.readBits(bitOffset, 1, &v); return v; }
    inline void setBool_1x(uint bitOffset, bool v)          { m_mem_1x.writeBits(bitOffset, 1, &v); }
    inline int8_t int8_1x(uint bitOffset) const             { int8_t v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt8_1x(uint bitOffset, int8_t v)        { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline uint8_t uint8_1x(uint bitOffset) const           { uint8_t v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt8_1x(uint bitOffset, uint8_t v)      { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int16_t int16_1x(uint bitOffset) const           { int16_t v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt16_1x(uint bitOffset, int16_t v)      { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline uint16_t uint16_1x(uint bitOffset) const         { uint16_t v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt16_1x(uint bitOffset, uint16_t v)    { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int32_t int32_1x(uint bitOffset) const           { int32_t v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt32_1x(uint bitOffset, int32_t v)      { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline uint32_t uint32_1x(uint bitOffset) const         { uint32_t v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt32_1x(uint bitOffset, uint32_t v)    { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int64_t int64_1x(uint bitOffset) const           { int64_t v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt64_1x(uint bitOffset, int64_t v)      { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline uint64_t uint64_1x(uint bitOffset) const         { uint64_t v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt64_1x(uint bitOffset, uint64_t v)    { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int int_1x(uint bitOffset) const                 { int v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt_1x(uint bitOffset, int v)            { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline unsigned int uint_1x(uint bitOffset) const       { unsigned int v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt_1x(uint bitOffset, unsigned int v)  { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline float float_1x(uint bitOffset) const             { float v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setFloat_1x(uint bitOffset, float v)        { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline double double_1x(uint bitOffset) const           { double v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setDouble_1x(uint bitOffset, double v)      { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }

    inline Block &memBlockRef_1x() { return m_mem_1x; }
    const void *memptr_1x() const { return m_mem_1x.data(); }

public: // memory-3x management functions
    void realloc_3x(size_t count);
    inline uint changeCounter_3x() const { return m_mem_3x.changeCounter(); }
    inline size_t count_3x() const { return m_mem_3x.sizeRegs(); }
    inline size_t count_3x_bites() const { return m_mem_3x.sizeBits(); }
    inline size_t count_3x_bytes() const { return m_mem_3x.sizeBytes(); }
    inline size_t count_3x_reges() const { return m_mem_3x.sizeRegs(); }
    inline void zerroAll_3x() { m_mem_3x.zerroAll(); }
    inline Modbus::StatusCode read_3x (uint offset, uint regCount, void* values, uint *fact = nullptr) const { return m_mem_3x.readRegs (offset, regCount, reinterpret_cast<uint16_t*>(values), fact); }
    inline Modbus::StatusCode write_3x(uint offset, uint regCount, const void* values, uint *fact = nullptr) { return m_mem_3x.writeRegs(offset, regCount, reinterpret_cast<const uint16_t*>(values), fact); }
    inline Modbus::StatusCode read_3x_bit (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return m_mem_3x.readBits (bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode write_3x_bit(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return m_mem_3x.writeBits(bitOffset, bitCount, bites, fact); }
    inline bool bool_3x(uint bitOffset) const              { bool v = false; m_mem_3x.readBits(bitOffset, 1, &v); return v; }
    inline void setBool_3x(uint bitOffset, bool v)         { m_mem_3x.writeBits(bitOffset, 1, &v); }
    inline int8_t int8_3x(uint32_t byteOffset) const       { int8_t v = 0; m_mem_3x.read(byteOffset, sizeof(v), &v); return v; }
    inline void setInt8_3x(uint32_t byteOffset, int8_t v)  { m_mem_3x.write(byteOffset, sizeof(v), &v); }
    inline uint8_t uint8_3x(uint32_t byteOffset) const     { uint8_t v = 0; m_mem_3x.read(byteOffset, sizeof(v), &v); return v; }
    inline void setUInt8_3x(uint32_t byteOffset, uint8_t v){ m_mem_3x.write(byteOffset, sizeof(v), &v); }
    inline int16_t int16_3x(uint regOffset) const          { int16_t v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setInt16_3x(uint regOffset, int16_t v)     { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline uint16_t uint16_3x(uint regOffset) const        { uint16_t v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setUInt16_3x(uint regOffset, uint16_t v)   { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline int32_t int32_3x(uint regOffset) const          { int32_t v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setInt32_3x(uint regOffset, int32_t v)     { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline uint32_t uint32_3x(uint regOffset) const        { uint32_t v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setUInt32_3x(uint regOffset, uint32_t v)   { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline int64_t int64_3x(uint regOffset) const          { int64_t v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setInt64_3x(uint regOffset, int64_t v)     { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline uint64_t uint64_3x(uint regOffset) const        { uint64_t v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setUInt64_3x(uint regOffset, uint64_t v)   { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline int int_3x(uint regOffset) const                { int v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setInt_3x(uint regOffset, int v)           { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline unsigned int uint_3x(uint regOffset) const      { unsigned int v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setUInt_3x(uint regOffset, unsigned int v) { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline float float_3x(uint regOffset) const            { float v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setFloat_3x(uint regOffset, float v)       { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline double double_3x(uint regOffset) const          { double v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setDouble_3x(uint regOffset, double v)     { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }

    inline Block &memBlockRef_3x() { return m_mem_3x; }
    const void *memptr_3x() const { return m_mem_3x.data(); }

public: // memory-4x management functions
    void realloc_4x(size_t count);
    inline uint changeCounter_4x() const { return m_mem_4x.changeCounter(); }
    inline size_t count_4x() const { return m_mem_4x.sizeRegs(); }
    inline size_t count_4x_bites() const { return m_mem_4x.sizeBits(); }
    inline size_t count_4x_bytes() const { return m_mem_4x.sizeBytes(); }
    inline size_t count_4x_reges() const { return m_mem_4x.sizeRegs(); }
    inline void zerroAll_4x() { m_mem_4x.zerroAll(); }
    inline Modbus::StatusCode read_4x (uint offset, uint regCount, void* values, uint *fact = nullptr) const { return m_mem_4x.readRegs (offset, regCount, reinterpret_cast<uint16_t*>(values), fact); }
    inline Modbus::StatusCode write_4x(uint offset, uint regCount, const void* values, uint *fact = nullptr) { return m_mem_4x.writeRegs(offset, regCount, reinterpret_cast<const uint16_t*>(values), fact); }
    inline Modbus::StatusCode read_4x_bit (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return m_mem_4x.readBits (bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode write_4x_bit(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return m_mem_4x.writeBits(bitOffset, bitCount, bites, fact); }
    inline bool bool_4x(uint bitOffset) const              { bool v = false; m_mem_4x.readBits(bitOffset, 1, &v); return v; }
    inline void setBool_4x(uint bitOffset, bool v)         { m_mem_4x.writeBits(bitOffset, 1, &v); }
    inline int8_t int8_4x(uint32_t byteOffset) const       { int8_t v = 0; m_mem_4x.read(byteOffset, sizeof(v), &v); return v; }
    inline void setInt8_4x(uint32_t byteOffset, int8_t v)  { m_mem_4x.write(byteOffset, sizeof(v), &v); }
    inline uint8_t uint8_4x(uint32_t byteOffset) const     { uint8_t v = 0; m_mem_4x.read(byteOffset, sizeof(v), &v); return v; }
    inline void setUInt8_4x(uint32_t byteOffset, uint8_t v){ m_mem_4x.write(byteOffset, sizeof(v), &v); }
    inline int16_t int16_4x(uint regOffset) const          { int16_t v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setInt16_4x(uint regOffset, int16_t v)     { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline uint16_t uint16_4x(uint regOffset) const        { uint16_t v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setUInt16_4x(uint regOffset, uint16_t v)   { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline int32_t int32_4x(uint regOffset) const          { int32_t v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setInt32_4x(uint regOffset, int32_t v)     { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline uint32_t uint32_4x(uint regOffset) const        { uint32_t v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setUInt32_4x(uint regOffset, uint32_t v)   { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline int64_t int64_4x(uint regOffset) const          { int64_t v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setInt64_4x(uint regOffset, int64_t v)     { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline uint64_t uint64_4x(uint regOffset) const        { uint64_t v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setUInt64_4x(uint regOffset, uint64_t v)   { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline int int_4x(uint regOffset) const                { int v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setInt_4x(uint regOffset, int v)           { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline unsigned int uint_4x(uint regOffset) const      { unsigned int v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setUInt_4x(uint regOffset, unsigned int v) { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline float float_4x(uint regOffset) const            { float v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setFloat_4x(uint regOffset, float v)       { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    inline double double_4x(uint regOffset) const          { double v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    inline void setDouble_4x(uint regOffset, double v)     { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }

    inline Block &memBlockRef_4x() { return m_mem_4x; }
    const void *memptr_4x() const { return m_mem_4x.data(); }

public:
    Modbus::StatusCode read(Modbus::Address address, uint count, void* buff, uint *fact = nullptr) const;
    Modbus::StatusCode write(Modbus::Address address, uint count, const void* buff, uint *fact = nullptr);

    uint16_t getUInt16(Modbus::Address address) const;
    void setUInt16(Modbus::Address address, uint16_t value);
    
public: // Exception Status
    inline Modbus::Address exceptionStatusAddress() const { return m_exceptionStatusAddress; }
    inline void setExceptionStatusAddress(Modbus::Address exceptionStatusAddress) { m_exceptionStatusAddress = exceptionStatusAddress; }
    uint8_t exceptionStatus() const;

private:
    Block m_mem_0x;
    Block m_mem_1x;
    Block m_mem_3x;
    Block m_mem_4x;
    Modbus::Address m_exceptionStatusAddress;
};

#endif // PMB_MEMORY_H
