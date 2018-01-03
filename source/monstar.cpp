#include "monstar.hpp"
#include "TS_message.hpp"
#include "detail/TS_generator.hpp"
#include "detail/TS_processor.hpp"
#include <boost/uuid/uuid_generators.hpp>

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>


namespace monstar {

namespace detail {
// We have a few static variables to allocate/initialize.
std::string graphite_poster::graphite_ip{"127.0.0.1"};
std::string graphite_poster::graphite_port{"2003"};
std::string graphite_poster::prefix{""};
es_data_t TS_generator::instance_data{};
}

/// implementation details:
namespace {

std::string es_server{"127.0.0.1"};
std::string es_port{"9200"};

std::once_flag onceFlag;
static detail::TS_processor* ts_processor = nullptr;

/**
    An internal detail, called using the once flag to make sure
    there's only one processing thread.
    @todo error handling.
*/
void really_start(int period)
{
	auto process_messages = [period]() {
		static detail::TS_processor tp(es_server, es_port, period);
		ts_processor = &tp;
		while (true) {
			tp.process(); /// tp knows how long to sleep.
		}
	};

	std::thread ts_thread(process_messages);
	ts_thread.detach();
	assert(not ts_thread.joinable());
}

} // anon ns.

void configure_graphite(std::string ip, int port, std::string prefix)
{
	detail::graphite_poster::set_server(ip, port);
	detail::graphite_poster::set_prefix(prefix);
}

/**
   @todo error handling:
         what if server's not available, timeout, buffering...
*/
void initialize_ts_processor(int period)
{
	using namespace std::chrono_literals;
	std::call_once(onceFlag, really_start, period);
}

/**
   I allow for the possiblity of a nullptr on ts_processor to allow
   disabling of messaging, either intentionally or because a
   connection problem occured , and we want do nothing.

   I need to implement something to handle some not-so-fringe cases:

     - server connection timeout.
     - get first notifications while waiting for server to connect.
     - temporary timeouts.

*/
void notify(const TS_message& msg)
{
	if (not ts_processor) {
		return;
	}
	ts_processor->add_message(msg);
}

///@todo move semantics?
void configure_elasticsearch(std::string ip, int port, const detail::es_data_t instance_data)
{
	es_server = ip;
	es_port = std::to_string(port);
	detail::TS_generator::instance_data = instance_data;
}

} // ns monstar
