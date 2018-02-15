#include "detail/graphite_provider.hpp"
#include "detail/ES_provider.hpp"
#include "monstar.hpp"
#include "notification.hpp"
#include "notification_handler.hpp"
#include "epoch.hpp"

#include <cassert>
#include <iostream>
#include <chrono>
#include <iomanip>

#define FMT_HEADER_ONLY
#include <fmt/format.h>


namespace monstar {

namespace {

/// Clients of the library must take care of lifetime management of
/// the notification handler.
monstar::notification_handler* note_handler{nullptr};
} // anon ns.

void configure_graphite(std::string ip, int port, std::string prefix)
{
	detail::graphite_provider::set_server(ip, port);
	detail::graphite_provider::set_prefix(prefix);
}

void configure_elasticsearch(std::string ip, int port, const es_data_t& instance_data)
{
	detail::ES_provider::es_server = ip;
	detail::ES_provider::es_port = std::to_string(port);
	detail::ES_provider::instance_data = instance_data;
}

/// We'll set this up to handle other kinds of notification handlers.
void set_notification_handler(notification_handler& nh) { note_handler = &nh; }

/// You shouldn't be calling notify unless you're already instantiated
/// a notification handler which will live long enough to support all
/// notifications.
void notify(const notification& note)
{
	if (note_handler) {
		note_handler->add(note);
	} else {
		assert(false);
		std::cerr << "Called monstar::notify() before initializing the api -- ignoring."
		          << std::endl;
	}
}

void send_annotation(const std::string& index,
                     const std::string& title,
                     const std::string& text,
                     const std::string& tags)
{
  using sc = std::chrono::system_clock;
  auto tm = sc::to_time_t(sc::now());


  std::stringstream ss;
  ss << "\"@timestamp\" : \"" << std::put_time(std::localtime(&tm), "%Y-%m-%dT%X%z") << "\",";
  ss << fmt::format("\"{}\" : \"{}\"", "title", title) << ",";
  ss << fmt::format("\"{}\" : \"{}\"", "text", text) << ",";
  ss << fmt::format("\"{}\" : \"{}\"", "tags", tags) ;
  std::cout << ss.str() << std::endl;
  detail::ES_provider esp;
  esp.post_message(index, "events", ss.str());
}

} // ns monstar
