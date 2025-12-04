#include <gtest/gtest.h>

#include <log/pmb_log.h>
#include <log/pmbLogConsole.h>

namespace {

TEST(pmbLogTest, ToColorMapsCategories)
{
    EXPECT_EQ(pmb::toColor(pmb::Log_Error), Modbus::Color_Red);
    EXPECT_EQ(pmb::toColor(pmb::Log_Warning), Modbus::Color_Yellow);
    EXPECT_EQ(pmb::toColor(pmb::Log_Dump), Modbus::Color_Green);
    EXPECT_EQ(pmb::toColor(pmb::Log_Info), Modbus::Color_Default);
    EXPECT_EQ(pmb::toColor(pmb::Log_Tx), Modbus::Color_Default);
}

TEST(pmbLogTest, ToConstCharPtrAndBack)
{
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Log_Error), "ERR");
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Log_Warning), "WARN");
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Log_Info), "INFO");
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Log_Dump), "DUMP");
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Log_Connection), "CONN");
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Log_Tx), "TX");
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Log_Rx), "RX");

    EXPECT_EQ(pmb::toLogFlag("ERR"), pmb::Log_Error);
    EXPECT_EQ(pmb::toLogFlag("WARN"), pmb::Log_Warning);
    EXPECT_EQ(pmb::toLogFlag("INFO"), pmb::Log_Info);
    EXPECT_EQ(pmb::toLogFlag("DUMP"), pmb::Log_Dump);
    EXPECT_EQ(pmb::toLogFlag("CONN"), pmb::Log_Connection);
    EXPECT_EQ(pmb::toLogFlag("TX"), pmb::Log_Tx);
    EXPECT_EQ(pmb::toLogFlag("RX"), pmb::Log_Rx);

    EXPECT_EQ(pmb::toLogFlag("UNKNOWN"), static_cast<pmb::LogFlag>(0));
}

TEST(pmbLogTest, ToLogFlagsParsesList)
{
    pmb::String s = "ERR, WARN; INFO RX";
    pmb::LogFlags flags = pmb::toLogFlags(s);
    EXPECT_TRUE(flags & pmb::Log_Error);
    EXPECT_TRUE(flags & pmb::Log_Warning);
    EXPECT_TRUE(flags & pmb::Log_Info);
    EXPECT_TRUE(flags & pmb::Log_Rx);
    EXPECT_FALSE(flags & pmb::Log_Dump);
}

TEST(pmbLogTest, SetAndGetLogFlags)
{
    pmb::setLogFlags(0);
    EXPECT_EQ(pmb::logFlags(), static_cast<pmb::LogFlags>(0));
    pmb::setLogFlags(pmb::Log_Error | pmb::Log_Info);
    EXPECT_EQ(pmb::logFlags(), static_cast<pmb::LogFlags>(pmb::Log_Error | pmb::Log_Info));
}

TEST(pmbLogTest, ConsoleFormatParsing)
{
    pmbLogConsole c;
    c.setLogFormat("%time [%cat] %text");
    auto tokens = c.formatTokens();
    ASSERT_EQ(tokens.size(), static_cast<size_t>(5));
    // Expect: Time, Literal(" ["), Category, Literal("] "), Text
    auto it = tokens.begin();
    EXPECT_EQ(it->type, pmbLogConsole::Token::Time); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Literal); EXPECT_EQ(it->str, std::string(" [")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Category); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Literal); EXPECT_EQ(it->str, std::string("] ")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Text);

    c.setLogTimeFormat("%Y-%m-%d %H:%M:%S.%f");
    auto tt = c.timeFormatTokens();
    EXPECT_FALSE(tt.empty());
}

} // namespace
