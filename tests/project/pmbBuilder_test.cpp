// Tests for pmbBuilder: parsing config into pmbProject and commands
#include <gtest/gtest.h>

#include <fstream>
#include <cstdio>
#include <string>

#include <core/pmb_core.h>
#include <project/pmbBuilder.h>
#include <project/pmbProject.h>
#include <project/pmbCommand.h>
#include <project/pmbClient.h>
#include <project/pmbServer.h>
#include <pmbMemory.h>

#include <ModbusServerResource.h>
#include <ModbusTcpServer.h>
#include <ModbusSerialPort.h>
#include <ModbusTcpPort.h>
#include <ModbusRtuPort.h>
#include <ModbusAscPort.h>
#include <ModbusClientPort.h>

namespace {


// Helper: write `content` to a file `path`. Returns true on success.
bool writeTextFile(const std::string &path, const std::string &content)
{
    // TODO: Need to set current working directory <HOME>/pmbridge/tests/ for relative paths

	std::ofstream f(path, std::ios::out | std::ios::trunc);
	if (!f.is_open())
		return false;
	f << content;
	return static_cast<bool>(f);
}

// Helper: simple unique filename per test
std::string uniqueFile(const char* base)
{
	// Keep it simple and deterministic for CI: append PID-like counter using address
	//char buf[256] = {0};
	//std::snprintf(buf, sizeof(buf), "%s_%p.conf", base, (void*)buf);
    char buf[] = "pmbridge_test.conf";
	return std::string(buf);
}

} // namespace

class pmbBuilderTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Remove previous conf file before testing
        std::remove("pmbridge_test.conf");
    }

    void TearDown() override
    {
        // Remove the temporary conf file after testing
        std::remove("pmbridge_test.conf");
    }
};

TEST_F(pmbBuilderTest, Load_MEMORY_Test)
{
    const std::string cfg = "MEMORY = 16, 32, 10, 20\n";
    const std::string path = uniqueFile("pmb_builder_memory");
    ASSERT_TRUE(writeTextFile(path, cfg)) << "Failed to write test config file";
    pmbBuilder builder;
    pmbProject* project = builder.load(path);
    ASSERT_NE(project, nullptr) << builder.lastError();
    EXPECT_FALSE(builder.hasError()) << builder.lastError();
    auto* mem = pmbMemory::global();
    EXPECT_EQ(mem->count_0x(), static_cast<size_t>(16));
    EXPECT_EQ(mem->count_1x(), static_cast<size_t>(32));
    EXPECT_EQ(mem->count_3x(), static_cast<size_t>(10));
    EXPECT_EQ(mem->count_4x(), static_cast<size_t>(20));
}

TEST_F(pmbBuilderTest, Load_SERVER_TCP_2Params)
{
    const std::string cfg = "SERVER = TCP, srv1\n";
    const std::string path = uniqueFile("pmb_builder_server_2params");
    ASSERT_TRUE(writeTextFile(path, cfg)) << "Failed to write test config file";
    pmbBuilder builder;
    pmbProject* project = builder.load(path);
    ASSERT_NE(project, nullptr) << builder.lastError();
    EXPECT_FALSE(builder.hasError()) << builder.lastError();
    EXPECT_EQ(project->servers().size(), static_cast<size_t>(1));
    auto* server = project->server("srv1");
    ASSERT_NE(server, nullptr);
    auto* serverPort = server->port();
    ASSERT_NE(serverPort, nullptr);
    ASSERT_EQ(serverPort->type(), Modbus::TCP);
    auto *tcpServer = static_cast<ModbusTcpServer*>(serverPort);
    const auto& d = ModbusTcpServer::Defaults::instance();
    EXPECT_EQ(tcpServer->port(), d.port);
    EXPECT_EQ(tcpServer->timeout(), d.timeout);
    EXPECT_EQ(tcpServer->maxConnections(), d.maxconn);
}

TEST_F(pmbBuilderTest, Load_SERVER_TCP_3Params_PortOverride)
{
	const std::string cfg = "SERVER = TCP, srv1, 1600\n"; // override port only
	const std::string path = uniqueFile("pmb_builder_server_3params");
	ASSERT_TRUE(writeTextFile(path, cfg)) << "Failed to write test config file";
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	EXPECT_EQ(project->servers().size(), static_cast<size_t>(1));
	auto* server = project->server("srv1");
	ASSERT_NE(server, nullptr);
	auto* serverPort = server->port();
	ASSERT_NE(serverPort, nullptr);
	ASSERT_EQ(serverPort->type(), Modbus::TCP);
	auto *tcpServer = static_cast<ModbusTcpServer*>(serverPort);
	const auto& d = ModbusTcpServer::Defaults::instance();
	EXPECT_EQ(tcpServer->port(), 1600);
	EXPECT_EQ(tcpServer->timeout(), d.timeout);   // default retained
	EXPECT_EQ(tcpServer->maxConnections(), d.maxconn); // default retained
}

TEST_F(pmbBuilderTest, Load_SERVER_TCP_4Params_PortAndTimeoutOverride)
{
	const std::string cfg = "SERVER = TCP, srv1, 1502, 3000\n"; // override port and timeout
	const std::string path = uniqueFile("pmb_builder_server_4params");
	ASSERT_TRUE(writeTextFile(path, cfg)) << "Failed to write test config file";
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	EXPECT_EQ(project->servers().size(), static_cast<size_t>(1));
	auto* server = project->server("srv1");
	ASSERT_NE(server, nullptr);
	auto* serverPort = server->port();
	ASSERT_NE(serverPort, nullptr);
	ASSERT_EQ(serverPort->type(), Modbus::TCP);
	auto *tcpServer = static_cast<ModbusTcpServer*>(serverPort);
	const auto& d = ModbusTcpServer::Defaults::instance();
	EXPECT_EQ(tcpServer->port(), 1502);
	EXPECT_EQ(tcpServer->timeout(), 3000);
	EXPECT_EQ(tcpServer->maxConnections(), d.maxconn); // default retained
}

TEST_F(pmbBuilderTest, Load_SERVER_TCP_5Params_AllOverrides)
{
	const std::string cfg = "SERVER = TCP, srv1, 1502, 2500, 20\n"; // override port, timeout, maxconn
	const std::string path = uniqueFile("pmb_builder_server_5params");
	ASSERT_TRUE(writeTextFile(path, cfg)) << "Failed to write test config file";
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	EXPECT_EQ(project->servers().size(), static_cast<size_t>(1));
	auto* server = project->server("srv1");
	ASSERT_NE(server, nullptr);
	auto* serverPort = server->port();
	ASSERT_NE(serverPort, nullptr);
	ASSERT_EQ(serverPort->type(), Modbus::TCP);
	auto *tcpServer = static_cast<ModbusTcpServer*>(serverPort);
	EXPECT_EQ(tcpServer->port(), 1502);
	EXPECT_EQ(tcpServer->timeout(), 2500);
	EXPECT_EQ(tcpServer->maxConnections(), 20);
}

// ---- RTU serial server tests: args count from 3 (minimum) up to 10 (all optional provided)

TEST_F(pmbBuilderTest, Load_SERVER_RTU_3Params_Defaults)
{
	const std::string cfg = "SERVER = RTU, srv_rtu, COM1\n"; // only port name; defaults applied
	const std::string path = uniqueFile("pmb_builder_server_rtu_3params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* server = project->server("srv_rtu");
	ASSERT_NE(server, nullptr);
	auto* serverPort = server->port();
	ASSERT_NE(serverPort, nullptr);
	ASSERT_EQ(serverPort->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(rtu->portName(), "COM1");
    EXPECT_EQ(rtu->baudRate(), d.baudRate);
    EXPECT_EQ(rtu->dataBits(), d.dataBits);
    EXPECT_EQ(rtu->parity(), d.parity);
    EXPECT_EQ(rtu->stopBits(), d.stopBits);
    EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
    EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_RTU_4Params_BaudOverride)
{
	const std::string cfg = "SERVER = RTU, srv_rtu, COM1, 19200\n"; // baud override
	const std::string path = uniqueFile("pmb_builder_server_rtu_4params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_rtu")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(rtu->portName(), "COM1");
	EXPECT_EQ(rtu->baudRate(), 19200);
	EXPECT_EQ(rtu->dataBits(), d.dataBits);
	EXPECT_EQ(rtu->parity(), d.parity);
	EXPECT_EQ(rtu->stopBits(), d.stopBits);
	EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_RTU_5Params_BaudDataBitsOverride)
{
	const std::string cfg = "SERVER = RTU, srv_rtu, COM1, 38400, 7\n"; // baud+data bits
	const std::string path = uniqueFile("pmb_builder_server_rtu_5params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_rtu")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(rtu->portName(), "COM1");
	EXPECT_EQ(rtu->baudRate(), 38400);
	EXPECT_EQ(rtu->dataBits(), 7);
	EXPECT_EQ(rtu->parity(), d.parity);
	EXPECT_EQ(rtu->stopBits(), d.stopBits);
	EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_RTU_6Params_AddParity)
{
	const std::string cfg = "SERVER = RTU, srv_rtu, COM1, 9600, 8, E\n"; // baud+data+parity
	const std::string path = uniqueFile("pmb_builder_server_rtu_6params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_rtu")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(rtu->portName(), "COM1");
	EXPECT_EQ(rtu->baudRate(), 9600);
	EXPECT_EQ(rtu->dataBits(), 8);
	EXPECT_EQ(rtu->parity(), Modbus::EvenParity);
	EXPECT_EQ(rtu->stopBits(), d.stopBits);
	EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_RTU_7Params_AddStopBits)
{
	const std::string cfg = "SERVER = RTU, srv_rtu, COM1, 9600, 8, N, 2\n"; // +stop bits
	const std::string path = uniqueFile("pmb_builder_server_rtu_7params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_rtu")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(rtu->portName(), "COM1");
	EXPECT_EQ(rtu->baudRate(), 9600);
	EXPECT_EQ(rtu->dataBits(), 8);
	EXPECT_EQ(rtu->parity(), Modbus::NoParity);
	EXPECT_EQ(rtu->stopBits(), Modbus::TwoStop);
	EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_RTU_8Params_AddFlowControl)
{
	const std::string cfg = "SERVER = RTU, srv_rtu, COM1, 19200, 6, O, 1, Hard\n"; // +flow control
	const std::string path = uniqueFile("pmb_builder_server_rtu_8params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_rtu")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(rtu->portName(), "COM1");
	EXPECT_EQ(rtu->baudRate(), 19200);
	EXPECT_EQ(rtu->dataBits(), 6);
	EXPECT_EQ(rtu->parity(), Modbus::OddParity);
	EXPECT_EQ(rtu->stopBits(), Modbus::OneStop);
	EXPECT_EQ(rtu->flowControl(), Modbus::HardwareControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_RTU_9Params_AddTimeoutFirstByte)
{
	const std::string cfg = "SERVER = RTU, srv_rtu, /dev/ttyUSB0, 4800, 8, No, 1.5, Soft, 2500\n"; // +timeoutFirstByte
	const std::string path = uniqueFile("pmb_builder_server_rtu_9params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_rtu")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(rtu->portName(), "/dev/ttyUSB0");
	EXPECT_EQ(rtu->baudRate(), 4800);
	EXPECT_EQ(rtu->dataBits(), 8);
	EXPECT_EQ(rtu->parity(), Modbus::NoParity);
	EXPECT_EQ(rtu->stopBits(), Modbus::OneAndHalfStop);
	EXPECT_EQ(rtu->flowControl(), Modbus::SoftwareControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), 2500);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_RTU_10Params_AddTimeoutInterByte)
{
	const std::string cfg = "SERVER = RTU, srv_rtu, /dev/ttyS0, 2400, 8, Even, 1, No, 3000, 100\n"; // +timeoutInterByte
	const std::string path = uniqueFile("pmb_builder_server_rtu_10params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_rtu")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	EXPECT_STREQ(rtu->portName(), "/dev/ttyS0");
	EXPECT_EQ(rtu->baudRate(), 2400);
	EXPECT_EQ(rtu->dataBits(), 8);
	EXPECT_EQ(rtu->parity(), Modbus::EvenParity);
	EXPECT_EQ(rtu->stopBits(), Modbus::OneStop);
	EXPECT_EQ(rtu->flowControl(), Modbus::NoFlowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), 3000);
	EXPECT_EQ(rtu->timeoutInterByte(), 100);
}

// ---- ASC serial server tests: same param progression

TEST_F(pmbBuilderTest, Load_SERVER_ASC_3Params_Defaults)
{
	const std::string cfg = "SERVER = ASC, srv_asc, COM2\n";
	const std::string path = uniqueFile("pmb_builder_server_asc_3params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_asc")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::ASC);
	auto *asc = static_cast<ModbusAscPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "COM2");
	EXPECT_EQ(asc->baudRate(), d.baudRate);
	EXPECT_EQ(asc->dataBits(), d.dataBits);
	EXPECT_EQ(asc->parity(), d.parity);
	EXPECT_EQ(asc->stopBits(), d.stopBits);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_ASC_4Params_BaudOverride)
{
	const std::string cfg = "SERVER = ASC, srv_asc, COM1, 19200\n"; // baud override
	const std::string path = uniqueFile("pmb_builder_server_asc_4params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_asc")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::ASC);
	auto *asc = static_cast<ModbusAscPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "COM1");
	EXPECT_EQ(asc->baudRate(), 19200);
	EXPECT_EQ(asc->dataBits(), d.dataBits);
	EXPECT_EQ(asc->parity(), d.parity);
	EXPECT_EQ(asc->stopBits(), d.stopBits);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_ASC_5Params_BaudDataBitsOverride)
{
	const std::string cfg = "SERVER = ASC, srv_asc, COM1, 38400, 7\n"; // baud+data bits
	const std::string path = uniqueFile("pmb_builder_server_asc_5params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_asc")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::ASC);
	auto *asc = static_cast<ModbusAscPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "COM1");
	EXPECT_EQ(asc->baudRate(), 38400);
	EXPECT_EQ(asc->dataBits(), 7);
	EXPECT_EQ(asc->parity(), d.parity);
	EXPECT_EQ(asc->stopBits(), d.stopBits);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_ASC_6Params_AddParity)
{
	const std::string cfg = "SERVER = ASC, srv_asc, COM2, 9600, 7, E\n";
	const std::string path = uniqueFile("pmb_builder_server_asc_6params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_asc")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::ASC);
	auto *asc = static_cast<ModbusAscPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "COM2");
	EXPECT_EQ(asc->baudRate(), 9600);
	EXPECT_EQ(asc->dataBits(), 7);
	EXPECT_EQ(asc->parity(), Modbus::EvenParity);
	EXPECT_EQ(asc->stopBits(), d.stopBits);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_ASC_7Params_AddStopBits)
{
	const std::string cfg = "SERVER = ASC, srv_asc, COM1, 9600, 8, N, 2\n"; // +stop bits
	const std::string path = uniqueFile("pmb_builder_server_asc_7params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_asc")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::ASC);
	auto *asc = static_cast<ModbusAscPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "COM1");
	EXPECT_EQ(asc->baudRate(), 9600);
	EXPECT_EQ(asc->dataBits(), 8);
	EXPECT_EQ(asc->parity(), Modbus::NoParity);
	EXPECT_EQ(asc->stopBits(), Modbus::TwoStop);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_ASC_8Params_AddFlowControl)
{
	const std::string cfg = "SERVER = ASC, srv_asc, COM1, 19200, 6, O, 1, Hard\n"; // +flow control
	const std::string path = uniqueFile("pmb_builder_server_asc_8params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_asc")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::ASC);
	auto *asc = static_cast<ModbusAscPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "COM1");
	EXPECT_EQ(asc->baudRate(), 19200);
	EXPECT_EQ(asc->dataBits(), 6);
	EXPECT_EQ(asc->parity(), Modbus::OddParity);
	EXPECT_EQ(asc->stopBits(), Modbus::OneStop);
	EXPECT_EQ(asc->flowControl(), Modbus::HardwareControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_ASC_9Params_AddTimeoutFirstByte)
{
	const std::string cfg = "SERVER = ASC, srv_asc, /dev/ttyS0, 1200, 7, Odd, 1, No, 4000\n";
	const std::string path = uniqueFile("pmb_builder_server_asc_9params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_asc")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::ASC);
	auto *asc = static_cast<ModbusAscPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "/dev/ttyS0");
	EXPECT_EQ(asc->baudRate(), 1200);
	EXPECT_EQ(asc->dataBits(), 7);
	EXPECT_EQ(asc->parity(), Modbus::OddParity);
	EXPECT_EQ(asc->stopBits(), Modbus::OneStop);
	EXPECT_EQ(asc->flowControl(), Modbus::NoFlowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), 4000);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_SERVER_ASC_10Params_AddTimeoutInterByte)
{
	const std::string cfg = "SERVER = ASC, srv_asc, /dev/ttyS2, 115200, 8, Even, 1, Hard, 4000, 200\n";
	const std::string path = uniqueFile("pmb_builder_server_asc_10params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* serverPort = project->server("srv_asc")->port();
	ASSERT_NE(serverPort, nullptr);
	EXPECT_EQ(serverPort->type(), Modbus::ASC);
	auto *asc = static_cast<ModbusAscPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
	EXPECT_STREQ(asc->portName(), "/dev/ttyS2");
	EXPECT_EQ(asc->baudRate(), 115200);
	EXPECT_EQ(asc->dataBits(), 8);
	EXPECT_EQ(asc->parity(), Modbus::EvenParity);
	EXPECT_EQ(asc->stopBits(), Modbus::OneStop);
	EXPECT_EQ(asc->flowControl(), Modbus::HardwareControl);
	EXPECT_EQ(asc->timeoutFirstByte(), 4000);
	EXPECT_EQ(asc->timeoutInterByte(), 200);
}

// ---- CLIENT TCP tests

TEST_F(pmbBuilderTest, Load_CLIENT_TCP_3Params_Minimal)
{
	const std::string cfg = "CLIENT = TCP, cli_tcp, 127.0.0.1\n";
	const std::string path = uniqueFile("pmb_builder_client_tcp_3params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* client = project->client("cli_tcp");
	ASSERT_NE(client, nullptr);
	auto* cliPort = client->port();
	ASSERT_NE(cliPort, nullptr);
	EXPECT_EQ(cliPort->type(), Modbus::TCP);
	auto *tcp = static_cast<ModbusTcpPort*>(cliPort->port());
	const auto& d = ModbusTcpPort::Defaults::instance();
	EXPECT_STREQ(tcp->host(), "127.0.0.1");
	EXPECT_EQ(tcp->port(), d.port);
	EXPECT_EQ(tcp->timeout(), d.timeout);
}

TEST_F(pmbBuilderTest, Load_CLIENT_TCP_4Params_PortOverride)
{
	const std::string cfg = "CLIENT = TCP, cli_tcp, localhost, 1600\n";
	const std::string path = uniqueFile("pmb_builder_client_tcp_4params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto* tcp = static_cast<ModbusTcpPort*>(project->client("cli_tcp")->port()->port());
	const auto& d = ModbusTcpPort::Defaults::instance();
	EXPECT_STREQ(tcp->host(), "localhost");
	EXPECT_EQ(tcp->port(), 1600);
	EXPECT_EQ(tcp->timeout(), d.timeout);
}

TEST_F(pmbBuilderTest, Load_CLIENT_TCP_5Params_PortAndTimeoutOverride)
{
	const std::string cfg = "CLIENT = TCP, cli_tcp, 10.0.0.5, 1502, 2500\n";
	const std::string path = uniqueFile("pmb_builder_client_tcp_5params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto* cliPort = project->client("cli_tcp")->port();
	auto* tcp = static_cast<ModbusTcpPort*>(cliPort->port());
	EXPECT_STREQ(tcp->host(), "10.0.0.5");
	EXPECT_EQ(tcp->port(), 1502);
	EXPECT_EQ(tcp->timeout(), 2500u);
}

// ---- CLIENT RTU tests (serial settings progression)

TEST_F(pmbBuilderTest, Load_CLIENT_RTU_3Params_Defaults)
{
	const std::string cfg = "CLIENT = RTU, cli_rtu, COM3\n";
	const std::string path = uniqueFile("pmb_builder_client_rtu_3params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto* cli = project->client("cli_rtu");
	ASSERT_NE(cli, nullptr);
	auto* port = cli->port();
	ASSERT_NE(port, nullptr);
	EXPECT_EQ(port->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(port->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(rtu->portName(), "COM3");
	EXPECT_EQ(rtu->baudRate(), d.baudRate);
	EXPECT_EQ(rtu->dataBits(), d.dataBits);
	EXPECT_EQ(rtu->parity(), d.parity);
	EXPECT_EQ(rtu->stopBits(), d.stopBits);
	EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_RTU_4Params_BaudOverride)
{
	const std::string cfg = "CLIENT = RTU, cli_rtu, COM3, 19200\n";
	const std::string path = uniqueFile("pmb_builder_client_rtu_4params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *rtu = static_cast<ModbusRtuPort*>(project->client("cli_rtu")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(rtu->portName(), "COM3");
	EXPECT_EQ(rtu->baudRate(), 19200);
	EXPECT_EQ(rtu->dataBits(), d.dataBits);
	EXPECT_EQ(rtu->parity(), d.parity);
	EXPECT_EQ(rtu->stopBits(), d.stopBits);
	EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_RTU_5Params_BaudDataBitsOverride)
{
	const std::string cfg = "CLIENT = RTU, cli_rtu, COM3, 38400, 7\n"; // baud+data bits
	const std::string path = uniqueFile("pmb_builder_client_rtu_5params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* clientPort = project->client("cli_rtu")->port();
	ASSERT_NE(clientPort, nullptr);
	EXPECT_EQ(clientPort->type(), Modbus::RTU);
	auto *rtu = static_cast<ModbusRtuPort*>(project->client("cli_rtu")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(rtu->portName(), "COM3");
	EXPECT_EQ(rtu->baudRate(), 38400);
	EXPECT_EQ(rtu->dataBits(), 7);
	EXPECT_EQ(rtu->parity(), d.parity);
	EXPECT_EQ(rtu->stopBits(), d.stopBits);
	EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_RTU_6Params_AddParity)
{
	const std::string cfg = "CLIENT = RTU, cli_rtu, COM3, 9600, 8, E\n";
	const std::string path = uniqueFile("pmb_builder_client_rtu_6params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *rtu = static_cast<ModbusRtuPort*>(project->client("cli_rtu")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(rtu->portName(), "COM3");
	EXPECT_EQ(rtu->baudRate(), 9600);
	EXPECT_EQ(rtu->dataBits(), 8);
	EXPECT_EQ(rtu->parity(), Modbus::EvenParity);
	EXPECT_EQ(rtu->stopBits(), d.stopBits);
	EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_RTU_7Params_AddStopBits)
{
	const std::string cfg = "CLIENT = RTU, cli_rtu, COM3, 9600, 8, E, 1.5\n";
	const std::string path = uniqueFile("pmb_builder_client_rtu_7params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *rtu = static_cast<ModbusRtuPort*>(project->client("cli_rtu")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(rtu->portName(), "COM3");
	EXPECT_EQ(rtu->baudRate(), 9600);
	EXPECT_EQ(rtu->dataBits(), 8);
	EXPECT_EQ(rtu->parity(), Modbus::EvenParity);
	EXPECT_EQ(rtu->stopBits(), Modbus::OneAndHalfStop);
	EXPECT_EQ(rtu->flowControl(), d.flowControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_RTU_8Params_AddFlowControl)
{
	const std::string cfg = "CLIENT = RTU, cli_rtu, COM3, 9600, 8, E, 1.5, Soft\n";
	const std::string path = uniqueFile("pmb_builder_client_rtu_8params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *rtu = static_cast<ModbusRtuPort*>(project->client("cli_rtu")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(rtu->portName(), "COM3");
	EXPECT_EQ(rtu->baudRate(), 9600);
	EXPECT_EQ(rtu->dataBits(), 8);
	EXPECT_EQ(rtu->parity(), Modbus::EvenParity);
	EXPECT_EQ(rtu->stopBits(), Modbus::OneAndHalfStop);
	EXPECT_EQ(rtu->flowControl(), Modbus::SoftwareControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_RTU_9Params_AddTimeoutFirstByte)
{
	const std::string cfg = "CLIENT = RTU, cli_rtu, COM3, 9600, 8, E, 1.5, Soft, 2500\n";
	const std::string path = uniqueFile("pmb_builder_client_rtu_9params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *rtu = static_cast<ModbusRtuPort*>(project->client("cli_rtu")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(rtu->portName(), "COM3");
	EXPECT_EQ(rtu->baudRate(), 9600);
	EXPECT_EQ(rtu->dataBits(), 8);
	EXPECT_EQ(rtu->parity(), Modbus::EvenParity);
	EXPECT_EQ(rtu->stopBits(), Modbus::OneAndHalfStop);
	EXPECT_EQ(rtu->flowControl(), Modbus::SoftwareControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), 2500);
	EXPECT_EQ(rtu->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_RTU_10Params_AddTimeoutInterByte)
{
	const std::string cfg = "CLIENT = RTU, cli_rtu, COM3, 9600, 8, E, 1.5, Soft, 2500, 100\n";
	const std::string path = uniqueFile("pmb_builder_client_rtu_10params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *rtu = static_cast<ModbusRtuPort*>(project->client("cli_rtu")->port()->port());
    EXPECT_STREQ(rtu->portName(), "COM3");
	EXPECT_EQ(rtu->baudRate(), 9600);
	EXPECT_EQ(rtu->dataBits(), 8);
	EXPECT_EQ(rtu->parity(), Modbus::EvenParity);
	EXPECT_EQ(rtu->stopBits(), Modbus::OneAndHalfStop);
	EXPECT_EQ(rtu->flowControl(), Modbus::SoftwareControl);
	EXPECT_EQ(rtu->timeoutFirstByte(), 2500);
	EXPECT_EQ(rtu->timeoutInterByte(), 100);
}

// ---- CLIENT ASC tests

TEST_F(pmbBuilderTest, Load_CLIENT_ASC_3Params_Defaults)
{
	const std::string cfg = "CLIENT = ASC, cli_asc, COM4\n";
	const std::string path = uniqueFile("pmb_builder_client_asc_3params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *asc = static_cast<ModbusAscPort*>(project->client("cli_asc")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "COM4");
	EXPECT_EQ(asc->baudRate(), d.baudRate);
	EXPECT_EQ(asc->dataBits(), d.dataBits);
	EXPECT_EQ(asc->parity(), d.parity);
	EXPECT_EQ(asc->stopBits(), d.stopBits);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
    EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
    EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_ASC_4Params_BaudOverride)
{
	const std::string cfg = "CLIENT = ASC, cli_asc, COM3, 19200\n";
	const std::string path = uniqueFile("pmb_builder_client_asc_4params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *asc = static_cast<ModbusAscPort*>(project->client("cli_asc")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "COM3");
	EXPECT_EQ(asc->baudRate(), 19200);
	EXPECT_EQ(asc->dataBits(), d.dataBits);
	EXPECT_EQ(asc->parity(), d.parity);
	EXPECT_EQ(asc->stopBits(), d.stopBits);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_ASC_5Params_BaudDataBitsOverride)
{
	const std::string cfg = "CLIENT = ASC, cli_asc, COM3, 38400, 7\n"; // baud+data bits
	const std::string path = uniqueFile("pmb_builder_client_asc_5params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	auto* clientPort = project->client("cli_asc")->port();
	ASSERT_NE(clientPort, nullptr);
	EXPECT_EQ(clientPort->type(), Modbus::ASC);
	auto *asc = static_cast<ModbusAscPort*>(project->client("cli_asc")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
	EXPECT_STREQ(asc->portName(), "COM3");
	EXPECT_EQ(asc->baudRate(), 38400);
	EXPECT_EQ(asc->dataBits(), 7);
	EXPECT_EQ(asc->parity(), d.parity);
	EXPECT_EQ(asc->stopBits(), d.stopBits);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_ASC_6Params_AddParity)
{
	const std::string cfg = "CLIENT = ASC, cli_asc, COM3, 9600, 8, E\n";
	const std::string path = uniqueFile("pmb_builder_client_asc_6params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *asc = static_cast<ModbusAscPort*>(project->client("cli_asc")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(asc->portName(), "COM3");
	EXPECT_EQ(asc->baudRate(), 9600);
	EXPECT_EQ(asc->dataBits(), 8);
	EXPECT_EQ(asc->parity(), Modbus::EvenParity);
	EXPECT_EQ(asc->stopBits(), d.stopBits);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_ASC_7Params_AddStopBits)
{
	const std::string cfg = "CLIENT = ASC, cli_asc, COM3, 9600, 8, E, 1.5\n";
	const std::string path = uniqueFile("pmb_builder_client_asc_7params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *asc = static_cast<ModbusAscPort*>(project->client("cli_asc")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(asc->portName(), "COM3");
	EXPECT_EQ(asc->baudRate(), 9600);
	EXPECT_EQ(asc->dataBits(), 8);
	EXPECT_EQ(asc->parity(), Modbus::EvenParity);
	EXPECT_EQ(asc->stopBits(), Modbus::OneAndHalfStop);
	EXPECT_EQ(asc->flowControl(), d.flowControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_ASC_8Params_AddFlowControl)
{
	const std::string cfg = "CLIENT = ASC, cli_asc, COM3, 9600, 8, E, 1.5, Soft\n";
	const std::string path = uniqueFile("pmb_builder_client_asc_8params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *asc = static_cast<ModbusAscPort*>(project->client("cli_asc")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(asc->portName(), "COM3");
	EXPECT_EQ(asc->baudRate(), 9600);
	EXPECT_EQ(asc->dataBits(), 8);
	EXPECT_EQ(asc->parity(), Modbus::EvenParity);
	EXPECT_EQ(asc->stopBits(), Modbus::OneAndHalfStop);
	EXPECT_EQ(asc->flowControl(), Modbus::SoftwareControl);
	EXPECT_EQ(asc->timeoutFirstByte(), d.timeoutFirstByte);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_ASC_9Params_AddTimeoutFirstByte)
{
	const std::string cfg = "CLIENT = ASC, cli_asc, COM3, 9600, 8, E, 1.5, Soft, 2500\n";
	const std::string path = uniqueFile("pmb_builder_client_asc_9params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *asc = static_cast<ModbusAscPort*>(project->client("cli_asc")->port()->port());
	const auto& d = ModbusSerialPort::Defaults::instance();
    EXPECT_STREQ(asc->portName(), "COM3");
	EXPECT_EQ(asc->baudRate(), 9600);
	EXPECT_EQ(asc->dataBits(), 8);
	EXPECT_EQ(asc->parity(), Modbus::EvenParity);
	EXPECT_EQ(asc->stopBits(), Modbus::OneAndHalfStop);
	EXPECT_EQ(asc->flowControl(), Modbus::SoftwareControl);
	EXPECT_EQ(asc->timeoutFirstByte(), 2500);
	EXPECT_EQ(asc->timeoutInterByte(), d.timeoutInterByte);
}

TEST_F(pmbBuilderTest, Load_CLIENT_ASC_10Params_AddTimeoutInterByte)
{
	const std::string cfg = 
    "CLIENT = {ASC,     \n"
    "          cli_asc, \n"
    "          COM3,    \n"
    "          9600,    \n"
    "          8,       \n"
    "          E,       \n"
    "          1.5,     \n"
    "          Soft,    \n"
    "          2500,    \n"
    "          100,     \n"
    "}\n"
    ;
	const std::string path = uniqueFile("pmb_builder_client_asc_10params");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	auto *asc = static_cast<ModbusAscPort*>(project->client("cli_asc")->port()->port());
    EXPECT_STREQ(asc->portName(), "COM3");
	EXPECT_EQ(asc->baudRate(), 9600);
	EXPECT_EQ(asc->dataBits(), 8);
	EXPECT_EQ(asc->parity(), Modbus::EvenParity);
	EXPECT_EQ(asc->stopBits(), Modbus::OneAndHalfStop);
	EXPECT_EQ(asc->flowControl(), Modbus::SoftwareControl);
	EXPECT_EQ(asc->timeoutFirstByte(), 2500);
	EXPECT_EQ(asc->timeoutInterByte(), 100);
}

// ------------------------------- QUERY tests ---------------------------------

TEST_F(pmbBuilderTest, Parse_QUERY_RD_ForAllMemoryTypes)
{
	const std::string cfg =
		// client definition
		"CLIENT = TCP, cli1, 127.0.0.1, 1502, 1000\n"
		// read coils (0x)
		"QUERY = cli1, 1, RD, 000001, 8, 400001, 1, 000100, 000101, 000102\n"
		// read discrete inputs (1x)
		"QUERY = cli1, 1, RD, 100010, 16, 400002, 2, 000110, 000111, 000112\n"
		// read input registers (3x)
		"QUERY = cli1, 1, RD, 300005, 4, 400010, 3, 000120, 000121, 000122\n"
		// read holding registers (4x)
		"QUERY = cli1, 1, RD, 400100, 6, 300200, 4, 000130, 000131, 000132\n";

	const std::string path = uniqueFile("pmb_query_rd_all");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	ASSERT_EQ(project->commands().size(), static_cast<size_t>(4));

	auto it = project->commands().begin();
	// 0x
	{
		auto* cmd = dynamic_cast<mbCommandQueryReadCoils*>(*it++);
		ASSERT_NE(cmd, nullptr);
		EXPECT_EQ(cmd->unit(), 1u);
		EXPECT_EQ(cmd->offset(), 0u);
		EXPECT_EQ(cmd->count(), 8u);
		EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_4x);
		EXPECT_EQ(cmd->memAddress().offset(), 0u);
		EXPECT_EQ(cmd->execPattern(), 1);
		EXPECT_EQ(cmd->succAddress().toInt(), Modbus::Address(100).toInt());
		EXPECT_EQ(cmd->errcAddress().toInt(), Modbus::Address(101).toInt());
		EXPECT_EQ(cmd->errvAddress().toInt(), Modbus::Address(102).toInt());
	}
	// 1x
	{
		auto* cmd = dynamic_cast<mbCommandQueryReadDiscreteInputs*>(*it++);
		ASSERT_NE(cmd, nullptr);
		EXPECT_EQ(cmd->offset(), 9u);
		EXPECT_EQ(cmd->count(), 16u);
		EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_4x);
		EXPECT_EQ(cmd->memAddress().offset(), 1u);
		EXPECT_EQ(cmd->execPattern(), 2);
	}
	// 3x
	{
		auto* cmd = dynamic_cast<mbCommandQueryReadInputRegisters*>(*it++);
		ASSERT_NE(cmd, nullptr);
		EXPECT_EQ(cmd->offset(), 4u);
		EXPECT_EQ(cmd->count(), 4u);
		EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_4x);
		EXPECT_EQ(cmd->memAddress().offset(), 9u);
		EXPECT_EQ(cmd->execPattern(), 3);
	}
	// 4x
	{
		auto* cmd = dynamic_cast<mbCommandQueryReadHoldingRegisters*>(*it++);
		ASSERT_NE(cmd, nullptr);
		EXPECT_EQ(cmd->offset(), 99u);
		EXPECT_EQ(cmd->count(), 6u);
		EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_3x);
		EXPECT_EQ(cmd->memAddress().offset(), 199u);
		EXPECT_EQ(cmd->execPattern(), 4);
	}
}

TEST_F(pmbBuilderTest, Parse_QUERY_WR_ForAllowedMemoryTypes)
{
	const std::string cfg =
		"CLIENT = TCP, cli1, 127.0.0.1, 1502, 1000\n"
		// write coils (0x)
		"QUERY = cli1, 10, WR, 000010, 3, 300001, 5, 000201, 000202, 000203\n"
		// write holding registers (4x)
		"QUERY = cli1, 10, WR, 400001, 2, 300010, 6, 000211, 000212, 000213\n";

	const std::string path = uniqueFile("pmb_query_wr_ok");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();
	ASSERT_EQ(project->commands().size(), static_cast<size_t>(2));

	auto it = project->commands().begin();
	// 0x
	{
		auto* cmd = dynamic_cast<mbCommandQueryWriteMultipleCoils*>(*it++);
		ASSERT_NE(cmd, nullptr);
		EXPECT_EQ(cmd->unit(), 10u);
		EXPECT_EQ(cmd->offset(), 9u);
		EXPECT_EQ(cmd->count(), 3u);
		EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_3x);
		EXPECT_EQ(cmd->execPattern(), 5);
	}
	// 4x
	{
		auto* cmd = dynamic_cast<mbCommandQueryWriteMultipleRegisters*>(*it++);
		ASSERT_NE(cmd, nullptr);
		EXPECT_EQ(cmd->unit(), 10u);
		EXPECT_EQ(cmd->offset(), 0u);
		EXPECT_EQ(cmd->count(), 2u);
		EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_3x);
		EXPECT_EQ(cmd->memAddress().offset(), 9u);
		EXPECT_EQ(cmd->execPattern(), 6);
	}
}

TEST_F(pmbBuilderTest, Parse_QUERY_WR_Rejects_1x_3x)
{
	const std::string cfg =
		"CLIENT = TCP, cli1, 127.0.0.1, 1502\n"
		// invalid WR for 1x
		"QUERY = cli1, 1, WR, 100001, 1, 300001, 1, 000001, 000002, 000003\n";
	const std::string path = uniqueFile("pmb_query_wr_bad");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	EXPECT_EQ(project, nullptr);
	EXPECT_TRUE(builder.hasError());
	EXPECT_NE(std::string(builder.lastError()).find("Unknown memory type for WR"), std::string::npos);
}

// ------------------------------- COPY tests ---------------------------------

TEST_F(pmbBuilderTest, Parse_COPY_SetsAddressesAndCount)
{
	const std::string cfg = "COPY = 400010, 5, 300020\n";
	const std::string path = uniqueFile("pmb_copy_ok");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	ASSERT_EQ(project->commands().size(), static_cast<size_t>(1));
	auto* cmd = dynamic_cast<mbCommandCopy*>(project->commands().front());
	ASSERT_NE(cmd, nullptr);
	EXPECT_EQ(cmd->srcAddress().type(), Modbus::Memory_4x);
	EXPECT_EQ(cmd->srcAddress().offset(), 9u);
	EXPECT_EQ(cmd->dstAddress().type(), Modbus::Memory_3x);
	EXPECT_EQ(cmd->dstAddress().offset(), 19u);
	EXPECT_EQ(cmd->count(), 5u);
}

TEST_F(pmbBuilderTest, Parse_COPY_RequiresThreeParams)
{
	const std::string cfg = "COPY = 400010, 5\n"; // missing dest
	const std::string path = uniqueFile("pmb_copy_bad");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	EXPECT_EQ(project, nullptr);
	EXPECT_TRUE(builder.hasError());
	EXPECT_NE(std::string(builder.lastError()).find("COPY-command must have 3 params"), std::string::npos);
}

// ------------------------------- DELAY tests ---------------------------------

TEST_F(pmbBuilderTest, Parse_DELAY_SetsMilliseconds)
{
	const std::string cfg = "DELAY = 750\n";
	const std::string path = uniqueFile("pmb_delay_ok");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	ASSERT_EQ(project->commands().size(), static_cast<size_t>(1));
	auto* cmd = dynamic_cast<mbCommandDelay*>(project->commands().front());
	ASSERT_NE(cmd, nullptr);
	EXPECT_EQ(cmd->milliseconds(), 750u);
}

TEST_F(pmbBuilderTest, Parse_DELAY_RequiresOneParam)
{
	const std::string cfg = "DELAY = \n";
	const std::string path = uniqueFile("pmb_delay_bad");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	EXPECT_EQ(project, nullptr);
	EXPECT_TRUE(builder.hasError());
	EXPECT_NE(std::string(builder.lastError()).find("DELAY-command must have 1 param"), std::string::npos);
}

// ------------------------------- DUMP tests ----------------------------------

TEST_F(pmbBuilderTest, Parse_DUMP_SetsAddressFormatCount)
{
	const std::string cfg = "DUMP = 300001, 8, Dec16\n";
	const std::string path = uniqueFile("pmb_dump_ok");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	ASSERT_EQ(project->commands().size(), static_cast<size_t>(1));
	auto* cmd = dynamic_cast<mbCommandDump*>(project->commands().front());
	ASSERT_NE(cmd, nullptr);
	EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_3x);
	EXPECT_EQ(cmd->memAddress().offset(), 0u);
	EXPECT_EQ(cmd->count(), 8u);
	EXPECT_EQ(cmd->format(), pmb::Format_Dec16);
}

TEST_F(pmbBuilderTest, Parse_DUMP_RejectsUnknownFormat)
{
	const std::string cfg = "DUMP = 300001, 8, WeirdFmt\n";
	const std::string path = uniqueFile("pmb_dump_bad");
	ASSERT_TRUE(writeTextFile(path, cfg));
	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	EXPECT_EQ(project, nullptr);
	EXPECT_TRUE(builder.hasError());
	EXPECT_NE(std::string(builder.lastError()).find("Unknown format"), std::string::npos);
}

TEST_F(pmbBuilderTest, LoadValidConfig_BuildsProjectAndCommands)
{
	// Prepare configuration with comments, blank lines, and mixed commands
	const std::string cfg =
		"# Memory sizing\n"
		"MEMORY = 8, 8, 5, 4\n" // 0x bits=8, 1x bits=8, 3x regs=5, 4x regs=4
		"\n"
		"; Servers (TCP and RTU)\n"
		"SERVER = TCP, srv_tcp, 1502, 2000, 10\n"
		"SERVER = RTU, srv_rtu, COM1, 9600, 8, N, 1\n"
		"\n"
		"# Client (TCP)\n"
		"CLIENT = TCP, cli1, 127.0.0.1, 1502, 2000\n"
		"\n"
		"# QUERY RD (read holding registers 4x)\n"
		"QUERY = cli1, 1, RD, 400001, 2, 300001, 0, 000001, 000002, 000003\n"
		"\n"
		"# QUERY WR (write holding registers 4x) using braces and quotes, multiline\n"
		"QUERY = { cli1, 1, 'WR', 400005, 3, 300010, 0, 000011, 000012, 000013 }\n"
		"\n"
		"# COPY\n"
		"COPY = 400001, 2, 300005\n"
		"\n"
		"# DELAY\n"
		"DELAY = 100\n"
		"\n"
		"# DUMP with Hex16 format\n"
		"DUMP = 300001, 4, Hex16\n";

	const std::string path = uniqueFile("pmb_builder_valid");
	ASSERT_TRUE(writeTextFile(path, cfg)) << "Failed to write test config file";

	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	ASSERT_NE(project, nullptr) << builder.lastError();
	EXPECT_FALSE(builder.hasError()) << builder.lastError();

	// Validate MEMORY sizes applied to global pmbMemory
	auto* mem = pmbMemory::global();
	EXPECT_EQ(mem->count_0x(), static_cast<size_t>(8)) << "0x bit count";
	EXPECT_EQ(mem->count_1x(), static_cast<size_t>(8)) << "1x bit count";
	EXPECT_EQ(mem->count_3x(), static_cast<size_t>(5)) << "3x reg count";
	EXPECT_EQ(mem->count_4x(), static_cast<size_t>(4)) << "4x reg count";

	// Servers and clients created
	EXPECT_EQ(project->servers().size(), static_cast<size_t>(2));
	EXPECT_NE(project->server("srv_tcp"), nullptr);
	EXPECT_NE(project->server("srv_rtu"), nullptr);
	EXPECT_EQ(project->clients().size(), static_cast<size_t>(1));
	EXPECT_NE(project->client("cli1"), nullptr);

	// Commands created (QUERY RD, QUERY WR, COPY, DELAY, DUMP)
	ASSERT_EQ(project->commands().size(), static_cast<size_t>(5));

	auto it = project->commands().begin();

	// 1) QUERY RD 4x -> mbCommandQueryReadHoldingRegisters
	{
		auto* cmd = dynamic_cast<mbCommandQueryReadHoldingRegisters*>(*it++);
		ASSERT_NE(cmd, nullptr) << "First command must be RD 4x";
		EXPECT_EQ(cmd->unit(), 1u);
		EXPECT_EQ(cmd->offset(), 0u);           // 400001 -> offset 0
		EXPECT_EQ(cmd->count(), 2u);
		EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_3x); // 300001
		EXPECT_EQ(cmd->memAddress().offset(), 0u);
		EXPECT_EQ(cmd->execPattern(), 1); // pattern can not be less than 1
		EXPECT_EQ(cmd->succAddress().toInt(), Modbus::Address(1).toInt());      // 000001 -> 0x address
		EXPECT_EQ(cmd->errcAddress().toInt(), Modbus::Address(2).toInt());
		EXPECT_EQ(cmd->errvAddress().toInt(), Modbus::Address(3).toInt());
	}

	// 2) QUERY WR 4x -> mbCommandQueryWriteMultipleRegisters
	{
		auto* cmd = dynamic_cast<mbCommandQueryWriteMultipleRegisters*>(*it++);
		ASSERT_NE(cmd, nullptr) << "Second command must be WR 4x";
		EXPECT_EQ(cmd->unit(), 1u);
		EXPECT_EQ(cmd->offset(), 4u);          // 400005 -> offset 4
		EXPECT_EQ(cmd->count(), 3u);
		EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_3x); // 300010
		EXPECT_EQ(cmd->memAddress().offset(), 9u);
		EXPECT_EQ(cmd->execPattern(), 1); // pattern can not be less than 1
		EXPECT_EQ(cmd->succAddress().toInt(), Modbus::Address(11).toInt());
		EXPECT_EQ(cmd->errcAddress().toInt(), Modbus::Address(12).toInt());
		EXPECT_EQ(cmd->errvAddress().toInt(), Modbus::Address(13).toInt());
	}

	// 3) COPY
	{
		auto* cmd = dynamic_cast<mbCommandCopy*>(*it++);
		ASSERT_NE(cmd, nullptr) << "Third command must be COPY";
		EXPECT_EQ(cmd->srcAddress().type(), Modbus::Memory_4x);
		EXPECT_EQ(cmd->srcAddress().offset(), 0u); // 400001 -> 0
		EXPECT_EQ(cmd->dstAddress().type(), Modbus::Memory_3x);
		EXPECT_EQ(cmd->dstAddress().offset(), 4u); // 300005 -> 4
		EXPECT_EQ(cmd->count(), 2u);
	}

	// 4) DELAY
	{
		auto* cmd = dynamic_cast<mbCommandDelay*>(*it++);
		ASSERT_NE(cmd, nullptr) << "Fourth command must be DELAY";
		EXPECT_EQ(cmd->milliseconds(), 100u);
	}

	// 5) DUMP
	{
		auto* cmd = dynamic_cast<mbCommandDump*>(*it++);
		ASSERT_NE(cmd, nullptr) << "Fifth command must be DUMP";
		EXPECT_EQ(cmd->memAddress().type(), Modbus::Memory_3x);
		EXPECT_EQ(cmd->memAddress().offset(), 0u); // 300001 -> 0
		EXPECT_EQ(cmd->count(), 4u);
		EXPECT_EQ(cmd->format(), pmb::Format_Hex16);
	}

	delete project;
}

TEST_F(pmbBuilderTest, LoadInvalidConfig_MissingEquals_ReportsError)
{
	const std::string cfg =
		"# Missing '=' after key\n"
		"CLIENT TCP, cli1, 127.0.0.1\n"; // invalid line

	const std::string path = uniqueFile("pmb_builder_invalid_eq");
	ASSERT_TRUE(writeTextFile(path, cfg));

	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	EXPECT_EQ(project, nullptr);
	EXPECT_TRUE(builder.hasError());
}

TEST_F(pmbBuilderTest, LoadInvalidConfig_UnknownFunction_ReportsError)
{
	const std::string cfg =
		"CLIENT = TCP, cli1, 127.0.0.1, 1502\n"
		"QUERY = cli1, 1, XX, 400001, 2, 300001, 0, 000001, 000002, 000003\n"; // XX unknown

	const std::string path = uniqueFile("pmb_builder_invalid_func");
	ASSERT_TRUE(writeTextFile(path, cfg));

	pmbBuilder builder;
	pmbProject* project = builder.load(path);
	EXPECT_EQ(project, nullptr);
	EXPECT_TRUE(builder.hasError());
}

TEST_F(pmbBuilderTest, LoadConfig_Github_Issue1)
{
    const std::string cfg =
        "MEMORY={1000,1000,1000,5000}\n"
        "#SERVER={RTU,mod,\"/dev/ttyUSB0\"}\n"
        "SERVER={RTU,modicon1,/dev/ttyUSB0,19200,8,N,2,Hard,1000,50}\n"
        "# Declaration. Port as TCP server with port 502, timeout 5000ms and max 10 connections\n"
        "CLIENT={TCP,client1,192.168.0.3,503}\n"
        "CLIENT={TCP,client2,192.168.0.5,503}\n"; // cli1 not defined

    const std::string path = uniqueFile("pmb_builder_github_issue1");
    ASSERT_TRUE(writeTextFile(path, cfg));

    pmbBuilder builder;
    pmbProject* project = builder.load(path);
    ASSERT_NE(project, nullptr);
    EXPECT_FALSE(builder.hasError());

    // Validate MEMORY sizes applied to global pmbMemory
    auto* mem = pmbMemory::global();
    EXPECT_EQ(mem->count_0x(), static_cast<size_t>(1000));
    EXPECT_EQ(mem->count_1x(), static_cast<size_t>(1000));
    EXPECT_EQ(mem->count_3x(), static_cast<size_t>(1000));
    EXPECT_EQ(mem->count_4x(), static_cast<size_t>(5000));

    // Servers and clients created
    EXPECT_EQ(project->servers().size(), static_cast<size_t>(1));
    EXPECT_EQ(project->clients().size(), static_cast<size_t>(2));

    // Check server params
    auto* server = project->server("modicon1");
    ASSERT_NE(server, nullptr);
    auto* serverPort = server->port();
    ASSERT_NE(serverPort, nullptr);
    ASSERT_EQ(serverPort->type(), Modbus::RTU);
    auto *rtu = static_cast<ModbusRtuPort*>(static_cast<ModbusServerResource*>(serverPort)->port());
    EXPECT_STREQ(rtu->portName(), "/dev/ttyUSB0");
    EXPECT_EQ(rtu->baudRate(), 19200);
    EXPECT_EQ(rtu->dataBits(), 8);
    EXPECT_EQ(rtu->parity(), Modbus::NoParity);
    EXPECT_EQ(rtu->stopBits(), Modbus::TwoStop);
    EXPECT_EQ(rtu->flowControl(), Modbus::HardwareControl);
    EXPECT_EQ(rtu->timeoutFirstByte(), 1000);
    EXPECT_EQ(rtu->timeoutInterByte(), 50);

    // Check client1 params
    auto* client1 = project->client("client1");
    ASSERT_NE(client1, nullptr);
    auto* clientPort1 = client1->port();
    EXPECT_EQ(clientPort1->type(), Modbus::TCP);
    auto* tcp1 = static_cast<ModbusTcpPort*>(clientPort1->port());
    ASSERT_NE(tcp1, nullptr);
    ASSERT_NE(tcp1->host(), nullptr);
    EXPECT_STREQ(tcp1->host(), "192.168.0.3");
    EXPECT_EQ(tcp1->port(), 503);

    // Check client2 params
    auto* client2 = project->client("client2");
    ASSERT_NE(client2, nullptr);
    auto* clientPort2 = client2->port();
    EXPECT_EQ(clientPort2->type(), Modbus::TCP);
    auto* tcp2 = static_cast<ModbusTcpPort*>(clientPort2->port());
    ASSERT_NE(tcp2, nullptr);
    ASSERT_NE(tcp2->host(), nullptr);
    EXPECT_STREQ(tcp2->host(), "192.168.0.5");
    EXPECT_EQ(tcp2->port(), 503);
}
