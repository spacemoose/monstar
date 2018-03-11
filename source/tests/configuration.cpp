#include "source/detail/configuration.hpp"
#include "catch.hpp"
#include "monstar.hpp"
#include "notification.hpp"
#include "scoped_timer.hpp"

/// I want to test what gets written to stderr and stdout, so I have a
/// redirector object to help with that.
class redirector
{
  public:
	redirector()
	  : m_old_out(std::cout.rdbuf(m_out.rdbuf()))
	  , m_old_err(std::cerr.rdbuf(m_err.rdbuf()))
	{
	}

	~redirector()
	{
		std::cout.rdbuf(m_old_out);
		std::cerr.rdbuf(m_old_err);
	}

	std::string get_out() { return m_out.str(); };
	std::string get_err() { return m_err.str(); };
	redirector(const redirector&) = delete;
	redirector& operator=(const redirector&) = delete;

  private:
	std::stringstream m_out;
	std::stringstream m_err;
	std::streambuf* m_old_out;
	std::streambuf* m_old_err;
};

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
TEST_CASE("Bad ES configuration")
{

}
