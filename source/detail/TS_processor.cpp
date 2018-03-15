#include "TS_processor.hpp"
#include "notification.hpp"
#include <chrono>
#include <iostream>

namespace monstar {
namespace detail {

TS_processor::TS_processor(int period)
  : m_period(period)
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
	check_performance(time_taken);
	std::this_thread::sleep_for(m_period - time_taken);
}

/// This throws a runtime_error in the event that we are taking too
/// long to process the messages, allowing the notification handler to
/// go into 'do nothing' mode.
void TS_processor::check_performance(
  const std::chrono::duration<double, std::milli> time_taken) const
{
	static int consecutive_overtime_count = 0;
	if (time_taken > m_period) {
		++consecutive_overtime_count;
		if (consecutive_overtime_count > 2) {
			throw std::runtime_error(
			  "MONSTAR-LIB : the performance check on the TS_processor failed");
		}
	} else {
		consecutive_overtime_count = 0;
	}
}

void TS_processor::send_TS_data()
{
	for (auto& pair : m_generators) {
		pair.second.send_TS_data(m_esProvider);
	}
}

// Remove any finished generators.
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
