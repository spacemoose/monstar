#pragma once

#include "ES_provider.hpp"
#include "MPSC_queue.hpp"
#include "TS_generator.hpp"
#include "notification.hpp"
#include "simple_recorder.hpp"

#include <chrono>
#include <deque>

namespace monstar {
namespace detail {

/// This consumes Notifications and produces time-series data which it
/// sends to the TSDB.
///
/// It maintains a container of TS_generators, which generate
/// time-series data for elasticsearch.  Every beat it uses these to
/// generate and send TS data.  When a Notification is received it
/// processes the message, and upates the generator.
///
/// The current implementation assumes concurrent message generation,
/// but single threaded processiing.
class TS_processor
{
  public:
	TS_processor(int period);

	/// Not intended for concurrent use.
	void process();

	/// Can be called concurrently.
	void add_message(notification msg) { m_notifications.push(msg); }

  private:
	size_t count_notifications() const;
	void process_notifications(size_t num);
	void send_TS_data();
	void cull(); ///< Remove finished generators.
	void check_performance(
	  const std::chrono::duration<double, std::milli> time_taken) const;

	ES_provider m_esProvider;
	std::map<msg_id_t, TS_generator> m_generators;
	MPSC_queue<notification> m_notifications;
	const std::chrono::seconds m_period; ///< @todo get from configuration.
	                                     /// Records the time taken to execute process.
	simple_recorder m_proc_time_recorder;
	/// Records the size of the queue when process is called.
	simple_recorder m_queue_size_recorder;
};
}
}
