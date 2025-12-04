#include <gtest/gtest.h>

#include <project/pmbCommand.h>
#include <project/pmbClient.h>
#include <pmbMemory.h>
#include <ModbusTcpPort.h>
#include <ModbusGlobal.h>

// Query Read Holding Registers: simulate memory interaction
TEST(pmbCommandTest, QueryReadHoldingRegisters_Construct)
{
    pmbMemory mem;
    pmbClient cli;
    cli.setName("cli");
    Modbus::TcpSettings ts{}; ts.host = "127.0.0.1"; ts.port = Modbus::STANDARD_TCP_PORT; ts.timeout = 1000;
    cli.setSettings(Modbus::TCP, &ts);

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
