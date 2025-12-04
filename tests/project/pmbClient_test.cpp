#include <gtest/gtest.h>

#include <project/pmbClient.h>
#include <project/pmbProject.h>
#include <pmbMemory.h>

#include <ModbusTcpPort.h>
#include <ModbusRtuPort.h>
#include <ModbusAscPort.h>
#include <ModbusSerialPort.h>
#include <ModbusGlobal.h>

TEST(pmbClientTest, Create_TCP_Client_WithDefaults)
{
    pmbClient cli;
    cli.setName("cli_tcp");

    Modbus::TcpSettings ts{};
    ts.host = ModbusTcpPort::Defaults::instance().host;
    ts.port = ModbusTcpPort::Defaults::instance().port;
    ts.timeout = ModbusTcpPort::Defaults::instance().timeout;
    cli.setSettings(Modbus::TCP, &ts);

    ASSERT_NE(cli.port(), nullptr);
    EXPECT_EQ(cli.port()->type(), Modbus::TCP);
    auto *tcp = static_cast<ModbusTcpPort*>(cli.port()->port());
    const auto &d = ModbusTcpPort::Defaults::instance();
    EXPECT_STREQ(tcp->host(), d.host);
    EXPECT_EQ(tcp->port(), d.port);
    EXPECT_EQ(tcp->timeout(), d.timeout);
}

TEST(pmbClientTest, Apply_TCP_Settings)
{
    pmbClient cli;
    cli.setName("cli_tcp");

    Modbus::TcpSettings ts{};
    ts.host = "127.0.0.1";
    ts.port = 1502;
    ts.timeout = 2500;
    cli.setSettings(Modbus::TCP, &ts);

    auto *tcp = static_cast<ModbusTcpPort*>(cli.port()->port());
    ASSERT_NE(tcp, nullptr);
    EXPECT_STREQ(tcp->host(), "127.0.0.1");
    EXPECT_EQ(tcp->port(), 1502);
    EXPECT_EQ(tcp->timeout(), 2500);
}

TEST(pmbClientTest, Create_RTU_Client_WithDefaults)
{
    const auto &d = ModbusSerialPort::Defaults::instance();
    pmbClient cli;
    cli.setName("cli_rtu");
    Modbus::SerialSettings ss{};
    ss.portName = "COM1";
    ss.baudRate = d.baudRate;
    ss.dataBits = d.dataBits;
    ss.parity = d.parity;
    ss.stopBits = d.stopBits;
    ss.flowControl = d.flowControl;
    ss.timeoutFirstByte = d.timeoutFirstByte;
    ss.timeoutInterByte = d.timeoutInterByte;
    cli.setSettings(Modbus::RTU, &ss);

    auto *rtu = static_cast<ModbusRtuPort*>(cli.port()->port());
    ASSERT_NE(rtu, nullptr);
    EXPECT_EQ(rtu->type(), Modbus::RTU);
    EXPECT_STREQ(rtu->portName(), "COM1");
    EXPECT_EQ(rtu->baudRate(), d.baudRate);
    EXPECT_EQ(rtu->dataBits(), d.dataBits);
    EXPECT_EQ(rtu->parity(), d.parity);
    EXPECT_EQ(rtu->stopBits(), d.stopBits);
    EXPECT_EQ(rtu->flowControl(), d.flowControl);
}

TEST(pmbClientTest, Create_ASC_Client_WithCustom)
{
    pmbClient cli;
    cli.setName("cli_asc");
    Modbus::SerialSettings ss{};
    ss.portName = "COM2";
    ss.baudRate = 19200;
    ss.dataBits = 7;
    ss.parity = Modbus::EvenParity;
    ss.stopBits = Modbus::TwoStop;
    ss.flowControl = Modbus::HardwareControl;
    ss.timeoutFirstByte = 3000;
    ss.timeoutInterByte = 100;
    cli.setSettings(Modbus::ASC, &ss);

    auto *asc = static_cast<ModbusAscPort*>(cli.port()->port());
    ASSERT_NE(asc, nullptr);
    EXPECT_EQ(asc->type(), Modbus::ASC);
    EXPECT_STREQ(asc->portName(), "COM2");
    EXPECT_EQ(asc->baudRate(), 19200);
    EXPECT_EQ(asc->dataBits(), 7);
    EXPECT_EQ(asc->parity(), Modbus::EvenParity);
    EXPECT_EQ(asc->stopBits(), Modbus::TwoStop);
    EXPECT_EQ(asc->flowControl(), Modbus::HardwareControl);
    EXPECT_EQ(asc->timeoutFirstByte(), 3000);
    EXPECT_EQ(asc->timeoutInterByte(), 100);
}
