#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <project/pmbCommand.h>
#include <project/pmbClient.h>
#include <pmbMemory.h>
#include <ModbusTcpPort.h>
#include <ModbusGlobal.h>

using namespace testing;

class MockModbusClientPort : public ModbusClientPort
{
public:
    MockModbusClientPort() : ModbusClientPort(new ModbusTcpPort())
    {
    }

public:
    MOCK_METHOD(Modbus::StatusCode, readCoils, (uint8_t unit, uint16_t offset, uint16_t count, void *values), (override));
    MOCK_METHOD(Modbus::StatusCode, readDiscreteInputs, (uint8_t unit, uint16_t offset, uint16_t count, void *values), (override));
    MOCK_METHOD(Modbus::StatusCode, readInputRegisters, (uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values), (override));
    MOCK_METHOD(Modbus::StatusCode, readHoldingRegisters, (uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values), (override));
    MOCK_METHOD(Modbus::StatusCode, writeSingleCoil, (uint8_t unit, uint16_t offset, bool value), (override));
    MOCK_METHOD(Modbus::StatusCode, writeSingleRegister, (uint8_t unit, uint16_t offset, uint16_t value), (override));
    MOCK_METHOD(Modbus::StatusCode, readExceptionStatus, (uint8_t unit, uint8_t *status), (override));
    MOCK_METHOD(Modbus::StatusCode, diagnostics, (uint8_t unit, uint16_t subfunc, uint8_t insize, const void *indata, uint8_t *outsize, void *outdata), (override));
    MOCK_METHOD(Modbus::StatusCode, getCommEventCounter, (uint8_t unit, uint16_t *status, uint16_t *eventCount), (override));
    MOCK_METHOD(Modbus::StatusCode, getCommEventLog, (uint8_t unit, uint16_t *status, uint16_t *eventCount, uint16_t *messageCount, uint8_t *eventBuffSize, uint8_t *eventBuff), (override));
    MOCK_METHOD(Modbus::StatusCode, writeMultipleCoils, (uint8_t unit, uint16_t offset, uint16_t count, const void *values), (override));
    MOCK_METHOD(Modbus::StatusCode, writeMultipleRegisters, (uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values), (override));
    MOCK_METHOD(Modbus::StatusCode, reportServerID, (uint8_t unit, uint8_t *count, uint8_t *data), (override));
    MOCK_METHOD(Modbus::StatusCode, maskWriteRegister, (uint8_t unit, uint16_t offset, uint16_t andMask, uint16_t orMask), (override));
    MOCK_METHOD(Modbus::StatusCode, readWriteMultipleRegisters, (uint8_t unit, uint16_t readOffset, uint16_t readCount, uint16_t *readValues, uint16_t writeOffset, uint16_t writeCount, const uint16_t *writeValues), (override));
};

// Query Read Holding Registers: simulate memory interaction
TEST(pmbCommandTest, QueryReadHoldingRegisters_Construct)
{
    pmbMemory mem;
    MockModbusClientPort *mockClientPort = new MockModbusClientPort();
    pmbClient cli(mockClientPort);
    cli.setName("cli");

    auto *cmd = new pmbCommandQueryReadHoldingRegisters(&mem, &cli);
    ASSERT_NE(cmd, nullptr);
    cmd->setUnit(1);
    cmd->setOffset(0);
    cmd->setCount(2);
    cmd->setMemAddress(Modbus::Address(300001));
    cmd->setExecPattern(1);
    cmd->setSuccAddress(Modbus::Address(1));
    cmd->setErrcAddress(Modbus::Address(2));
    cmd->setErrvAddress(Modbus::Address(3));

    EXPECT_EQ(cmd->unit(), 1);
    EXPECT_EQ(cmd->offset(), 0);
    EXPECT_EQ(cmd->count(), 2);
    EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_3x);
    EXPECT_EQ(cmd->memAddress().offset(), 0);
    EXPECT_EQ(cmd->execPattern(), 1);
    EXPECT_EQ(cmd->succAddress().toInt(), Modbus::Address(1).toInt());
    EXPECT_EQ(cmd->errcAddress().toInt(), Modbus::Address(2).toInt());
    EXPECT_EQ(cmd->errvAddress().toInt(), Modbus::Address(3).toInt());
    delete cmd;
}

TEST(pmbCommandTest, QueryReadCoils_Run)
{
    pmbMemory mem;
    MockModbusClientPort *mockClientPort = new MockModbusClientPort();
    pmbClient cli(mockClientPort);
    cli.setName("cli");

    auto *cmd = new pmbCommandQueryReadCoils(&mem, &cli);
    cmd->setUnit(1);
    cmd->setDevAddress(Modbus::Address(101));
    cmd->setCount(10);

    EXPECT_CALL(*mockClientPort, readCoils(1, 100, 10, _))
        .Times(1)
        .WillOnce(Return(Modbus::Status_Good));
        
    bool rc = cmd->run();
    EXPECT_TRUE(rc);
    delete cmd;
}

TEST(pmbCommandTest, QueryReadDiscreteInputs_Run)
{
    pmbMemory mem;
    MockModbusClientPort *mockClientPort = new MockModbusClientPort();
    pmbClient cli(mockClientPort);
    cli.setName("cli");

    auto *cmd = new pmbCommandQueryReadDiscreteInputs(&mem, &cli);
    cmd->setUnit(1);
    cmd->setDevAddress(Modbus::Address(100101));
    cmd->setCount(10);

    EXPECT_CALL(*mockClientPort, readDiscreteInputs(1, 100, 10, _))
        .Times(1)
        .WillOnce(Return(Modbus::Status_Good));
        
    bool rc = cmd->run();
    EXPECT_TRUE(rc);
    delete cmd;
}

TEST(pmbCommandTest, QueryReadInputRegisters_Run)
{
    pmbMemory mem;
    MockModbusClientPort *mockClientPort = new MockModbusClientPort();
    pmbClient cli(mockClientPort);
    cli.setName("cli");

    auto *cmd = new pmbCommandQueryReadInputRegisters(&mem, &cli);
    cmd->setUnit(1);
    cmd->setDevAddress(Modbus::Address(300101));
    cmd->setCount(10);

    EXPECT_CALL(*mockClientPort, readInputRegisters(1, 100, 10, _))
        .Times(1)
        .WillOnce(Return(Modbus::Status_Good));
        
    bool rc = cmd->run();
    EXPECT_TRUE(rc);
    delete cmd;
}

TEST(pmbCommandTest, QueryReadHoldingRegisters_Run)
{
    pmbMemory mem;
    MockModbusClientPort *mockClientPort = new MockModbusClientPort();
    pmbClient cli(mockClientPort);
    cli.setName("cli");

    auto *cmd = new pmbCommandQueryReadHoldingRegisters(&mem, &cli);
    cmd->setUnit(1);
    cmd->setDevAddress(Modbus::Address(400101));
    cmd->setCount(10);

    EXPECT_CALL(*mockClientPort, readHoldingRegisters(1, 100, 10, _))
        .Times(1)
        .WillOnce(Return(Modbus::Status_Good));
        
    bool rc = cmd->run();
    EXPECT_TRUE(rc);
    delete cmd;
}

TEST(pmbCommandTest, QueryWriteMultipleCoils_Run)
{
    pmbMemory mem;
    mem.realloc_4x(1000);
    MockModbusClientPort *mockClientPort = new MockModbusClientPort();
    pmbClient cli(mockClientPort);
    cli.setName("cli");

    auto *cmd = new pmbCommandQueryWriteMultipleCoils(&mem, &cli);
    cmd->setUnit(1);
    cmd->setDevAddress(Modbus::Address(101));
    cmd->setCount(10);
    cmd->setMemAddress(Modbus::Address(400001));

    EXPECT_CALL(*mockClientPort, writeMultipleCoils(1, 100, 10, _))
        .Times(1)
        .WillOnce(Return(Modbus::Status_Good));
        
    bool rc = cmd->run();
    EXPECT_TRUE(rc);
    delete cmd;
}

TEST(pmbCommandTest, QueryWriteMultipleRegisters_Run)
{
    pmbMemory mem;
    mem.realloc_4x(1000);
    MockModbusClientPort *mockClientPort = new MockModbusClientPort();
    pmbClient cli(mockClientPort);
    cli.setName("cli");

    auto *cmd = new pmbCommandQueryWriteMultipleRegisters(&mem, &cli);
    cmd->setUnit(1);
    cmd->setDevAddress(Modbus::Address(101));
    cmd->setCount(10);
    cmd->setMemAddress(Modbus::Address(400001));

    EXPECT_CALL(*mockClientPort, writeMultipleRegisters(1, 100, 10, _))
        .Times(1)
        .WillOnce(Return(Modbus::Status_Good));
        
    bool rc = cmd->run();
    EXPECT_TRUE(rc);
    delete cmd;
}

// Copy command: validate addresses and count
TEST(pmbCommandTest, CopyCommand_Construct)
{
    pmbMemory mem;
    auto *cmd = new pmbCommandCopy(&mem);
    ASSERT_NE(cmd, nullptr);
    cmd->setParams(Modbus::Address(400001), Modbus::Address(300010), 3);
    EXPECT_EQ(cmd->srcAddress().type(), Modbus::Memory_4x);
    EXPECT_EQ(cmd->srcAddress().offset(), 0);
    EXPECT_EQ(cmd->dstAddress().type(), Modbus::Memory_3x);
    EXPECT_EQ(cmd->dstAddress().offset(), 9);
    EXPECT_EQ(cmd->count(), 3);
    delete cmd;
}

// Delay command: milliseconds set
TEST(pmbCommandTest, DelayCommand_Construct)
{
    auto *cmd = new pmbCommandDelay();
    ASSERT_NE(cmd, nullptr);
    cmd->setMilliseconds(250);
    EXPECT_EQ(cmd->milliseconds(), 250);
    delete cmd;
}

// Dump command: format and address
TEST(pmbCommandTest, DumpCommand_Construct)
{
    pmbMemory mem;
    auto *cmd = new pmbCommandDump(&mem);
    ASSERT_NE(cmd, nullptr);
    cmd->setParams(Modbus::Address(300001), pmb::Format_Hex16, 4);
    EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_3x);
    EXPECT_EQ(cmd->memAddress().offset(), 0);
    EXPECT_EQ(cmd->count(), 4);
    EXPECT_EQ(cmd->format(), pmb::Format_Hex16);
    delete cmd;
}
