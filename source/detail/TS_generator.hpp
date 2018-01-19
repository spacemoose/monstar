#pragma once

#include "epoch.hpp"
#include "monstar.hpp"

#include <map>

namespace monstar {

class notification;

namespace detail {

class ES_provider;

using state_t = std::string;
using state_timings_t = std::map<state_t, epoch::timestamp_t>;

/// When implementing a concrete Generator, the api implementation promises the following:
///    - process_notification will be guarenteed to be called sequentially.
///    - process_notificaiton may be called multiple times before sending ts data.
///    - a state may be revisited, so it should accumulate time correctly.
class TS_generator
{
  public:
	/// @todo move semantics
	TS_generator(const notification& note);
	void process_notification(notification& note);
	void send_TS_data(ES_provider& esp);
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
