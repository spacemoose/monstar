#include "catch.hpp"
#include "detail/util.hpp"
#include "monstar.hpp"


TEST_CASE("Correctly escapes quotes")
{

	SECTION("pair of quotes")
	{
		constexpr auto t1{"Here's a string with a \"quotation\" "};
		constexpr auto r1{"Here's a string with a \\\"quotation\\\" "};
		CHECK(util::escape_quotes(t1) == r1);
	}

	SECTION("multiple quotes")
	{
		const std::string t2{"Here's a string with \"more\" \"pairs\" \"of\" \"quotes\""};
		const std::string r2{
		  "Here's a string with \\\"more\\\" \\\"pairs\\\" \\\"of\\\" \\\"quotes\\\""};
		CHECK(util::escape_quotes(t2) ==  r2);
	}

	SECTION("no quotes")
	{
		const std::string t3{"I have no quotes"};
		CHECK(util::escape_quotes(t3) == t3);
	}
}
