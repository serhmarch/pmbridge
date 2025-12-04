#include <gtest/gtest.h>

#include <core/pmb_core.h>

namespace {

TEST(pmbCoreTest, VersionMacrosAreConsistent)
{
    // Basic sanity: string must contain three dot-separated numbers
    std::string v(PMBRIDGE_VERSION_STR);
    ASSERT_FALSE(v.empty());
    auto firstDot = v.find('.');
    auto secondDot = v.find('.', firstDot == std::string::npos ? 0 : firstDot + 1);
    EXPECT_NE(firstDot, std::string::npos);
    EXPECT_NE(secondDot, std::string::npos);
}

TEST(pmbCoreTest, ToProtocolTypeRecognizesAll)
{
    bool ok = false;
    EXPECT_EQ(pmb::toProtocolType("TCP", &ok), Modbus::TCP); EXPECT_TRUE(ok);
    EXPECT_EQ(pmb::toProtocolType("RTU", &ok), Modbus::RTU); EXPECT_TRUE(ok);
    EXPECT_EQ(pmb::toProtocolType("ASC", &ok), Modbus::ASC); EXPECT_TRUE(ok);
    // Lowercase variants are not recognized; should set ok=false
    EXPECT_FALSE(pmb::toProtocolType("tcp", &ok) != -1 || ok);
    EXPECT_FALSE(pmb::toProtocolType("rtu", &ok) != -1 || ok);
    EXPECT_FALSE(pmb::toProtocolType("asc", &ok) != -1 || ok);
    // Unknown strings should set ok=false; returned enum may be implementation-defined
    EXPECT_FALSE(ok);
    (void)pmb::toProtocolType("unknown", &ok);
}

TEST(pmbCoreTest, SizeofFormatCoversCommonTypes)
{
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Bin16 ), sizeof(uint16_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Oct16 ), sizeof(uint16_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Dec16 ), sizeof(uint16_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_UDec16), sizeof(uint16_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Hex16 ), sizeof(uint16_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Bin32 ), sizeof(uint32_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Oct32 ), sizeof(uint32_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Dec32 ), sizeof(uint32_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_UDec32), sizeof(uint32_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Hex32 ), sizeof(uint32_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Bin64 ), sizeof(uint64_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Oct64 ), sizeof(uint64_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Dec64 ), sizeof(uint64_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_UDec64), sizeof(uint64_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Hex64 ), sizeof(uint64_t));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Float ), sizeof(float   ));
    EXPECT_EQ(pmb::sizeofFormat(pmb::Format_Double), sizeof(double  ));
}

TEST(pmbCoreTest, ToFormatParsesNames)
{
    EXPECT_EQ(pmb::toFormat("Bin16" ), pmb::Format_Bin16 );
    EXPECT_EQ(pmb::toFormat("Oct16" ), pmb::Format_Oct16 );
    EXPECT_EQ(pmb::toFormat("Dec16" ), pmb::Format_Dec16 );
    EXPECT_EQ(pmb::toFormat("UDec16"), pmb::Format_UDec16);
    EXPECT_EQ(pmb::toFormat("Hex16" ), pmb::Format_Hex16 );
    EXPECT_EQ(pmb::toFormat("Bin32" ), pmb::Format_Bin32 );
    EXPECT_EQ(pmb::toFormat("Oct32" ), pmb::Format_Oct32 );
    EXPECT_EQ(pmb::toFormat("Dec32" ), pmb::Format_Dec32 );
    EXPECT_EQ(pmb::toFormat("UDec32"), pmb::Format_UDec32);
    EXPECT_EQ(pmb::toFormat("Hex32" ), pmb::Format_Hex32 );
    EXPECT_EQ(pmb::toFormat("Bin64" ), pmb::Format_Bin64 );
    EXPECT_EQ(pmb::toFormat("Oct64" ), pmb::Format_Oct64 );
    EXPECT_EQ(pmb::toFormat("Dec64" ), pmb::Format_Dec64 );
    EXPECT_EQ(pmb::toFormat("UDec64"), pmb::Format_UDec64);
    EXPECT_EQ(pmb::toFormat("Hex64" ), pmb::Format_Hex64 );
    EXPECT_EQ(pmb::toFormat("Float" ), pmb::Format_Float );
    EXPECT_EQ(pmb::toFormat("Double"), pmb::Format_Double);

    EXPECT_EQ(pmb::toFormat(" WeirdFmt "), pmb::Format_Unknown);
}

TEST(pmbCoreTest, ToConstCharPtrProvidesCanonicalNames)
{
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Bin16 ), "Bin16" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Oct16 ), "Oct16" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Dec16 ), "Dec16" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_UDec16), "UDec16");
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Hex16 ), "Hex16" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Bin32 ), "Bin32" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Oct32 ), "Oct32" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Dec32 ), "Dec32" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_UDec32), "UDec32");
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Hex32 ), "Hex32" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Bin64 ), "Bin64" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Oct64 ), "Oct64" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Dec64 ), "Dec64" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_UDec64), "UDec64");
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Hex64 ), "Hex64" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Float ), "Float" );
    EXPECT_STREQ(pmb::toConstCharPtr(pmb::Format_Double), "Double");
    
    EXPECT_EQ(pmb::toConstCharPtr(pmb::Format_Unknown), nullptr);
}

TEST(pmbCoreTest, NumericToStringHelpers)
{
    // 16-bit value 0x12AB
    uint16_t v16 = 0x12AB;
    EXPECT_EQ(pmb::toHexString(v16), std::string("12AB"));
    EXPECT_EQ(pmb::toOctString(v16), std::string(Modbus::toOctString<pmb::String>(v16))); // same backend
    EXPECT_EQ(pmb::toBinString(v16), std::string(Modbus::toBinString<pmb::String>(v16)));
    EXPECT_EQ(pmb::toDecString(v16), std::string("4779"));

    // 32-bit value
    uint32_t v32 = 0x00ABCDEFu;
    EXPECT_EQ(pmb::toHexString(v32), std::string("00ABCDEF"));
    EXPECT_EQ(pmb::toDecString(v32), std::to_string(v32));

    // padded decimal
    EXPECT_EQ(pmb::toDecString(123u, 6), std::string("000123"));
    EXPECT_EQ(pmb::toDecString(123u, 6, 'x'), std::string("xxx123"));
}

TEST(pmbCoreTest, ToStringListSplitsByCommonDelimiters)
{
    pmb::StringList l1 = pmb::toStringList("a,b;c d");
    ASSERT_EQ(l1.size(), static_cast<size_t>(4));
    auto it = l1.begin();
    EXPECT_EQ(*it++, std::string("a"));
    EXPECT_EQ(*it++, std::string("b"));
    EXPECT_EQ(*it++, std::string("c"));
    EXPECT_EQ(*it++, std::string("d"));

    pmb::StringList l2 = pmb::toStringList("  one ,  two ; three   ");
    ASSERT_EQ(l2.size(), static_cast<size_t>(3));
    auto it2 = l2.begin();
    EXPECT_EQ(*it2++, std::string("one"));
    EXPECT_EQ(*it2++, std::string("two"));
    EXPECT_EQ(*it2++, std::string("three"));
}

} // namespace
