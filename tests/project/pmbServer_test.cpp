#include <gtest/gtest.h>

#include <project/pmbServer.h>
#include <project/pmbProject.h>
#include <pmbMemory.h>

#include <ModbusServerResource.h>
#include <ModbusTcpServer.h>
#include <ModbusRtuPort.h>
#include <ModbusAscPort.h>
#include <ModbusSerialPort.h>
#include <ModbusGlobal.h>

TEST(pmbServerTest, Create_TCP_Server_Defaults)
{
    pmbMemory mem;
    pmbServer srv(&mem);
    srv.setName("srv_tcp");

    Modbus::TcpSettings ts{};
    ts.ipaddr = ModbusTcpServer::Defaults::instance().ipaddr;
    ts.port = ModbusTcpServer::Defaults::instance().port;
    ts.timeout = ModbusTcpServer::Defaults::instance().timeout;
    ts.maxconn = ModbusTcpServer::Defaults::instance().maxconn;
    srv.setSettings(Modbus::TCP, &ts);

    ASSERT_NE(srv.port(), nullptr);
    EXPECT_EQ(srv.port()->type(), Modbus::TCP);
    auto *tcp = static_cast<ModbusTcpServer*>(srv.port());
    const auto &d = ModbusTcpServer::Defaults::instance();
    EXPECT_EQ(tcp->port(), d.port);
    EXPECT_EQ(tcp->timeout(), d.timeout);
    EXPECT_EQ(tcp->maxConnections(), d.maxconn);
}

TEST(pmbServerTest, Apply_TCP_Server_Settings)
{
    pmbMemory mem;
    pmbServer srv(&mem);
    srv.setName("srv_tcp");

    Modbus::TcpSettings ts{};
    ts.ipaddr = "0.0.0.0";
    ts.port = 1502;
    ts.timeout = 3000;
    ts.maxconn = 20;
    srv.setSettings(Modbus::TCP, &ts);

    auto *tcp = static_cast<ModbusTcpServer*>(srv.port());
    ASSERT_NE(tcp, nullptr);
    EXPECT_EQ(tcp->port(), 1502);
    EXPECT_EQ(tcp->timeout(), 3000);
    EXPECT_EQ(tcp->maxConnections(), 20);
}

TEST(pmbServerTest, Create_RTU_Server_Defaults)
{
    pmbMemory mem;
    pmbServer srv(&mem);
    srv.setName("srv_rtu");
    Modbus::SerialSettings ss{};
    ss.portName = "COM1";
    ss.baudRate = ModbusSerialPort::Defaults::instance().baudRate;
    ss.dataBits = ModbusSerialPort::Defaults::instance().dataBits;
    ss.parity = ModbusSerialPort::Defaults::instance().parity;
    ss.stopBits = ModbusSerialPort::Defaults::instance().stopBits;
    ss.flowControl = ModbusSerialPort::Defaults::instance().flowControl;
    ss.timeoutFirstByte = ModbusSerialPort::Defaults::instance().timeoutFirstByte;
    ss.timeoutInterByte = ModbusSerialPort::Defaults::instance().timeoutInterByte;
    srv.setSettings(Modbus::RTU, &ss);

    ASSERT_EQ(srv.port()->type(), Modbus::RTU);
    auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(srv.port())->port());
    const auto &d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(rtu->portName(), "COM1");
    EXPECT_EQ(rtu->baudRate(), d.baudRate);
    EXPECT_EQ(rtu->dataBits(), d.dataBits);
    EXPECT_EQ(rtu->parity(), d.parity);
    EXPECT_EQ(rtu->stopBits(), d.stopBits);
    EXPECT_EQ(rtu->flowControl(), d.flowControl);
}

TEST(pmbServerTest, Create_ASC_Server_Custom)
{
    pmbMemory mem;
    pmbServer srv(&mem);
    srv.setName("srv_asc");
    Modbus::SerialSettings ss{};
    ss.portName = "COM2";
    ss.baudRate = 19200;
    ss.dataBits = 7;
    ss.parity = Modbus::EvenParity;
    ss.stopBits = Modbus::TwoStop;
    ss.flowControl = Modbus::HardwareControl;
    ss.timeoutFirstByte = 4000;
    ss.timeoutInterByte = 150;
    srv.setSettings(Modbus::ASC, &ss);

    ASSERT_EQ(srv.port()->type(), Modbus::ASC);
    auto *asc = static_cast<ModbusAscPort*>(static_cast<ModbusServerResource*>(srv.port())->port());
    ASSERT_NE(asc, nullptr);
    EXPECT_STREQ(asc->portName(), "COM2");
    EXPECT_EQ(asc->baudRate(), 19200);
    EXPECT_EQ(asc->dataBits(), 7);
    EXPECT_EQ(asc->parity(), Modbus::EvenParity);
    EXPECT_EQ(asc->stopBits(), Modbus::TwoStop);
    EXPECT_EQ(asc->flowControl(), Modbus::HardwareControl);
    EXPECT_EQ(asc->timeoutFirstByte(), 4000);
    EXPECT_EQ(asc->timeoutInterByte(), 150);
}
