#include <gtest/gtest.h>

#include <project/pmbProject.h>
#include <project/pmbServer.h>
#include <project/pmbClient.h>
#include <project/pmbCommand.h>
#include <pmbMemory.h>

#include <ModbusTcpServer.h>
#include <ModbusTcpPort.h>

TEST(pmbProjectTest, AddAndLookupServerClient)
{
    pmbProject prj;

    // Add server
    auto *mem = new pmbMemory();
    auto *srv = new pmbServer(mem);
    srv->setName("srv_tcp");
    Modbus::TcpSettings ss{};
    ss.ipaddr = "0.0.0.0";
    ss.port = 1502;
    ss.timeout = 3000;
    ss.maxconn = 10;
    srv->setSettings(Modbus::TCP, &ss);
    prj.addServer(srv);

    // Add client
    auto *cli = new pmbClient();
    cli->setName("cli_tcp");
    Modbus::TcpSettings cs{};
    cs.host = "127.0.0.1";
    cs.port = 1502;
    cs.timeout = 2500;
    cli->setSettings(Modbus::TCP, &cs);
    prj.addClient(cli);

    // Lookup
    auto *srv2 = prj.server("srv_tcp");
    auto *cli2 = prj.client("cli_tcp");
    ASSERT_NE(srv2, nullptr);
    ASSERT_NE(cli2, nullptr);
    EXPECT_STREQ(srv2->name().data(), "srv_tcp");
    EXPECT_STREQ(cli2->name().data(), "cli_tcp");
}

TEST(pmbProjectTest, AddCommands_OrderMaintained)
{
    pmbProject prj;
    auto *d1 = new pmbCommandDelay();
    d1->setMilliseconds(100);
    prj.addCommand(d1);

    auto *d2 = new pmbCommandDelay();
    d2->setMilliseconds(200);
    prj.addCommand(d2);

    auto *dump = new pmbCommandDump(pmbMemory::global());
    dump->setParams(Modbus::Address(1), pmb::Format_Hex16, 2);
    prj.addCommand(dump);

    ASSERT_EQ(prj.commands().size(), static_cast<size_t>(3));
    auto it = prj.commands().begin();
    auto *c1 = dynamic_cast<pmbCommandDelay*>(*it++);
    auto *c2 = dynamic_cast<pmbCommandDelay*>(*it++);
    auto *c3 = dynamic_cast<pmbCommandDump*>(*it++);
    ASSERT_NE(c1, nullptr);
    ASSERT_NE(c2, nullptr);
    ASSERT_NE(c3, nullptr);
    EXPECT_EQ(c1->milliseconds(), 100u);
    EXPECT_EQ(c2->milliseconds(), 200u);
    EXPECT_EQ(c3->count(), 2u);
}
