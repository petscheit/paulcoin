#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-pac");
    BOOST_CHECK(GetBoolArg("-pac"));
    BOOST_CHECK(GetBoolArg("-pac", false));
    BOOST_CHECK(GetBoolArg("-pac", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-paco"));
    BOOST_CHECK(!GetBoolArg("-paco", false));
    BOOST_CHECK(GetBoolArg("-paco", true));

    ResetArgs("-pac=0");
    BOOST_CHECK(!GetBoolArg("-pac"));
    BOOST_CHECK(!GetBoolArg("-pac", false));
    BOOST_CHECK(!GetBoolArg("-pac", true));

    ResetArgs("-pac=1");
    BOOST_CHECK(GetBoolArg("-pac"));
    BOOST_CHECK(GetBoolArg("-pac", false));
    BOOST_CHECK(GetBoolArg("-pac", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nopac");
    BOOST_CHECK(!GetBoolArg("-pac"));
    BOOST_CHECK(!GetBoolArg("-pac", false));
    BOOST_CHECK(!GetBoolArg("-pac", true));

    ResetArgs("-nopac=1");
    BOOST_CHECK(!GetBoolArg("-pac"));
    BOOST_CHECK(!GetBoolArg("-pac", false));
    BOOST_CHECK(!GetBoolArg("-pac", true));

    ResetArgs("-pac -nopac");  // -pac should win
    BOOST_CHECK(GetBoolArg("-pac"));
    BOOST_CHECK(GetBoolArg("-pac", false));
    BOOST_CHECK(GetBoolArg("-pac", true));

    ResetArgs("-pac=1 -nopac=1");  // -pac should win
    BOOST_CHECK(GetBoolArg("-pac"));
    BOOST_CHECK(GetBoolArg("-pac", false));
    BOOST_CHECK(GetBoolArg("-pac", true));

    ResetArgs("-pac=0 -nopac=0");  // -pac should win
    BOOST_CHECK(!GetBoolArg("-pac"));
    BOOST_CHECK(!GetBoolArg("-pac", false));
    BOOST_CHECK(!GetBoolArg("-pac", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--pac=1");
    BOOST_CHECK(GetBoolArg("-pac"));
    BOOST_CHECK(GetBoolArg("-pac", false));
    BOOST_CHECK(GetBoolArg("-pac", true));

    ResetArgs("--nopac=1");
    BOOST_CHECK(!GetBoolArg("-pac"));
    BOOST_CHECK(!GetBoolArg("-pac", false));
    BOOST_CHECK(!GetBoolArg("-pac", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-pac", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-pac", "eleven"), "eleven");

    ResetArgs("-pac -bar");
    BOOST_CHECK_EQUAL(GetArg("-pac", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-pac", "eleven"), "");

    ResetArgs("-pac=");
    BOOST_CHECK_EQUAL(GetArg("-pac", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-pac", "eleven"), "");

    ResetArgs("-pac=11");
    BOOST_CHECK_EQUAL(GetArg("-pac", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-pac", "eleven"), "11");

    ResetArgs("-pac=eleven");
    BOOST_CHECK_EQUAL(GetArg("-pac", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-pac", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-pac", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-pac", 0), 0);

    ResetArgs("-pac -bar");
    BOOST_CHECK_EQUAL(GetArg("-pac", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-pac=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-pac", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-pac=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-pac", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--pac");
    BOOST_CHECK_EQUAL(GetBoolArg("-pac"), true);

    ResetArgs("--pac=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-pac", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nopac");
    BOOST_CHECK(!GetBoolArg("-pac"));
    BOOST_CHECK(!GetBoolArg("-pac", true));
    BOOST_CHECK(!GetBoolArg("-pac", false));

    ResetArgs("-nopac=1");
    BOOST_CHECK(!GetBoolArg("-pac"));
    BOOST_CHECK(!GetBoolArg("-pac", true));
    BOOST_CHECK(!GetBoolArg("-pac", false));

    ResetArgs("-nopac=0");
    BOOST_CHECK(GetBoolArg("-pac"));
    BOOST_CHECK(GetBoolArg("-pac", true));
    BOOST_CHECK(GetBoolArg("-pac", false));

    ResetArgs("-pac --nopac");
    BOOST_CHECK(GetBoolArg("-pac"));

    ResetArgs("-nopac -pac"); // pac always wins:
    BOOST_CHECK(GetBoolArg("-pac"));
}

BOOST_AUTO_TEST_SUITE_END()
