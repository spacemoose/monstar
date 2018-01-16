#include "TS_processor.hpp"
#include "Notification.hpp"
#include <chrono>
#include <iostream>

namespace monstar {
namespace detail {

TS_processor::TS_processor(std::string es_server, std::string es_port, int period)
  : m_esPoster(es_server, es_port)
  , m_period(period)
  , m_proc_time_recorder("monstar.queue.proc_time")
  , m_queue_size_recorder("monstar.queue.size")
{
}

/// While we only have one thread processing, we have to take care that concurrent
/// threads may be adding notifications.
void TS_processor::process()
{
	/// @todo use scoped benchmarker here.
	namespace sc = std::chrono;
	auto start_time = sc::high_resolution_clock::now();
	auto num_notifications = m_notifications.count();
	process_notifications(num_notifications);
	send_TS_data();
	cull();
	m_queue_size_recorder(num_notifications);
	std::chrono::duration<double, std::milli> time_taken =
	  sc::system_clock::now() - start_time;
	m_proc_time_recorder(time_taken.count());
	std::this_thread::sleep_for(m_period - time_taken);
}

void TS_processor::send_TS_data()
{
	for (auto& pair : m_generators) {
		pair.second.send_TS_data(m_esPoster);
	}
}

// Remove any finished generators.
//
///@todo is this thread safe?  Could I have gotten a finished notification
// before sending?
void TS_processor::cull()
{
	std::map<msg_id_t, TS_generator> foo;
	for (auto iter = m_generators.begin(); iter != m_generators.end();) {
		if (iter->second.finished()) {
			iter = m_generators.erase(iter);
		} else {
			++iter;
		}
	}
}

void TS_processor::process_notifications(size_t num_notifications)
{
	while (num_notifications > 0) {
		--num_notifications;
		auto msg = m_notifications.pop();
		auto found = m_generators.find(msg.get_identifier());
		if (found == m_generators.end()) {
			m_generators.insert({msg.get_identifier(), msg});
		} else {
			found->second.process_notification(msg);
		}
	}
}
}
}
