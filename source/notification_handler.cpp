#include "notification_handler.hpp"
#include "detail/TS_processor.hpp"

namespace monstar {

notification_handler::notification_handler(){};

/// This should only be called after all notifications are done.
notification_handler::~notification_handler()
{
	m_ready_to_join = true;
	m_thread.join();
}

void notification_handler::start(int period)
{
	assert(not m_ts_processor);
	m_ts_processor = std::make_unique<detail::TS_processor>(period);
	m_thread = std::thread(&notification_handler::processing_loop, this);
}

/// This assumes that no calls to notify are made after calling the
/// notification handler's destructor.  Otherwise difficulties arise.
void notification_handler::processing_loop()
{
	while (not m_ready_to_join) {
		try {
			m_ts_processor->process();
		} catch (std::runtime_error& e) {
			m_ready_to_join = true;
			std::cerr << "MONSTAR LIB: An exception was handled in the notification handler.  "
			             "Time-serializetion will be disabled.  The exception text was:\n"
			          << e.what();
		}
	}
}

notification_handler::notification_handler(notification_handler&& nh) noexcept
  : m_ts_processor(std::move(nh.m_ts_processor))
  , m_thread(std::move(nh.m_thread))
  , m_ready_to_join(std::move(nh.m_ready_to_join))
{
}

/// If  either:
///
///   - the TS_processor is null
///   - m_ready_to_join has been set (probably b/c an exception was thrown from 'process'
///
/// we just discard the notification object.  Otherwise we add it to
/// the queue to be time-serialized.
void notification_handler::add(const notification& note)
{
	if (not m_ts_processor || m_ready_to_join) {
		return;
	}
	m_ts_processor->add_message(note);
}

}
