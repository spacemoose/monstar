#include "source/detail/configuration.hpp"
#include "catch.hpp"
#include "monstar.hpp"
#include "notification.hpp"
#include "scoped_timer.hpp"
#include "redirector.hpp"

using namespace testing;

TEST_CASE("Redirection infrastructure is working correctly")
{
	SECTION("one")
	{
		redirector r;
		auto errt = "errt";
		auto outt = "outt";
		std::cout << outt;
		std::cerr << errt;
		CHECK(r.get_out() == outt);
		CHECK(r.get_err() == errt);
	}
}

TEST_CASE("Unconfigured Monstar", "[configuration]")
{

	auto no_output_test = [](std::function<void()> f) {
		redirector r;
		f();
		CHECK(r.get_out().empty());
		CHECK(r.get_err().empty());
	};

	SECTION("send_annotation is silent.")
	{
		no_output_test([]() { monstar::send_annotation("index", "title", "text", "tags"); });
	}

	SECTION("notify is silent.")
	{
		no_output_test([]() {
			monstar::notification note(1, "new", "idx", "type", {{"foo", "bar"}});
			monstar::notify(note);
		});
	}

	SECTION("simple_recorder is silent.")
	{
		no_output_test([]() {
			monstar::simple_recorder sr("some_metric");
			sr(5);
		});
	}

	SECTION("scoped_timer is silent.")
	{
		no_output_test([]() { monstar::scoped_timer st("some.metric"); });
	}
}

/// What if elasticsearch is configured, but we aren't able to connect?
TEST_CASE("NO Elasticsearch, no Graphite.")
{

}
