#include "TS_generator.hpp"
#include "ES_provider.hpp"
#include "notification.hpp"
#include "epoch.hpp"
#include "io_ops.hpp"
#include <iostream>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace monstar {
namespace detail {

/// @todo move semantics
TS_generator::TS_generator(const notification& note)
  : m_cur_state(note.get_state())
  , m_es_index(note.get_es_index())
  , m_es_type(note.get_es_type())
  , m_finished(note.finished())
  , m_fixed_data(note.get_data())
  , m_id(to_string(note.get_identifier()))
  , m_last_timestamp(note.get_timestamp())

{
}

std::ostream& operator<<(std::ostream& os, const data_t& data)
{
	for (auto& pair : data)
		os << "  " << pair.first << " :: " << pair.second << "\n";
	return os;
}

/// This takes data from a note, and performs all calculations so that the
/// updateTimings method (which is called before serializing data) produces correct results.
/// One must consider the following boundary cases to ensure corrct behavior:
void TS_generator::process_notification(notification& note)
{
  // apparently this is valid?  Consider and remove the warning if so.
	if (m_finished) {
	    /// This generally indicates something fishy going on:
		std::cerr << "\nMONSTAR WARNING:  updating a finished notification"
		          << "\n  timestamp: " << m_last_timestamp << " :: " << note.get_timestamp()
		          << "\n  id:        " << m_id << " :: " << to_string(note.get_identifier())
		          << "\n  state:     " << m_cur_state << " :: " << note.get_state()
		          << "\n  data:      \n"
		          << m_fixed_data << "\n  note data:  \n"
		          << note.get_data() << std::endl;
	}
	update_timings(note.get_timestamp());
	m_cur_state = note.get_state();
	m_finished = note.finished();
}


/// This updates the timings.  It might be called durings a process_notification call or prior
/// to the TS_processor sending notifications.
void TS_generator::update_timings(epoch::timestamp_t cur_timestamp)
{
	auto dt = cur_timestamp - m_last_timestamp;

	// this can happen if we just missed the cutoff on the last
	// send_ts_notifications, so we set to zero and call it rounding error.
	if (cur_timestamp < m_last_timestamp) {
		dt = 0;
	}

	if (m_finished) {
		return;
	}
	if (m_timings.count(m_cur_state)) {
		m_timings[m_cur_state] += dt;
	} else {
		m_timings.insert({m_cur_state, dt});
	}
	m_last_timestamp = cur_timestamp;
}

/// Send the timeseries data to monstar (elasticsearch).
/// @todo is this the right index/type breakdown?  Maybe we want an index per machine?
/// @todo maybe we want timestamp managed int the ts_processor?
void TS_generator::send_TS_data(ES_provider& ep)
{
	auto cur_timestamp = epoch::now();
	update_timings(cur_timestamp);
	std::stringstream ss;

	ss << "\"ID\":\"" << m_id << "\",";
	for (auto& p : m_fixed_data) {
		ss << fmt::format("\"{}\" : \"{}\"", p.first, p.second) << ",";
	}
	for (auto& p : m_timings) {
		ss << fmt::format("\"@{}\" : \"{}\"", p.first, p.second) << ",";
	}
	ss << "\"@timestamp\" : \"" << cur_timestamp << "\",";

	ss << "\"status\" : \"" << m_cur_state << "\"";
	ep.post_message(m_es_index, m_es_type, ss.str());
}

}
}
