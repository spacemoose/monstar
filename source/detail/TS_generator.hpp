#pragma once

#include "epoch.hpp"
#include "monstar.hpp"

#include <map>

namespace monstar {

class Notification;

namespace detail {

class ES_poster;

using state_t = std::string;
using state_timings_t = std::map<state_t, epoch::timestamp_t>;
using es_data_t = std::map<std::string, std::string>;

/// When implementing a concrete Generator, the api implementation promises the following:
///    - processMsg will be guarenteed to be called sequentially.
///    - processMsg may be called multiple times before sending ts data.
///    - a state may be revisited, so it should accumulate time correctly.
class TS_generator
{
	friend void monstar::configure_elasticsearch(std::string,
	                                             int,
	                                             const monstar::detail::es_data_t);

  public:
	/// @todo move semantics
	TS_generator(Notification& msg);
	void process_message(Notification& msg);
	void send_TS_data(ES_poster& esp);
	void update_timings(epoch::timestamp_t cur_timestamp);
	bool finished() const { return m_finished; }

  private:
	state_t m_cur_state;
	const std::string m_es_index;
	const std::string m_es_type;
	bool m_finished{false};

	/// @todo format this just once.
	es_data_t m_fixed_data; ///< unchanging message data:
	const std::string m_id;
	epoch::timestamp_t m_last_timestamp;
	state_timings_t m_timings; ///< time spent in each state (secs)

	/// Data that needs to be sent for each elastic search data point.
	static es_data_t instance_data;
};
}
}
