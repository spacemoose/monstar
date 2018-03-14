#include "catch.hpp"
#include "monstar.hpp"
#include "notification.hpp"
#include "redirector.hpp"
#include "scoped_timer.hpp"
#include "source/detail/configuration.hpp"

using namespace testing;

/// For now this is disabled by default, since it relies on a hard
/// coded Graphtie and ES configuration.  @todo have cmake get the
/// values, and only run this if integration environment is setup.
///
/// @todo verify that messages were received and are accessable.
TEST_CASE("Connects successfully to an available ES and Graphite", "[integration]")
{
	redirector r;
	monstar::configure_elasticsearch("127.0.0.1", 9200, {{"environmnent", "test"}});
	monstar::configure_graphite("127.0.0.1", 2003, "foo.test");
}
