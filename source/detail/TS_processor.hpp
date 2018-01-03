#pragma once

#include "simple_recorder.hpp"
#include "MPSC_queue.hpp"
#include "TS_generator.hpp"
#include "ES_poster.hpp"
#include "TS_message.hpp"

#include <chrono>
#include <deque>

namespace monstar {
namespace detail {

/// This consumes TS_messages and produces time-series data which it sends to the
/// TSDB.
///
/// It maintains a container of TS_generators, which generate time-series data for
/// elasticsearch.  Every beat it uses these to generate and send TS data.  When a
/// TS_message is received it processes the message, and upates the generator.
///
/// The current implementation assumes concurrent message generation, but single
/// threaded processiing.
class TS_processor
{
  public:

	TS_processor(std::string es_server, std::string es_port, int period);

	/// Not intended for concurrent use.
	void process();

	/// Can be called concurrently.
	void add_message(TS_message msg){m_messages.push(msg);}

  private:
	size_t count_messages() const;
	void process_messages(size_t numMessages);
	void send_TS_data();
	void cull(); ///< Remove finished generators.

	ES_poster							m_esPoster;
	std::map<msg_id_t, TS_generator>	m_generators;
	MPSC_queue<TS_message>				m_messages;
	const std::chrono::seconds			m_period; ///< @todo get from configuration.
    /// Records the time taken to execute process.
	simple_recorder					m_proc_time_recorder;
    /// Records the size of the queue when process is called.
    simple_recorder					m_queue_size_recorder;
};



}
}
