#include <gtest/gtest.h>

#include <log/pmbLogConsole.h>
#include <log/pmb_log.h>

namespace {

TEST(pmbLogConsoleTest, ParseFormatRecognizesTokens)
{
    auto tokens = pmbLogConsole::parseFormat("%time|%cat|%text");
    ASSERT_EQ(tokens.size(), static_cast<size_t>(5));
    auto it = tokens.begin();
    EXPECT_EQ(it->type, pmbLogConsole::Token::Time); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Literal); EXPECT_EQ(it->str, std::string("|")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Category); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Literal); EXPECT_EQ(it->str, std::string("|")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Text);
}

TEST(pmbLogConsoleTest, ParseTimeformatRecognizesUnits)
{
    auto tokens = pmbLogConsole::parseTimeformat("%Y-%M-%D %h:%m:%s.%f");
    ASSERT_EQ(tokens.size(), static_cast<size_t>(13));

    auto it = tokens.begin();
    
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Year); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Literal); EXPECT_EQ(it->str, std::string("-")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Month); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Literal); EXPECT_EQ(it->str, std::string("-")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Day); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Literal); EXPECT_EQ(it->str, std::string(" ")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Hour); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Literal); EXPECT_EQ(it->str, std::string(":")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Minute); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Literal); EXPECT_EQ(it->str, std::string(":")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Second); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Literal); EXPECT_EQ(it->str, std::string(".")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Millisecond); ++it;

    EXPECT_EQ(it, tokens.end());
}

TEST(pmbLogConsoleTest, SetLogFormatsAffectTokens)
{
    pmbLogConsole c;
    c.setLogFormat("[%cat] %text");
    auto tokens = c.formatTokens();
    ASSERT_EQ(tokens.size(), static_cast<size_t>(4));
    auto it = tokens.begin();
    EXPECT_EQ(it->type, pmbLogConsole::Token::Literal); EXPECT_EQ(it->str, std::string("[")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Category); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Literal); EXPECT_EQ(it->str, std::string("] ")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::Token::Text); ++it;

    EXPECT_EQ(it, tokens.end());
}

TEST(pmbLogConsoleTest, SetLogTimeFormatsAffectTokens)
{
    pmbLogConsole c;

    c.setLogTimeFormat("%h:%m:%s");
    auto tokens = c.timeFormatTokens();
    ASSERT_EQ(tokens.size(), static_cast<size_t>(5));

    auto it = tokens.begin();
    
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Hour); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Literal); EXPECT_EQ(it->str, std::string(":")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Minute); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Literal); EXPECT_EQ(it->str, std::string(":")); ++it;
    EXPECT_EQ(it->type, pmbLogConsole::TimeToken::Second); ++it;

    EXPECT_EQ(it, tokens.end());
}

TEST(pmbLogConsoleTest, ToStringBuildsPieces)
{
    pmbLogConsole c;
    // Literal + category + text
    pmbLogConsole::Token lit(pmbLogConsole::Token::Literal); lit.str = "(";
    EXPECT_EQ(c.toString(lit, pmb::Log_Info, "hello"), std::string("("));

    pmbLogConsole::Token cat(pmbLogConsole::Token::Category);
    EXPECT_STREQ(c.toString(cat, pmb::Log_Info, "hello").c_str(), pmb::toConstCharPtr(pmb::Log_Info));

    pmbLogConsole::Token txt(pmbLogConsole::Token::Text);
    EXPECT_EQ(c.toString(txt, pmb::Log_Info, "hello"), std::string("hello"));
}

TEST(pmbLogConsoleTest, GetTimeStringProducesNonEmpty)
{
    pmbLogConsole c;
    c.setLogTimeFormat("%h:%m:%s.%f");
    auto s = c.getTimeString();
    ASSERT_FALSE(s.empty());
}

} // namespace
