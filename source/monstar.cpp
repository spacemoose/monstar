#include "monstar.hpp"
#include "TS_message.hpp"
#include "detail/TS_generator.hpp"
#include "detail/TS_processor.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <condition_variable>
#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>

namespace monstar
{

namespace detail
{
// We have a few static variables to allocate/initialize.
std::string graphite_poster::graphite_ip{"127.0.0.1"};
std::string graphite_poster::graphite_port{"2003"};
std::string graphite_poster::prefix{""};
es_data_t TS_generator::instance_data{};
}

/// implementation details:
namespace
{

std::string es_server{"127.0.0.1"};
std::string es_port{"9200"};

std::once_flag onceFlag;
static detail::TS_processor* ts_processor = nullptr;

/**
    An internal detail, called using the once flag to make sure
    there's only one processing thread.
*/
void really_start(int period)
{
	std::condition_variable initialized_cond;
	std::mutex mut;
	auto process_messages = [ period, &mut, &initialized_cond ]()
	{
		std::unique_lock<std::mutex> lock(mut);
		static detail::TS_processor tp(es_server, es_port, period);
		ts_processor = &tp;
		initialized_cond.notify_one();
		lock.unlock();
		while (true) {
			tp.process(); /// tp knows how long to sleep.
		}
	};
	std::thread ts_thread(process_messages);
	ts_thread.detach();
	std::unique_lock<std::mutex> lock(mut);
	initialized_cond.wait(lock, [] { return ts_processor != nullptr; });
	lock.unlock();
	/// I don't want this thread to continue until the
	/// ts_processor has been initialized or timed-out.
	assert(not ts_thread.joinable());
}

} // anon ns.

void configure_graphite(std::string ip, int port, std::string prefix)
{
	detail::graphite_poster::set_server(ip, port);
	detail::graphite_poster::set_prefix(prefix);
}

/**
   This will throw an exception if an error occurs connecting to the
   Graphite or Elasticsearch servers.

   If an exception is thrown, the library remains uninitialized, and
   notifications will be consumed silently.

   @todo set timeouts appropriately.

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

   @todo handle/test not-so-fringe cases:

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

void configure_elasticsearch(std::string ip, int port, const detail::es_data_t instance_data)
{
	es_server = ip;
	es_port = std::to_string(port);
	detail::TS_generator::instance_data = instance_data;
}

} // ns monstar
