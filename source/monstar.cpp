#include "monstar.hpp"
#include "detail/ES_provider.hpp"
#include "detail/configuration.hpp"
#include "epoch.hpp"
#include "notification.hpp"
#include "notification_handler.hpp"

#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace monstar {

namespace {

/// Clients of the library must take care of lifetime management of
/// the notification handler.
monstar::notification_handler* note_handler{nullptr};
} // anon ns.

using namespace detail;

void configure_graphite(std::string ip, int port, std::string prefix)
{
    configuration::instance().configure_service(connection_type::graphite, ip, port, prefix);
}

void configure_elasticsearch(std::string ip, int port, const es_data_t& instance_data)
{
	/// format the instance data for elasticsearch:
	std::stringstream ss;
	for (auto& p : instance_data) {
		ss << fmt::format(",\"{}\" : \"{}\"", p.first, p.second);
	};
	configuration::instance().configure_service(
	  connection_type::elastic_search, ip, port, ss.str());
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
	ss << "\"@timestamp\" : \"" << std::put_time(std::localtime(&tm), "%Y-%m-%dT%X%z")
	   << "\",";
	ss << fmt::format("\"{}\" : \"{}\"", "title", title) << ",";
	ss << fmt::format("\"{}\" : \"{}\"", "text", text) << ",";
	ss << fmt::format("\"{}\" : \"{}\"", "tags", tags);

	detail::ES_provider esp;
	esp.post_message(index, "events", ss.str());
}

} // ns monstar
